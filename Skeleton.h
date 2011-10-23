#pragma once

#include <string>
#include <vector>

class Skeleton {
public:
	bool operator==(Skeleton rhs) const;

	void drawPose( double *poseData );

	int dof() const;
private:

};
