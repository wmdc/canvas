/*
   Utility functions.

   by Michael Welsman-Dinelle
*/

#pragma once;

#include "Point2D.h"

class GeomUtil {
public:
	/* Find the point on the line such that the tangent to the line at that
	   position intersects the given point pt, if it exists.
	   
	   Not always possible.  Return true if there is a valid resulting point
	   returned and false otherwise.  If the point doesn't project onto the segment,
	   return the closest point on the segment.
	   
	   Nice explanation at: http://local.wasp.uwa.edu.au/~pbourke/geometry/pointline/
	*/
	static bool projectToSegment( Point2D pt, Point2D a, Point2D b, Point2D *result, double *t = NULL ) {
        //Our segment is a-b.  We are looking for a point along this segment, so
		//it is something like... result = a + u(a+b), where u is unknown but on the interval [0,1]

		//Failure for degenerate segment
		if( a == b ) {
			return false;
		}

		double magnitude = a.dist( b );
		double u = ((pt.x-a.x)*(b.x-a.x)+(pt.y-a.y)*(b.y-a.y))/(magnitude*magnitude);

		bool onSegment = true;

		if( u < 0 ) {
			u = 0;
			onSegment = false;
		} else if( u > 1 ) {
			u = 1;
			onSegment = false;
		}

		result->x = a.x + (int)(u*(b.x-a.x)+0.5);
		result->y = a.y + (int)(u*(b.y-a.y)+0.5);

		if( t != NULL ) {
			*t = u;
		}

		return onSegment;
	}
	/*
	 * Project the point p onto the line a-b.
	 * Return NULL if no suitable projection exists.
	*/
	/*static inline Point2D *projectToSegment( Point2D p, Point2D a, Point2D b ) {
		if( a == b ) return NULL;

		double magnitude = a.dist( b );
		double u = ((p.x-a.x)*(b.x-a.x)+(p.y-a.y)*(b.y-a.y))/(magnitude*magnitude);

		bool onSegment = true;

		if( u < 0 ) {
            return new Point2D( a );
		} else if( u > 1 ) {
			return new Point2D( b );
		}

		return new Point2D( a.x + (int)(u*(b.x-a.x)+0.5), a.y + (int)(u*(b.y-a.y)+0.5) );
	} */
};