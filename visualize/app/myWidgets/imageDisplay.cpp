#include "imageDisplay.h"
#include "imgModelWrapper.h"

void ImageDisplay::draw() {
	if (_model == NULL) {
		fl_color(labelcolor());
		fl_draw("Click to Open",_x,_y,_w,_h,FL_ALIGN_CENTER);
		return;
	}
	fl_overlay_clear();
	calcPadding();
	for (int x = 0; x < _w; x++) {
		for (int y = 0; y < _h; y++) {
			if (x > _padX && x<_w - _padX && y>_padY && y < _h - _padY) {
				const Byte *pix = _model->getPixel(x-_padX, y - _padY);
				//for image out of range, draw back ground color.
				fl_color(pix[2], pix[1], pix[0]);
				//y direction is opposite.
				fl_point(_x + x, _y + _h - 1 - y);
			}
		}
	}
}

void ImageDisplay::setImage(Image *src) {
	//build bridge.
	_model = new ImgModelWrapper(*src,_w,_h);
}

void ImageDisplay::calcPadding() {
	//set info of image
	if (_model->width() < _w)
		_padX = (_w - _model->width() + 1) / 2;
	else
		_padX = 0;
	if (_model->height() < _h)
		_padY = (_h - _model->height() + 1) / 2;
	else
		_padY = 0;
}

ImageDisplay::ImageDisplay(int x, int y, int w, int h) :
	Fl_Widget(x, y, w, h),
	_x(x), _y(y), _w(w), _h(h), _model(NULL){
}

int ImageDisplay::handle(int e) {
	switch (e) {
	case FL_PUSH:
		//when pressed, handle to proxy.
		ImgEventHandler_ns::imgMouseDown(*_model);
		break;
	case FL_RELEASE:
		ImgEventHandler_ns::imgMouseUp(*_model);
		break;
	case FL_MOUSEWHEEL:
		ImgEventHandler_ns::imgScroll(*_model);
		break;
	}
	//update model.
	if (_model->isUpdated) {
		redraw();
		_model->isUpdated = false;
	}
	return 1;
}