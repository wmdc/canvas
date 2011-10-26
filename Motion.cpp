#include "precompiled.h"

#include "Motion.h"
#include "BVH.h"
#include "Pose.h"

#include "DrawingConstants.h"

#include "oglMatrixMath.h"

#include <iostream>
#include <cstdlib>
#include <vector>
#include <iostream>

using namespace std;

const double Motion::KEYFRAME_BLEND_TIME = 0.3;

const int N_ROOT_TRANS_CHANNELS = 3;
const int N_GLOBAL_CHANNELS = 3;

Motion::Motion( int nchannels, int nframes, float dt, float maxOffset, BVHJoint *root, float *data, float *faceDir )
		: m_nchannels( nchannels ), m_nframes( nframes ), m_dt( dt ), 
	      m_maxOffset( maxOffset ), m_root( root ), m_data( data ), m_faceDir( faceDir ), m_useRoty( false ) {
			  drawScale = 2.0f / maxOffset;

			  label = string("");

			  m_rate = 1.0 / dt;


	// Processing the data : make sure we start at the origin
	float initFaceDir = m_faceDir[0];

	float initPosOri[N_ROOT_TRANS_CHANNELS];

	for( unsigned channel = 0; channel < N_ROOT_TRANS_CHANNELS; channel++ )
	{
		initPosOri[channel] = m_data[channel];
	}

	for( unsigned frame = 0; frame < m_nframes; frame++ )
	{
		m_faceDir[frame] -= initFaceDir;

		for( unsigned channel = 0; channel < N_ROOT_TRANS_CHANNELS; channel++ )
		{
			m_data[frame * m_nchannels + channel] -= initPosOri[channel];
		}
	}
}

Motion::Motion( std::vector<Pose *> poses, std::vector<clock_t> tSample, float dt ) : m_dt(dt), m_useRoty( false ) {
	m_root = poses[0]->getSkeleton();

    m_nchannels = poses[0]->getChannelCount();

	clock_t runtime = tSample.empty() ? 0 : tSample[tSample.size()-1] - tSample[0];

	m_nframes = tSample.empty() ? 0 : runtime / ( dt * CLOCKS_PER_SEC );

    m_data = new float[ m_nchannels * m_nframes ];
	m_faceDir = new float[ m_nframes ];

	int pIndex = 0;
    
	for( unsigned frame = 0; frame < m_nframes; frame++ ) {
        clock_t currentFrameTime = tSample[0] + frame * ( dt * CLOCKS_PER_SEC );
        //increment until we find the next pose defined after the current frame
		while( tSample[ pIndex ] < currentFrameTime && pIndex < (int)poses.size() - 1 ) { pIndex++; }

		float weightBefore, weightAfter;

		if( pIndex - 1 < 0 ) {
			weightBefore = 0; //out of bounds
			weightAfter = 1;
		} else if( tSample[pIndex - 1] == currentFrameTime ) {
			weightBefore = 1; //sampled at exactly this time
			weightAfter = 0;
		} else if( tSample[pIndex] == currentFrameTime ) {
			weightBefore = 0;
			weightAfter = 1; //sampled at exactly this time
		} else { //blend the two based on the inverse of the distance from the sampled time
			weightBefore = 1.0f / ( currentFrameTime - tSample[pIndex-1] );
			weightAfter = 1.0f / ( tSample[pIndex] - currentFrameTime);
		}

		//printf("frame %d {", frame );
		for( unsigned channel = 0; channel < m_nchannels; channel++ ) {
			float poseBefore = pIndex - 1 < 0 ? 0 : poses[pIndex - 1]->getJointAngle( channel );
			float poseAfter = poses[pIndex]->getJointAngle( channel );

			m_data[ frame * m_nchannels + channel ] = 
				( weightBefore * poseBefore + weightAfter * poseAfter ) / ( weightBefore + weightAfter ); //linear blend

			//printf("%f ", m_data[ frame * m_nchannels + channel ] );
		}
		//printf("}\n");
		m_faceDir[frame] = 0.0; //add something more sophisticated here?
	}

	m_maxOffset = poses[0]->getMaxOffset();
	drawScale = 2.0f / m_maxOffset;

	label = string("spatial_keyframe_motion");
	m_rate = 1.0 / dt;

	printf("generatedmotion\n");
}
Motion::Motion( const Motion &m, const int startFrame, const int endFrame ) : m_useRoty( false ) {
	m_root = m.getRoot();
	m_nchannels = m.getChannelCount();
	m_dt = m.getFrameTime();
	m_rate = m.getFrameRate();

	m_maxOffset = m.getMaxOffset();
	drawScale = 2.0f / m_maxOffset;

	m_nframes = endFrame - startFrame;

	m_faceDir = new float[ m_nframes ];
	m_data = new float[ m_nchannels * m_nframes ];

	//We will need to move the figure back to the origin to start
	float origin[3] = {
			m.getData(m_nchannels * startFrame),
			0.0f, //Don't bother with Y-axis
			m.getData(m_nchannels * startFrame + 2)
	};

	//Also need to rotate so facing dir starts at 0
	float originFace = m.getFacing(startFrame);

	for( unsigned int frame = 0; frame < m_nframes; frame++ ) {
		int oldFrame = startFrame + frame;

		for( unsigned int transChan = 0; transChan < 3; transChan++ )
			m_data[ m_nchannels * frame + transChan ] = m.getData( m_nchannels * oldFrame + transChan) - origin[transChan];

		for( unsigned int channel = 3; channel < m_nchannels; channel++ )
			m_data[ m_nchannels * frame + channel ] = m.getData( m_nchannels * oldFrame + channel);

		//apply the facing direction rotation so the motion points the right way
		//rotateEuler(-originFace, 0.0f, 1.0f, 0.0f, 
		//	&m_data[ m_nchannels * frame + 3 ], 
		//	&m_data[ m_nchannels * frame + 4 ], 
		//	&m_data[ m_nchannels * frame + 5 ]);

		m_faceDir[ frame ] = m.getFacing( oldFrame );
	}

	label = m.getLabel();
}

/* Blend together a series of motions */
Motion::Motion( std::vector<Motion *> motion ) : m_useRoty( true ) {
	label = string("combined_motion");
}

/* Copy the motion m */
Motion::Motion( const Motion &m ) : m_root( m.getRoot() ), m_useRoty( false ) {
	m_nchannels = m.getChannelCount();
	m_nframes = m.getFrameCount();
	m_dt = m.getFrameTime();
	m_rate = m.getFrameRate();

	m_maxOffset = m.getMaxOffset();
	drawScale = 2.0f / m_maxOffset;

	m_faceDir = new float[ m_nframes ];
	m_data = new float[ m_nchannels * m_nframes ];

	for( unsigned frame = 0; frame < m_nframes; frame++ ) {
		m_faceDir[ frame ] = m.getFacing( frame );

		for( unsigned channel = 0; channel < m_nchannels; channel++ ) {
			m_data[ frame * m_nchannels + channel ] = m.getData( frame * m_nchannels + channel );
		}
	}
	label = m.getLabel();
}

Motion::~Motion() {
	delete [] m_faceDir;
	delete [] m_data;
}

Motion& Motion::operator=(const Motion& rhs) {
	if (this != &rhs) {  // make sure not same object
		delete [] m_faceDir;
		delete [] m_data;

		m_nchannels = rhs.getChannelCount();
		m_nframes = rhs.getFrameCount();
		m_dt = rhs.getFrameTime();

		m_maxOffset = rhs.getMaxOffset();
		drawScale = 2.0f / m_maxOffset;

		m_faceDir = new float[ m_nframes ];
		m_data = new float[ m_nchannels * m_nframes ];

		for( unsigned frame = 0; frame < m_nframes; frame++ ) {
			m_faceDir[ frame ] = rhs.getFacing( frame );

			for( unsigned channel = 0; channel < m_nchannels; channel++ ) {
				m_data[ frame * m_nchannels + channel ] = rhs.getData( frame * m_nchannels + channel );
			}
		}
		label = rhs.getLabel();
	}
	return *this;
}

void Motion::draw( unsigned frame, bool applyRoot ) const {
	if( frame >= getFrameCount() ) {
		return;
	}

	// Draw motion path
	/*//if(!applyRoot)
	{
		glColor4dv(COLOR_MOTION_PATH);
		glBegin(GL_LINE_STRIP);
		for( int i = 0; i < m_nframes; i++ ) 
		{
			glVertex3f(m_data[i*m_nchannels + 0], m_data[i*m_nchannels + 1], m_data[i*m_nchannels + 2]);
		}
		glEnd();
	}*/

	glColor4dv(COLOR_MOTION);

	glPointSize( 3.0 );
	glLineWidth( 3.0 );

	//Facing direction vector
	/*glPushMatrix(); {
		glRotatef( getFacing( frame ), 0.0f, 1.0f, 0.0f );

		glBegin( GL_LINES );
		  glVertex3f(0.0f, 0.0f, 0.0f);
		  glVertex3f(0.0f, 0.0f, 12.0f);
		glEnd();
	} glPopMatrix();*/

	glPushMatrix(); {
		glScalef(3.0f, 3.0f, 3.0f);
		m_root->draw(m_data + frame * m_nchannels);
	} glPopMatrix();
}

void Motion::drawPose(unsigned frame) const 
{
	if( frame >= getFrameCount() ) {
		return;
	}

	glColor4dv(COLOR_MOTION);

	glPointSize( 3.0 );
	glLineWidth( 3.0 );

	glPushMatrix(); {
		m_root->draw(m_data + frame * m_nchannels, false);
	} glPopMatrix();
}

/*
 *  Return a motion clip that is the sub-piece of this motion on the given
 *  range of frames.
 *
 *  Returns NULL if the motion could not be created.
 */
Motion *Motion::getSubmotion( unsigned startframe, unsigned endframe ) const {
	if( startframe > endframe ) { //swap
		unsigned temp = startframe;
		startframe = endframe;
		endframe = temp;
	}
    
    float *motionData = new float[ (endframe-startframe) * m_nchannels ];
	
	float *faceDir = new float[ m_nframes ];

	if( !motionData || !faceDir ) return NULL;

	unsigned deltaIndex = startframe * m_nchannels;

	for( unsigned index = deltaIndex; index < endframe * m_nchannels; index++ ) {
		motionData[index - deltaIndex] = m_data[index];
	}

	for( unsigned frame = 0; frame < m_nframes; frame++ ) {
		faceDir[ frame ] = m_faceDir[ frame ];
	}

	Motion *m = new Motion( m_nchannels, endframe - startframe, m_dt, m_maxOffset, m_root, motionData, faceDir );

	m->setLabel(std::string("clip"));
	//if( m ) {
		//std::string l( label );
		//l.append( "subclip " );
		//m->setLabel( string("_") );
	//}
	
    //should copy the models and the face dirs!
    return new Motion( m_nchannels, endframe - startframe, m_dt, m_maxOffset, m_root, motionData, faceDir );
	
}
/*
 * Append a motion clip onto this one.
 */
void Motion::append( Motion &clip ) 
{
	//Eventually this function can just be removed.
	operator +=(clip);
}

unsigned Motion::getFrame(time_t dt) {
	if( dt < 0 ) dt = 0;

    time_t msecFrame = 1000 * m_dt;

	unsigned frame = dt / msecFrame;

	return frame >= m_nframes ? 0 : frame;
}

void Motion::operator +=(const Motion &appendMotion)
{
	if( !appendMotion.getRoot() || appendMotion.getRoot()->getChannelCount() != m_root->getChannelCount() )
	{
		fprintf(stderr, "Warning: motion channel count mismatch. Motion was not appended.\n");
		return;
	}

    float *m_data2 = new float[ ( m_nframes + appendMotion.m_nframes ) * m_nchannels ];
	float *m_faceDir2 = new float[ m_nframes + appendMotion.m_nframes ];

	for( unsigned entry = 0; entry < m_nframes * m_nchannels; entry++ )
	   m_data2[entry] = m_data[entry];

	for( unsigned frame = 0; frame < m_nframes; frame++ )
		m_faceDir2[frame] = m_faceDir[frame];

	//How many overlapping frames will we have?
	//Note: cannot have more blend frames than motion frames!
	int nBlendFrames = min(KEYFRAME_BLEND_TIME / m_dt, min(m_nframes, appendMotion.m_nframes));

	//Figure out starting facing dir, position, and orientation for the next clip
	float endFaceDir = m_faceDir[m_nframes - nBlendFrames - 1];

	int finalFrame = m_nframes - nBlendFrames - 1;

	float endX = m_data[m_nchannels*finalFrame + 2];
	float endY = m_data[m_nchannels*finalFrame + 1];
	float endZ = m_data[m_nchannels*finalFrame];

	//append motion
	for( unsigned f = 0; f < appendMotion.m_nframes; f++ )
	{
		//blend weights of the motions
		float wBlend2 = sin((M_PI/2) * min(1.0f, ((float)f)/nBlendFrames));
		float wBlend1 = 1.0f - wBlend2;
		
		//UPDATE GLOBAL TRANSLATION
		//X and Z: apply a 2D rotation in the XZ plane
		//           v[2] = {x,z}
		{
        float x = appendMotion.m_data[appendMotion.m_nchannels*f+2];
		float z = appendMotion.m_data[appendMotion.m_nchannels*f];
		float theta = deg2rad(endFaceDir);
		
		m_data2[m_nchannels*(m_nframes + f - nBlendFrames) + 2] = endX + x*cos(theta) - z*sin(theta); //+v[0]*cos(theta) + v[1]*sin(theta);
		m_data2[m_nchannels*(m_nframes + f - nBlendFrames)] = endZ + x*sin(theta) + z*cos(theta); //- v[0]*sin(theta) + v[1]*cos(theta);

		//Y: blend
		float y1 = (f >= nBlendFrames) ? 0.0f : m_data[m_nchannels*(m_nframes-nBlendFrames+f) + 1];
		float y2 = appendMotion.m_data[appendMotion.m_nchannels*f + 1];
		m_data2[m_nchannels*(m_nframes + f - nBlendFrames) + 1] = wBlend1*y1 + wBlend2*y2;
		}

		//UPDATE GLOBAL ORIENTATION
		float x, y, z;
		if(f < nBlendFrames)
		{
			z = wBlend2*appendMotion.m_data[appendMotion.m_nchannels*f + 3] + wBlend1*m_data[m_nchannels*(m_nframes - nBlendFrames + f) + 3];
			y = wBlend2*appendMotion.m_data[appendMotion.m_nchannels*f + 4] + wBlend1*m_data[m_nchannels*(m_nframes - nBlendFrames + f) + 4];
			x = wBlend2*appendMotion.m_data[appendMotion.m_nchannels*f + 5] + wBlend1*m_data[m_nchannels*(m_nframes - nBlendFrames + f) + 5];
		}
		else
		{
		    z = appendMotion.m_data[appendMotion.m_nchannels*f + 3];
		    y = appendMotion.m_data[appendMotion.m_nchannels*f + 4];
		    x = appendMotion.m_data[appendMotion.m_nchannels*f + 5];
		}

		float m[16];
		
		glPushMatrix();
		glLoadIdentity();

		glRotatef(endFaceDir - appendMotion.getFacing(0), 0.0f, 1.0f, 0.0f);

		glRotatef(z, 0.0f, 0.0f, 1.0f); //could write an optimized function for euler->matrix
		glRotatef(y, 0.0f, 1.0f, 0.0f);
		glRotatef(x, 1.0f, 0.0f, 0.0f);

		glGetFloatv(GL_MODELVIEW_MATRIX, m);
		glPopMatrix();

		//Write the euler angles out to the motion data channels representing global orientation
		columnMajorMatrixToEuler(m, &x, &y, &z);

		m_data2[m_nchannels*(m_nframes + f - nBlendFrames) + 3] = rad2deg(z);
		m_data2[m_nchannels*(m_nframes + f - nBlendFrames) + 4] = rad2deg(y);
		m_data2[m_nchannels*(m_nframes + f - nBlendFrames) + 5] = rad2deg(x);

		//Update local channels
		for( unsigned c = 6; c < appendMotion.m_nchannels; c++)
		{
			if(f < nBlendFrames)
			{
				float appendMotionComponent = wBlend2 * appendMotion.m_data[appendMotion.m_nchannels*f + c];
				float startMotionComponent = wBlend1 * m_data[m_nchannels*(m_nframes - nBlendFrames + f) + c];

				m_data2[m_nchannels*(m_nframes + f - nBlendFrames) + c] = startMotionComponent + appendMotionComponent;
			}
			else
			{
			    m_data2[m_nchannels*(m_nframes + f - nBlendFrames) + c] = appendMotion.m_data[appendMotion.m_nchannels*f + c];
			}
		}

		//Calculate facing direction
		m_faceDir2[m_nframes + f - nBlendFrames] = m_root->getFaceDir(&m_data2[m_nchannels*(m_nframes + f - nBlendFrames)]);
	}

	//Clean up
	delete [] m_data;
	delete [] m_faceDir;

	m_data = m_data2;
	m_faceDir = m_faceDir2;

	m_nframes += appendMotion.getFrameCount() - nBlendFrames;
}