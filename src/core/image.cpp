#include <iostream>
#include <stdlib.h>
#include "image.h"

Image::Image(int w,int h,int bitCounts,ColorMode mode):_w(w),_h(h),mode(mode){
    if(bitCounts%4!=0)
        throw "The number of bits stored per pixel of the image being read is not a multiple of 4!\n";
    this->_byteCounts=bitCounts/8;
    this->_img=(Byte *)malloc(w*h*this->_byteCounts);
    _histogram=NULL;
}

Image::Image(Image &img):_w(img._w),_h(img._h),mode(img.mode),_byteCounts(img._byteCounts){
    this->_img=(Byte *)malloc(img.size());
    memcpy(this->_img,img._img,img.size());
    _histogram=NULL;
}

void Image::readFile(FILE *fp){
    fread(this->_img,sizeof(Byte),size(),fp);
}

void Image::readBytes(const Byte *src){
    memcpy(_img,src,size());
}


//do operation to all pixel, also need to use bitCounts to make sure operating zone.
void Image::iterateAll(PixelWalker walker){
    for(int y=0;y<_h;y++){
        for(int x=0;x<_w;x++){
            walker(getPixel(x,y),this->mode);
        }
    }
}

void Image::iterateAll(PixelWalker2 iterate){
    for(int y=0;y<_h;y++){
        for(int x=0;x<_w;x++){
            iterate(getPixel(x,y),this);
        }
    }
}


Image* Image::splitChannel(int channel){
    if(channel<0 || channel>=this->_byteCounts) throw "try to split unexist image channel";
    
    Image *ret=new Image(this->_w,this->_h,8,GRAY);
    int k=0;
    for(int y=0;y<this->_w;y++){
        for(int x=0;x<this->_h;x++){
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

const float * Image::getHistogram(){
    if(_histogram!=NULL)
        return _histogram;
    const int size=sizeof(float)*ColorLevel*_byteCounts;
    _histogram=(float *)malloc(size*2);
    memset(_histogram,0,size*2);

    for(int y=0;y<_h;y++){
        for(int x=0;x<_w;x++){
            Byte *pix=getPixel(x,y);
            for(int cha=0;cha<_byteCounts;cha++){
                //jump to color scale sum space.
                _histogram[*(pix+cha)+cha*ColorLevel]++;
            }
        }
    }
    const int pixNum=_w*_h;
    const int hisSize=ColorLevel*_byteCounts;
    float *accumulate=_histogram+hisSize;
    for(int q=0;q<hisSize;q++){
        _histogram[q]/=pixNum;
    }

    for(int cha=0;cha<_byteCounts;cha++){
        float *dest=accumulate+ColorLevel*cha, *src=_histogram+ColorLevel*cha;
        for(int level=0;level<ColorLevel;level++){
            dest[level]=src[level]+(level==0?0:dest[level-1]);
        }
    }
    return _histogram;
}

void Image::remakeHistogram(){
    if(_histogram!=NULL) free(_histogram);
    _histogram=NULL;
    getHistogram();
}

Image::~Image(){
    free (this->_img);
    if(_histogram!=NULL){
        free(_histogram);
    }
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