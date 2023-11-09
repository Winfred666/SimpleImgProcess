#ifndef COLORTRANS_H
#define COLORTRANS_H

#include "image.h"
//transfer color mode from RGB2HSV
void RGB2HSV(Image *img);

//transfer color mode from RGB2YUV
void RGB2YUV(Image *img);

//transfer color mode from YUV to BGR
void YUV2BGR(Image *img);

//atom operation of transfer.
void RGB2YUV_Pixel(Byte *pixel,ColorMode mode);
void YUV2BGR_Pixel(Byte *pixel,ColorMode mode);
void RGB2HSV_Pixel(Byte *pixel,ColorMode mode);
Byte thres(int num);

#endif