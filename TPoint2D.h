/* 
   A time-stamped point class.

   by Michael Welsman-Dinelle
*/
#pragma once

#include <ctime>

class TPoint2D : public Point2D {
public:
	TPoint2D() : Point2D() {
		t = time();
	}
	TPoint2D( int _x, int _y ) : Point2D( _x, _y ) {
		t = time();
	}

	time_t getTime() { return t; }

    bool TPoint2D::operator==( const TPoint2D &rhs ) const {
		return t == rhs.getTime() && ((Point2D)rhs)==((Point2D)*this);
    }

private:
	time_t t;
};