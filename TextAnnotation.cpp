#include "precompiled.h"

#include "TextAnnotation.h"

#include "DrawingConstants.h"

void TextAnnotation::keystroke( unsigned char key ) {
	if( key == '\n' || key == '\r' ) {
		active = false;
	}

	if( active ) {
		if( ( key == 8 || key == 127 ) && !text.empty() ) { //backspace
			width -= glutBitmapWidth( font, text.data()[ text.length() - 1 ] );
			text = text.substr( 0, text.length() - 1 );
		} else {
			text.append( 1, key );
			width += glutBitmapWidth( font, key );
		}
	}
}

void TextAnnotation::draw() const {
	glColor4dv( COLOR_TEXT_ANNOTATION );

	//draw a cursor
	glLineWidth( 2.0 );
	if( active ) {
		glPushMatrix(); {
			glBegin(GL_LINES); {
				glVertex2d( location.x + width, location.y );
				glVertex2d( location.x + width, location.y + height );
			} glEnd();
		} glPopMatrix();
	}
    //draw the text
	glRasterPos2d( location.x, location.y );

	for( size_t index = 0; index < text.size(); index++ ) {
        glutBitmapCharacter( font, text.at(index) );
	}
}