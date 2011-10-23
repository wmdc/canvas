/*
   A 2-D Canvas control class.  Can be controlled through GLUT
   callbacks, keeps track of strokes created, and draws what the user draws.

   by Michael Welsman-Dinelle
   2008-2010
*/

#pragma once

#include <vector>
#include <ctime>
#include <iostream>

#include "CanvasItem.h"
#include "Point2D.h"
#include "CanvasViewport.h"
#include "BrushPalette.h"
#include "MotionBrush.h"
#include "Annotation.h"
#include "ControlBrush.h"
#include "SKFRegion.h"

class Gesture;
class MotionCurve;
class Motion;
class Brush;
class Path;
class Pose;
class TextAnnotation;

using namespace std;

static const float CANVAS_SCALE_RATIO = 20.0f; //how much length to drag for doubling or halving?
static const float MAX_DRAG = 80.0f;

static const float NEIGHBOUR_RADIUS = 10.0f; //how close before controls are "neighbours"?

class Canvas {
public:
	/* Mouse input types */
	static const int MOUSE_DOWN = 0;
	static const int MOUSE_UP = 1;
	static const int MOUSE_DRAG = 2;
	static const int MOUSE_MOVE = 3;
	static const int MOUSE_DOUBLE_CLICK = 4;
	/* Interaction modes */
	static const int MODE_SKETCH = 0;
    static const int MODE_ANIMATE = 1;
	static const int MODE_MANIPULATE = 2;
	static const int MODE_STITCH = 3;
	static const int MODE_ZOOMPAN = 4;
	static const int MODE_ANNOTATE = 5;
	static const int MODE_LINK = 6;
	static const int MODE_MOTION_SKETCH = 7;
	static const int MODE_SKF = 8;
	static const int MODE_TEXT = 9;

	static const int MODE_COUNT = 10;
	
	Canvas( vector<Motion *> initialMotions );

	/* Set the interaction mode */
	void setMode( int mode_ ) {
		mode = mode_;
        //THIS IS HACKISH -> could implement a more general solution
		if( mode == Canvas::MODE_SKETCH ) {
			controlPalette->displaySelected( 2 );
		}
	}
	/* Get the interaction mode */
	int getMode() const {
		return mode;
	}
	/* Set the motion clip associated with new curves drawn */
	void setDrawingMotion( Motion *clip ) {
		if( clip ) {
		    drawingMotion = clip;
		}
	}
	void addMotion( Motion *m, bool flash = false ) {
		if( m != NULL ) {
			motions.push_back( m );

			MotionBrush *b = new MotionBrush( *m, false, *this, *motionPalette );
			motionPalette->add( b, flash );
		} else {
			printf("Warning: attempted to add a null motion to the canvas.\n");
		}
	}
	size_t getMotionCount() const {
		return motions.size();
	}
	Motion *getMotion( int index ) {
		return motions[index];
	}
	Motion *getDrawingMotion() const {
		return drawingMotion;
	}
	/* Begin an animation */
	void animate( Motion &clip, bool repeat = false );
	void animate( Motion &clip, bool repeat, int startFrame, int endFrame );
	
	void pause();

	/* Show a single frame of an animation. */
	void displayFrame( Motion &clip, int frame );
    void displayPose( Pose &p );

    /* Add an item to the canvas. */
	void addElement( CanvasItem *item );

	void addRegion( SKFRegion *region );
	unsigned getRegionCount() const { return regions.size(); }
	SKFRegion *getRegion( int index ) const { return regions[index]; }

	void addViewport( CanvasViewport *viewport );

	void setMotionPalette( BrushPalette *palette ) {
		motionPalette = palette;
	}

	void beginSketchingCurve( MotionCurve *parent, Point2D start );
	MotionCurve *getDrawingCurve() { return drawingCurve; }

	bool removeElement( CanvasItem *item ) {
		for( unsigned i = 0; i < elements.size(); i++ ) {
			if( elements[i] == item ) {
				elements.erase( elements.begin() + i );
				return true;
			}
		}
		return false;
	}

	/* Get an item from the canvas. */
	CanvasItem *getElement( int index ) {
		return elements[index];
	}
	size_t getElementCount() {
		return elements.size();
	}
	/* Mouse/stylus handling */
	void mouseDown( int button, Point2D p, bool doubleClick );
	void mouseUp( int button, Point2D p );
	void mouseMove( Point2D p );

	bool isDragging() { return dragging; }

	void flagMouseOver() { mouseOverFound = true; } // a control responded to mouse over from mouseMove event
	bool gotMouseOver() const { return mouseOverFound; }

	Point2D getLastMouseDown() const {
		return pLast;
	}

	void keystroke( unsigned char key );
	
	/* Redraw the canvas */
	void draw();

	/* See if the given pose is positioned s.t. it should be inserted into an
	 * existing motion curve. */
	void insertPose( Pose *p );

	/*
	 * Check each region for contains(item).  If yes, add it.  If no, remove it.
	 */
	void updateRegions( Pose *p );


	void addDisplayPose( Pose *p, float alpha = 1.0f ) {
		displayPoses.push_back( p );
		displayPoseAlphas.push_back( alpha );
	}
	bool removeDisplayPose( Pose *p ) {
		return false;
	}

	static const double SKETCHING_MOTION_SCALE;

	static const int GRID_SPACING_PIXELS = 40;
private:
	//current mode of the canvas
	int mode;

	bool dragging, annotating, sketching, panning;

	bool mouseOverFound; // has something "contained" the last mousemove event?
    //drawable elements
	vector<CanvasItem*> elements;
	vector<SKFRegion *> regions;
	vector<Annotation*> annotations;
	vector<TextAnnotation*> textAnnotations;
	vector<CanvasViewport *> viewports;

	//displayed elements
	vector<Pose *> displayPoses;
	vector<float>  displayPoseAlphas;

	BrushPalette *motionPalette;
	BrushPalette *controlPalette;
    //list of motions to be processed using the canvas
	vector<Motion *> motions;
	Motion *drawingMotion;
    
	MotionCurve *drawingCurve; //motion curve being drawn
	SKFRegion *drawingRegion;  //spatial keyframing region being drawn

	bool playback;             //are we playing back a motion clip?
	Motion *playbackClip;
	Point2D *playbackOrigin;
	int playbackFrame;
	clock_t startTime;

	Point2D dOrigin, pLast;

	Path *curPath;

	CanvasItem *dragElement;

	//displays a grid of lines on the canvas
	void drawGrid();
	GLuint dlGrid;
};