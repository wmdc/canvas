/*
 * Pose.h
 *
 * A single character pose - a frame from a character animation.
 *
 * by Michael Welsman-Dinelle 14/07/2008
 */
#pragma once

#include "BVH.h"

#include <GL/glut.h>
#include <vector>

class Pose {
public:
	Pose( Motion &m, int frame );
	Pose( std::vector<Pose *> poses, std::vector<float> weights );
	Pose( Pose &p );
	Pose( unsigned nChannels, float _maxOffset, float *_data, BVHJoint *_skeleton ) : channelCount(nChannels), maxOffset(_maxOffset), data(_data), skeleton(_skeleton) {}
    ~Pose();

	virtual void draw() const;

	unsigned getChannelCount() const { return channelCount; }
	
	float getJointAngle( int index ) const { return data[ index ]; }

	float getMaxOffset() const { return maxOffset; }

	BVHJoint *getSkeleton() const { return skeleton; }

	bool comparableTo( Pose otherPose ) {
		return skeleton == otherPose.getSkeleton();
	}

	float *data;
private:
	unsigned channelCount;

	float maxOffset; //farthest distance of any vertex from the root
	
	BVHJoint *skeleton;
};