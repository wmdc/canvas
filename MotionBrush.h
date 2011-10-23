/*
 * Motion "brush" icon on the canvas that can be selected to determine
 * the current motion to be drawn on the canvas.
 *
 * by Michael Welsman-Dinelle, 10 Oct 2008.
 */
#pragma once 

#include "Brush.h"

#include <ctime>

class Motion;

class MotionBrush : public Brush {
public:
	MotionBrush( Motion &_m, bool _selected, Canvas &_c, BrushPalette &_p );

	void draw(); //override Brush::draw()

	void mouseDown( int button, Point2D p, bool doubleClick );
    void mouseMove( Point2D p );
	
	void drawIcon() const;
	void select();
	void deselect();
private:
	Motion &m;

	time_t aniT; //When did the animation start running?

	static const unsigned LABEL_SIZE = 10;
};