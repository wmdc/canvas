#include "CanvasRegion.h"
#include "Point2D.h"

#include <vector>
#include <ctime>

class MotionInterpolationRegion : public CanvasRegion {
public:
	public MotionInterpolationRegion( Canvas &c ) : CanvasRegion( c ) {
		dragging = false;
		dragIndex = 0;

		interpolating = false;

		curMotion = NULL;

		startTime = 0;
	}
	void draw() {
		CanvasRegion::draw(); //draw the region outline

		time_t dt = interpolating ? clock() - startTime : 0;

		//draw the motions
		for( unsigned i = 0; i < motions.size(); i++ ) {
			motions[i]->draw()
		}
	}

	void mouseDown( int button, Point2D p, bool doubleClick ) {
        //add the motion to the region as an interpolation point
		if( canvas.getMode() == Canvas::MODE_ANIMATE && canvas.getMotion() ) {
			motions.push_back( canvas.getMotion() );
			motionPoints.push_back( p );
			dragIndex = motionPoints.size() - 1;
			dragging = true;
		} else {
		}
	}
	void mouseUp( int button, Point2D p ) {
		if( curMotion )
	}
	void mouseMove( Point2D p ) {
		if( dragging ) {
			motionPoints[dragIndex] = p;
		}
	}

	//Identify the canvasitem
	int type() const { return CanvasItem::TYPE_MOTION_REGION; }
private:
	Motion *curMotion;
	vector<Motion *> motions;
	vector<Point2D> motionPoints;
	bool dragging;
	int dragIndex;

	time_t startTime;

	bool interpolating;
};