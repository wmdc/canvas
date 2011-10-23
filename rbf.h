// rbf.h
// Implements radial basis function interpolation
// Uses Armadillo linear algebra library: http://arma.sourceforge.net
// by Michael Welsman-Dinelle, August 11, 2011

#pragma once

#include <armadillo>
#include <vector>

using namespace arma;
using std::vector;

namespace Rbf {

struct point
{
	float x, y;

	point(float x1, float y1) : x(x1), y(y1) {};
};

//make generic n-dimensional point and use [] notation

class Interpolant {
	fmat mSolved;
	colvec mCoeff; //coefficients of interpolation
	vector<point> mPts;

	float dist(point& p1, point& p2);

	//TODO: make this configurable
	inline float rbf(float x) { return x*x; }
public:
	Interpolant(vector<point>& mPts);

	float interpolate(point &p, vector<float>& values);
};
}