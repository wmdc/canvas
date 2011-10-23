/*
 * CanvasPose.h
 *
 * A character pose associated with a point on the canvas.
 *
 * by Michael Welsman-Dinelle, 15/03/2010
 */

#pragma once

#include "Canvas.h"
#include "Pose.h"
#include "Point2D.h"
#include "CanvasItem.h"

class SKFRegion;

class CanvasPose : public Pose, public CanvasItem {
public:
	CanvasPose( Pose &pose, Point2D p, Canvas &c, bool dragEnabled = FALSE );
	CanvasPose( Motion &m, int frame, Point2D location, Canvas &c, bool dragEnabled = FALSE );

    void draw();
	void drawHighlight();

	bool contains( Point2D p ) const;
	void mouseDown( int button, Point2D pt, bool doubleClick );
	void mouseUp( int button, Point2D pt );
	void mouseMove( Point2D pt );

	Point2D getPosition() const { return p; }

	void setDragging() {
		printf("dragging\n");
		dragging = true;
		pLast = p;
	}

	int type() const { return CanvasItem::TYPE_POSE; }
private:
	Point2D p, pLast;

	bool dragging;

	SKFRegion *region;

	static const float CANVAS_POSE_RADIUS;
};