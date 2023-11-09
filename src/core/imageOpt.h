#ifndef IMAGEOPT_H
#define IMAGEOPT_H

#include "image.h"

//input: 0-1; output: 0-1, a histogram transform function
typedef float (*HistoTransFunc)(float ori);

void histogramEqualize(Image *img);
void histogramEqualize_Y(Image *img);
void histogramEqualize_V(Image *img);

void logarithmic_Y(Image *img,float contrastVal=1);
void histoTrans(Image *img,HistoTransFunc func,int channel=-1);


namespace imageOpt_ns{
    extern int seleCha;
    extern int highestLumi;
    extern int lowestLumi;
    extern HistoTransFunc histTrans;
    void histoEqualize_Y_pixel(Byte *pixel, Image *img);
    void histoEqualize_pixel(Byte *pixel, Image *img);
    void logarithmic_Y_pixel(Byte *pixel, Image *img);
    void findMaxMinLumi_pixel(Byte *pixel,Image *img);
    void histoEqualize_V_pixel(Byte *pixel, Image *img);
    void histoTrans_pixel(Byte *pixel, Image *img);

}

#endif