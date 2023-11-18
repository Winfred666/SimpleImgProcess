#ifndef IMGEVENTHANDLER_H
#define IMGEVENTHANDLER_H

#include "image.h"
#include "imgModelWrapper.h"
#include <FL/Fl.H>

enum EditorState{DRAGGING, MAGNIFIER,NOTHING};
//has important status, only handling function according to status of editor.
namespace ImgEventHandler_ns {
	void imgMouseDown(ImgModelWrapper &model, int ex=Fl::event_x(), int ey = Fl::event_y());
	extern int lastPosi[2];
	extern EditorState globalState;
	void dragCallback(void *model);
	void imgMouseUp(ImgModelWrapper &model, int ex = Fl::event_x(), int ey = Fl::event_y());
	void imgScroll(ImgModelWrapper &model, int scroll=Fl::event_dy());
}


#endif