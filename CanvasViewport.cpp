#include "precompiled.h"

#include <iostream>
#include <cmath>
#include <sstream>

#include "UtilityConstants.h"
#include "DrawingConstants.h"
#include "CanvasViewport.h"
#include <GL/glut.h>
#include "BVH.h"
#include "Canvas.h"

#include "IL/ilut.h"

const double CanvasViewport::ORBIT_DRAG_SCALE = 0.01;
const double CanvasViewport::PITCH_DRAG_SCALE = 0.01;
const double CanvasViewport::DIST_DRAG_SCALE = 0.25;
const double CanvasViewport::MODEL_SCALE_FACTOR = 0.1;

const double CanvasViewport::VIEWPORT_INITIAL_CAMERA_DISTANCE = 50.5;
const double CanvasViewport::VIEWPORT_INITIAL_CAMERA_ORBIT = 1.0;
const double CanvasViewport::VIEWPORT_INITIAL_CAMERA_PITCH = 0.35;

const double CanvasViewport::FLOOR_SIZE = 100.0;
const double CanvasViewport::FLOOR_TEXTURE_REPETITIONS = 35.0;
const double CanvasViewport::FLOOR_DY = -5.0; //a way to manually tune the ground plane location

const double CanvasViewport::TIME_FADE_MS = 350.0;

GLuint CanvasViewport::viewportDLFloor = 0; //no texture assigned

void CanvasViewport::animate( Motion &m, bool repeatClip ) {
    clip = &m;
	tPlayback = clock();
	repeat = repeatClip;

	frame = 0;
	startFrame = 0;
	endFrame = clip->getFrameCount() - 1;

	playing = true;
	pose = NULL;
}

void CanvasViewport::animate( Motion &m, bool repeatClip, unsigned _startFrame, unsigned _endFrame ) {
    clip = &m;
	tPlayback = clock();
	repeat = repeatClip;

	if( _startFrame < 0 ) _startFrame = 0;
	if( _startFrame >= clip->getFrameCount() ) _startFrame = clip->getFrameCount();
    if( _endFrame < _startFrame ) _endFrame = _startFrame;

	frame = _startFrame;
    startFrame = _startFrame;
	endFrame = _endFrame;

	playing = true;
	pose = NULL;
}

void CanvasViewport::display( Motion &m, int mFrame ) {
	playing = false;
	clip = &m;
	frame = mFrame;
	pose = NULL;
}

void CanvasViewport::display( Pose &p ) {
	pose = &p;
	playing = false;
}

void CanvasViewport::draw() {
	time_t dts = clock() - tCreation;

	//Draw the viewport border
	glLineWidth(3.0);

	glColor4dv( COLOR_VIEWPORT_BORDER );
	glBegin( GL_QUADS );
	  glVertex2i( location.x + 1, location.y );
	  glVertex2i( location.x + width - 1, location.y );
	  glVertex2i( location.x + width - 1, location.y + height );
	  glVertex2i( location.x + 1, location.y + height );
	glEnd();

	//Draw the inside
	dts > TIME_FADE_MS ? glColor3d( 0.0, 0.0, 0.0 ) : glColor4d( 0.0, 0.0, 0.0, 1.0 * (dts / TIME_FADE_MS) );
	glBegin( GL_QUADS );
	  glVertex2i( location.x + BORDER_SIZE, location.y + BORDER_SIZE );
	  glVertex2i( location.x + width - BORDER_SIZE, location.y + BORDER_SIZE );
	  glVertex2i( location.x + width - BORDER_SIZE, location.y + height - BORDER_SIZE );
	  glVertex2i( location.x + BORDER_SIZE, location.y + height - BORDER_SIZE );
	glEnd();

	//glEnable( GL_FOG );
	glFogf( GL_FOG_DENSITY, VIEWPORT_FOG_DENSITY );
	glFogfv( GL_FOG_COLOR, VIEWPORT_FOG_COLOR );

	//Need to change the viewport and scissor params then restore them later
	GLfloat projM[16]; //will also need to restore the projection
	glGetFloatv( GL_PROJECTION_MATRIX, projM );

	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	gluPerspective( VIEWPORT_FOVY, (float)width/(float)height, 0, 100 );

	glMatrixMode(GL_MODELVIEW);

	GLint vPort[4];
	glGetIntegerv( GL_VIEWPORT, vPort ); 
	glViewport( location.x + BORDER_SIZE, location.y + BORDER_SIZE, width - BORDER_SIZE * 2, height - BORDER_SIZE * 2 );

	double dt = ( clock() - tPlayback ) / 1000.0;

	if( playing && clip != NULL ) {
		frame = startFrame + clip->getFrameRate() * dt;
		if( frame > endFrame ) {
			if( repeat ) {
				tPlayback = clock();
			}
			frame = endFrame;
		}
	}
	
	glPushMatrix(); {
		cam.applyViewTransform();

		//Draw the floor
		if( glIsList( viewportDLFloor ) ) {
			glCallList( viewportDLFloor );
		} else {
			//Generate the display list and load the texture
			viewportDLFloor = glGenLists(1);
			glNewList(viewportDLFloor, GL_COMPILE_AND_EXECUTE ); {
				glEnable (GL_TEXTURE_2D);
				//glTexEnvi( GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_DECAL );
				glBindTexture( GL_TEXTURE_2D, ilutGLLoadImage( FLOOR_TEXTURE_PATH ));

				glColor4d( 1.0, 1.0, 1.0, 1.0 );

				glBegin( GL_QUADS ); {
					glTexCoord2d( 0.0, 0.0 );
					glVertex3d( -FLOOR_SIZE, FLOOR_DY, -FLOOR_SIZE );
					glTexCoord2d( 0.0, FLOOR_TEXTURE_REPETITIONS );
					glVertex3d( -FLOOR_SIZE, FLOOR_DY, FLOOR_SIZE );
					glTexCoord2d( FLOOR_TEXTURE_REPETITIONS, FLOOR_TEXTURE_REPETITIONS );
					glVertex3d( FLOOR_SIZE, FLOOR_DY, FLOOR_SIZE );
					glTexCoord2d( FLOOR_TEXTURE_REPETITIONS, 0 );
					glVertex3d( FLOOR_SIZE, FLOOR_DY, -FLOOR_SIZE );
				} glEnd();

				glDisable(GL_TEXTURE_2D);

			} glEndList();
		}
		//Draw the model

		glPushMatrix(); {
			glScaled( MODEL_SCALE_FACTOR, MODEL_SCALE_FACTOR, MODEL_SCALE_FACTOR );
			if( clip && !pose ) {
				dt > clip->getRuntime() ? glColor3d( 0.8, 0.8, 0.4 ) : glColor3d( 1.0, 1.0, 0.0 );
			} else {
				glColor3d( 1.0, 1.0, 0.0 );
			}
			glLineWidth( 4.0f );

			if( pose ) {
				pose->draw();
			} else if( clip ) {
				clip->draw( frame );
			}
		} glPopMatrix();
	} glPopMatrix();

	glMatrixMode(GL_PROJECTION); //restore old projection
	glLoadIdentity();
	glMultMatrixf( projM );
	glMatrixMode(GL_MODELVIEW);
	glViewport( vPort[0], vPort[1], vPort[2], vPort[3] );
	glDisable( GL_FOG );

	//Draw model details (text with current frame etc.)
	if( !mDetails || clip == NULL ) {
		return;
	}

	glEnable( GL_SCISSOR_TEST );
	glScissor( location.x + BORDER_SIZE, location.y + BORDER_SIZE, width - BORDER_SIZE * 2, height - BORDER_SIZE * 2 );
	
	std::stringstream mTime;// << clip->getFrameCount();

    mTime << clip->getFrameCount();
	int maxWidth = mTime.str().length();
	mTime.str("");

	mTime.precision( 2 );
	mTime.width( maxWidth );
	mTime.setf(std::ios::fixed);
	mTime.setf(std::ios::right );
	mTime.fill('0');
	mTime << ( frame + 1 ) << " / " << clip->getFrameCount() << " frames ";
	mTime.setf(std::ios::left );
	mTime << " - " << ( (double)( frame + 1 ) / clip->getFrameCount())*clip->getRuntime() << " s";
	mTime.str().length();

	int renderLengthD = glutBitmapLength( MOTION_DETAILS_FONT, (const unsigned char *)mTime.str().c_str() );
	int renderLengthL = glutBitmapLength( MOTION_DETAILS_FONT, (const unsigned char *)clip->getLabel().c_str() );

	glColor4dv( COLOR_INSET_GREY );
	glBegin( GL_QUADS );
	  glVertex2i( location.x + 2* BORDER_SIZE, location.y + 2* BORDER_SIZE );
	  glVertex2i( location.x + 4* BORDER_SIZE + renderLengthD, location.y + 2* BORDER_SIZE );
	  glVertex2i( location.x + 4* BORDER_SIZE + renderLengthD, location.y + 4* BORDER_SIZE + 8 );
	  glVertex2i( location.x + 2* BORDER_SIZE, location.y + 4* BORDER_SIZE + 8 );

      glVertex2i( location.x + 2* BORDER_SIZE, location.y + height - 2 * BORDER_SIZE );
	  glVertex2i( location.x + 4* BORDER_SIZE + renderLengthL, location.y + height - 2* BORDER_SIZE );
	  glVertex2i( location.x + 4* BORDER_SIZE + renderLengthL, location.y + height - 4* BORDER_SIZE - 8 );
	  glVertex2i( location.x + 2* BORDER_SIZE, location.y + height - 4* BORDER_SIZE - 8 );
	glEnd();
	glColor4dv( COLOR_INSET_TEXT );
	drawString( location.x + 3* BORDER_SIZE, location.y + 3* BORDER_SIZE, MOTION_DETAILS_FONT, (char *)mTime.str().c_str() );
	drawString( location.x + 3* BORDER_SIZE, location.y + height - 3* BORDER_SIZE - 8, MOTION_DETAILS_FONT, (char *)clip->getLabel().c_str() );

	glDisable( GL_SCISSOR_TEST );
}

bool CanvasViewport::contains( Point2D p ) const {
	return p > location && p < Point2D( location.x + width, location.y + height );
}

void CanvasViewport::mouseDown( int button, Point2D p, bool doubleClick ) {
	dragPoint = p;

	if( button == GLUT_LEFT_BUTTON ) { //drag
		if( doubleClick ) {
			CanvasViewport *v = new CanvasViewport( p.x - width / 4, p.y - height / 4, width / 2, height / 2, canvas, false, true );
			v->setCam( cam );
			canvas.addViewport( v );
		} else {
			dragging = true;
			p.x - location.x < BORDER_SIZE ? dragLeft = true : dragLeft = false;
			p.y - location.y > height - BORDER_SIZE ? dragTop = true : dragTop = false;
			p.x >= location.x + width - BORDER_SIZE ? dragRight = true : dragRight = false;
			p.y - location.y < BORDER_SIZE ? dragBottom = true : dragBottom = false;
		}
	} else if( button == GLUT_RIGHT_BUTTON ) { //view
		viewChange = true;
	} else if( button == GLUT_MIDDLE_BUTTON ) {
		distChange = true;
	}
}
void CanvasViewport::mouseUp( int button, Point2D p ) {
	dragging = false;
	viewChange = false;
	distChange = false;

}
/* Dragging the viewport, resizing, or zoom/pan/view inside */
void CanvasViewport::mouseMove( Point2D p ) {
	if( dragging ) {
		if( !dragLeft && !dragRight && !dragTop && !dragBottom ) {
			location += p - dragPoint;
			dragPoint = p;
			return;
		}

		float dx = p.x - dragPoint.x;
		float dy = p.y - dragPoint.y;

		if( dragLeft ) {
			width -= dx;
			location.x += dx;
		} else if( dragRight ) {
			width += dx;
		}

		if( dragTop ) {
			height += dy;
		} else if( dragBottom ) {
			height -= dy;
		    location.y += dy;
		}

		if( width < MIN_WIDTH ) width = MIN_WIDTH;
		if( height < MIN_HEIGHT ) height = MIN_HEIGHT;
	} else if( viewChange ) {
		cam.move( -ORBIT_DRAG_SCALE * ( dragPoint.x -  p.x ), PITCH_DRAG_SCALE * ( dragPoint.y -  p.y ), 0.0 );
	} else if( distChange ) {
		cam.move( 0.0, 0.0, ( dragPoint.y - p.y ) * DIST_DRAG_SCALE );
	}
	dragPoint = p;
}