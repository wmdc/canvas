#include "precompiled.h"

#include "SKFRegion.h"

#include "CanvasPose.h"

#include <algorithm>
#include <GL/glut.h>
#include <assert.h>

SKFRegion::SKFRegion( Canvas *c ) : canvas(c), needsRecalculate(true) {
	empty = true;
	drawingBounds = true;
	drawingMotion = false;

	displayPose = NULL;
	drawnMotion = NULL;

	dragging = false;

	mAdd = NULL;

	selectionPath.setRadius( 4.0 * selectionPath.getRadius() );
}

void SKFRegion::addPose( CanvasPose *p ) {
	needsRecalculate = true;
	poses.push_back( p );
}

void SKFRegion::removePose( CanvasPose *p ) {
	needsRecalculate = true;
	//for( std::vector<CanvasPose *>::const_iterator i = poses.begin(); i <= poses.end(); i++ ) {
		//if( (*i) == p ) poses.erase( i );
	//}
}

void SKFRegion::draw() {
	glColor3f( 0.1f, 0.5f, 0.1f );
	glLineWidth( 6.0 );
	glLineStipple( 1, 0xFF00 );
	glEnable( GL_LINE_STIPPLE );

	outerBounds.drawGLVertices( drawingBounds ? GL_LINE_STRIP : GL_LINE_LOOP );

	glDisable( GL_LINE_STIPPLE );

    //Draw recently added motion if it exists
	if( mAdd ) {
		clock_t dt = clock() - tAdd;
		if( dt > TICKS_ADD_MOTION_FEEDBACK ) {
			mAdd = NULL;
		} else {
			float percentDone = (float)dt / TICKS_ADD_MOTION_FEEDBACK;
			glPushMatrix(); {
				glColor4d( 0.8f, 0.0f, 0.0f, 1 - percentDone );
				glTranslated( pAdd.x, pAdd.y, 0.0 );
			    glScaled( 20.0 * percentDone, 20.0 * percentDone, 1.0 );
				mAdd->draw( mAdd->getFrameCount() * percentDone );
			} glPopMatrix();
		}
	}

    //Draw the selection curve and point.
	if( !selectionPath.empty() && selectionPath.size() > 1 ) {
		glLineWidth( 7.0 );
	    glPointSize( 7.0 );
	    glColor3d( 0.7, 0.7, 0.7 );
	    selectionPath.drawGLVertices( GL_LINE_STRIP );
	    selectionPath.drawGLVertices( GL_POINTS );

		glColor3d( 1.0, 0.2, 0.2 );
		selectionPath.drawGLVertices( GL_POINTS, selectionPath.size() - 1, selectionPath.size() - 1 );
	}

	//Draw the interpolated pose
	if( displayPose ) {
		displayPose->drawHighlight();
	}
}

bool SKFRegion::contains( Point2D p ) const {
	//just use the bounding box
	return bBox.contains( p.x, p.y );
}

void SKFRegion::mouseDown( int button, Point2D p, bool doubleClick ) {
	if( contains( p ) ) {
		dragging = true;

		if( selectionPath.contains( p ) ) {
			if( selectionPath.size() == 1 ) {
				displayPose->setDragging();
				canvas->addElement( displayPose );
				displayPose = NULL;
				selectionPath.erase( selectionPath.begin(), selectionPath.end() );
			    selectionTimes.erase( selectionTimes.begin(), selectionTimes.end() );
		    } else if( selectionPath.size() > 1 ) {
				tAdd = clock();
				mAdd = generateMotion();
				pAdd = p;
				canvas->addMotion( mAdd );
				selectionPath.erase( selectionPath.begin(), selectionPath.end() );
			    selectionTimes.erase( selectionTimes.begin(), selectionTimes.end() );
		    }
		} else if( !poses.empty() ) {
		    selectionPath.erase( selectionPath.begin(), selectionPath.end() );
			selectionTimes.erase( selectionTimes.begin(), selectionTimes.end() );
		    selectionPath.push_back( p );
			selectionTimes.push_back( clock() );

			//delete displayPose;

			displayPose = new CanvasPose( *generatePose( p ), p, *canvas );

			if( button == CanvasItem::MOUSE_BTN_RIGHT )
			{
				drawingMotion = true;
			}
		}

		canvas->displayPose( *displayPose );
	}
}

void SKFRegion::mouseUp( int button, Point2D p ) {
	dragging = false;
	drawingMotion = false;
}

void SKFRegion::mouseMove( Point2D p ) {
	if( drawingBounds ) {
		return;
	}
	
	if( !drawingBounds && dragging )
	{
		displayPose = new CanvasPose( *generatePose( p ), p, *canvas );
		canvas->displayPose( *displayPose );
		//delete displayPose;
	}

	if( drawingMotion ) {//!poses.empty() && dragging && contains( p ) && canvas->isDragging() ) {
        selectionPath.push_back( p );
		selectionTimes.push_back( clock() );
	}
}

Pose *SKFRegion::generatePose( Point2D p ) {
	if( poses.empty() )
	{
		return NULL;
	}
	
	recalculate(); //TODO: only call this if points change

	assert(interpolator != NULL);

	unsigned nchannels = poses[0]->getChannelCount();
	float *data = new float[nchannels];

	for( unsigned i = 0; i < nchannels; i++ )
	{
		vector<float> values;
		for( unsigned j = 0; j < poses.size(); j++ )
		{
			values.push_back(poses[j]->getJointAngle(i));
		}
		data[i] = interpolator->interpolate( Rbf::point(p.x, p.y), values);
	}

	return new Pose( nchannels, 
		             poses[0]->getMaxOffset(), 
					 data,
					 poses[0]->getSkeleton());
}

Motion *SKFRegion::generateMotion() {
	float framedt = canvas->getMotion( 0 )->getFrameTime();

	std::vector<Pose *> poseList;

	for( unsigned i = 0; i < selectionPath.size(); i++ ) {
		poseList.push_back( generatePose( selectionPath[i] ) );
	}

	return new Motion( poseList, selectionTimes, framedt );
}

void SKFRegion::recalculate()
{
	vector<Rbf::point> pts;

	for( int i = 0; i < poses.size(); i++ )
	{
		pts.push_back(Rbf::point(poses[i]->getPosition().x, poses[i]->getPosition().y));
	}

	interpolator = new Rbf::Interpolant(pts);
}