#pragma once

#include <ctime>
#include <vector>

class BVHJoint;
class Pose;

/* Represents a motion clip, i.e. a series of poses of a model. */
class Motion {
public:
	Motion( int nchannels, int nframes, float dt, float maxOffset, BVHJoint *root, float *data, float *faceDir );
	Motion( std::vector<Pose *> poses, std::vector<clock_t> tSample, float dt ); //from poses sampled in time
	Motion( std::vector<Motion *> motion );
	Motion( const Motion &m );
    /* Construct a motion that is a clip taken from an existing motion. */
	Motion( const Motion &m, const int startFrame, const int endFrame );

	~Motion();

	Motion& operator=(const Motion& rhs);

	/* Appends the given motion to the end of this motion. */
	void operator +=(const Motion &appendMotion);
	void operator +=(const Motion *appendMotion) { if( appendMotion) operator +=(*appendMotion); }

	
	void draw( unsigned frame, bool applyTranslation = true ) const; /* Draw a frame of animation */

	void drawPose(unsigned frame) const;

	std::string getLabel() const {
		return label;
	}

	void setLabel( std::string s ) {
		label = s;
	}

	unsigned getFrameCount() const { return m_nframes; }
	unsigned getChannelCount() const { return m_nchannels; }

	float getMaxOffset() const { return m_maxOffset; }

	/* What frame of the animation would play at the given time in ms (looping)? */
	unsigned getFrame( time_t dt );

	float getRuntime() const {
		return m_dt * m_nframes;
	}
	float getFrameTime() const {
		return m_dt;
	}
	double getFrameRate() const {
		return m_rate;
	}

	float getData( int index ) const {
		return m_data[index];
	}

	float getFacing( int frame ) const {
		return m_faceDir[frame];
	}

	BVHJoint *getRoot() const {
		return m_root;
	}

    /* Get the position of a joint at a given frame. */
	//void setFocalJoint( const char *jName );
	//Point3D getFocalPoint( int frame );

	Motion *getSubmotion( unsigned startFrame, unsigned endFrame ) const; //DEPRECATED
	/* Append the given motion to the end of the current motion. */
	void append( Motion &clip );
    /* Add keyframes interpolated at runtime */
    void addKeyPose( Pose *p, int frame );
	void removeKeyPose( Pose *p );

	static const double KEYFRAME_BLEND_TIME;  //how long is keyframe blending?
	static const int BLEND_FRAMES = 25;

	static const int LOCAL_CHANNEL_START = 6;

	static const int CHANNEL_INDEX_ROT_Z = 3;
	static const int CHANNEL_INDEX_ROT_Y = 4;
	static const int CHANNEL_INDEX_ROT_X = 5;
private:
	unsigned m_nchannels, m_nframes;
	float m_dt, m_rate, m_maxOffset, drawScale;
	BVHJoint *m_root;
	float *m_data, *m_faceDir;
	float *m_roty;
	bool m_useRoty;

	std::string label;
};