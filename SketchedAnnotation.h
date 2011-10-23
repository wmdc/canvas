/*
   SketchedAnnotation.h

   Annotation sketched onto the canvas.  Can be changed as more points are added.

   by Michael Welsman-Dinelle 19/04/2009
*/

#include <string>
#include <vector>

#include "Gesture.h"
#include "Point2D.h"
#include "CanvasItem.h"

using namespace std;

class Annotation : public CanvasItem {
public:
	//Create a sketched annotation from a gesture
	Annotation( Gesture &g );
	Annotation( string &txt, Point2D pt );
	//Draw the annotation
	void draw();
    //Manipulate the annotation if in the proper mode
	void processMouse( int mode, int mType, float x, float y );

	//Identify the canvasitem (return CanvasItem::TYPE_ANNOTATION)
	int type() const {
		return CanvasItem::TYPE_ANNOTATION;
	}

private:
	Point2D tOrigin;
	string text;
	vector<Point2D> curve;
};