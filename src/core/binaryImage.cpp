
#include "binaryImage.h"
#include <string.h>

//first row offset, second column offset, top to bottom, left to right.
//const int BinaryImage::STEPS[8][2]={{-1,-1},{-1,0},{-1,1},{0,-1},{0,1},{1,-1},{1,0},{1,1}};

BinaryImage::BinaryImage(int w,int h):_w(w),_h(h){
    //initialize _bits vector.
    _bits=_bitsVector(w*h);
}
BinaryImage::BinaryImage(BinaryImage &img):_w(img._w),_h(img._h),_bits(img._bits){}

/**
 * get string information of this binary image, usually for output
 * remember to free 
*/
string BinaryImage::toString(){
    string ret="";
    for(bool it:_bits){
        ret=ret+(it?"1":"0");
    }
    return ret;
}

//friend function , simple morphological function.


BinaryImage *gray2Binary(Image* img,int thres){
    if(img->_mode != GRAY){
        throw "try to binarize a colored image, which has more than one channel!";
    }
    int imgH=img->height();
    int imgW=img->width();
    BinaryImage *ret=new BinaryImage(imgW,imgH);
    for(int x=0;x<imgW;x++){
        for(int y=0;y<imgH;y++){
            ret->setPixel(x,y,
                (*(img->getPixel(x,y)))>=thres);
        }
    }
    return ret;
}

//self adjustable, base on OTSU
BinaryImage * gray2Binary(Image *img){
    //count the pixel number in every gray value
    double count[256];
    memset(count,0,sizeof(count));
    int imgH=img->height();
    int imgW=img->width();
    int imgS=img->size();
    for(int x=0;x<imgW;x++){
        for(int y=0;y<imgH;y++){
            count[*(img->getPixel(x,y))]++;
        }
    }
    //normalize every part.
    for(int q=0;q<256;q++){
        count[q]/=imgS;
    }
    //cout<<accumu[0]<<" "<<accumu[100]<<" "<<accumu[255]<<endl;
    //iterate from zero and set foreground as 0-thres
    int thres=0;
    int maxVar=0;
    for(int q=0;q<256;q++){
        double fg=0,bg=0,fgAccumu=0,bgAccumu=0;
        for(int w=0;w<q;w++){
            bg+=w*count[w];
            bgAccumu+=count[w];
        }
        if(bgAccumu>0) bg/=bgAccumu;
        for(int w=q;w<256;w++){
            fg+=w*count[w];
            fgAccumu+=count[w];
        }
        if(fgAccumu>0) fg/=fgAccumu;
        int var=bgAccumu*fgAccumu*(fg-bg)*(fg-bg);
        //cout<<"var: "<<var<<" "<<bgAccumu<<" "<<fg<<" "<<bg<<endl;
        if(var>maxVar){
            maxVar=var;
            thres=q;
        }
    }
    cout<<"threshold calculate by OTSU:"<<thres<<endl;
    return gray2Binary(img,thres);
}


Image *binary2Gray(BinaryImage *img){
    int w=img->width();
    int h=img->height();
    Image *ret=new Image(w,h,8,GRAY);
    for(int x=0;x<w;x++){
        for(int y=0;y<h;y++){
            *(ret->getPixel(x,y))=(img->getPixel(x,y)?255:0);
        }
    }
    return ret;
}

//most crucial method,get kernal region.
RegionSet* BinaryImage::getAllKernals(int kernal,bool padding){
    int imgSize=size();
    if(kernal%2==0) throw "getAllKernals:try to build kernals with even size!";
    int offset=kernal/2;
    //size of one pixel, in kernalSet.
    int regionSize=sizeof(bool)*kernal*kernal;
    RegionSet* ret=new RegionSet(_w,_h,regionSize*8,ONLY_REGION);
    for(int pi=0;pi<imgSize;pi++){
        //remember we only have kernal*kernal space per region.
        bool *rCorner=(bool *)(ret->getDataPtr()+regionSize*pi);
        //jump to center from corner.
        bool *rCenter=rCorner + kernal*offset+offset;
        for(int row=-offset;row<=offset;row++){
            for(int col=-offset;col<=offset;col++){
                int posi=pi+row*_w+col;
                int value=padding;
                if(posi>=0 && posi<imgSize){
                    value=_bits[posi];
                }
                //jump to specifit region
                *(rCenter+row*kernal+col) = value;
            }
        }
    }
    return ret;
}




RegionSet* BinaryImage::getAllRegions(int xSize,int ySize,bool padding){
    int rh=_h/ySize+(_h%ySize!=0);
    int rw=_w/xSize+(_w%xSize!=0);
    int imgSize=this->size();
    int regionSize=xSize*ySize*sizeof(bool);
    RegionSet* ret=new RegionSet(rw,rw,regionSize*8,ONLY_REGION);
    for(int y=0;y<rh;y++){
        for(int x=0;x<rw;x++){
            bool *one=(bool *)ret->getPixel(x,y);
            for(int oney=0;oney<ySize;oney++){
                for(int onex=0;onex<xSize;onex++){
                    int regoffset=(oney*ySize+onex);
                    int picoffset=(y*rw+x)*regionSize+regoffset;
                    one[regoffset]=padding;
                    if(picoffset>0 && picoffset<imgSize)
                        one[regoffset]=_bits[picoffset];
                }
            }
        }
    }
    return ret;
}


BinaryImage* BinaryImage::kernalConvolute(int kernalWidth,MorphologyFunc kernalFunc,bool padding=0){
    if(kernalWidth%2==0) throw "kernal size of erosion must be odd!";
    BinaryImage *ret=new BinaryImage(*this);
    
    RegionSet *refer=getAllKernals(kernalWidth,padding);

    //now a good condition appears, ret and kernals could use the same coordinate.    
    int imgW=refer->width();
    int imgH=refer->height();
    //do convolution to every pixel.
    for(int y=0;y<imgH;y++){
        for(int x=0;x<imgW;x++){
            //get the reference , that is , the pixel of regionset.
            bool *pixel=(bool *)(refer->getPixel(x,y));
            //call the callback to get value.
            bool val=(*kernalFunc)(pixel,kernalWidth);
            //set the result value.
            ret->setPixel(x,y,val);
        }
    }
    delete refer;
    return ret;
}


//very smart to extract the core logic outside!
bool erose_core(bool *kernal,int width){
    int size=width*width;
    for(int q=0;q<size;q++)
        if(!kernal[q]) return false;
    return true;
}

bool dilate_core(bool *kernal,int width){
    int size=width*width;
    for(int q=0;q<size;q++){
        if(kernal[q]) return true;
    }
    return false;
}

BinaryImage* BinaryImage::erose(int kernalWidth){
    return kernalConvolute(kernalWidth,erose_core,1);
}


BinaryImage* BinaryImage::dilate(int kernalWidth){
    return kernalConvolute(kernalWidth,dilate_core,0);
}

BinaryImage* BinaryImage::open(int kernalWidth){
    BinaryImage* erose=this->erose(kernalWidth);
    BinaryImage* ret=erose->dilate(kernalWidth);
    delete erose;
    return ret;
}
BinaryImage* BinaryImage::close(int kernalWidth){
    BinaryImage* dilate=this->dilate(kernalWidth);
    BinaryImage* ret=dilate->erose(kernalWidth);
    delete dilate;
    return ret;
}