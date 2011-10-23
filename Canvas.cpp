#include "precompiled.h"

#include "Canvas.h"
#include "CanvasItem.h"
#include "Point2D.h"
#include "Motion.h"
#include "MotionCurve.h"
#include "CanvasViewport.h"
#include "Pose.h"
#include "Path.h"
#include "MotionBrush.h"
#include "ControlBrush.h"
#include "Brush.h"
#include "SKFRegion.h"
#include "TextAnnotation.h"
#include "BrushPalette.h"
#include <GL/glut.h>

#include <iostream>

const double Canvas::SKETCHING_MOTION_SCALE = 8.0;

Canvas::Canvas( vector<Motion *> initialMotions ) {
	drawingCurve = 0;
	annotating = false;
	playback = false;
	sketching = false;
	panning = false;
	mouseOverFound = false;

	drawingMotion = NULL;
	drawingRegion = NULL;
	dragElement = NULL;

	dOrigin = Point2D( 0, 0 ); //panning

	setMode( Canvas::MODE_ANNOTATE );

	dlGrid = 0;

	runTime = 0;

	//set up the control palette
	controlPalette = new BrushPalette( 984, 730, 156, 50, *this, false );
	controlPalette->add( new ControlBrush( Canvas::MODE_ANNOTATE, true, *this, *controlPalette ) );
	controlPalette->add( new ControlBrush( Canvas::MODE_SKF, false, *this, *controlPalette ));
	controlPalette->add( new ControlBrush( Canvas::MODE_SKETCH, false, *this, *controlPalette ));
	controlPalette->add( new ControlBrush( Canvas::MODE_TEXT, false, *this, *controlPalette ));

	motionPalette = new BrushPalette( 20, 20, 660, 80, *this, true );

	for( unsigned i = 0; i < initialMotions.size(); i++ ) {
		addMotion( initialMotions[i] );
	}

	addViewport( new CanvasViewport( 20, 280, 500, 500, *this, true, false ) );
}

bool Canvas::isAnimating() const {
	return clock() - startTime < runTime * 1000.0f;
}

/* Start animating a given motion clip somewhere on the canvas. */
void Canvas::animate( Motion &clip, bool repeat ) {
	startTime = clock();
	runTime = clip.getRuntime();

	for( unsigned i = 0; i < viewports.size(); i++ ) {
		viewports[i]->animate( clip, repeat );
	}
}

void Canvas::animate( Motion &clip, bool repeat, int startFrame, int endFrame ) {
	startTime = clock();
	runTime = clip.getFrameTime() * (endFrame - startFrame);

	for( unsigned i = 0; i < viewports.size(); i++ ) {
		viewports[i]->animate( clip, repeat, startFrame, endFrame );
	}
}

void Canvas::displayPose( Pose &p ) {
	for( unsigned i = 0; i < viewports.size(); i++ ) {
		viewports[i]->display( p );
	}
}

void Canvas::displayFrame( Motion &clip, int frame ) {
	for( unsigned i = 0; i < viewports.size(); i++ ) {
		viewports[i]->display( clip, frame );
	}
}

/* Draw the canvas. */
void Canvas::draw() {
	drawGrid();

	glPushMatrix(); {
		glTranslatef( dOrigin.x, dOrigin.y, 0.0f );

		for( size_t i = 0; i < regions.size(); i++ ) { regions[i]->draw(); }

		if( drawingRegion ) {
			drawingRegion->draw();
		}

		for( size_t i = 0; i < elements.size(); i++ ) { elements.at(i)->draw(); }

		for( unsigned i = 0; i < annotations.size(); i++ ) { annotations[i]->draw(); }

		for( unsigned i = 0; i < textAnnotations.size(); i++ ) { textAnnotations[i]->draw(); }

	} glPopMatrix();

    for( size_t i = 0; i < viewports.size(); i++ ) { viewports[i]->draw(); }
    
	controlPalette->draw();
	motionPalette->draw();
}

void Canvas::addElement( CanvasItem *item ) {
	elements.push_back( item );//elements.begin(), item );//push_back( &item );
}

void Canvas::addRegion( SKFRegion *region ) {
	if( region ) {
	    regions.push_back( region );
	} else {
		std::cerr << "warning: attempted to add a NULL region to the canvas.\n";
	}
}

void Canvas::addViewport( CanvasViewport *viewport ) {
	//show the cameras of other viewports in each scene
	for( unsigned i = 0; i < viewports.size(); i++ ) {
		viewport->addCam( viewports[i]->getCam() );
		viewports[i]->addCam( viewport->getCam() );
	}
	viewport->setCamNumber( viewports.size() );
	viewports.push_back( viewport );
}

void Canvas::insertPose( Pose *p ) {
	/*for( unsigned i = 0; i < elements.size(); i++ ) {
		if( elements.at(i)->type() == CanvasItem::TYPE_MOTION ) {
			MotionCurve *m = (MotionCurve *)elements.at(i);

			double t = m->getPath()->getT( p->getPoint() );

			if( t != Path::NOT_ON_PATH ) {
				m->getClip()->addKeyPose( p );
				return;
			}
		}
	}*/
}

void Canvas::mouseDown( int button, Point2D p, bool doubleClick ) {
	dragging = true;

	//mouse is over controls
	if( controlPalette->contains( p ) ) {
		controlPalette->mouseDown( button, p, doubleClick );
		return;
	} else if( motionPalette->contains( p ) ) {
		motionPalette->mouseDown( button, p, doubleClick );
		return;
	}
    //mouse is over viewports
    for( int i = (int)viewports.size() - 1; i >= 0; i-- ) {
		if( viewports[i]->contains( p ) ) {
			viewports[i]->mouseDown( button, p, doubleClick );
			return;
		}
	}
	for( int i = elements.size() - 1; i >= 0; i-- ) {
		if( elements.at(i)->contains( p - dOrigin ) ) {
		    elements.at(i)->mouseDown( button, p - dOrigin, doubleClick );
			return;
		}
	}
	//mouse is over (panned) regions and canvas elements
	for( size_t i = 0; i < regions.size(); i++ ) {
		if( regions[i]->contains( p - dOrigin ) ) {
			regions[i]->mouseDown( button, p - dOrigin, doubleClick );
			return;
		}
	}

	//no controls clicked on; interact with the canvas
	if( button == GLUT_LEFT_BUTTON ) {
		if( mode == MODE_ANNOTATE ) {
			annotating = true;
			annotations.push_back( new Annotation( *this ) );
			return;
		} else if( mode == MODE_SKETCH && drawingMotion ) {
			beginSketchingCurve(NULL, p - dOrigin);
		} else if( mode == MODE_TEXT ) {
			if( !textAnnotations.empty() ) {
				textAnnotations.at( textAnnotations.size() - 1 )->setActive( false );
			}
			textAnnotations.push_back( new TextAnnotation( CANVAS_TEXT_A_FONT, CANVAS_TEXT_A_HEIGHT, p - dOrigin ));
		} else if( mode == MODE_SKF ) {
            //create a spatial keyframing region
			drawingRegion = new SKFRegion( this );
			drawingRegion->addBoundsPoint( p - dOrigin );
		}
	} else if( button = GLUT_RIGHT_BUTTON ) { //right mouse button is for panning
		panning = true;
		glutSetCursor(GLUT_CURSOR_INFO);
	}
	pLast = p;
}
void Canvas::mouseUp( int button, Point2D p ) {
	dragging = false;
	annotating = false;
	sketching = false;
	panning = false;

	//Finished sketching a motion curve
	if( drawingCurve ) {
		if( drawingCurve->getLength() == 0 ) {
			removeElement( drawingCurve );
			//delete drawingCurve; //may want to fix the destructor at some point
			//for now, doesn't really matter
		} else {
			drawingCurve->doneSketching();
			drawingCurve = NULL;
		}
	}
    //Finished sketching a spatial keyframing region
	if( drawingRegion ) {
		if( !drawingRegion->isEmpty() ) {
			drawingRegion->doneBounds();
			this->addRegion( drawingRegion );
		} else {
			delete drawingRegion;
		}
		drawingRegion = NULL;
	}

	motionPalette->mouseUp( button, p );
	controlPalette->mouseUp( button, p );

	for( size_t i = 0; i < viewports.size(); i++ ) {
		viewports[i]->mouseUp( button, p );
	}

	for( size_t i = 0; i < regions.size(); i++ ) {
		regions[i]->mouseUp( button, p - dOrigin );
	}

	for( unsigned i = 0; i < elements.size(); i++ ) {
		elements.at(i)->mouseUp( button, p - dOrigin );
	}

	glutSetCursor(GLUT_CURSOR_INHERIT);
}
void Canvas::keystroke( unsigned char key ) {
	for( size_t index = 0; index < textAnnotations.size(); index++ ) {
		textAnnotations.at(index)->keystroke( key );
	}
}
void Canvas::mouseMove( Point2D p ) {
	mouseOverFound = false;

	if( panning ) {
		dOrigin.x += p.x - pLast.x;
		dOrigin.y += p.y - pLast.y;

		pLast = p;
	}
  
	motionPalette->mouseMove( p );
	controlPalette->mouseMove( p );

    for( size_t i = 0; i < viewports.size(); i++ ) {
		viewports[i]->mouseMove( p );
	}

	if( annotating ) {
		annotations.back()->addPoint( p - dOrigin );
	} else if( sketching ) {
		drawingCurve->addPoint( p - dOrigin );
	} else if( drawingRegion ) {
		drawingRegion->addBoundsPoint( p - dOrigin );
	}

	for( size_t i = 0; i < elements.size(); i++ ) {
		elements.at(i)->mouseMove( p - dOrigin );
	}

	for( size_t i = 0; i < regions.size(); i++ ) {
		regions[i]->mouseMove( p - dOrigin );
	}
}

void Canvas::drawGrid() {
	GLint vPort[4];
	glGetIntegerv( GL_VIEWPORT, vPort ); //x, y, width, height to array

	float offsetX = ( dOrigin.x / GRID_SPACING_PIXELS - (int)( dOrigin.x / GRID_SPACING_PIXELS ) ) * GRID_SPACING_PIXELS;
	float offsetY = ( dOrigin.y / GRID_SPACING_PIXELS - (int)( dOrigin.y / GRID_SPACING_PIXELS ) ) * GRID_SPACING_PIXELS;

	int rows = vPort[3] / GRID_SPACING_PIXELS;
	int cols = vPort[2] / GRID_SPACING_PIXELS;

    glPushMatrix(); {
		glTranslatef( offsetX, offsetY, 0.0f );

		if( glIsList( dlGrid ) ) {
			glCallList( dlGrid );
		} else {
			dlGrid = glGenLists(1);
			glNewList(dlGrid, GL_COMPILE_AND_EXECUTE ); {
				glColor4dv( COLOR_GRIDLINE );
				glLineWidth( 0.0f ); //'hairline'
				glBegin( GL_LINES );
				for( int i = 0; i <= rows; i++ ) {
					glVertex2i( vPort[0] - GRID_SPACING_PIXELS, i * GRID_SPACING_PIXELS );
					glVertex2i( vPort[2] + GRID_SPACING_PIXELS, i * GRID_SPACING_PIXELS );
				}
				for( int i = 0; i <= cols; i++ ) {
					glVertex2i( i * GRID_SPACING_PIXELS, vPort[1] - GRID_SPACING_PIXELS );
					glVertex2i( i * GRID_SPACING_PIXELS, vPort[3] + GRID_SPACING_PIXELS );
				}
				glEnd();
			} glEndList();
		}
	} glPopMatrix();
}

void Canvas::beginSketchingCurve( MotionCurve *parent, Point2D start ) {
	if( mode == MODE_SKETCH && drawingMotion != NULL ) {
		sketching = true;
		drawingCurve = new MotionCurve( *drawingMotion, *this );
		drawingCurve->addPoint( start );

		if( parent != NULL ) {
		    drawingCurve->addParent( parent );
		}
		addElement( drawingCurve );
	}
}