#ifndef IMGMODELWRAPPER_H
#define IMGMODELWRAPPER_H

//in fact model transform go through geometry transform
#include "image.h"
#include "geometry.h"

class ImgModelWrapper{
private:
	//_src is hide behind this image, as real data resource;
	Image *_src;
	
	//get from _zone, for quicker draw
	const Image *_outputBuf;
	const float *_offset;
	TransZone *_zone;
	
	int _viewW, _viewH;
	void postChange_view() {
		_outputBuf = _zone->getOriOutput();
		isUpdated = true;
	}
public:
	bool isUpdated;

	ImgModelWrapper(Image &img,int viewW,int viewH) : _src(&img),isUpdated(false),_viewH(viewH),_viewW(viewW){
		_zone = new TransZone(&img);
		_outputBuf = _zone->getOriOutput();
		_offset = _zone->getCanvasOffset();
	}

	//only view method, no change to _src.
	//view moving.
	inline const Byte* getPixel(int x, int y) {
		return _outputBuf->getPixel(x-_offset[0], y-_offset[1]);
	}

	void getImageBox(BoundingBox &box) {
		//if an edge crush boundary, stop moving.
		_zone->getBoudingBox(box);
		for (int q = 0; q < 4; q++) {
			box[q][0] += _offset[0];
			box[q][1] += _offset[1];
		}
	}

	void translate_view(int tx, int ty) {
		TransMatrix mat;
		BoundingBox box;
		getImageBox(box);
		if ((tx > 0 && box[0][0]>=0) || (tx < 0 && box[2][0] <= _viewW)) {
			tx = 0;
		}
		if ((ty > 0 && box[0][1] >= 0) || (ty < 0 && box[2][1] <= _viewH)) {
			ty = 0;
		}
		if (tx == 0 && ty == 0) return;
		getTranslateMat(mat, tx, ty);
		_zone->applyTrans(mat);
		postChange_view();
	}

	void scale_view(float ratio) {
		TransMatrix mat;
		getScaleMat(mat, ratio, ratio);
		_zone->applyTrans(mat);
		postChange_view();
	}

	void rotate_view(float angle) {
		TransMatrix mat;
		getRotateMat(mat, angle);
		_zone->applyTrans(mat);
		postChange_view();
	}

	inline int width() {
		return _outputBuf->width();
	}
	inline int height(){
		return _outputBuf->height();
	}

	~ImgModelWrapper() {
		delete _zone;
	}
};

#endif
