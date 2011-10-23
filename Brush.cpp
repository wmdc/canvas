#include "precompiled.h"

#include "Brush.h"
#include "Canvas.h"

#include <GL/glut.h>


void Brush::select() {
	selected = true;
}

void Brush::deselect() {
	selected = false;
}

void Brush::setSelectionDisplay( bool showSelected ) {
	selected = showSelected;
}

void Brush::mouseDown( int button, Point2D p, bool doubleClick ) {
	down = contains( p );
}

void Brush::mouseUp( int button, Point2D p ) {
	if( down ) {
		if( contains(p) ) {
			//deselect all other brushes in the palette
			palette.select( this );
		}
		down = false;
	}
}
void Brush::mouseMove( Point2D p ) {
	hover = contains( p );
}

void Brush::draw() {
	//draw the back panel
	glColor4dv(COLOR_INSET_GREY);
	glBegin( GL_POLYGON ); {
		glVertex2i( 0, 0 );
		glVertex2i( width, 0 );
		glVertex2i( width, height );
		glVertex2i( 0, height );
	} glEnd();

	/*glLineWidth( 0.5 );
	glBegin( GL_LINE_LOOP );
	glVertex3d( 0, 0, 0.0 );
	glVertex3d( width, 0, 0.0 );
	glVertex3d( width, height, 0.0 );
	glVertex3d( 0, height, 0.0 );
	glEnd(); */

	if( selected || hover ) {
		glColor3d( 0.4, 0.4, 0.2 );
	} else {
		glColor3d( 0.3, 0.3, 0.3 );
	}

	drawIcon();

	//crosshairs if selected
	glLineWidth( 2.0 );
	if( selected ) {
		glColor3d( 0.5, 0.2, 0.2 );

		double cLen = min( width, height ) / 4;
		glBegin( GL_LINES );
		//bottom left
		glVertex3d( 0, 0, 0.0 );
		glVertex3d( cLen, 0, 0.0 );

		glVertex3d( 0, 0, 0.0 );
		glVertex3d( 0, cLen, 0.0 );
		//bottom right
		glVertex3d( width, 0, 0.0 );
		glVertex3d( width - cLen, 0, 0.0 );

		glVertex3d( width, 0, 0.0 );
		glVertex3d( width,cLen, 0.0 );
		//top left
		glVertex3d( 0, height, 0.0 );
		glVertex3d( 0, height - cLen, 0.0 );

		glVertex3d( 0, height, 0.0 );
		glVertex3d( cLen, height, 0.0 );
		//top right
		glVertex3d( width, height, 0.0 );
		glVertex3d( width, height - cLen, 0.0 );

		glVertex3d( width, height, 0.0 );
		glVertex3d( width - cLen, height, 0.0 );
		glEnd();
	}
}