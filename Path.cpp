#include "precompiled.h"

#include "Path.h"

#include <float.h>

const double Path::DEFAULT_PATH_RADIUS = 3.0;

Path::Path() {
	length = 0.0;
	radius = DEFAULT_PATH_RADIUS;
	validVertexArray = false;
}

void Path::push_back( Point2D p ) {
	if( size() > 0 ) {
		double distance = back().dist( p );
		length += distance;

		if( distance > 1.0 ) {
			int newPoints = (int)( distance / DEFAULT_PATH_RADIUS );
            //add some intermediary points
			for( int i = 1; i < newPoints; i++ ) {
				double t = (double)i / newPoints;
				Point2D pt = Point2D( (1-t) * back().x + t * p.x, (1-t) * back().y + t * p.y );
				deque<Point2D>::push_back( pt );
	            
			}
		}
	}
    deque<Point2D>::push_back( p );
	bBox.addPoint( p.x, p.y );
	validVertexArray = false;
}

void Path::smooth() {
	if( size() < 4 ) { return; }

	for( deque::iterator it = begin() + 2; it != end() - 1; it++ ) {
		(it-1)->x = ( (it-2)->x + it->x ) / 2.0; 
		(it-1)->y = ( (it-2)->y + it->y ) / 2.0; 
	}

	validVertexArray = false;
}

void Path::drawGLVertices( GLenum drawModeGL, int firstVertex, int lastVertex ) {
	if( !validVertexArray ) {
		buildVertexArray();
	}

    glEnableClientState(GL_VERTEX_ARRAY);

	glPushClientAttrib( GL_CLIENT_VERTEX_ARRAY_BIT ); {
        glVertexPointer(2, GL_FLOAT, 0, vertexArray);

		if( firstVertex < 0 || lastVertex < 0 ) {
			glDrawArrays(drawModeGL, 0, size() );
		} else {
			glDrawArrays(drawModeGL, firstVertex, lastVertex - firstVertex );
		}
	} glPopClientAttrib();

    glDisableClientState(GL_VERTEX_ARRAY);
}

void Path::getVerticesForRange( double startLength, double endLength, unsigned *firstVertex, unsigned *lastVertex ) {
	if( size() < 2 ) {
		*firstVertex = 0;
		*lastVertex = 0;
		return;
	}

	if( startLength > endLength ) { //swap
		double temp = startLength;
		startLength = endLength;
		endLength = temp;
	}

	int index = 0;
	double distance = 0.0;

	for( deque<Point2D>::const_iterator a = begin(); a != end() - 1; a++ ) {
		if( distance <= startLength ) {
			*firstVertex = index;
		}
		if( distance >= endLength ) {
			*lastVertex = index;
			return;
		}
		distance += a->dist( *(a+1) );
		index++;
	}
	
}

bool Path::contains( Point2D p ) const {
	//first check the bounding box
	//printf("%d - %d\n", this, bBox.contains( p.x, p.y ) );
	//if( bBox.contains( p.x, p.y ) ) {
	    for( unsigned i = 0; i < size(); i++ ) {
			if( at(i).dist( p ) <= DEFAULT_PATH_RADIUS * 6.0 ) { return true; }
		}
	//}
	return false;
}

Point2D Path::projectToPath( Point2D p, double *lengthAlongCurve ) const {
	Point2D min;
	double dist = DBL_MAX;
	double lengthSoFar = 0.0;

	for( deque<Point2D>::const_iterator a = begin(); a != end() - 1; a++ ) {
		Point2D temp;

		deque<Point2D>::const_iterator b = a + 1;

		double magnitude = a->dist( *b );
		double u = ((p.x-a->x)*(b->x-a->x)+(p.y-a->y)*(b->y-a->y))/(magnitude*magnitude);

		if( u < 0 ) temp = *a;
		else if( u > 1 ) temp = *b;
		else temp = Point2D( a->x + (int)(u*(b->x-a->x)+0.5), a->y + (int)(u*(b->y-a->y)+0.5) );

		double tmpDist = p.dist( temp );

		if( tmpDist < dist ) {
			min = temp;
			dist = tmpDist;
			
			if( lengthAlongCurve ) { //how far are we along the path?
				*lengthAlongCurve = lengthSoFar + dist;
			}
		}

		lengthSoFar += a->dist( *b );
    }

	return min;
}

Point2D Path::getPointAtDistance( double distance ) const {
	if( distance <= 0 ) {
		return front();
	} else if( distance >= length ) {
		return back();
	} else {
		double lengthSoFar = 0.0;
		for( deque<Point2D>::const_iterator a = begin(); a != end() - 1; a++ ) {
			double lengthNextSegment = a->dist( *(a + 1) );

			if( lengthSoFar + lengthNextSegment >= distance ) {
				double weightNext = ( lengthSoFar + lengthNextSegment - distance ) / lengthNextSegment;
				double weightCur = 1.0 - weightNext;

				return Point2D( weightCur * a->x + weightNext * (a+1)->x, weightCur * a->y + weightNext * (a+1)->y );
				//return *a;
			}
			lengthSoFar += lengthNextSegment;			
		}
		return back();
	}
}

void Path::buildVertexArray() {
	vertexArray = new GLfloat[ 2 * size() ];

	for( unsigned i = 0; i < size(); i++ ) {
		vertexArray[ 2 * i ] = at(i).x;
		vertexArray[ 2 * i + 1 ] = at(i).y;
	}

	validVertexArray = true;
}