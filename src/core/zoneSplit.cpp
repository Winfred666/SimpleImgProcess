/**
 * a imitation of binary region getter and 
 * however in normal image with multiple channels we need to pay more attention to pointer operation.
*/

#include "image.h"
#include <string.h>


RegionSet* Image::getAllKernals(int kernal,Byte * padding){
    int pixelNum=_h*_w;
    if(kernal%2==0) throw "getAllKernals:try to build kernals with even size!";
    int offset=kernal/2;
    int regionSize=_byteCounts*kernal*kernal;
    RegionSet* ret=new RegionSet(_w,_h,regionSize*8,ONLY_REGION);
    for(int pi=0;pi<pixelNum;pi++){
        //jump to center from corner
        Byte *rCenter=(Byte *)(ret->getDataPtr()+regionSize*pi)
            +kernal*offset+offset;
        for(int row=-offset;row<=offset;row++){
            for(int col=-offset;col<=offset;col++){
                //get the nearby pixel around pi.
                int posi=pi+row*_w+col;
                if(posi>=0&&posi<pixelNum){
                    memcpy(rCenter+row*kernal+col,_img+posi,_byteCounts);
                }else{
                    memcpy(rCenter+row*kernal+col,padding,_byteCounts);
                }
            }
        }
    }
    return ret;
}


RegionSet* Image::getAllRegions(int xSize,int ySize,Byte * padding){
    int rh=_h/ySize+(_h%ySize!=0);
    int rw=_w/xSize+(_w%xSize!=0);
    int imgSize=this->size();
    int regionSize=xSize*ySize*_byteCounts;
    RegionSet* ret=new RegionSet(rw,rh,regionSize*8,ONLY_REGION);
    for(int y=0;y<rh;y++){
        for(int x=0;x<rw;x++){
            //jump to specifit region
            Byte *one=ret->getPixel(x,y);
            for(int oney=0;oney<ySize;oney++){
                for(int onex=0;onex<xSize;onex++){
                    //fillin this region
                    int regoffset=(oney*xSize+onex)*_byteCounts;
                    //remember that the original pixel should be at the same x and y
                    int picoffset=(( y*ySize + oney )* _w + x*xSize+onex)*_byteCounts;
                    
                    if(picoffset>=0 && picoffset<imgSize)
                        memcpy(one+regoffset,_img+picoffset, _byteCounts);
                    else
                        memcpy(one+regoffset,padding, _byteCounts);
                }
            }
        }
    }
    return ret;
}


Image* Image::getROI(int stax,int stay,int endx,int endy){
    int neww=endx-stax;
    int newh=endy-stay;
    Image *ret=new Image(neww,newh,_byteCounts*8,mode);
    for(int y=0;y<newh;y++){
        for(int x=0;x<neww;x++){
            memcpy(ret->getPixel(x,y),getPixel(stax+x,stay+y),_byteCounts);
        }
    }
    return ret;
}