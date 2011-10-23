#include <precompiled.h>

#include <vector>
#include <cmath>
#include <iostream>

#include <GL/glut.h>

#include "MotionCurve.h"
#include "Canvas.h"
#include "Pose.h"
#include "UtilityConstants.h"
#include "DrawingConstants.h"

const double MotionCurve::PLAYBACK_SEG_LENGTH = 3.5;

MotionCurve::MotionCurve( Path &pts, Motion &m, Canvas &c ) : motion(m), curve(pts), CanvasItem(c) { init(); }
MotionCurve::MotionCurve( Motion &m, Canvas &c ) : motion(m), CanvasItem(c), curve(Path()) { init(); sketching = true; }

void MotionCurve::init() {
	curve.setRadius( CURVE_LINE_WIDTH * 2.0 );

	animating = false;

	selecting = false;

	highlightFrame = NO_SELECTION;
	highlightEndFrame = NO_SELECTION;
    highlightIndex = NO_SELECTION;
	highlightEndIndex = NO_SELECTION;

	highlightEndArrow = false;

	flagSelected = false;

	mAdd = NULL;

	playbackFrame = NO_SELECTION;
}

/* Draw this curve on the canvas. */
void MotionCurve::draw() {
	if( !sketching ) { drawEndArrows(); }
	//update animation
	if( animating ) {
	    time_t dt = clock() - animationStartTime;

	    if( dt > motion.getRuntime() ) {
		    animating = false;
			highlightFrame = -1;
		} else {
			double highlightFrame = motion.getFrame( dt );
			highlightPoint = curve.getPointAtDistance( curve.getLength() * ( dt / motion.getRuntime() ) );
		}
	}
    // draw the highlighted frame (frame for mouse over) background
    if( !highlightEndArrow && playbackFrame != NO_SELECTION ) {
		glPointSize( 25.0 );
		glColor4d( COLOR_CURVE[0], COLOR_CURVE[1], COLOR_CURVE[2], 0.5 );
		glBegin( GL_POINTS ); {
			glVertex2d( playbackSeg.x, playbackSeg.y );
		} glEnd();
	}

	//draw the curve
	glColor4dv( sketching ? COLOR_BORDER_GREY : COLOR_CURVE_BORDER );
    glLineWidth( 10.0 );
	glPointSize( 10.0 );

	MotionCurve *before = neighbourSelectedBefore();
	MotionCurve *after = neighbourSelectedAfter();

	curve.drawGLVertices( GL_LINE_STRIP, before ? 0 : 0, curve.size()-1 );
	curve.drawGLVertices( GL_POINTS, before ? 0 : 0, curve.size()-1 );

	if( !sketching ) { //inner curve piece to cue user when curves overlap
		glColor4dv( COLOR_CURVE );
	    glLineWidth( 20.0 / 3.0 );
	    glPointSize( 20.0 / 3.0 );

		curve.drawGLVertices( GL_LINE_STRIP, before ? 0 : 0, curve.size()-1 );
	    curve.drawGLVertices( GL_POINTS, before ? 0 : 0, curve.size()-1 );
	}

	//draw the selected portion
	if( highlightFrame != NO_SELECTION && highlightEndFrame != NO_SELECTION ) {
		glColor4dv( COLOR_CURVE_HIGHLIGHT );
		glLineWidth( 20.0 / 3.0 );
		glPointSize( 20.0 / 3.0 );

		curve.drawGLVertices( GL_LINE_STRIP, before ? 0 : highlightIndex, after ? curve.size()-1: highlightEndIndex );
		curve.drawGLVertices( GL_POINTS, before ? 0 : highlightIndex, after ? curve.size()-1 : highlightEndIndex );

		if( before && before->getCurve()->size() > 1 ) {
			glBegin( GL_POINTS ); {
				glVertex2d( before->getCurve()->at( before->getCurve()->size() - 2 ).x, before->getCurve()->at( before->getCurve()->size() - 2 ).y );
			} glEnd();
		}
	}

	if(  !highlightEndArrow && playbackFrame != NO_SELECTION ) {
		glPointSize( 20.0 / 3.0 );
		glColor4d( 1.0, 0.0, 0.0, 0.8 );
		glBegin( GL_POINTS ); {
			glVertex2d( playbackSeg.x, playbackSeg.y );
		} glEnd();
	}

	if( mAdd ) {
		clock_t dt = clock() - tAdd;
		if( dt > TICKS_ADD_MOTION_FEEDBACK ) {
			mAdd = NULL;
		} else {
			float percentDone = (float)dt / TICKS_ADD_MOTION_FEEDBACK;
			glLineWidth( 3.0 );
			glPushMatrix(); {
				glColor4d( 0.8f, 0.0f, 0.0f, 1 - percentDone );
				glTranslated( pAdd.x, pAdd.y, 0.0 );
			    glScaled( 20.0, 20.0, 1.0 );
				mAdd->draw( mAdd->getFrame( dt ) );//mAdd->getFrameCount() * percentDone );
			} glPopMatrix();
		}
	}
}

void MotionCurve::drawEndArrows() const {
	glEnable( GL_POLYGON_SMOOTH );

	//MotionCurve *after = neighbourSelectedAfter();
	//if( after && after->getHighlightFrame() == 0 ) {
	if( highlightEndArrow ) {
	    glColor4dv( COLOR_CURVE_ARROW_HIGHLIGHT );
	} else {
		glColor4dv( COLOR_CURVE_ARROW );
	}
	

	glPushMatrix(); {
		glTranslated( curve.back().x, curve.back().y, 0 );

		Point2D curveFront = curve.size() > 6 ? curve[curve.size() - 5] : curve.front();

		if( curve.back().x >= curveFront.x ) {
			glRotated( DEG_PER_RAD * atan( ( curve.back().y - curveFront.y ) / ( curve.back().x - curveFront.x )), 0.0, 0.0, 1.0 );
		} else {
			glRotated( 180 + DEG_PER_RAD * atan( ( curve.back().y - curveFront.y ) / ( curve.back().x - curveFront.x )), 0.0, 0.0, 1.0 );
		}
		glScaled( CURVE_END_WIDTH * 1.4, CURVE_END_WIDTH * 1.4, 0 );

		glLineWidth( 3.0 );
		glBegin( GL_POLYGON ); {
			glVertex2d( -0.6, -0.7 );
			glVertex2d( -0.6,  0.7 );
			glVertex2i(  1.0,  0.0 );
		} glEnd();
	} glPopMatrix();
	glDisable( GL_POLYGON_SMOOTH );
}

void MotionCurve::startAnimation( int frame ) {
	animating = true;
    animationStartTime = clock();
	animationStartFrame = frame;
}

void MotionCurve::stopAnimation() {
	animating = false;
}

bool MotionCurve::contains( Point2D p ) const {
	if( p.dist( curve.front() ) <= CURVE_END_WIDTH || p.dist( curve.back() ) <= CURVE_END_WIDTH ) {
		return true;
	} else {
	    return curve.contains( p );
	}
}

void MotionCurve::mouseDown( int button, Point2D p, bool doubleClick ) {
	//start a new motion curve
	if( p.dist( curve.back() ) < CURVE_LINE_WIDTH ) {
		canvas.beginSketchingCurve( this, curve.back() );
		addChild( canvas.getDrawingCurve() );
		return;
	} else if( !graphPrev.empty() && p.dist( curve.front() ) < CURVE_LINE_WIDTH ) {
		//starting from the end of the last curve where the two overlap
		canvas.beginSketchingCurve( graphPrev[0], curve.front() );
		for( unsigned i = 0; i < graphPrev.size(); i++ ) {
			graphPrev[i]->addChild( canvas.getDrawingCurve() );
		}
		return;
	}
	//handle selection
	double lengthAlongCurve;
	Point2D proj = curve.projectToPath( p, &lengthAlongCurve );

	if( proj.dist( p ) <= CURVE_LINE_WIDTH * 2 ) {
		unsigned frame = getFrameFromLength( lengthAlongCurve );

		//add a canvas pose
		if( highlightFrame != -1 && doubleClick ) {
			highlightFrame = frame;
			updateHighlightIndices();
			canvas.addElement( new CanvasPose( motion, frame, curve[highlightIndex], canvas, true ));
			canvas.displayFrame( motion, frame );
			selecting = false;
			clearGraphSelection();
		} else {
			//highlight some part of the motion
			if( ( frame >= highlightFrame && frame <= highlightEndFrame ) || ( frame <= highlightFrame && frame >= highlightEndFrame ) ) {
				Motion *m = createMotionFromSelection();

				//THIS CREATES A MEMORY LEAK! Don't care for now, but is there a good way to improve this design?
				if( button == 1 )
				{
				    canvas.animate(*m);
				}
				else
				{
					canvas.addMotion(m);
					clearGraphSelection();
				}

			} else {
				clearGraphSelection();

				highlightFrame = frame;
				highlightEndFrame = frame;

				updateHighlightIndices();
				selecting = true;
                flagSelected = true;
			}
		}
	}
}

void MotionCurve::mouseUp( int button, Point2D p ) {
	selecting = false;
}

void MotionCurve::mouseMove( Point2D p ) {
	if( canvas.gotMouseOver() || !curve.contains( p ) ) {
		// if another control already responded to mouse over or this curve is not involved.
		setPlaybackFrame( MotionCurve::NO_SELECTION, false ); // do not display this frame on the canvas viewports
		return;
	}

	canvas.flagMouseOver(); // this motion curve had a mouseOver event.

	if( p.dist( curve.back() ) < CURVE_LINE_WIDTH ) {
		highlightEndArrow = true;
		return;
	} else {
		highlightEndArrow = false;
	}
    
	if( animating || sketching ) return;

	//mouse drag selection
	double lengthAlongCurve;
	Point2D proj = curve.projectToPath( p, &lengthAlongCurve );

	unsigned frame = getFrameFromLength( lengthAlongCurve );

	setPlaybackFrame( frame );

	MotionCurve *before = neighbourSelectedBefore();
	MotionCurve *after = neighbourSelectedAfter();

	if( selecting || ( before && before->isSelecting() ) || ( after && after->isSelecting() ) ) {
		selecting = true;
		flagSelected = true;

		if( before && !after ) {
			highlightFrame = 0;
			highlightEndFrame = frame;
		} else if( after && !before ) {
			highlightFrame = frame;
			highlightEndFrame = motion.getFrameCount() - 1;
		} else if( !after && !before ) {
			//no selection currently
			if( frame >= highlightFrame )
			    highlightEndFrame = frame;
			else
				highlightFrame = frame;
		}
		updateHighlightIndices();
	}
}

Motion *MotionCurve::createMotionFromSelection() const {
	//traverse the motion graph.
	//note: there is no check for cycles here!

	const MotionCurve *cur = this;

	while( cur->neighbourSelectedBefore() )
	{
		cur = cur->neighbourSelectedBefore();
	}

	Motion *result = NULL;

	while(cur)
	{
		Motion *m = new Motion(*cur->getMotion(), cur->getHighlightFrame(), cur->getHighlightEndFrame());

		if(result == NULL)
		{
			result = m;
		}
		else if(m)
		{
			*result += m;
		}

		cur = cur->neighbourSelectedAfter();
	}

	return result;
}

void MotionCurve::setPlaybackFrame( unsigned frame, bool showNewFrame ) {
	if( frame < 0 || frame >= motion.getFrameCount() ) {
		playbackFrame = NO_SELECTION;
	}
	//set the line segment to be drawn to highlight this frame on the canvas next time it is drawn
	playbackFrame = frame;

	playbackSeg = getCurve()->getPointAtDistance( frame / getMotion()->getFrameCount() );

	playbackSeg = curve.getPointAtDistance( curve.getLength() * ((double)playbackFrame ) / motion.getFrameCount()  );
	playbackSegEnd = curve.getPointAtDistance( 
		curve.getLength() * (playbackFrame + playbackFrame >= motion.getFrameCount() ? -0.5 : 0.5 ) / motion.getFrameCount()  );

	//rescale the segment such that it has length PLAYBACK_SEG_LENGTH
	double scale = PLAYBACK_SEG_LENGTH / playbackSeg.dist( playbackSegEnd );

	playbackSegEnd.x = playbackSeg.x + ( playbackSegEnd.x - playbackSeg.x ) * scale;
	playbackSegEnd.y = playbackSeg.y + ( playbackSegEnd.y - playbackSeg.y ) * scale;

    //display the figure on the canvas
	if( !canvas.isAnimating() && showNewFrame ) {
	  canvas.animate( motion, false, frame, frame );
	}
}