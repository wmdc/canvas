#include "precompiled.h"

#include "BrushPalette.h"

#include "UtilityConstants.h"
#include "DrawingConstants.h"

#include <GL/glut.h>

#include <cmath>
#include <iostream>

const double BrushPalette::CORNER_BEVEL_SIZE_RATIO = 0.1;

void BrushPalette::draw() {
	//update scrolling animation
	if( scrollLeft ) {
		time_t curTime = clock();
		dx -= ( scrollTime - curTime ) / TICKS_PER_PIXEL_SCROLLED;
		scrollTime = curTime;
		if( dx > 0 ) 
			dx = 0;
	} else if( scrollRight ) {
		time_t curTime = clock();
		dx += ( scrollTime - curTime ) / TICKS_PER_PIXEL_SCROLLED;
		scrollTime = curTime;
		if( -dx > contentWidth - width + 2 * SCROLL_ARROW_PADDING ) dx = -(contentWidth - width + 2 * SCROLL_ARROW_PADDING );
	}

	double bevelSize = height * CORNER_BEVEL_SIZE_RATIO;

	clock_t tNow = clock();
	double flash = 0;//tNow - addTime > TICKS_ADD_MOTION_FLASH ? 0 : 1 - ( tNow - addTime ) / (float)TICKS_ADD_MOTION_FLASH;

	//draw points for rounded corners
	glPointSize( bevelSize * 2 );
	glBegin( GL_POINTS ); {
		glColor4d( COLOR_PALETTE_LIGHT_O[0] + flash, COLOR_PALETTE_LIGHT_O[1] + flash, COLOR_PALETTE_LIGHT_O[2] + flash / 2, COLOR_PALETTE_LIGHT_O[3] + flash / 2 );//glColor4d( 0.4, 0.4, 0.4, 1.0 );
		glVertex2i( location.x + bevelSize, location.y + height - bevelSize );
		glVertex2i( location.x + width - bevelSize, location.y + height - bevelSize );
		glColor4d( COLOR_PALETTE_DARK_O[0] + flash, COLOR_PALETTE_DARK_O[1] + flash, COLOR_PALETTE_DARK_O[2] + flash / 2, COLOR_PALETTE_DARK_O[3] + flash / 2 );//glColor4d( 0.2, 0.2, 0.2, 1.0 );
		glVertex2i( location.x + width - bevelSize, location.y + bevelSize );
		glVertex2i( location.x + bevelSize, location.y + bevelSize );
	} glEnd();


	
	glPushMatrix();
	glBegin( GL_POLYGON ); {
	  glColor4d( COLOR_PALETTE_DARK_O[0] + flash, COLOR_PALETTE_DARK_O[1] + flash, COLOR_PALETTE_DARK_O[2] + flash / 2, COLOR_PALETTE_DARK_O[3] + flash / 2 );
	  glVertex2i( location.x + bevelSize, location.y );
	  glVertex2i( location.x + width - bevelSize, location.y );
	  glVertex2i( location.x + width, location.y + bevelSize );
	  glColor4d( COLOR_PALETTE_LIGHT_O[0] + flash, COLOR_PALETTE_LIGHT_O[1] + flash, COLOR_PALETTE_LIGHT_O[2] + flash / 2, COLOR_PALETTE_LIGHT_O[3] + flash / 2 );
	  glVertex2i( location.x + width, location.y + height - bevelSize );
      glVertex2i( location.x + width - bevelSize, location.y + height );
	  glVertex2i( location.x + bevelSize, location.y + height );
      glVertex2i( location.x, location.y + height - bevelSize );
      glColor4d( COLOR_PALETTE_DARK_O[0] + flash, COLOR_PALETTE_DARK_O[1] + flash, COLOR_PALETTE_DARK_O[2] + flash / 2, COLOR_PALETTE_DARK_O[3] + flash / 2 );
	  glVertex2i( location.x, location.y + bevelSize );
	} glEnd();
	glPopMatrix();

	//draw the brushes (scrolled)
	glEnable( GL_SCISSOR_TEST );
	glScissor( location.x + SCROLL_ARROW_PADDING, location.y, width - SCROLL_ARROW_PADDING * 2, height );

	double vPort[4];
	glGetDoublev( GL_VIEWPORT, vPort ); //save viewport
	//glViewport( location.x, location.y, width, height ); //draw only overtop palette

    glPushMatrix();
	  glTranslated( location.x, location.y, 0 );
	  glTranslated( dx + SCROLL_ARROW_PADDING, BRUSH_DRAW_PADDING, 0 );
	  for( unsigned i = 0; i < brushes.size(); i++ ) {
		  brushes[i]->draw();
		  glTranslated( brushSize + BRUSH_DRAW_PADDING, 0, 0 );
	  }
	glPopMatrix();

	glDisable( GL_SCISSOR_TEST );

	if( scroll ) {
	//draw the scroll arrows

	//draw  the arrow outlines
	glLineWidth( 0.10 );
	glPushMatrix();
	glTranslated( location.x, location.y, 0.0 );
	if( leftScrollHover ) {
		glColor3d( 0.7, 0.7, 0.0 );
	} else {
		glColor3d( 0.6, 0.6, 0.6 );
	}
	glBegin( GL_LINE_LOOP );
	  glVertex2i( 2*SCROLL_ARROW_PADDING/3, height/2 + SCROLL_ARROW_PADDING/3 );
	  glVertex2i( SCROLL_ARROW_PADDING/3, height/2 );
	  glVertex2i( 2*SCROLL_ARROW_PADDING/3, height/2 - SCROLL_ARROW_PADDING/3 );
	glEnd();

	if( dx != 0 ) {
		glBegin( GL_POLYGON );
	      glVertex2i( 2*SCROLL_ARROW_PADDING/3, height/2 + SCROLL_ARROW_PADDING/3 );
	      glVertex2i( SCROLL_ARROW_PADDING/3, height/2 );
	      glVertex2i( 2*SCROLL_ARROW_PADDING/3, height/2 - SCROLL_ARROW_PADDING/3 );
	    glEnd();
	}

    if( rightScrollHover ) {
		glColor3d( 0.7, 0.7, 0.0 );
	} else {
		glColor3d( 0.6, 0.6, 0.6 );
	}
	glBegin( GL_LINE_LOOP );
	  glVertex2i( width - 2*SCROLL_ARROW_PADDING/3, height/2 + SCROLL_ARROW_PADDING/3 );
	  glVertex2i( width - SCROLL_ARROW_PADDING/3, height/2 );
	  glVertex2i( width - 2*SCROLL_ARROW_PADDING/3, height/2 - SCROLL_ARROW_PADDING/3 );
	glEnd();

	if( -dx < contentWidth - width + 2 * SCROLL_ARROW_PADDING ) {
		glBegin( GL_POLYGON );
	      glVertex2i( width - 2*SCROLL_ARROW_PADDING/3, height/2 + SCROLL_ARROW_PADDING/3 );
	      glVertex2i( width - SCROLL_ARROW_PADDING/3, height/2 );
	      glVertex2i( width - 2*SCROLL_ARROW_PADDING/3, height/2 - SCROLL_ARROW_PADDING/3 );
	    glEnd();
	}
	glPopMatrix();
	}
}

void BrushPalette::add( Brush *b, bool flash ) {
	brushes.push_back( b );
	b->setSize( brushSize, brushSize );

	if( brushes.size() == 1 ) {
		contentWidth += brushSize;
	} else {
		contentWidth += brushSize + BRUSH_DRAW_PADDING;
	}

	if( !scroll ) {
		width = contentWidth + 2 * SCROLL_ARROW_PADDING + 2;
	}

	if( flash ) {
		addTime = clock();
	}
}

void BrushPalette::select( Brush *b ) {
	b->select();
	for( unsigned i = 0; i < brushes.size(); i++ ) {
		if( brushes[i] != b ) {
			brushes[i]->deselect();
		}
	}
}
void BrushPalette::displaySelected( int index ) {
    for( unsigned i = 0; i < brushes.size(); i++ ) {
		brushes[i]->setSelectionDisplay( index == i );
	}
}

bool BrushPalette::contains( Point2D p ) const {
    return location.x <= p.x && location.x+width >= p.x &&
		   location.y <= p.y && location.y+height >= p.y;
}

void BrushPalette::mouseDown( int button, Point2D p, bool doubleClick ) {
	if( !contains(p) ) {
		return;
	}

	dragging = true;
	dragPt = p;

	if( scroll && p.x <= location.x + SCROLL_ARROW_PADDING ) {
		scrollTime = clock();
		scrollRight = false;
		scrollLeft = true;
		return;
	} else if( scroll && p.x >= location.x + width - SCROLL_ARROW_PADDING ) {
		scrollTime = clock();
		scrollRight = true;
		scrollLeft = false;
		return;
	}

	Point2D pRel = Point2D( p.x - location.x - dx - SCROLL_ARROW_PADDING, p.y - location.y );
	for( unsigned i = 0; i < brushes.size(); i++ ) {
		if( brushes[i]->contains( pRel ) ) {
			brushes[i]->mouseDown( button, pRel, doubleClick );
			return;
		}
		pRel.x -= brushSize + BRUSH_DRAW_PADDING;
	}
}

void BrushPalette::mouseUp( int button, Point2D p ) {
    dragging = false;

	Point2D pRel = Point2D( p.x - location.x - dx - SCROLL_ARROW_PADDING, p.y - location.y );
	for( unsigned i = 0; i < brushes.size(); i++ ) {
		brushes[i]->mouseUp( button, pRel );
		pRel.x -= brushSize + BRUSH_DRAW_PADDING;
	}

	scrollRight = false;
	scrollLeft = false;
}

void BrushPalette::mouseMove( Point2D p ) {
	if( dragging ) {
	    location += p - dragPt;
        dragPt = p;
	}

	if( contains( p ) && p.x <= location.x + SCROLL_ARROW_PADDING ) {
		leftScrollHover = true;
	} else {
		leftScrollHover = false;
	}
	if( contains( p ) && p.x >= location.x + width - SCROLL_ARROW_PADDING ) {
		rightScrollHover = true;
	} else {
		rightScrollHover = false;
	}
	
	//"outside" if not contained - don't mouse over invisible scrolled buttons!
	Point2D pRel = contains(p) ? Point2D( p.x - location.x - dx - SCROLL_ARROW_PADDING, p.y - location.y ) : Point2D(-1,-1);
	for( unsigned i = 0; i < brushes.size(); i++ ) {
		brushes[i]->mouseMove( pRel );
		pRel.x -= brushSize + BRUSH_DRAW_PADDING;
	}
}