#include "precompiled.h"

// Disable VC++ scanf warnings
#ifdef _MSC_VER
#pragma warning( disable : 4996 )
#endif

#include "BVH.h"

#include <iostream>
#include <cmath>

const double PI(4.0 * std::atan2(1.0, 1.0));

char *BVHJoint::focusName = NULL;
double BVHJoint::focalPoint[3] = {0.0, 0.0, 0.0};

/* ------------ BVHReader Methods ----------------- */

/*
   Read in a .bvh file and return the motion (frames + hierarchical model).
*/
Motion *BVHReader::read(FILE *fp) {
	LONGSTR line;   //current line of the file.
	int nframes = 0;    //number of frames read in.
	float dt = 0;       //time between each frame.
	int nchannels = 0;  //number of channels.
    float maxOffset = 0; //largest offset

	STR rootname;
    
	nextline( line, fp );

	expect("HIERARCHY", line );

	nextline( line, fp );
	expect("ROOT", line );
	sscanf(line,"ROOT %s",rootname);

	nextline( line, fp );
	BVHJoint *root = new BVHJoint(rootname, &nchannels, &maxOffset, line, fp );     // recursive:  parses remainder of header

	lookfor("MOTION", line, fp );
	
	nextline( line, fp );
	expect("Frames:", line );
	sscanf(line,"Frames:%d",&nframes);

	nextline( line, fp );
    expect("Frame Time:", line );
	sscanf(line,"Frame Time:%f",&dt);

	float *data = new float[ nframes * nchannels ];
	float *faceDir = new float[ nframes ];

	for (int n=0, indx=0; n<nframes; n++) {
		for (int c=0; c<nchannels; c++) {
			fscanf(fp,"%f",&data[indx++]);
		}
		faceDir[n] = root->getFaceDir(&data[n*nchannels]);    // compute facing direction
	}
	
	fclose(fp);

	/* Build the motion instance from the raw data. */
	return new Motion( nchannels, nframes, dt, maxOffset, root, data, faceDir );
}

//////////////////////////////////////////////////
// PROC:   BVH::nextline()
// DOES:   reads in new, non-empty line from BVH file
//////////////////////////////////////////////////

int BVHReader::nextline( LONGSTR line, FILE *fp ) {
	char *start;
	LONGSTR tmp;

	while (!feof(fp)) {
		char *rv = fgets(line,STRLEN,fp);

		if (!rv) break;
		  // find first char in line
		for (start=line; *start==' ' || *start==TAB; start++);
		if (*start=='\n') continue;
		if (strlen(start)>0) {
			strcpy(tmp,start);          // get rid of leading white space
			strcpy(line,tmp);
			return 1;
		}
	}
	printf("Hit end-of-file\n");
	return 0;
}

//////////////////////////////////////////////////
// PROC:   BVH::expect()
// DOES:   checks for first word on line
//////////////////////////////////////////////////

int BVHReader::expect( char *word, LONGSTR line )
{
	if (!strncmp(word,line,strlen(word))) {
		return 1;
	}
	fprintf( stderr, "Parse error: %s", word );
	return 0; //not found
}

//////////////////////////////////////////////////
// PROC:   BVH::lookfor()
// DOES:   checks for first word on line.
//////////////////////////////////////////////////

int BVHReader::lookfor(char *word, LONGSTR line, FILE *fp ) {
	while (!feof(fp)) {
		if (!strncmp(word,line,strlen(word))) {
			return 1;
		}
		nextline( line, fp );
	}
	return 0; //not found
}

/* ------------ JOINT Methods ----------------- */

BVHJoint::BVHJoint( char *joint_name, int *nchans, float *maxOffset, LONGSTR line, FILE *fp )
{
	STR childName;
	strcpy( name, joint_name ); //save as this joint's name

	endSite = 0;
	nchildren = 0;
	transJoint = false;
	chOffset = 0;

	BVHReader::expect("{", line );
	BVHReader::nextline( line, fp );

	BVHReader::expect("OFFSET", line );
	sscanf( line,"OFFSET %f %f %f\n",&offset[0], &offset[1], &offset[2]);

    //Update Max Offset for the motion
	for (int d=0; d<3; d++) {
		float absval = float(fabs(offset[d]));
		if (absval > *maxOffset )
			*maxOffset = absval;
	}

	BVHReader::nextline( line, fp );

	BVHReader::expect("CHANNELS", line );
	sscanf( line,"CHANNELS %d %s %s %s %s %s %s",&nchannels,
		 &chtype[0], &chtype[1], &chtype[2], &chtype[3], &chtype[4], &chtype[5]);
	if (nchannels==6) {
		if (strcmp(chtype[0],"Xposition") || strcmp(chtype[1],"Yposition") || strcmp(chtype[2],"Zposition")) {
			printf("Warning: Non-typical translation order: '%s'\n", line );
			return;
		}
		transJoint = true;                // flag this joint as having XYZ translations
	}
	//check_channel_names();                // ensure all rotational channels are correctly named

	chOffset = *nchans;             // record starting channel number

	(*nchans) += nchannels;
	BVHReader::nextline( line, fp );

	while (1) {
		if (!strncmp(line,"JOINT",5)) {
			sscanf(line,"JOINT %s",childName);
			BVHReader::nextline( line, fp );
			child[nchildren] = new BVHJoint(childName, nchans, maxOffset, line, fp );
			nchildren++;
			continue;
		} else if (!strncmp( line,"End Site",8)) {
			BVHReader::nextline( line, fp );
			BVHReader::expect("{", line );
			BVHReader::nextline( line, fp );
			BVHReader::expect("OFFSET", line );
			sscanf( line,"OFFSET %f %f %f",&endLength[0], &endLength[1], &endLength[2]);
			endSite = 1;
			BVHReader::nextline( line, fp );
			BVHReader::expect("}", line );
			BVHReader::nextline( line, fp );
			break;
		} else {
			break;
		}
	}
	BVHReader::expect( "}", line );
	BVHReader::nextline( line, fp );
}
/*
   Get facing directions.
*/
float BVHJoint::getFaceDir(float *frameData)
{
	glPushMatrix();
	glLoadIdentity();
		  // apply rotations
	for (int r=3; r<6; r++) {
		float th = frameData[chOffset+r];
		switch(chtype[r][0]) {
		case 'X':
			glRotatef(th,1,0,0);
			break;
		case 'Y':
			glRotatef(th,0,1,0);
			break;
		case 'Z':
			glRotatef(th,0,0,1);
			break;
		default:
			printf("ch type '%c': error\n",chtype[r][0]);
			return 0;
		}
	}
	float m[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, m);
	float faceX = m[0];
	float faceY = m[1];
	float faceZ = m[2];
//	printf("faceX=%6.3f faceY=%6.3f faceZ=%6.3f\n",faceX,faceY,faceZ);
	glPopMatrix();
	return 180.0 / PI * atan2(-faceZ, faceX);   // return angle
}

void BVHJoint::draw(float *frameData, bool translate) const
{
	glPushMatrix(); 
	int nc = 0;

    // apply joint offset (zero for the root joint)
	glTranslatef(offset[0],offset[1],offset[2]);

	float x = (translate && transJoint) ? frameData[chOffset] : 0.0f;
	float y = transJoint ? frameData[chOffset+1] : 0.0f;
	float z = (translate && transJoint) ? frameData[chOffset+2] : 0.0f;
		
	glTranslatef(x,y,z);

	nc += 3;

	// apply Euler rotations in order specified by skeleton
	for (int r=nc; r<nchannels; r++) {
		float th = frameData[chOffset+r];  // theta 
		switch(chtype[r][0]) {
		case 'X':
			glRotatef(th,1,0,0);
			break;
		case 'Y':
			glRotatef(th,0,1,0);
			break;
		case 'Z':
			glRotatef(th,0,0,1);
			break;
		default:
			printf("ch type '%c': error\n",chtype[r][0]);
			return;
		}
	}
	
	if (endSite) {
		glBegin(GL_LINES);
		glVertex3f(0,0,0);
		glVertex3f(endLength[0],endLength[1],endLength[2]);
		glEnd();
	}

	for (int n=0; n<nchildren; n++)
	{
		child[n]->drawChild(frameData);
	}

	glPopMatrix();
}

void BVHJoint::drawChild(float *frameData) const
{
	glPushMatrix();
	int nc = 0;       // number of channels already processed

	glBegin( GL_POINTS ); {
		glVertex3f( 0.0f, 0.0f, 0.0f );
	} glEnd();

	// draw line from parent joint to current joint
	glBegin(GL_LINES);
	glVertex3f(0,0,0);
	glVertex3f(offset[0],offset[1],offset[2]);
	glEnd();

    // apply joint offset (zero for the root joint)
	glTranslatef(offset[0],offset[1],offset[2]);

	// apply translation channels (typically only for root joint)
	if ( transJoint) {
		float x = frameData[chOffset];
		float y = frameData[chOffset+1];
		float z = frameData[chOffset+2];
		glTranslatef(x,y,z);
		nc += 3;
	}
	  // apply Euler rotations, in order specified by skeleton
	for (int r=nc; r<nchannels; r++) {
		float th = frameData[chOffset+r];  // theta 
		switch(chtype[r][0]) {
		case 'X':
			glRotatef(th,1,0,0);
			break;
		case 'Y':
			glRotatef(th,0,1,0);
			break;
		case 'Z':
			glRotatef(th,0,0,1);
			break;
		default:
			printf("ch type '%c': error\n",chtype[r][0]);
			return;
		}
	}
	
	  // draw line from joint to end site (if necessary)
	if (endSite) {
		//glLineWidth(3);
		//glColor3f(0.8,0.4,0);
		glBegin(GL_LINES);
		glVertex3f(0,0,0);
		glVertex3f(endLength[0],endLength[1],endLength[2]);
		glEnd();
	}

	  // recursively draw all child joints
	for (int n=0; n<nchildren; n++)
		child[n]->drawChild(frameData);
	glPopMatrix();
}