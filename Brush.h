/*
   Brush.h

   Superclass for brush buttons on the canvas. Brushes support callbacks.

   by Michael Welsman-Dinelle
*/

#pragma once

#include "CanvasItem.h"
#include "Motion.h"
#include "Point2D.h"

class Canvas;
class BrushPalette;

class Brush : public CanvasItem {
public:
	/* Create an inoperative brush control - must add functionality in subclass. */
	Brush( bool _selected, Canvas &_c, BrushPalette &_p )
		: CanvasItem(_c), selected(_selected), palette(_p), width(0), height(0), location(0,0), hover(false), down(false) {
		if( selected ) {
			select();
		}
		btnFunc = NULL;
		funcArg = 0;
	}
	/* Create a brush control that calls a given function when constructed. */
	Brush( bool _selected, Canvas &_c, BrushPalette &_p, void (*btnFunction)(int), int argument )
		: CanvasItem(_c), selected(_selected), palette(_p), width(0), height(0), location(0,0), hover(false), down(false),
	      btnFunc(btnFunction), funcArg(argument) {

		if( selected ) {
			select();
		}
		btnFunc = NULL;
	}

	virtual void mouseDown( int button, Point2D p, bool doubleClick );
	virtual void mouseUp( int button, Point2D p );
	virtual void mouseMove( Point2D p );

	void setSize( int w, int h ) {
		width = w;
		height = h;
	}
	void setPosition( Point2D pos ) {
		location = pos;
	}

	virtual void drawIcon() const = 0; //icon changes depending on type

    void draw(); //base brush panel

	Point2D getPosition() const { return location; }
	int getWidth() const { return width; }
	int getHeight() const { return height; }

	virtual void select();
	virtual void deselect();

    /* Change whether it is selected without triggering the resulting action. */
	void setSelectionDisplay( bool showSelected );

	int type() const {
		return CanvasItem::TYPE_BRUSH;
	}
    /* Does this button contain the given point? */
	bool contains( Point2D p ) const {
		return p.x <= location.x + width && p.x >= location.x && p.y >= location.y && p.y <= location.y + height;
	}

	const static int DEFAULT_WIDTH = 50;
	const static int DEFAULT_HEIGHT = 50;
protected:
	BrushPalette &palette;

	Point2D location;
	int width, height;

	bool selected, down, hover;

	/* Function to be called when the button is selected. */
    void (*btnFunc)(int);
	int funcArg;
};