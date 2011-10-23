/*
 * Basic (improved) class to represent and manipulate a path drawn 
 * on the canvas.
 *
 * by Michael Welsman-Dinelle
 * Oct 12, 2008
 *
 */

#pragma once

#include <deque>
#include <GL/glut.h>

#include "Point2D.h"
#include "BoundingBox.h"

using std::deque;

class Path : public deque<Point2D> {
public:
	Path();
	
    void push_back( Point2D p );

	double getLength() const { return length; }
    double getRadius() const { return radius; }
    void setRadius( double r ) { radius = r; }
    /* Blend the path by taking the average of each vertex plus weighted neighbours. */
	void smooth();

	void drawGLVertices( GLenum drawModeGL, int firstVertex = -1, int lastVertex = -1 );

	bool contains( Point2D pt ) const;
    
	Point2D projectToPath( Point2D p, double *lengthAlongCurve = NULL ) const;
	Point2D getPointAtDistance( double distance ) const;
	void getVerticesForRange( double startLength, double endLength, unsigned *firstVertex, unsigned *lastVertex );

    static const double DEFAULT_PATH_RADIUS; //determines what is "on" the path

	BoundingBox bBox;
private:
	double length, radius;
	
	GLfloat *vertexArray;
	bool validVertexArray;
	void buildVertexArray();
};