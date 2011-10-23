#include "Point2D.h"

class Rect2D {
	Rect2D() {
		x = 0;
		y = 0;
		width = 0;
		height = 0;
	}
	Rect2D( double _x, double _y, double _width, double _height ) {
		x = _x;
		y = _y;
		width = _width;
		height = _height;
	}

	bool contains( Point2D p ) {
		return p.x >= x && p.x <= x + width && p.y >= y && p.y <= y + height;
	}

	void setX( double _x ) { x = _x; }
	void setY( double _y ) { y = _y; }
	void setWidth( double _width ) { width = _width; }
	void setHeight( double _height ) { height = _height; }
	void setPosition( Point2D position ) { x = position.x; y = position.y; }
	void setSize( Point2D lowerLeft, Point2D upperRight ) {
		x = lowerLeft.x;
		y = lowerLeft.y;
		width = upperRight.x - x;
		height = upperRight.y - y;
	}
    /* Extend the boundaries of the rect so they contain the given point. */
	void extendTo( Point2D p ) {
		if( p.x < x ) x = p.x;
		if( p.y < y ) y = p.y;
		if( p.x > x + width ) width = p.x - x;
		if( p.y > y + height ) height = p.y - y;
	}

	double getX() { return x; }
	double getY() { return y; }
	double getWidth() { return width; }
	double getHeight() { return height; }
	Point2D getPosition() { return Point2D( x, y ); }

private:
	double x, y, width, height;
};