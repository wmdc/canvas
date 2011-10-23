/* 
   A simple point class.

   by Michael Welsman-Dinelle
*/
#pragma once

class Point2D {
public:
	Point2D();
	Point2D( double _x, double _y );

	double dist( Point2D p2 ) const;

	bool operator==( const Point2D &rhs ) const;
	bool operator<( const Point2D &rhs ) const;
	bool operator>( const Point2D &rhs ) const;

	Point2D operator+( const Point2D &rhs ) const;
	Point2D operator-( const Point2D &rhs ) const;

	void operator+=( const Point2D &rhs );
	void operator-=( const Point2D &rhs );

	double x;
	double y;
};