#include "CanvasItem.h"
#include "Point2D.h"
#include "Rect2D.h"

#include "DrawingConstants.h"

#include <vector>

#include <GL/glut.h>

class Canvas;

class CanvasRegion : public CanvasItem {
	CanvasRegion( Canvas &c ) : CanvasItem( c ) {
		minX = 0;
		maxX = 0;
		minY = 0;
		maxY = 0;
	}

	void draw() {
		glColor4dv( COLOR_INSET_GREY );
		glBegin( GL_POLYGON );
		  for( unsigned i = 0; i < boundary.size(); i++ )
			  glVertex2d( boundary[i].x, boundary[i].y );
		glEnd();
        glColor4dv( COLOR_BORDER_GREY );
		glBegin( GL_LINE_LOOP );
		  for( unsigned i = 0; i < boundary.size(); i++ )
			  glVertex2d( boundary[i].x, boundary[i].y );
		glEnd();
	}

	void addPoint( Point2D p ) {
		boundary.push_back( p );

		if( p.x > maxX ) maxX = p.x;
		else if( p.x < minX ) minX = p.x;

		if( p.y > maxY ) maxY = p.y;
		else if( p.y < minY ) minY = p.y;
	}

	/* Test whether or not this item contains the given point */
	bool contains( Point2D p ) {
		if( !boundingBox.contains( p ) ) {
			return false;
		}

		//test for point inside polygon - ray casting algorithm that assumes simple polygon
		for( unsigned i = 0; i < boundary.size(); i++ ) {
			;
		}
	}

private:
	Rect2D boundingBox;
	double minX, maxX, minY, maxY;
	std::vector<Point2D> boundary;
};