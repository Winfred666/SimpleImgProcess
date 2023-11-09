#include <iostream>
#include <cmath>
#include <vector>

#include "./io/readBMP.h"
#include "./io/readJPEG.h"

#include "image.h"
#include "imageOpt.h"

using namespace std;

float linear_expand(float a){
	float ret=0;
	const float low=0.2f;
	const float high=0.8f;
	if(a<low) ret=0;
	else if(a>high) ret=1;
	else{
		ret=(a-low)/(high-low);
	}
	return ret;
}
float linear_shrink(float a){
	const float low=0.2f;
	const float high=0.8f;
	float ret=(high-low)*a+low;
	return ret;
}

float pow4(float a){
	return a*a*a*a;
}

int main(){
	char fileName[20]="test.bmp";
	//wrapped by try-catch to collect and print string exception.
	try{
		FILE *fp=fopen_s(fileName,"rb");
		FILE *out=fopen_s("verify/out_hiseql_V.bmp","wb");
		//FILE *out2=fopen_s("verify/out_logarithm.bmp","wb");
		
		Image *in=readBMP(fp);
		//Image *copy=new Image(*in);
		RGB2HSV(in);
		//histogramEqualize_V(in);
		histoTrans(in,pow4,2);
		HSV2BGR(in);
		//still have bug when write jpeg.
		//writeJPEG(out,in);
		writeBMP(out,in);
		
		//logarithmic_Y(copy,1.1);
		//writeBMP(out2,copy);

		fclose(fp);
		fclose(out);
		//fclose(out2);
		delete in;
		//delete copy;
	}catch(char const *e){
		printf("catch Exception: %s\n",e);
	}catch(string e){
		printf("cath Exception with info: %s\n",e.c_str());
	}catch(...){
		printf("Unexpected error occur!");
	}
}

