#include "colorTrans.h"
#include "image.h"
#include <cmath>

#define PI acos(-1)

//transfer every pixel from RGB to HSV
void RGB2HSV(Image *img){
	if(!isRGB(img->_mode)) throw "Try to transfer non-RGB image from RGB to HSV!";
	if(img->bytePerPixel()!=3) throw "try to transfer non-24Bit per pixel image from RGB to HSV!";
	img->iterateAll(RGB2HSV_Pixel);
	img->_mode=HSV;
}

void RGB2YUV(Image *img){
	if(!isRGB(img->_mode)) throw "Try to transfer non-RGB image from RGB to YUV!";
	if(img->bytePerPixel()!=3) throw "try to transfer non-24Bit per pixel image from RGB to YUV!";
	img->iterateAll(RGB2YUV_Pixel);
	img->_mode=YUV;
}

void YUV2BGR(Image *img){
	if(img->_mode!=YUV) throw "YUV2BGR: Try to transfer a non-YUV image!";
	img->iterateAll(YUV2BGR_Pixel);
	img->_mode=BGR;
}


void YUV2BGR_Pixel(Byte *pixel,ColorMode mode){
	Byte Y=*(pixel+0);
	Byte U=*(pixel+1);
	Byte V=*(pixel+2);
	int R = Y + 1.403 * (V-128);
	int G = Y - 0.343 * (U-128) - 0.714*(V-128);
	int B = Y + 1.770 * (U-128);
	*pixel=thres(B);
	*(pixel+1)=thres(G);
	*(pixel+2)=thres(R);
}

Byte thres(int num){
	return (num>255)?255:((num<0)?0:num);
}

//make sure there are three bytes in every pixel!
void RGB2HSV_Pixel(Byte *pixel,ColorMode mode){
	Byte max,min,R=*(pixel+getR(mode));
	max=min=R;
	Byte G=*(pixel+getG(mode));
	if(G>max) max=G;
	if(G<min) min=G;
	Byte B=*(pixel+getB(mode));
	if(B>max) max=B;
	if(B<min) min=B;

	Byte H=acos(
		((R-G)+(R-B))/
		(2*sqrt(
			(R-G)*(R-G)+(R-B)*(G-B)
		))
	);
	if(B>G) H=2*PI-H;
	Byte S=(max-min)/max;
	Byte V=max/255;

	*pixel=H;
	*(pixel+1)=S;
	*(pixel+2)=V;
}

void RGB2YUV_Pixel(Byte *pixel,ColorMode mode){
	Byte R=*(pixel+getR(mode));
	Byte G=*(pixel+getG(mode));
	Byte B=*(pixel+getB(mode));

	//follow this fomular,range of Y will definitely between 0 and 255;
	Byte Y=0.299*R +0.587*G +0.114*B;
	Byte U= -0.169*R -0.331*G +0.5*B +128;
	Byte V= 0.5*R -0.419*G -0.081*B+128;
	*pixel=Y;
	*(pixel+1)=U;
	*(pixel+2)=V;
}