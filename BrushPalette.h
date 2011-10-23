/*
   Palette of brushes that can be dragged around on the canvas.
   New brushes can be created by dropping canvas items onto the
   palette.
  
   by Michael Welsman-Dinelle, April 2009
*/

#pragma once

#include "CanvasItem.h"
#include "Brush.h"

#include <vector>
#include <ctime>

class Canvas;

class BrushPalette : public CanvasItem {
public:
	BrushPalette( int x, int y, int _width, int _height, Canvas &_c, bool _scroll ) : 
	  CanvasItem(_c), width(_width), height(_height), location(x, y), scroll(_scroll), dx(0), dragging(false), leftScrollHover(false), rightScrollHover(false), contentWidth(0), scrollLeft(false),
	  scrollRight(false), addTime( 0 ) {
        brushSize = height - 2 * BRUSH_DRAW_PADDING;
	}
	/* Test whether or not this item contains the given point */
	bool contains( Point2D p ) const;
    /* Handle mouse/stylus input */
	void mouseDown( int button, Point2D p, bool doubleClick );
	void mouseUp( int button, Point2D p );
	void mouseMove( Point2D p );

	void add( Brush *b, bool flash = false );
	void select( Brush *b );

	void displaySelected( int index );

	int getBrushIconWidth() { return brushSize; }
	int getBrushIconHeight() { return brushSize; }

	void draw();

	int type() const { return CanvasItem::TYPE_BRUSH_PALETTE; }

	static const int CORNER_BEVEL_DETAIL_PTS = 6;
	static const double CORNER_BEVEL_SIZE_RATIO;
	static const int BRUSH_DRAW_PADDING = 6;
	static const int SCROLL_ARROW_PADDING = 15;
	static const int TICKS_PER_PIXEL_SCROLLED = 2;
	static const int TICKS_ADD_MOTION_FLASH = 200;
private:
	Point2D location;
	int width, height, brushSize;
	int contentWidth; //width of the brushes contained inside, drawn in a row
	int dx; //scrolling
	bool dragging;
	bool leftScrollHover, rightScrollHover;
	bool scrollLeft, scrollRight;

	time_t scrollTime;
	clock_t addTime;
	Point2D dragPt;

	std::vector<Brush *> brushes;

	bool scroll, opaque;
};