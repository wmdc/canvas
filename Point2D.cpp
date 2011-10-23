#include "precompiled.h"

#include <cmath>

#include "Point2D.h"

Point2D::Point2D() {
	x = 0;
	y = 0;
}

Point2D::Point2D( double _x, double _y ) {
	x = _x;
	y = _y;
}

double Point2D::dist( Point2D p2 ) const {
	return sqrt( (( x - p2.x ) * ( x - p2.x ) + ( y - p2.y ) * ( y - p2.y )) );
}

bool Point2D::operator==( const Point2D &rhs ) const {
	return x == rhs.x && y == rhs.y;
}

bool Point2D::operator<( const Point2D &rhs ) const {
	return x < rhs.x && y < rhs.y;
}

bool Point2D::operator>( const Point2D &rhs ) const {
	return x > rhs.x && y > rhs.y;
}

Point2D Point2D::operator+( const Point2D &rhs ) const {
   return Point2D( x + rhs.x, y + rhs.y );
}

Point2D Point2D::operator-( const Point2D &rhs ) const {
   return Point2D( x - rhs.x, y - rhs.y );
}

void Point2D::operator+=( const Point2D &rhs ) {
	x += rhs.x;
	y += rhs.y;
}

void Point2D::operator-=( const Point2D &rhs ) {
	x -= rhs.x;
	y -= rhs.y;
}