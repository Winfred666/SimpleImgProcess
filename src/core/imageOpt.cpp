
#include "imageOpt.h"
#include <cmath>


namespace imageOpt_ns{
    int highestLumi;
    int lowestLumi;
    float contrast;
    HistoTransFunc histTrans=NULL;
    //histogram transform channel, -1 (for all channel) by default
    int seleCha=-1;
}

using namespace imageOpt_ns;

void histogramEqualize(Image *img){
    img->iterateAll(histoEqualize_pixel);
}

void histogramEqualize_Y(Image *img){
    img->iterateAll(histoEqualize_Y_pixel);
}

void histogramEqualize_V(Image *img){
    img->iterateAll(histoEqualize_V_pixel);
}

void logarithmic_Y(Image *img,float contrastVal){
    //find the max luminance and store it.
    contrast=contrastVal;
    highestLumi=0;
    lowestLumi=img->ColorLevel-1;
    img->iterateAll(findMaxMinLumi_pixel);
    img->iterateAll(logarithmic_Y_pixel);
}

void histoTrans(Image *img,HistoTransFunc func,int channel){
    histTrans=func;
    seleCha=channel;
    img->iterateAll(histoTrans_pixel);
}

void imageOpt_ns::histoTrans_pixel(Byte *pixel, Image *img){
    if(histTrans==NULL) return;
    if(seleCha==-1){
        const int channels=img->bytePerPixel();
        for(int q=0;q<channels;q++){
            pixel[q]=histTrans(img->getHistoAccumu(q,pixel[q]))*(img->ColorLevel-1);
        }
    }else{
        pixel[seleCha]=histTrans(img->getHistoAccumu(seleCha,pixel[seleCha]))*(img->ColorLevel-1);
    }
}

void imageOpt_ns::findMaxMinLumi_pixel(Byte *pixel,Image *img){
    if(*pixel>highestLumi)
        highestLumi=*pixel;
    if(*pixel<lowestLumi)
        lowestLumi=*pixel;
}

void imageOpt_ns::histoEqualize_pixel(Byte *pixel, Image *img){
    const int Channels=img->bytePerPixel();
    for(int cha=0;cha<Channels;cha++){
        int res=round(
            (img->getHistoAccumu(cha,*(pixel+cha)))*(img->ColorLevel-1)
        );
        *(pixel+cha)=res;
    }
}

void imageOpt_ns::histoEqualize_Y_pixel(Byte *pixel, Image *img){
    *(pixel)=round(
        (img->getHistoAccumu(0,*(pixel)))*(img->ColorLevel-1)
    );
}

void imageOpt_ns::histoEqualize_V_pixel(Byte *pixel, Image *img){
    pixel[2]=round(
        (img->getHistoAccumu(0,pixel[2]))*(img->ColorLevel-1)
    );
}

void imageOpt_ns::logarithmic_Y_pixel(Byte *pixel, Image *img){
    int val=contrast*(log2(1.0f*(*pixel)/highestLumi+1))*(img->ColorLevel-1);
    *pixel=(val>=img->ColorLevel ? img->ColorLevel-1 : val);
}

