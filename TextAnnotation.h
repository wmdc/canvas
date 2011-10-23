/*
   Text annotation class for the Animation Canvas.

   by Michael Welsman-Dinelle 10/02/2010
*/

#pragma once

#include <GL/glut.h>
#include <string>

#include "Point2D.h"

class TextAnnotation {
public:
	/* Construct a new text annotation field at the insertion point
	   to be rendered by the given GLUT font, e.g. GLUT_BITMAP_8_BY_13 */
	TextAnnotation( void *glutFont, int fontHeight, Point2D insertionPoint ) {
		font = glutFont;
		height = fontHeight;
		width = 0;

		active = true;
		text = "";
		location = insertionPoint;
	}
    /* Add another character to the annotation, or terminate editing
	   when a return or enter key is sent. */
	void keystroke( unsigned char key );
	void setActive( bool isActive ) {
		active = isActive;
	}
    /* Display the text annotation. */
	void draw() const;

	std::string text;
private:
	bool active;
	void *font;

	unsigned height, width;

	Point2D location;
};