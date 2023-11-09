#ifndef IMAGE_H
#define IMAGE_H

#include <iostream>
#include <stdlib.h>
//indicate mode when color encoding is ambigious.
enum ColorMode{
	RGB,RGBA,BGR,BGRA,HSV,YUV,GRAY
};

typedef unsigned char Byte;

typedef void (*PixelWalker)(Byte *pixel,ColorMode mode);

class Image{
private:
	//first dimention,x.
	int _w;
	//second dimention,y.
	int _h;

	//store all img data, when do address addition, only move one bit.
	Byte *_img;

	//indicate byte per pixel.
	int _byteCounts;
public:
	ColorMode _mode;
	
	Image(int w,int h,int bitCounts,ColorMode mode);
    Image(Image &img);
    //fill image data using file.
    void readFile(FILE *fp);

	//get pointer position of certain pixel, need to use bitCounts,could be NULL if w and h out of range.
	Byte * getPixel(int x,int y);

	//core function
    //operate every pixel, also need to use bitCounts to make sure operating zone.
	void iterateAll(PixelWalker iterate);

	//get a h*w size byte array, store one channel of this image.
	Image* splitChannel(int channel);
	//important, know the number of byte one pixel has.
	int bytePerPixel();

	int height();
	
	int width();

    //count _img size by bytes.
    int size();

	const Byte *getDataPtr();

	~Image();
};
//because we have RGB/BGR color mode, so need to find the exact R/G/B index.
int getG(ColorMode);

int getB(ColorMode);

int getR(ColorMode);

bool isRGB(ColorMode);
#endif