/*
   PointList.h

   Stores a list of Point2D items.

   by Michael Welsman-Dinelle, 19/04/2009
*/

#include <vector>
#include "Point2D.h"

class PointList {
public:
	PointList() {}
	
	void add( Point2D p ) {
		list.push_back( p );
	}
	Point2D *get( int index ) {
		if( index >= list.size() || index < 0 ) return null;
		return &list.at( index );
	}
	Point2D *head() {
		if( list.size() == 0 ) return null;
		return list[list.size()-1];
	}
	Point2D *tail() {
		if( list.size() == 0 ) return null;
		return list[0];
	}
private:
	std::vector<Point2D> list;
};