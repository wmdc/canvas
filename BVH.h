/*
   .bvh mocap file reader class.

   Based on BVH class by Michiel van de Panne 
*/

#pragma once

#include <windows.h>
#include <GL/glut.h>

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <math.h>

#include "Motion.h"

const int LONGSTRLEN = 1000;
typedef char LONGSTR[LONGSTRLEN];
const int STRLEN = 200;
typedef char STR[STRLEN];

const int TAB = 9;

class BVHReader {
public:
	static Motion *read(FILE *);      //reads in a BVH file and returns the mocap clip.

	/* File processing - probably do not want to call these directly */
	static int expect(char *word, LONGSTR line);     // verifies first word on the line
	static int lookfor(char *word, LONGSTR line, FILE *fp);   // scans until line is found with the given first word
	static int nextline( LONGSTR line, FILE *fp);             // read in next line
private:
	virtual void dummy() = 0;        //don't need to instantiate
};

const int MAX_CHILDREN = 5;
const float SPHERE_SIZE = 0.5f; /* Joint sphere size. */

class BVHJoint {
private:
	STR name;                      // joint name
	float offset[3];               // location of joint in parent's coordinate system
	int nchannels;                 // #DOFs associated with the joint
	STR chtype[6];                 // DOF types associated with joint
	int chOffset;                  // DOF data occupies channels [ chOffset to chOffset+nchannels-1]
	bool transJoint;               // are there translation-DOFs?  (usually only for root)
	int nchildren;                 // how many children for this joint?
	BVHJoint *child[MAX_CHILDREN]; // pointers to child joints
	bool endSite;                  // a hand or a foot ?
	float endLength[3];            // optional location of tip of hand or foot for endSites

	virtual void drawChild(float *frameData) const;

public:
	 // create JOINT and parse joint info (recursive)
	BVHJoint( char *joint_name, int *nchannels, float *maxOffset, LONGSTR line, FILE *fp );
	float getFaceDir(float *frameData);  // computes facing direction in xz-plane for root link
	virtual void draw(float *frameData, bool translate = 1) const;
	

	const float *getOffset() const { return offset; }


	int getChannelCount() const { return nchannels; }
	int getChannelOffset() const { return chOffset; }
	//Used for determining the camera "at" point easily
	static char *focusName;
	static double focalPoint[3];

	bool operator==( const BVHJoint &rhs ) const {
		return nchannels == rhs.getChannelCount();
	}
};