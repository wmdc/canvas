#pragma once

#include <cmath>

class Point3D {
public:
	Point3D() : x(0.0), y(0.0), z(0.0 ) {}
	Point3D( double _x, double _y, double _z ) : x(_x), y(_y), z(_z) {}

	double dist( Point3D p ) {
		return sqrt( (x - p.x)*(x - p.x) + (y - p.y)*(y - p.y) + (z - p.z)*(z - p.z) );
	}
	double x, y, z;
};