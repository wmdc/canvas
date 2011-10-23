#include "CanvasItem.h"

class PoseHighlightItem : public CanvasItem {
public:
	PoseHighlightItem() {
		hover = false;
	}

	void processMouse( int mode, int mType, float x, float y );

    void draw() const;

	int type() const { return CanvasItem::TYPE_POSE_HIGHLIGHT }
private:
	double x, y;
};