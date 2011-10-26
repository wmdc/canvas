#include "precompiled.h"

#include "MotionBrush.h"
#include "Canvas.h"
#include "Brush.h"

#include <GL/glut.h>
#include <iostream>

MotionBrush::MotionBrush( Motion &_m, bool _selected, Canvas &_c, BrushPalette &_p ) :
    Brush( _selected, _c, _p ), m(_m) {
		aniT = 0;
}

void MotionBrush::drawIcon() const {
	//turn on scissor box so we draw online inside the button
	//get the translation for this button so we know where to put it on screen




	GLfloat mTrans[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, mTrans);

	//find intersection of existing scissor and the current box
	/*float box[4];
	glGetFloatv(GL_SCISSOR_BOX, box);

	float intersectWidth = box[0] + box[2] - mTrans[12];
	float intersectHeight = box[1] + box[3] - mTrans[13];

	if(intersectWidth > width) intersectWidth = width;
	if(intersectHeight > height) intersectHeight = height;

	if(intersectWidth == 0 || intersectHeight == 0)
		return;*/

	//glPushAttrib(GL_SCISSOR_TEST);
	//glScissor(mTrans[12], mTrans[13], width, height);

	//glEnable(GL_SCISSOR_TEST);

	//draw the motion
	glLineWidth(2.0);
	glColor3d( 0.8, 0.8, 0.2 );
	glPushMatrix(); {
		static const double SCALE_FACTOR = 0.36;

		double minDim = min( width, height );

		glTranslated( width / 2, 3 * height / 5, 0 );
		glScaled( SCALE_FACTOR * minDim / 12, SCALE_FACTOR * minDim / 12, 0.0 );

		if( hover ) { //animate - what frame?
			m.drawPose(m.getFrame( clock() - aniT ));
		} else { //draw just one frame
			m.drawPose(m.getFrameCount() / 2);
		}
	} glPopMatrix();
	

	//draw the motion label
    glColor4dv(COLOR_INSET_TEXT);
	char label[ LABEL_SIZE + 1 ];
	char labelLen[ LABEL_SIZE + 1 ];

	strncpy_s( label, LABEL_SIZE + 1, m.getLabel().c_str(), LABEL_SIZE );
	sprintf_s( labelLen, LABEL_SIZE + 1, "%5.1f s", m.getRuntime() );

	drawString( 10, height - 10, FONT_MOTION_BUTTON, label );
    drawString( 2, 2, FONT_MOTION_BUTTON, labelLen );

	//glDisable(GL_SCISSOR_TEST);

	//glPopAttrib();
}

/* Override */
void MotionBrush::select() {
	selected = true;
	canvas.setDrawingMotion( &m );
	canvas.setMode( Canvas::MODE_SKETCH );
}

void MotionBrush::deselect() { selected = false; }

void MotionBrush::mouseDown(int button, Point2D p, bool doubleClick)
{
	if( button == CanvasItem::MOUSE_BTN_LEFT)
	{
		//select motion brush
		Brush::mouseDown(button, p, doubleClick);
	}
	else if(button == CanvasItem::MOUSE_BTN_RIGHT)
	{
		//play back the motion
		canvas.animate( m, false );
	    
	}
}

/* Must also start animation on hover. */
void MotionBrush::mouseMove( Point2D p ) {
	bool mouseOver = contains( p );

	if( !hover && mouseOver ) {
		hover = true;
		aniT = clock();
	}
	else if( !mouseOver ) {
		hover = false;
	}
}

void MotionBrush::draw() {
	//draw the back panel
	glColor4dv( selected ? COLOR_INSET_HIGHLIGHT : COLOR_INSET_GREY );
	glBegin( GL_POLYGON ); {
		glVertex2i( 0, 0 );
		glVertex2i( width, 0 );
		glVertex2i( width, height );
		glVertex2i( 0, height );
	} glEnd();
    //draw the motion icon
	drawIcon();
}