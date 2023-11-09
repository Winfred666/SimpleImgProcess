#ifndef BINARYIMAGE_H
#define BINARYIMAGE_H

#include <vector>
#include "image.h"
//create a binary image, wrap _bitset in it.
using namespace std;

typedef vector<bool> _bitsVector;

typedef bool (*MorphologyFunc)(bool *kernal,int width);

class BinaryImage{
private: 
    //first row offset, second column offset, top to bottom, left to right.
    //static const int STEPS[8][2];
    int _w;
    int _h;
    //store a bitmap. remember to clear it when died.
    _bitsVector _bits;
public:
    BinaryImage(int w,int h);
    BinaryImage(BinaryImage &img);
    

    //for faster calling use inline.
    inline void setPixel(int x,int y,bool val){
        _bits[y*_w+x]=val;
    }

    inline bool getPixel(int x,int y){
        return (bool)_bits[y*_w+x];
    }

    /**
     * just like cutting meat, get all small region for morphology calculation, remember to delete.
     * use kernal*kernal*size() bytes
     */
    RegionSet* getAllKernals(int kernal,bool padding);

    /**
     * split into regions, and every region is a pixel in RegionSet.
    */
    RegionSet* getAllRegions(int xSize,int ySize,bool padding);

    inline int height(){
        return _h;
    }
    inline int width(){
        return _w;
    }
    inline int size(){
        return _h*_w;
    }


    string toString();
    
    //operation that only for binary image , simple morphological function.
    BinaryImage* erose(int kernalWidth);
    BinaryImage* dilate(int kernalWidth);
    //first erose then dilate, get rid of white stick.
    BinaryImage* open(int kernalWidth);
    //first dilate then erose, fill in small black hole.
    BinaryImage* close(int kernalWidth);
    //crucial Function, do the convolution.
    BinaryImage* kernalConvolute(int kernalWidth,MorphologyFunc kernalFunc,bool padding);
};


//turn a gray-image to binary image according to specifit threshold.
BinaryImage *gray2Binary(Image* img,int thres);

//default function, using algorithms to determined a threshold 
BinaryImage * gray2Binary(Image *img);

//transfer binary image back to gray image.
Image *binary2Gray(BinaryImage *img);

#endif