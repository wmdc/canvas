/*
   Viewport that can be placed anywhere
   on the animation canvas to view the models.

   by Michael Welsman-Dinelle
*/
#pragma once;

#include "CanvasItem.h"
#include "Point2D.h"
#include "Point3D.h"
#include "Motion.h"
#include "UtilityConstants.h"
#include "BVH.h"
#include "OrbitalCam.h"

#include <ctime>

class Canvas;

class CanvasViewport : public CanvasItem {
public:
	/* Viewport has a position and size on the canvas, a playback mode, and 
	   a reference to its parent canvas. */
	CanvasViewport( int ix, int iy, int iWidth, int iHeight, Canvas &c, bool details, bool fade ) : CanvasItem(c) {
		location = Point2D( ix, iy );
		width = iWidth;
		height = iHeight;

		playing = false;
		clip = NULL;
		pose = NULL;
        frame = 0;
		startFrame = 0;
		endFrame = 0;
		repeat = false;

		dragLeft = false;
		dragRight = false;
		dragTop = false;
		dragBottom = false;

		dragging = false;
		viewChange = false;
		distChange = false;

        mDetails = details;

		cam = OrbitalCam( VIEWPORT_INITIAL_CAMERA_ORBIT, VIEWPORT_INITIAL_CAMERA_PITCH, VIEWPORT_INITIAL_CAMERA_DISTANCE );

		tCreation = fade ? clock() : clock() - TIME_FADE_MS;
	}

	void setCamNumber( int index ) { cam.setColourIndex( index ); }
	void positionCam( double orbit, double pitch, double distance ) {
		cam.position( orbit, pitch, distance );
	}

	void setCam( OrbitalCam c ) {
		cam = c;
	}

	void animate( Motion &m, bool repeat = false ); //show an animation in the viewport
	void animate( Motion &m, bool repeat, unsigned startFrame, unsigned endFrame );
	void display( Motion &m, int frame ); //show a frame in the viewport
	void display( Pose &p );
	void draw();

	bool contains( Point2D p ) const;

	void mouseDown( int button, Point2D p, bool doubleClick );
	void mouseUp( int button, Point2D p );
	void mouseMove( Point2D p );
    //Add another cam to be rendered in this scene
	void addCam( OrbitalCam &c ) {
		mViews.push_back( &c );
	}
	OrbitalCam &getCam() { return cam; }

	//Identify this CanvasItem as a viewport control
	int type() const {
		return CanvasItem::TYPE_VIEWPORT;
	}

	Point2D getLocation() const { return location; }
	int getWidth() const { return width; }
	int getHeight() const { return height; }

	static const int BORDER_SIZE = 6;
	static const int MIN_WIDTH = 90;
	static const int MIN_HEIGHT = 90;

	static const int VIEWPORT_FOVY = 65;
	static const double FLOOR_SIZE;
	static const double FLOOR_TEXTURE_REPETITIONS;
	static const double FLOOR_DY; //a way to manually position the ground plane

	//Possible playback modes
	static const int MODE_BASIC_PLAYBACK = 0;
	static const int MODE_STATEFUL_PLAYBACK = 1;

	static const int INVALID_FLOOR_TEXTURE = -1;

	static const int VIEWPORT_NO_FRAME = -1;

	static const double ORBIT_DRAG_SCALE;
	static const double PITCH_DRAG_SCALE;
	static const double DIST_DRAG_SCALE;

	static const double MODEL_SCALE_FACTOR;

	static const double VIEWPORT_INITIAL_CAMERA_DISTANCE;
	static const double VIEWPORT_INITIAL_CAMERA_ORBIT;
	static const double VIEWPORT_INITIAL_CAMERA_PITCH;
private:
	static const double TIME_FADE_MS;

	Point2D location;
	int width, height;

	static GLuint viewportDLFloor; //shared display list for the viewport floor

	Point2D dragPoint; //where the cursor began before the drag

	bool dragLeft, dragRight, dragTop, dragBottom;
	bool dragging, viewChange, distChange;

	Motion *clip;
	Pose *pose;

	bool playing;
	bool repeat;
	int frame, startFrame, endFrame;
    time_t tPlayback;

	time_t tCreation;

    bool mDetails; //display frame, clip name, etc?

	OrbitalCam cam;
	std::vector<OrbitalCam *> mViews; //show viewpoints of other viewports
};