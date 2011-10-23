/*
   Camera that "orbits" about a central point.  Calculates
   viewing transformations and can draw a camera in a scene
   to indicate other viewpoints.

   by Michael Welsman-Dinelle, 27 May 2009
*/

#include <GL/glut.h>
#include "DrawingConstants.h"

class OrbitalCam {
public:
	OrbitalCam() {
		tOrbit = 0.0; tPitch = 0.0; tDist = 5.0;
		cIndex = 0;

		toGLA();
	}
	OrbitalCam( double orbit, double pitch, double dist ) : tOrbit( orbit ), tPitch( pitch ), tDist( dist ) {
		toGLA();

		gluq = gluNewQuadric();
		cIndex = 0;
	}
    //Pick the colour for this camera
	void setColourIndex( int index ) { cIndex = index; }
	void move( double dOrbit, double dPitch, double dDist ) {
		tOrbit += dOrbit;
		tPitch += dPitch;
		tDist += dDist;

		//clamp pitch
		if( tPitch <= M_PI/32 )
			tPitch = M_PI/32;
		else if( tPitch >= M_PI / 2 - 0.1 )
			tPitch = M_PI / 2 - 0.1;

		toGLA();

		//printf("camera: orbit[%f], pitch[%f], dist[%f]\n", tOrbit, tPitch, tDist );
	}

    void position( double orbit, double pitch, double dist ) {
		tOrbit = orbit;
		tPitch = pitch;
		tDist = dist;

		toGLA();
	}

	void applyViewTransform() const {
		//printf("glulookat %f %f %f %f %f %f %f %f %f\n", eye.x, eye.y, eye.z, at.x, at.y, at.z, up.x, up.y, up.z );
		gluLookAt( eye.x, eye.y, eye.z, at.x, at.y, at.z, up.x, up.y, up.z );
	}

	void applyInverseViewTransform() const {
		//inverse of the matrix "M" from gluLookAt - just 3x3 inverse
		/*GLdouble m[16] = { s[0], u[0], -f[0], 0,
			               s[1], u[1], -f[1], 0,
						   s[2], u[2], -f[2], 0,
						   0,    0,     0, 1 };*/
		//inverse of gluLookAt translate
        glTranslated( eye.x, eye.y, eye.z );
	}

	void draw() const {
		  if( gluq != NULL ) {
		    glPushMatrix();
			  glEnable( GL_LIGHTING );
			  glEnable( GL_COLOR_MATERIAL );
			  GLfloat lpos[4] = { 0, 0, 0, 0 };
			  glLightfv( GL_LIGHT0, GL_POSITION, lpos );
			  glEnable( GL_LIGHT0 );

			  if( cIndex < COLOUR_SET_SIZE ) {
				  glColor3dv( COLOUR_SET[ cIndex ] );
			  }

			  applyInverseViewTransform();
              gluCylinder( gluq, 0.1, 0.2, 0.4, 4, 1 );

			  glDisable( GL_LIGHTING );
			  glDisable( GL_COLOR_MATERIAL );
			  glDisable( GL_LIGHT0 );
			glPopMatrix();
		  }
	}

private:
	GLUquadric *gluq;
	double tOrbit, tPitch, tDist;
	Point3D eye, at, up;

	int cIndex; //colour of this camera when it is drawn
    
	void toGLA() {
		double elev = sin(tPitch);

        eye = Point3D( tDist * cos( tOrbit ) * abs(( 1 - elev )), tDist * elev, tDist * sin( tOrbit ) * abs(( 1 - elev )) );
		at = Point3D( 0.0, 0.0, 0.0 );
		up = Point3D( 0.0, 1.0, 0.0 );
	}
};