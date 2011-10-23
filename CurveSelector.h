/*
 * Handles selection across multiple motion curves on the canvas.
 *
 * by Michael Welsman-Dinelle, 30/4/2010
 */

#pragma once

#include <deque>
#include <GL/glut.h>

#include "MotionCurve.h"
#include "Canvas.h"

using std::deque;

class CurveSelector {
public:
	CurveSelector() : vArray( NULL ) {}
	~CurveSelector() { delete [] vArray; }

	void addFront( Point2D p, MotionCurve *curve ) {
		points.push_front( p );
		pointSources.push_front( curve );
		
		buildVArray();
	}
	void addBack( Point2D p, MotionCurve *curve ) {
		points.push_back( p );
		pointSources.push_front( curve );
		
		buildVArray();
	}

	void draw() const {
		if( points.empty() ) {
			return;
		}

        glColor4dv( COLOR_CURVE_HIGHLIGHT );
		glLineWidth( CURVE_LINE_WIDTH * 2.0f / 3.0f );
		glPointSize( CURVE_LINE_WIDTH * 2.0f / 3.0f );

		glEnableClientState(GL_VERTEX_ARRAY);

		glPushClientAttrib( GL_CLIENT_VERTEX_ARRAY_BIT ); {
			glVertexPointer(2, GL_FLOAT, 0, vArray);

			glDrawArrays(GL_LINES, 0, points.size() );
			glDrawArrays(GL_POINTS, 0, points.size() );
		} glPopClientAttrib();

		glDisableClientState(GL_VERTEX_ARRAY);
	}

	Motion *createMotion() const {
		return NULL;
	}
private:
	deque<Point2D> points;
	deque<MotionCurve *> pointSources;

	GLfloat *vArray;

	void buildVArray() {
		vArray = new GLfloat[ 2 * points.size() ];

		for( unsigned index = 0; index < points.size(); index++ ) {
			vArray[ 2 * index ] = points[index].x;
			vArray[ 2 * index + 1 ] = points[index].y;
		}
	}
};