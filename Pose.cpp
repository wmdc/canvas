#include "precompiled.h"

#include "Pose.h"

Pose::Pose( Motion &m, int frame ) : channelCount( m.getChannelCount() ), skeleton( m.getRoot() ), maxOffset( m.getMaxOffset() ) {
	data = new float[ channelCount ];

	for( unsigned i = 0; i < channelCount; i++ ) { //copy over the joint angles for one frame
		data[i] = m.getData( frame * channelCount + i );
	}
}

Pose::Pose( std::vector<Pose *> poses, std::vector<float> weights ) : maxOffset( poses[0]->getMaxOffset() ) {
	if( poses.empty() ) return;

	channelCount = poses[0]->getChannelCount();
	skeleton = poses[0]->getSkeleton();
	data = new float[ channelCount ];

	float weightTotal = 0.0f;

	for( unsigned i = 0; i < weights.size(); i++ ) {
		weightTotal += weights[i];
	}

	for( unsigned i = 0; i < channelCount; i++ ) {
		data[i] = 0;
		for( unsigned j = 0; j < poses.size(); j++ ) {
			data[i] += poses[j]->getJointAngle(i) * ( weights[j] / weightTotal );
		}
	}
}

Pose::Pose( Pose &p ) : channelCount( p.getChannelCount() ), skeleton( p.getSkeleton() ), maxOffset( p.getMaxOffset() ) {
    data = new float[ channelCount ];

	for( unsigned i = 0; i < channelCount; i++ ) { //copy over the joint angles for one frame
		data[i] = p.getJointAngle( i );
	}
}

Pose::~Pose() {
	delete [] data;
}

void Pose::draw() const {
	glLineWidth( 2.0 );
	glPointSize( 2.0 );

	glPushMatrix(); {
		glScalef( 2.0f / maxOffset, 2.0f / maxOffset, 2.0f / maxOffset );
	    skeleton->draw( data, false );
	} glPopMatrix();
}