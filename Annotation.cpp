#include "precompiled.h"

#include "Annotation.h"
#include <GL/glut.h>

const double Annotation::ANNOTATION_WIDTH = 1.5;

void Annotation::draw() {
	glColor3d( 0.3, 0.3, 0.3 );
	glLineWidth( ANNOTATION_WIDTH );
	glBegin( GL_LINE_STRIP );
	for( unsigned i = 0; i < path.size(); i++ ) {
		glVertex2i( path[i].x, path[i].y );
	}
	glEnd();

	glPointSize( ANNOTATION_WIDTH );
	glBegin( GL_POINTS );
	for( unsigned i = 0; i < path.size(); i++ ) {
		glVertex2i( path[i].x, path[i].y );
	}
	glEnd();
}

void Annotation::mouseDown( int button, Point2D p, bool doubleClick ) {}
void Annotation::mouseUp( int button, Point2D p ) {}
void Annotation::mouseMove( Point2D p ) {}