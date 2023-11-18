#ifndef IMAGE_H
#define IMAGE_H

#include <iostream>
#include <stdlib.h>
#include <string.h>

//indicate mode when color encoding is ambigious.
enum ColorMode{
	RGB,RGBA,BGR,BGRA,HSV,HSI,YUV,GRAY,ONLY_REGION
};

typedef uint8_t Byte;

class Image;

typedef void (*PixelWalker)(Byte *pixel,ColorMode mode);
typedef void (*PixelWalker2)(Byte *pixel,Image* self);
typedef void (*PixelWalker3)(Byte *pixel,int x,int y,Image *self);

class Image;
//trick: take image as a region set, and a pixel is very large.
typedef Image RegionSet;

class Image{
private:
	//first dimention,x.
	int _w;
	//second dimention,y.
	int _h;

	//store all img data, when do address addition, only move one bit.
	Byte *_img;

	//histogram of this image, lazy loading .
	float *_histogram;
	//indicate byte per pixel.
	int _byteCounts;

	//create histogram equals to 255*channel,for gray levels is 255 by default.
	const float * getHistogram();

public:
	static const int ColorLevel=256;
	ColorMode mode;
	
	Image(int w,int h,int bitCounts,ColorMode mode);
    Image(Image &img);
    //fill image data using file.
    void readFile(FILE *fp);
	void readBytes(const Byte *src);
	//set all to 0.
	void clear();

	//get pointer position of certain pixel, need to use bitCounts,could be NULL if w and h out of range.
	inline Byte * getPixel(int x,int y){
		if(x<0 || x>=this->_w || y<0 || y>=this->_h){
			char err[100];
			sprintf(err,"Access pixel (%d , %d) out of bound!\n",x,y);
			throw (std::string)err;
		}
		return this->_img+((y*this->_w*this->_byteCounts)+(x*this->_byteCounts));
	}
	inline const Byte *getPixel(int x, int y) const{
		return this->_img + ((y*this->_w*this->_byteCounts) + (x*this->_byteCounts));
	}

	//core function
    //operate every pixel, also need to use bitCounts to make sure operating zone.
	void iterateAll(PixelWalker iterate);
	void iterateAll(PixelWalker2 iterate);
	void iterateAll(PixelWalker3 iterate);
	//get a h*w size byte array, store one channel of this image.
	Image* splitChannel(int channel);
	
	//important, know the number of byte one pixel has.
	inline int bytePerPixel() const{
		return this->_byteCounts;
	}

	inline int height() const{
		return this->_h;
	}
	
	inline int width() const{
	    return this->_w;
	}

    //count _img size by bytes.
    int size() const;

	const Byte *getDataPtr();

	/**
     * just like cutting meat, get all small region for morphology calculation, remember to delete.
     * use kernal*kernal*size() bytes
     */
    RegionSet* getAllKernals(int kernal,Byte* padding);

    /**
     * split into regions, and every region is a pixel in RegionSet.
    */
    RegionSet* getAllRegions(int xSize,int ySize,Byte* padding);

	Image* getROI(int stax,int stay,int endx,int endy);
	void setROI(int stax,int stay,Image *src);
	~Image();
	
	inline float getHistoRatio(int channel,int colorLevel){
		if(_histogram==NULL) getHistogram();
		// if(channel<0 || channel>_byteCounts || colorLevel>ColorLevel)
		// 	throw "getHistogram: try to access unexist colorLevel or channel";
		return *(_histogram+ (channel*ColorLevel) + colorLevel);
	}
	inline float getHistoAccumu(int channel,int colorLevel){
		if(_histogram==NULL) getHistogram();
		return *(_histogram+((_byteCounts+channel)*ColorLevel) + colorLevel);
	}

	void remakeHistogram();
};
//because we have RGB/BGR color mode, so need to find the exact R/G/B index.
int getG(ColorMode);

int getB(ColorMode);

int getR(ColorMode);

bool isRGB(ColorMode);



#endif