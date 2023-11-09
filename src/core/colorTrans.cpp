#include "colorTrans.h"
#include "image.h"
#include <cmath>

#define PI acos(-1)

using namespace colorTrans_ns;

//transfer every pixel from RGB to HSV
void RGB2HSV(Image *img){
	if(!isRGB(img->mode)) throw "Try to transfer non-RGB image from RGB to HSV!";
	if(img->bytePerPixel()!=3) throw "try to transfer non-24Bit per pixel image from RGB to HSV!";
	img->iterateAll(RGB2HSV_Pixel);
	img->mode=HSV;
}

void HSV2BGR(Image *img){
	if(img->mode!=HSV) throw "Try to transfer non-HSV image from HSV to BGR!";
	if(img->bytePerPixel()!=3) throw "try to transfer non-24Bit per pixel image from RGB to HSV!";
	img->iterateAll(HSV2BGR_Pixel);
	img->mode=BGR;
}


//transfer every pixel from RGB to HSI
void RGB2HSI(Image *img){
	if(!isRGB(img->mode)) throw "Try to transfer non-RGB image from RGB to HSV!";
	if(img->bytePerPixel()!=3) throw "try to transfer non-24Bit per pixel image from RGB to HSV!";
	img->iterateAll(RGB2HSI_Pixel);
	img->mode=HSI;
}

void HSI2BGR(Image *img){
	if(img->mode!=HSI) throw "Try to transfer non-HSI image to BGR!";
	img->iterateAll(HSI2BGR_Pixel);
	img->mode=BGR;
}

void RGB2YUV(Image *img){
	if(!isRGB(img->mode)) throw "Try to transfer non-RGB image from RGB to YUV!";
	if(img->bytePerPixel()!=3) throw "try to transfer non-24Bit per pixel image from RGB to YUV!";
	img->iterateAll(RGB2YUV_Pixel);
	img->mode=YUV;
}

void YUV2BGR(Image *img){
	if(img->mode!=YUV) throw "YUV2BGR: Try to transfer a non-YUV image!";
	img->iterateAll(YUV2BGR_Pixel);
	img->mode=BGR;
}

Image * GRAY2YUV(Image *img){
	if(img->mode!=GRAY) throw "GRAY2YUV: Tray to transfer a non-GRAY image!";
	int w=img->width(), h=img->height();
	Image *ret=new Image(w,h,24,YUV);
	for(int y=0;y<h;y++){
		for(int x=0;x<w;x++){
			//get byte of gray.
			Byte Y=*(img->getPixel(x,y));
			Byte *yuv=ret->getPixel(x,y);
			yuv[0]=Y;
			yuv[1]=yuv[2]=128;
		}
	}
	return ret;
}


void colorTrans_ns::YUV2BGR_Pixel(Byte *pixel,ColorMode mode){
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

Byte colorTrans_ns::thres(int num){
	return (num>255)?255:((num<0)?0:num);
}

//make sure there are three bytes in every pixel!
void colorTrans_ns::RGB2HSV_Pixel(Byte *pixel,ColorMode mode){
	Byte max,min,R=*(pixel+getR(mode));
	max=min=R;
	Byte G=*(pixel+getG(mode));
	if(G>max) max=G;
	if(G<min) min=G;
	Byte B=*(pixel+getB(mode));
	if(B>max) max=B;
	if(B<min) min=B;
	Byte V=max;
	Byte S=0;

	if(V!=0) S=(V-min)*255.0/V;
	float Hf=0;
	if(max==R) Hf=60.0f*(G-B)/(V-min);
	else if(max==G) Hf=120+60.0f*(B-R)/(V-min);
	else Hf=240+60.0f*(R-G)/(V-min);
	if(Hf<0) Hf+=360;
	Hf/=2;
	//std::cout<<Hf<<" "<<(int)S<<" "<<(int)V<<std::endl;
	pixel[0]=Hf;
	pixel[1]=S;
	pixel[2]=V;
}



void colorTrans_ns::HSV2BGR_Pixel(Byte *pixel,ColorMode mode){
	float H=2*pixel[0];
	float S=pixel[1]*1.0f/255;
	float V=pixel[2];
	int hi=H/60;
	float f=H*1.0f/60.0f-hi;
	Byte p=V*(1-S);
	Byte q=V*(1-f*S);
	Byte t=V*(1-(1-f)*S);
	if(hi==0){
		pixel[2]=V;pixel[1]=t;pixel[0]=p;
	}else if(hi==1){
		pixel[2]=q;pixel[1]=V;pixel[0]=p;
	}else if(hi==2){
		pixel[2]=p;pixel[1]=V;pixel[0]=t;
	}else if(hi==3){
		pixel[2]=p;pixel[1]=q;pixel[0]=V;
	}else if(hi==4){
		pixel[2]=t;pixel[1]=p;pixel[0]=V;
	}else if(hi==5){
		pixel[2]=V;pixel[1]=p;pixel[0]=q;
	}
}

void colorTrans_ns::RGB2HSI_Pixel(Byte *pixel,ColorMode mode){
	Byte R=*(pixel+getR(mode)), G=*(pixel+getG(mode)), B=*(pixel+getB(mode)),
	min=R;
	if(B<min) min=B;
	if(G<min) min=G;

	float cyta=acosf((2*R-G-B)/
		(2*sqrtf((R-G)*(R-G)+(R-B)*(G-B)))
	);
	
	pixel[0]= (B>G ? 2*PI-cyta : cyta)*1.0f*255/(2*PI);
	pixel[1]=(1-1.0f*min*3/(R+B+G))*255;
	pixel[2]=(R+B+G)/3.0f;
}

const float PI1D3=PI/3;
const float PI2D3=PI/3*2;
const float PI4D3=PI/3*4;

void colorTrans_ns::HSI2BGR_Pixel(Byte *pixel, ColorMode mode){
	float H=pixel[0]*1.0f*(2*PI)/255,
		S=pixel[1]*1.0f/255,
		I=pixel[2];
	Byte B,G,R;
	if(H<PI2D3){
		B=I*(1-S);
		R=I*(1+S*cosf(H)/cosf(PI1D3-H));
		G=3*I-(R+B);
	}else if(H<PI4D3){
		H-=PI2D3;
		R=I*(1-S);
		G=I*(1+S*cosf(H)/cosf(PI1D3-H));
		B=3*I-(R+G);
	}else{
		H-=PI4D3;
		G=I*(1-S);
		B=I*(1+S*cosf(H)/cosf(PI1D3-H));
		R=3*I-(G+B);
	}
	pixel[0]=B;pixel[1]=G;pixel[2]=R;
}


void colorTrans_ns::RGB2YUV_Pixel(Byte *pixel,ColorMode mode){
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