#include "precompiled.h"

#include "ControlBrush.h"
#include "Brush.h"
#include "Canvas.h"

#include "DrawingConstants.h"

#include <GL/glut.h>

#include "IL/ilut.h"

ControlBrush::ControlBrush( int _mode, bool _selected, Canvas &_c, BrushPalette &_p ) :
Brush( _selected, _c, _p ), mode(_mode) {}

void ControlBrush::select() {
	selected = true;
	canvas.setMode( mode );
	printf("controlbrush select\n");
}

void ControlBrush::deselect() {
	selected = false;
}

void ControlBrush::drawIcon() const {
	//draw the appropriate icon
	if( mode >= 0 && mode < BUTTON_NUM_TEXTURES ) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, BUTTON_TEXID[mode]);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		glBegin(GL_QUADS);
		  glTexCoord2d( 0.0, 0.0 );
		  glVertex2i( 1, 1 );
		
		  glTexCoord2d( 1.0, 0.0 );
		  glVertex2i( width - 1, 1 );
		
		  glTexCoord2d( 1.0, 1.0 );
		  glVertex2i( width - 1, height - 1 );
		 
		  glTexCoord2d( 0.0, 1.0 );
		  glVertex2i( 1, height - 1 );
		glEnd();
		glDisable(GL_TEXTURE_2D);
	}
}

void ControlBrush::initTextures() {
	for( int i = 0; i < BUTTON_NUM_TEXTURES; i++ ) {
        BUTTON_TEXID[i] = ilutGLLoadImage(BUTTON_TEXTURES[i]);
	}
}