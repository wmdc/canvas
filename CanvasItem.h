/*
 * CanvasItem.h
 *
 * Virtual base class for anything that is drawn or can be 
 * manipulated through the animation canvas.
 *
 * by Michael Welsman-Dinelle 06/18/2008
 */

#pragma once

class Point2D;
class Canvas;

class CanvasItem {
public:
	CanvasItem( Canvas &c ) : canvas(c) {}

	//Draw the canvas item
	virtual void draw() = 0;

	/* Test whether or not this item contains the given point */
	virtual bool contains( Point2D p ) const = 0;
    /* Handle mouse/stylus input */
	virtual void mouseDown( int button, Point2D p, bool doubleClick ) = 0;
	virtual void mouseUp( int button, Point2D p ) = 0;
	virtual void mouseMove( Point2D p ) = 0;

	//Identify the canvasitem
	virtual int type() const = 0;
    
    static const int TYPE_MOTION = 1;  //motions return this value from type()
	static const int TYPE_VIEWPORT = 2; //viewports
	static const int TYPE_ANNOTATION = 3;  //annotations
	static const int TYPE_POSE = 4;       //poses
	static const int TYPE_MOTION_LINK = 5;  //links between motions
	static const int TYPE_POSE_HIGHLIGHT = 6;
	static const int TYPE_MOTION_BRUSH = 7;
	static const int TYPE_BRUSH = 8;
	static const int TYPE_BRUSH_PALETTE = 9;
	static const int TYPE_SKF_REGION = 10;
    static const int TYPE_MOTION_REGION = 11;

	static const int MOUSE_BTN_LEFT = GLUT_LEFT_BUTTON;
	static const int MOUSE_BTN_MIDDLE = GLUT_MIDDLE_BUTTON;
	static const int MOUSE_BTN_RIGHT = GLUT_RIGHT_BUTTON;


protected:
	//Must have a reference to the parent canvas
	Canvas &canvas;
};