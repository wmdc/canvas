/*
   Constants related to drawing on the canvas.
   Kept in one spot so they can easily be tweaked.

   by Michael Welsman-Dinelle, 8 May 2009
*/

#pragma once

#include <ctime>
#include <GL/glut.h>


static const unsigned VIEWPORT_FLOOR_TEXTURE_SIZE     = 32;  //size of the checkerboard texture rendered onto the floor

static const GLdouble VIEWPORT_FOG_DENSITY            = 0.1; //density of fog in the viewport scenes

static const time_t HOVER_FADE_TIME                   = 240; //how quickly do items displayed on hover fade on the canvas?

static const GLfloat VIEWPORT_FOG_COLOR[4]        = { 0.4f, 0.4f, 0.4f, 1.0f }; //motion curve as it is sketched

static const GLdouble COLOR_GRIDLINE[4]               = { 0.85, 0.85, 0.85, 1.0 }; //grid lines on the canvas

static const GLdouble COLOR_CURVE_SKETCHING[4]        = { 0.6, 0.6, 0.8, 1.0 }; //motion curve as it is sketched
static const GLdouble COLOR_CURVE_BORDER[4]           = { 0.5, 0.5, 0.7, 1.0 }; //motion curve border once drawn
static const GLdouble COLOR_CURVE[4]                  = { 0.2, 0.2, 0.4, 1.0 }; //motion curve once drawn
static const GLdouble COLOR_CURVE_HIGHLIGHT[4]        = { 0.8, 0.8, 0.4, 1.0 }; //motion curve highlighted parts
static const GLdouble COLOR_CURVE_ARROW[4]            = { 0.4, 0.4, 0.6, 1.0 }; //motion curve 'preview' as it is dragged
static const GLdouble COLOR_CURVE_ARROW_HIGHLIGHT[4]  = { 0.6, 0.6, 0.4, 1.0 };
static const GLdouble COLOR_CURVE_HOVERING[4]         = { 0.4, 0.4, 0.6, 1.0 }; //motion curve non-highlighted part

static const GLdouble COLOR_CURVE_HOVER_POINT[4]      = { 0.9, 0.9, 0.5, 1.0 }; //hover point on curve

static const GLdouble COLOR_BORDER_GREY[4]            = { 0.2, 0.2, 0.2, 1.0 }; //semi-transparent grey borders
static const GLdouble COLOR_BORDER_LIGHT_GREY[4]      = { 0.3, 0.3, 0.3, 1.0 }; //
static const GLdouble COLOR_INSET_GREY[4]             = { 0.1, 0.1, 0.1, 0.4 }; //semi-transparent insets
static const GLdouble COLOR_INSET_LIGHT_GREY[4]       = { 0.8, 0.8, 0.8, 1.0 };
static const GLdouble COLOR_INSET_HIGHLIGHT[4]        = { 0.4, 0.1, 0.1, 1.0 };

static const GLdouble COLOR_PALETTE_DARK_O[4]         = { 0.2, 0.2, 0.2, 1.0 }; //opaque palette
static const GLdouble COLOR_PALETTE_LIGHT_O[4]        = { 0.4, 0.4, 0.4, 1.0 };

static const GLdouble COLOR_PALETTE_DARK[4]         = { 0.2, 0.2, 0.2, 0.6 }; //transparent palette
static const GLdouble COLOR_PALETTE_LIGHT[4]        = { 0.4, 0.4, 0.4, 0.6 };

static const GLdouble COLOR_INSET_TEXT[4]             = { 0.8, 0.8, 0.8, 1.0 };

static const GLdouble COLOR_MOTION_ICON[4]            = { 0.6, 0.4, 0.4, 1.0 }; //small animated figure
static const GLdouble COLOR_MOTION_ICON_FADE[4]       = { 0.6, 0.4, 0.4, 0.6 }; //animated figure (greyed out)

static const GLdouble COLOR_GRAPH_NODE[4]             = { 0.2, 0.2, 0.4, 1.0 }; //a link in the motion graph

static const GLdouble COLOR_POSE[4]                   = { 0.6, 0.6, 0.6, 0.6 };
static const GLdouble COLOR_POSE_HIGHLIGHT[4]         = { 0.9, 0.9, 0.2, 0.6 };

static const GLdouble COLOR_MOTION[4]				  = { 0.8, 0.8, 0.4, 0.8 };
static const GLdouble COLOR_MOTION_PATH[4]            = { 0.1, 0.4, 0.1, 0.5 };

static const GLdouble COLOR_TEXT_ANNOTATION[4]        = { 0.2, 0.2, 0.2, 0.8 };

static const GLdouble COLOR_VIEWPORT_BORDER[4]        = { 0.4, 0.4, 0.4, 0.6 };

static const GLdouble CURVE_ARROW_SIZE                = 20.0;
static const int CURVE_ARROW_SAMPLE                   = 10; //how frequently should the arrows be drawn?

static const GLdouble CURVE_LINE_WIDTH                = 10.0;
static const GLdouble CURVE_END_WIDTH                 = 16.0;

static const GLdouble CURVE_HOVER_LINE_WIDTH          = 6.0;
static const GLdouble CURVE_HOVER_END_WIDTH           = 16.0;

static const GLdouble VIEWPORT_CHAR_LINE_WIDTH        = 1.5;

static const GLdouble MOTION_ICON_LINE_WIDTH          = 1.5;

static const GLdouble MOTION_CURVE_BORDER_SIZE        = 2.0;

static void* MOTION_CURVE_FONT                        = GLUT_BITMAP_HELVETICA_12;
static void* MOTION_DETAILS_FONT                      = GLUT_BITMAP_8_BY_13;
static void *FONT_MOTION_BUTTON                       = GLUT_BITMAP_HELVETICA_10;

static const int COLOUR_SET_SIZE                      = 8;
static const double COLOUR_SET[ COLOUR_SET_SIZE ][3]  = {{ 1.0, 0.0, 0.0 },  //red
                                                         { 0.0, 0.0, 1.0 },  //blue
                                                         { 1.0, 1.0, 0.0 },  //yellow
                                                         { 0.0, 1.0, 0.0 },  //green
														 { 1.0, 0.6, 0.0 },  //orange
														 { 0.9, 0.6, 0.6 },  //pink
														 { 0.7, 0.7, 0.4 },  //brown
														 { 0.7, 0.7, 0.7 }}; //grey

//Icons for canvas control buttons - one for each canvas state
static const int BUTTON_NUM_TEXTURES = 10;
static char *BUTTON_TEXTURES[BUTTON_NUM_TEXTURES] = { "res/sketch.png", "", "", "", "", "res/annotate.png", "", "", "res/region.png", "res/text.png" };
static GLuint BUTTON_TEXID[BUTTON_NUM_TEXTURES];

static char *FLOOR_TEXTURE_PATH = "res/tile_gaus_sm.png";

static const int BUTTON_TEX_WIDTH = 40;
static const int BUTTON_TEX_HEIGHT = 40;

static void *CANVAS_TEXT_A_FONT = GLUT_BITMAP_HELVETICA_18;
static const int CANVAS_TEXT_A_HEIGHT = 18;

/*
   Draw a string on the canvas at the given position using the given font.
*/
void drawString( int x, int y, void *font, char *string );
/*
   Calls glColor, providing an appropriate unique colour based on the given integer value.
   This is a way of using "maximally distinguishable" colours.
*/
//void pickDiscreteSetColour( int c );

//deprecated
GLuint loadTexture( const char *filename, int width, int height, int wrap );