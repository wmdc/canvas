/*
 * MotionCurve.h
 *
 * A motion curve defines a curve drawn on the canvas by the user that is bound
 * to a Motion.
 *
 * by Michael Welsman-Dinelle, 19/04/2010
 */

#pragma once

#include <ctime>
#include <vector>

#include "Point2D.h"
#include "CanvasItem.h"
#include "Motion.h"
#include "Path.h"
#include "CanvasPose.h"
#include "BoundingBox.h"

class Canvas;

class MotionCurve : public CanvasItem {
public:
	MotionCurve( Motion &m, Canvas &c ); //must add points to the path
	MotionCurve( Path &pts, Motion &m, Canvas &c ); //path already generated

	/* Set the selected frame to the given point */
	void startAnimation( int frame );
	void stopAnimation();

	/* How long is this penstroke? */
	double getLength() const { return curve.getLength(); }

	void addPoint( Point2D pt ) {
		if( curve.empty() || pt.dist( curve.back() ) > 3.0 ) {
			//don't add points too close together
            curve.push_back( pt );
		}
	}

	Motion *getMotion() const { return &motion; }
	Motion *getSelectedMotion() const {
		return motion.getSubmotion( highlightFrame, highlightEndFrame );
	}
	
	void draw();

    /* Distinguish this particular CanvasItem from other types */
	int type() const {
		return CanvasItem::TYPE_MOTION;
	}

	unsigned getFrameFromLength( double lengthAlongCurve ) {
		if( lengthAlongCurve < 0 ) {
			return 0;
		} else if( lengthAlongCurve > curve.getLength() ) {
			return motion.getFrameCount();
		} else {
		    return lengthAlongCurve / curve.getLength() * motion.getFrameCount();
		}
	}

	void setHighlightFrame( unsigned frame ) {
		highlightFrame = frame;
	}
	void setHighlightEndFrame( unsigned frame ) {
		highlightEndFrame = frame;
	}

	void updateHighlightIndices() {
		curve.getVerticesForRange( (double)highlightFrame/motion.getFrameCount() * curve.getLength(), 
					(double)highlightEndFrame/motion.getFrameCount() * curve.getLength(), 
					&highlightIndex, &highlightEndIndex );
	}
    //Select a given point on the curve, subject to contraints of
	//selected neighbour curves.
	void selectPoint( Point2D proj, double distanceAlongCurve, bool mouseDown );

	bool contains( Point2D p ) const;

	void mouseDown( int button, Point2D p, bool doubleClick );
	void mouseUp( int button, Point2D p );
	void mouseMove( Point2D p );

	void addParent( MotionCurve *p ) { graphPrev.push_back( p ); }
	void addChild( MotionCurve *c ) { graphNext.push_back( c ); }

	bool hasParent() { return graphPrev.empty(); }
	bool hasChild() { return graphNext.empty(); }

	void doneSketching() {
		sketching = false;
		curve.smooth();
		curve.smooth();
	}

	bool isSelecting() const { return selecting; }
	unsigned getHighlightFrame() const { return highlightFrame; }
    unsigned getHighlightEndFrame() const { return highlightEndFrame; }

	MotionCurve *neighbourSelectedBefore() const {
		for( unsigned i = 0; i < graphPrev.size(); i++ ){
			if( graphPrev[i] && graphPrev[i]->flagSelected ) return graphPrev[i];
		}
		return NULL;
	}

	MotionCurve *neighbourSelectedAfter() const {
		for( unsigned i = 0; i < graphNext.size(); i++ ){
			if( graphNext[i] && graphNext[i]->flagSelected ) return graphNext[i];
		}
		return NULL;
	}

	void clearGraphSelection() {
		flagSelected = false;
		selecting = false;
		highlightFrame = -1;
		highlightEndFrame = -1;
		clearFlagsBefore( this );
		clearFlagsAfter( this );
	}

	Path *getCurve() { return &curve; }

	bool flagSelected;
	
	static const int TICKS_ADD_MOTION_FEEDBACK = 750;
	static const double PLAYBACK_SEG_LENGTH;
private:
	inline void init();
	void drawEndArrows() const;

	Motion &motion;
	Path curve;

    bool sketching, animating, selecting;
	time_t animationStartTime;
	unsigned animationStartFrame;

	unsigned  highlightFrame, highlightEndFrame;
	Point2D highlightPoint;
    unsigned highlightIndex, highlightEndIndex;
	Point2D highlightEndPoint;
	unsigned highlightOriginFrame;

	bool highlightEndArrow;

	unsigned playbackFrame;
	Point2D playbackSeg, playbackSegEnd;

	std::vector<MotionCurve *> graphNext, graphPrev;

	//Last added motion
	clock_t tAdd;
	Point2D pAdd;
	Motion *mAdd;

	static const int NO_SELECTION = -1;
    //Helper methods to traverse the graph (connected component) to clear selection labels
	void clearFlagsBefore( MotionCurve *origin ) {
		for( unsigned i = 0; i < graphPrev.size(); i++ ) {
			graphPrev[i]->selecting = false;
			graphPrev[i]->flagSelected = false;
			graphPrev[i]->setHighlightFrame( NO_SELECTION );
			graphPrev[i]->setHighlightEndFrame( NO_SELECTION );
			graphPrev[i]->clearFlagsBefore( origin );
		}
	}
	void clearFlagsAfter( MotionCurve *origin ) {
		for( unsigned i = 0; i < graphNext.size(); i++ ) {
			graphNext[i]->selecting = false;
			graphNext[i]->flagSelected = false;
			graphNext[i]->setHighlightFrame( NO_SELECTION );
			graphNext[i]->setHighlightEndFrame( NO_SELECTION );
			graphNext[i]->clearFlagsAfter( origin );
		}
	}

	Motion *createMotionFromSelection() const;

	void setPlaybackFrame( unsigned frame, bool showNewFrame = true );
};