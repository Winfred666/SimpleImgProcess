#include <iostream>
#include <cmath>

#include "./io/readBMP.h"
#include "image.h"
#include "colorTrans.h"

using namespace std;


//make Y scalar lighter by ten of YUV mode.
void lighter_Pixel(Byte *pixel,ColorMode mode){
	if(*pixel<255-50)
		(*pixel)+=50;
	else *pixel=255;
}

//make Y scalar dimmer by ten of YUV mode.
void darker_Pixel(Byte *pixel,ColorMode mode){
	if(*pixel>50)
		*pixel-=50;
	else *pixel=0;
}


int main(){
	FILE *fp=NULL;
	char fileName[20]="test.bmp";

	//wrapped by try-catch to collect and print string exception.
	try{
		printf("start to read BMP image: %s\n",fileName);
		fp=fopen_s(fileName,"rb");


		Image *src=readBMP(fp);
		//directly change src.
		RGB2YUV(src);
		
		printf("successfully read in image.width and height:%d %d\n",src->width(),src->height());

		//splite Y channel and write gray image.
		Image *gray=src->splitChannel(0);
		char outputName[20]="output_gray.bmp";
		
		printf("start to write gray image to '%s'.\n",outputName);
		
		FILE *out=fopen_s(outputName,"wb");
		writeBMP(out,gray);
		
		printf("successfully writing gray image to '%s'.\n",outputName);
		
		Image *copy=new Image(*src);
		
		//make Y channel dimmer.
		src->iterateAll(darker_Pixel);
		YUV2BGR(src);
		
		//write output darker image.
		writeBMP(fopen_s("output_darker.bmp","wb"),src);
		printf("successfully writing a darker image!");

		//make Y channel lighter.
		copy->iterateAll(lighter_Pixel);
		YUV2BGR(copy);
		
		//write output darker image.
		writeBMP(fopen_s("output_lighter.bmp","wb"),copy);
		printf("successfully writing a lighter image!");
		

		delete src;
		delete gray;
		delete copy;
		
	}catch(char const *e){
		printf("catch Exception: %s\n",e);
	}
}

