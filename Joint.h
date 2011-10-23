#pragma once
#ifndef AC_JOINT_H
#define AC_JOINT_H

/*
 Joint.h - Joint class for hierarchical model.

 by Michael Welsman-Dinelle, June 2009
*/

#include <string>
#include <vector>
#include <cmath>

#include <GL/glut.h>

class Joint {
public:
	const string name;
	vector<Joint *> children;

	double rot[3] const, trans[3] const;

	Joint( string jName, double yaw, double pitch, double roll ) : name(jName), 
			rota[0](yaw), rot[1](pitch), rot[2](roll), 
			trans[0](0.0), trans[1](0.0), trans[2](0.0) { preprocess(); }

	Joint( string jName, double yaw, double pitch, double roll, double transX, double transY, double transZ ) : name(jName), 
			rot[0](yaw), rot[1](pitch), rot[2](roll), 
			trans[0](transX), trans[1](transY), trans[2](transZ) { preprocess(); }

	void applyTransform() const {
		//multiplication of x,z,x rotation matrices - see http://en.wikipedia.org/wiki/Euler_angles#Matrix_expression_for_Euler_rotations
		//apply transformation first

		GLdouble m[16] = {};
		glMultMatrix(m);
	}

	bool isRoot() const {
		return children.size() == 0;
	}

private:
	void preprocess() {
	}
	//Precalculate the transformation matrix for this joint
	GLdouble m[16];
};

#endif /* AC_JOINT_H */