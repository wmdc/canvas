/*
   ControlBrush.h

   Simple buttons used to change the canvas mode.

   by Michael Welsman-Dinelle
*/

#pragma once

#include "Brush.h"

class ControlBrush : public Brush {
public:
	ControlBrush( int mode, bool _selected, Canvas &_c, BrushPalette &_p );

	void drawIcon() const;

	void select();
	void deselect();

	static void initTextures();
private:
	int mode; //canvas mode triggered by this brush
};