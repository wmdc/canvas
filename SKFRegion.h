/*
 * A spatial keyframing region on the canvas. Note that this is designed to interact
 * with Pose objects on the canvas.
 *
 * Handles mouse input and allows the user to drag between Poses to generate a new motion
 *
 * by Michael Welsman-Dinelle, 24 Oct 2008
 */

#pragma once

#include "Path.h"
#include "Pose.h"
#include "Point2D.h"
#include "BoundingBox.h"
#include "rbf.h"

#include <vector>
#include <ctime>

class Canvas;
class CanvasPose;

class SKFRegion {
public:
	SKFRegion( Canvas *c );

	void addPose( CanvasPose *p );
	void removePose( CanvasPose *p );

	//Declare that the interpolant must be fixed before new poses
	//or motions are generated. This happens because keyposes change.
	void flagRecalculate() { needsRecalculate = true; }

	unsigned poseCount() {
		return poses.size();
	}

	void draw();

	bool contains( Point2D p ) const;
	bool isEmpty() const { return outerBounds.empty(); }

	void mouseDown( int button, Point2D p, bool doubleClick );
	void mouseUp( int button, Point2D p );
	void mouseMove( Point2D p );

	void addBoundsPoint( Point2D p ) {
		if( outerBounds.empty() || p.dist( outerBounds.back() ) > 5.0 ) {
            outerBounds.push_back( p );
            bBox.addPoint( p.x, p.y );
		}
	}
	void doneBounds() {
		drawingBounds = false;

		//see http://www.opengl.org/resources/faq/technical/glu.htm for GLU polygon
		//tesselation details
		tesselator = gluNewTess();

		gluTessCallback(tesselator, GLU_TESS_BEGIN, (void (__stdcall *)(void))&tBegin);
		gluTessCallback(tesselator, GLU_TESS_VERTEX, (void (__stdcall *)(void))&glVertex3dv);
		gluTessCallback(tesselator, GLU_TESS_END, (void (__stdcall *)(void))&tEnd);
		gluTessCallback(tesselator, GLU_TESS_COMBINE, (void (__stdcall *)(void))&tCombine);
		//gluTessCallback(tesselator, GLU_TESS_ERROR, (void (__stdcall *)(void))&tError);

		vTess = new GLdouble[ 3 * outerBounds.size() ];

		int index = 0;
		for( deque<Point2D>::const_iterator a = outerBounds.begin(); a != outerBounds.end() - 1; a++ ) {
			vTess[ 3 * index ] = a->x;
			vTess[ 3 * index + 1 ] = a->y;
			vTess[ 3 * index + 2 ] = 0.0;
			index++;
		}
	}
    //Generate a pose at the given point in the region.
	Pose *generatePose( Point2D pose );
    Motion *SKFRegion::generateMotion();

	static const int TICKS_ADD_MOTION_FEEDBACK = 350;
private:
    Path outerBounds; //outer bounds of the region
	BoundingBox bBox;

	bool empty;         //empty if there is no area contained
	bool drawingBounds; //are we drawing out the region's boundaries?
	bool drawingMotion; //are we currently drawing a curve?
	bool dragging;
	bool needsRecalculate;

	std::vector<CanvasPose *> poses;

	Path selectionPath;
	std::vector<clock_t> selectionTimes;

	CanvasPose *displayPose; //drawn in the region when a pose is selected during "playback"

    Motion *drawnMotion;    //a valid motion from samples or NULL

	Canvas *canvas;

	//Interpolator for calculating the value of an interpolated pose
	//Constructed when we add key poses.
	//Interpolation triggered when the user clicks on an empty part of the region
	Rbf::Interpolant *interpolator;

	void recalculate(); //build the interpolator

	//Last added motion
	clock_t tAdd;
	Point2D pAdd;
	Motion *mAdd;
    
	//For correctly drawing a general polygon region
	GLUtesselator *tesselator;
	GLdouble *vTess;

	static void CALLBACK tBegin(GLenum type) { glBegin (type); }
    static void CALLBACK tEnd() { glEnd (); } 
    //static void CALLBACK tCombine(GLdouble c[3], void *d[4], GLfloat w[4], void **out) { GLdouble *nv = (GLdouble *) malloc(sizeof(GLdouble)*3); nv[0] = c[0]; nv[1] = c[1]; nv[2] = c[2]; *out = nv;  }
	static void CALLBACK tError( GLenum errno ) {
		printf( "tessellation error: %d\n", errno );
	}

	static void CALLBACK tCombine(GLdouble coords[3], 
                     GLdouble *vertex_data[4],
                     GLfloat weight[4], GLdouble **dataOut )
{
   GLdouble *vertex;
   vertex = (GLdouble *) malloc(3 * sizeof(GLdouble));
   vertex[0] = coords[0];
   vertex[1] = coords[1];
   vertex[2] = coords[2];
   *dataOut = vertex;
}

};