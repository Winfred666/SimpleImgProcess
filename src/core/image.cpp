#include <iostream>
#include <stdlib.h>
#include "image.h"

Image::Image(int w,int h,int bitCounts,ColorMode mode):_w(w),_h(h),_mode(mode){
        if(bitCounts%4!=0)
            throw "The number of bits stored per pixel of the image being read is not a multiple of 4!\n";
        this->_byteCounts=bitCounts/8;
        this->_img=(Byte *)malloc(w*h*this->_byteCounts);
}

Image::Image(Image &img):_w(img._w),_h(img._h),_mode(img._mode),_byteCounts(img._byteCounts){
    this->_img=(Byte *)malloc(img.size());
    memcpy(this->_img,img._img,img.size());
}

void Image::readFile(FILE *fp){
    fread(this->_img,sizeof(Byte),size(),fp);
}

//get pointer position of certain pixel, need to use bitCounts,could be NULL if w and h out of range.
Byte * Image::getPixel(int x,int y){
    if(x<0 || x>=this->_w || y<0 || y>=this->_h){
        throw "Access pixel out of bound!\n";
    }
    return this->_img+((y*this->_w*this->_byteCounts)+(x*this->_byteCounts));
}

//do operation to all pixel, also need to use bitCounts to make sure operating zone.
void Image::iterateAll(PixelWalker walker){
    for(int x=0;x<this->_w;x++){
        for(int y=0;y<this->_h;y++){
            walker(getPixel(x,y),this->_mode);
        }
    }
}

Image* Image::splitChannel(int channel){
    if(channel<0 || channel>=this->_byteCounts) throw "try to split unexist image channel";
    
    Image *ret=new Image(this->_w,this->_h,8,GRAY);
    int k=0;
    for(int x=0;x<this->_w;x++){
        for(int y=0;y<this->_h;y++){
            *(ret->_img+k)=*(getPixel(x,y)+channel);
            k++;
        }
    }
    return ret;
}

int Image::bytePerPixel(){
	return this->_byteCounts;
}

int Image::height(){
    return this->_h;
}

int Image::width(){
    return this->_w;
}

int Image::size(){
    return this->_h*this->_w*this->_byteCounts;
}

const Byte * Image::getDataPtr(){
    return this->_img;
}

Image::~Image(){
    free (this->_img);
}


//because we have RGB/BGR color mode, so need to find the exact R index.
int getR(ColorMode mode){
    switch (mode)
    {
    case RGB:
    case RGBA:
        return 0;
    case BGR:
    case BGRA:
        return 2;
    default:
        printf("warning: try to find R channel in non-RGB color mode");
        return 0;
    }
}

int getG(ColorMode mode){
    switch (mode)
    {
    case RGB:
    case RGBA:
    case BGR:
    case BGRA:
        return 1;
    default:
        printf("warning: try to find G channel in non-RGB color mode");
        return 0;
    }
}

int getB(ColorMode mode){
    switch (mode)
    {
    case RGB:
    case RGBA:
        return 2;
    case BGR:
    case BGRA:
        return 0;
    default:
        printf("warning: try to find B channel in non-RGB color mode");
        return 0;
    }
}

bool isRGB(ColorMode mode){
    if(mode==RGB||mode==RGBA||mode==BGR||mode==BGRA) return true;
    return false;
}