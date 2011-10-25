#include "precompiled.h"

#include "CanvasPose.h"
#include "SKFRegion.h"
#include "DrawingConstants.h"

#include <GL/glut.h>

const float CanvasPose::CANVAS_POSE_RADIUS = 20.0f;

CanvasPose::CanvasPose( Pose &pose, Point2D pt, Canvas &c, bool dragEnabled ) : 
    Pose( pose ), CanvasItem( c ), p( pt ), dragging(false), region(NULL) {

		for(int i = 0; i < 6; i++)
			data[i] = 0.0f;

        if( dragEnabled ) {
			dragging = dragEnabled;
			pLast = pt;
		}
}

CanvasPose::CanvasPose( Motion &m, int frame, Point2D location, Canvas &c, bool dragEnabled ) : 
    Pose( m, frame ), CanvasItem( c ), p( location ), dragging(false), region(NULL) {

		for(int i = 0; i < 6; i++)
			data[i] = 0.0f;

		if( dragEnabled ) {
			dragging = dragEnabled;
			pLast = location;
		}
}

void CanvasPose::draw() {
	glPushMatrix(); {
		glTranslated( p.x, p.y, 0.0 );
		glScaled( 10.0, 10.0, 1.0 );

		glPointSize(CURVE_LINE_WIDTH * 2);
		glColor4dv( dragging ? COLOR_CURVE_HIGHLIGHT : COLOR_POSE );

		glBegin( GL_POINTS ); {
			glVertex2i( 0, 0 );
		} glEnd();
        
		glColor3f( 0.1f, 0.1f, 0.1f );
		glLineWidth( 2.0f );
        Pose::draw();
	} glPopMatrix();
}

void CanvasPose::drawHighlight() {
	glPushMatrix(); {
		glTranslated( p.x, p.y, 0.0 );
		glScaled( 10.0, 10.0, 1.0 );

		glPointSize(CURVE_LINE_WIDTH * 2);
		glColor3d( 0.6, 0.1, 0.1 );

		glBegin( GL_POINTS ); {
			glVertex2i( 0, 0 );
		} glEnd();
        
		glColor3f(0.8f, 0.0f, 0.0f );
		glLineWidth( 2.0f );
        Pose::draw();
	} glPopMatrix();

}

bool CanvasPose::contains( Point2D pt ) const {
	return p.dist( pt ) <= CANVAS_POSE_RADIUS;
}

void CanvasPose::mouseDown( int button, Point2D pt, bool doubleClick ) {
	if( contains( pt ) ) {
		dragging = true;
		pLast = pt;
		canvas.displayPose( *this );
	}
}
void CanvasPose::mouseUp( int button, Point2D pt ) {
	dragging = false;
}
void CanvasPose::mouseMove( Point2D pt ) {
	if( dragging ) {
        p += ( pt - pLast );
		pLast = pt;

		//Check the canvas to see if we need to add this to a region
		if( region == NULL ) {
		    for( unsigned i = 0; i < canvas.getRegionCount(); i++ ) {
			    if( canvas.getRegion( i )->contains( pt ) ) {
				    canvas.getRegion( i )->addPose( this );
					region = canvas.getRegion( i );
					return;
			    }
		    }
		} else if( !region->contains( pt ) ) {
			//removed from a region
			region->removePose( this );
			region = NULL;
		}
		
	}
}