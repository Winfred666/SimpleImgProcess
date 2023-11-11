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
    for(int y=0;y<_h;y++){
        for(int x=0;x<_w;x++){
            Byte *region=ret->getPixel(x,y);
            for(int regy=0;regy<kernal;regy++){
                for(int regx=0;regx<kernal;regx++){
                    Byte *regPix=region+(regy*kernal+regx)*_byteCounts;
                    int realx=x+regx-offset;
                    int realy=y+regy-offset;
                    if(realx<0 || realx>=_w || realy<0 || realy>=_h)
                        memcpy(regPix,padding,_byteCounts);
                    else
                        memcpy(regPix,getPixel(realx,realy),_byteCounts);
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
    if(stax<0 || stay<0 || endx>=_w || endy>=_h) throw "getROI: out of range!";
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

void Image::setROI(int stax,int stay,Image *src){
    int srcBPP=src->bytePerPixel(),destBPP=bytePerPixel();
    if(srcBPP!=destBPP &&(!isRGB(src->mode) || !isRGB(mode))) throw "setROI:can't set an uncompatible image piece onto another one!"; 
    
    int coverMode=0;
    //foreGround
    if(srcBPP==4) coverMode=2;
    //backGround
    if(destBPP==4) coverMode+=1;
    int neww=src->width();
    int newh=src->height();
    for(int y=0;y<newh;y++){
        for(int x=0;x<neww;x++){
            Byte *srcB=src->getPixel(x,y);
            Byte temp[4];
            Byte *destB=getPixel(stax+x,stay+y);
            float srcOpa,opa,destOpa;
            switch (coverMode)
            {
            case 1:
                memcpy(destB,srcB,3);
                destB[3]=255;
                break;
            case 2:
                srcOpa=srcB[3]/255.0f;
                for(int q=0;q<3;q++){
                    temp[q]=(Byte)(srcB[q]*srcOpa+destB[q]*(1-srcOpa));
                }
                memcpy(destB,temp,3);
                break;
            case 3:
                srcOpa=srcB[3]/255.0f, destOpa=destB[3]/255.0f,
                opa=1-(1-srcOpa)*(1-destOpa);
                if(srcOpa<0.01f) break;
                for(int q=0;q<3;q++){
                    temp[q]=(Byte)((srcB[q]*srcOpa+destB[q]*(1-srcOpa)*destOpa)/opa);
                }
                memcpy(destB,temp,4);
                break;
            default:
                memcpy(destB,srcB,destBPP);
                break;
            }
        }
    }
}
