#include "imgEventHandler.h"
#include <Windows.h>

namespace ImgEventHandler_ns {
	int lastPosi[2] = { 0,0 };
	EditorState globalState = NOTHING, lastState = NOTHING;
}

void ImgEventHandler_ns::imgMouseDown(ImgModelWrapper &model, int ex, int ey) {
	if (Fl::event_key(' ')) {
		lastState = globalState;
		globalState = DRAGGING;
	}
	else {
		globalState = lastState;
	}
	//switch editor status
	switch (globalState)
	{
	case MAGNIFIER:
		//add up scale.
		model.scale_view(1 + 0.2);

		break;
	case DRAGGING:
		//initialize and begin to drag.
		lastPosi[0] = ex;
		lastPosi[1] = ey;
		if (! Fl::has_timeout(dragCallback, &model))
			Fl::add_timeout(0.4, dragCallback, &model);
		break;
	default:
		break;
	}
}

void ImgEventHandler_ns::dragCallback(void *model) {
	int ex = Fl::event_x();
	int ey = Fl::event_y();
	//press space, move canva.
	//continue dragging(y is inversed)
	((ImgModelWrapper*)model)->translate_view(ex-lastPosi[0],lastPosi[1]-ey);
	OutputDebugString("drag\n");
	lastPosi[0] = ex;
	lastPosi[1] = ey;
	Fl::repeat_timeout(0.4, dragCallback, model);
}

void ImgEventHandler_ns::imgMouseUp(ImgModelWrapper &model, int ex, int ey) {
	//remove drag timeout.
	if (Fl::has_timeout(dragCallback, &model)) {
		Fl::remove_timeout(dragCallback, &model);
	}
}

void ImgEventHandler_ns::imgScroll(ImgModelWrapper &model, int scroll) {
	//scale
	if (Fl::event_ctrl) {
		float ratio = -scroll / 6.0;
		model.scale_view(1+ratio);
		//TODO:translate after scale,according to mouse position.
	}
}

