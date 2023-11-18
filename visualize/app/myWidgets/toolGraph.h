#ifndef  TOOLGRAPH_H
#define TOOLGRAPH_H

#include <FL/Fl_Widget.H>
#include <FL/fl_draw.H>
#include <FL/fl_ask.H>
#include "geometry.h"

class ToolGraph :public Fl_Widget {
private:
	int _x, _y, _w, _h;
	//select zone, currently must be a rectangle zone.
	BoundingBox _select;
public:
	ToolGraph(int x, int y, int w, int h);

	void draw();
};

#endif // ! TOOLGRAPH_H
