/*
   Annotation.h

   Basically just a path that is drawn on the canvas.

   by Michael Welsman-Dinelle 09/07/2008
*/

#pragma once;

#include "CanvasItem.h"
#include "Path.h"
#include "Point2D.h"

class Canvas;

//TODO: handle deletion and splitting into multiple paths
//

class Annotation : public CanvasItem {
public:
	//Create a sketched annotation from a gesture
	Annotation( Canvas &c ) : CanvasItem(c) {}
	//Draw the annotation
	void draw();
   
	void mouseDown( int button, Point2D p, bool doubleClick );
	void mouseUp( int button, Point2D p );
	void mouseMove( Point2D p );

	bool contains( Point2D p ) const {
		return path.contains( p );
	}

	void addPoint( Point2D p ) { path.push_back( p ); }

	//Identify the canvasitem (return CanvasItem::TYPE_ANNOTATION)
	int type() const {
		return CanvasItem::TYPE_ANNOTATION;
	}

	static const double ANNOTATION_WIDTH;
private:
	Path path;
};