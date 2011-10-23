/*
 * Main.cpp
 *
 * Entry point and some general methods built around the animation canvas
 * (Canvas) class.  Handles OpenGL initialization and I/O.
 *
 * by Michael Welsman-Dinelle
 */

#include "precompiled.h"

#include <windows.h>
//In Visual Studio, add GLUT_BUILDING_LIB to the preprocessor definitions
#include <GL/glut.h>

#include <iostream>
#include <istream>
#include <vector>
#include <ctime>

#include "BVH.h"
#include "Canvas.h"
#include "Motion.h"
#include "WinIO.h"
#include "Point2D.h"
#include "MotionCurve.h"
#include "MotionBrush.h"
#include "ControlBrush.h"
#include "BrushPalette.h"
#include "DrawingConstants.h"
#include "UtilityConstants.h"
#include "ControlBrush.h"

#include "IL/ilut.h"

/* Display parameters */
float Fovy = 20.0;          // field of view  in the y-direction (in degrees)
int screenWidth = 1200;      // window width (pixels)
int screenHeight = 800;     // window height (pixels)
float zNear = 1.0;          // near clipping plane (distance from camera)
float zFar = 30.0;          // far clipping plane (distance from camera)

/* Picking and screen coordinate conversion */
GLdouble modelM[16];
GLdouble projM[16];
int viewport[4];

Canvas *canvas;

time_t lastClick = 0;               //handle double click events
int DOUBLE_CLICK_DELAY = 200;

void loadMotions();
Point2D screenToObjCoords( int x, int y ); //coord conversion

void generateTextures(); //generate and load textures
unsigned VIEWPORT_FLOOR_TEX_ID;

/*
   Load all of the bvh files into a vector of motions.
*/
vector<Motion *> loadFileMotions() {
	FILE *fp;
    vector<string> files = WinIO::getDirectoryFilenames( string("bvh"), string("*.bvh") );

	vector<Motion *> sequences;

	for( unsigned i = 0; i < files.size(); i++ ) {
		fopen_s( &fp, files.at(i).c_str(), "rt" );

		Motion *m = new Motion( *BVHReader::read( fp ) );

		static const int trashFrames = 5; //toss the first 5 frames

		if( m && m->getFrameCount() > trashFrames )
		{
			Motion *m_clip = new Motion( *m, 5, m->getFrameCount() - 1 );

			//delete m; //<-- this should work!

			string n = files.at(i);
			n.erase( n.begin(), n.begin() + 4 );
			n.erase( n.end() - 4, n.end() );
			m_clip->setLabel( n );

			sequences.push_back( m_clip );
		}
	}

	return sequences;
}

/* 
 * Draw the canvas.
 */
void display(void)
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
	canvas->draw();

	glutSwapBuffers();
}
/*
   Convert (x,y) screen coordinates to (x,y,0) in object
   coordinates
*/
Point2D screenToObjCoords( int x, int y ) {
	GLdouble pos[3];

	gluUnProject( x, screenHeight - y, 0, modelM, projM, viewport,
		          &pos[0], &pos[1], &pos[2] );

	return Point2D( pos[0], pos[1] );
}
/* 
   Handle keyboard input. 
*/
void keystroke(unsigned char key, int x, int y)
{
	canvas->keystroke( key );

	glutPostRedisplay();
}
/*
   Called when the mouse is clicked.
*/
void click( int button, int st, int x, int y ) {
	if( st == GLUT_DOWN ) {
		if( clock() <= lastClick + DOUBLE_CLICK_DELAY ) {
		    canvas->mouseDown( button, screenToObjCoords( x, y ), true );
		} else {
			canvas->mouseDown( button, screenToObjCoords( x, y ), false );
		}
		lastClick = clock();
	} else if( st == GLUT_UP ) {
		canvas->mouseUp( button, screenToObjCoords( x, y ) );
	}

	glutPostRedisplay();
}
/*
   Called when the mouse is dragged.
*/
void drag( int x, int y ) {
	canvas->mouseMove( screenToObjCoords( x, y ) );

	glutPostRedisplay();
}
void mouseMove( int x, int y ) {
	canvas->mouseMove( screenToObjCoords( x, y ) );

	glutPostRedisplay();
}
/*
   Called when the window is resized.
*/
void resize(int w, int h)
{
	screenHeight = h;
	screenWidth = w;

	glViewport( 0, 0, w, h );

	viewport[0] = 0;
	viewport[1] = 0;
	viewport[2] = w;
	viewport[3] = h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	//gluOrtho2D( 0, 0, w, h );
    glOrtho(0, w, 0, h, -100, 100);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	/* Save some state information for when we need to do picking */
	glGetDoublev(GL_MODELVIEW_MATRIX,modelM);
	glGetDoublev(GL_PROJECTION_MATRIX,projM);
	glGetIntegerv(GL_VIEWPORT,viewport);
}
/*
   Idle function.
*/
void idle( void ) {
    glutPostRedisplay();
}
/*
   Entry point.
*/
int main(int argc, char **argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
   glutInitWindowSize(screenWidth, screenHeight);
   glutCreateWindow("");

   glutDisplayFunc( display );
   glutMotionFunc( drag );

   glClearColor (0.99, 0.99, 0.99, 1.0);

   glutKeyboardFunc( keystroke );
   glutMouseFunc( click );
   glutPassiveMotionFunc( mouseMove );
   glutReshapeFunc( resize );

   glutInitWindowSize( screenHeight, screenHeight );

   //glutSetCursor( GLUT_CURSOR_CROSSHAIR );

   glutIdleFunc( idle );
   
   glEnable(GL_BLEND); 
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
   glEnable( GL_POINT_SMOOTH );
   glEnable( GL_LINE_SMOOTH );

   glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

   generateTextures(); //checkerboard for floor etc.

   /* Initalize the animation canvas. */
   canvas = new Canvas( loadFileMotions() );

   glutMainLoop();

   return 0;
}

void generateTextures() {
	//checkerboard floor texture for canvas viewport
	unsigned textureBytes = 4 * VIEWPORT_FLOOR_TEXTURE_SIZE * VIEWPORT_FLOOR_TEXTURE_SIZE;
	GLubyte *textureData = new GLubyte[ textureBytes ]; /*[] = { 0x00, 0x00, 0x00, 0xCC,
		                      0xFF, 0xFF, 0xFF, 0xCC,
                              0xFF, 0xFF, 0xFF, 0xCC,
							  0x00, 0x00, 0x00, 0xCC };*/

	for( unsigned row = 0; row < VIEWPORT_FLOOR_TEXTURE_SIZE; row++ ) {
		for( unsigned col = 0; col < VIEWPORT_FLOOR_TEXTURE_SIZE; col++ ) {
			GLubyte tileColour = 0x00;
			if( row > VIEWPORT_FLOOR_TEXTURE_SIZE/2 && col > VIEWPORT_FLOOR_TEXTURE_SIZE/2 ||
				row <= VIEWPORT_FLOOR_TEXTURE_SIZE/2 && col <= VIEWPORT_FLOOR_TEXTURE_SIZE/2) {
					tileColour = 0xFF;
			}
			textureData[ 4 * row * VIEWPORT_FLOOR_TEXTURE_SIZE + 4 * col ] = tileColour;
			textureData[ 4 * row * VIEWPORT_FLOOR_TEXTURE_SIZE + 4 * col +1] = tileColour;
			textureData[ 4 * row * VIEWPORT_FLOOR_TEXTURE_SIZE + 4 * col +2] = tileColour;
			textureData[ 4 * row * VIEWPORT_FLOOR_TEXTURE_SIZE + 4 * col +3] = tileColour;
		}
	}

	glHint( GL_FOG_HINT, GL_NICEST );

    glGenTextures( 1, &VIEWPORT_FLOOR_TEX_ID );
	glBindTexture( GL_TEXTURE_2D, VIEWPORT_FLOOR_TEX_ID );

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);//GL_NEAREST_MIPMAP_LINEAR);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	gluBuild2DMipmaps( GL_TEXTURE_2D, 4, VIEWPORT_FLOOR_TEXTURE_SIZE, VIEWPORT_FLOOR_TEXTURE_SIZE, GL_RGBA, GL_UNSIGNED_BYTE, textureData );

	//glTexImage2D(GL_TEXTURE_2D, 0, 4, VIEWPORT_FLOOR_TEXTURE_SIZE, VIEWPORT_FLOOR_TEXTURE_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData );

	delete textureData;

	//control button textures
	ilInit();
	
	ilutRenderer(ILUT_OPENGL);
	ilutEnable(ILUT_OPENGL_CONV);

	ControlBrush::initTextures();
}