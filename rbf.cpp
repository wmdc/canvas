#include "precompiled.h"

#include "rbf.h"

#include <assert.h>

namespace Rbf {

Interpolant::Interpolant(vector<point>& points)
{
	mPts = points;

	fmat m = fmat(mPts.size(), mPts.size());

	for(int row = 0; row < m.n_rows; row++) {
		for(int col = 0; col < m.n_cols; col++) {
			m(row, col) = rbf(dist( mPts[col], mPts[row] ));
		}
	}
	//This inverted matrix can be used to get our coefficients
	//for as long as the points are fixed
	mSolved = inv(m);
}

float Interpolant::interpolate(point &p, vector<float>& values)
{
	colvec f = conv_to< colvec >::from(values);

	colvec coeffs = mSolved * f;

	assert(coeffs.size() == values.size());
	assert(coeffs.size() == mPts.size());

	float result = 0;

	for(int i = 0; i < coeffs.size(); i++)
	{
		result += coeffs(i) * rbf( dist(p, mPts[i]) );
	}

	return result;
}

float Interpolant::dist(point& p1, point& p2)
{
	return sqrt((p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y));
}

}