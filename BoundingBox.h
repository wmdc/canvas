/*
 * BoundingBox.h
 *
 * Maintains a growable bounding box that is guaranteed to contain
 * all points passed in.
 *
 * by Michael Welsman-Dinelle, 15/04/2010
 */

#pragma once

#include <float.h>

class BoundingBox {
public:
	//construct a bounding box guaranteed to grow as points as added.
	BoundingBox() : minX( FLT_MAX ), minY( FLT_MAX ), maxX( FLT_MIN ), maxY( FLT_MIN ) {}
    //add a point and grow the bounding box
	void addPoint( float x, float y ) {
		if( x < minX ) {
			minX = x;
		} else if( x > maxX ) {
			maxX = x;
		}

		if( y < minY ) {
			minY = y;
		} else if( y > maxY ) {
			maxY = y;
		}
	}
    //test to see if a point is contained within the bounding box
	bool contains( float x, float y ) const {
		return x >= minX && x <= maxX
			&& y >= minY && y <= maxY;
	}
private:
    float minX, maxX;
	float minY, maxY;
};