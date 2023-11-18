#ifndef IMAGEDISPLAY_H
#define IMAGEDISPLAY_H

#include <FL/Fl_Widget.H>
#include <FL/fl_draw.H>
#include <FL/fl_ask.H>
#include <FL/Fl.H>
#include "image.h"
#include "imgModelWrapper.h"
#include "imgEventHandler.h"

//only a image displayer, could serve as layer.
class ImageDisplay :public Fl_Widget {
	//_display must be RGB, if color mode is illegal,there must be a bridge
	ImgModelWrapper *_model;
	int _x, _y, _w, _h;
	int _padX, _padY;
public:
	ImageDisplay(int x, int y, int w, int h);

	void setImage(Image *src);
	//instead it was a widgets drawn by pixel.
	void draw();
	void calcPadding();
	//register interact
	int handle(int e);
};


#endif