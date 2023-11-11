#include <iostream>

#include "readBMP.h"
#include "readJPEG.h"

#include "image.h"
#include "imageOpt.h"
#include "geometry.h"

using namespace std;

using namespace TransTool_ns;

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
		FILE *inputF=fopen_s(fileName,"rb");
		FILE *outputF=fopen_s("verify/scale.bmp","wb");
		Image *in=readBMP(inputF);
		
		TransZone trans(in,1,0);
		TransMatrix rota45,mid={{0.8,0,0},{0,-0.6,0},{0,0,1}};
		getScaleMat(rota45,2,2);
		trans.applyTrans(mid,true,TRANS_LINEAR);
		Image *temp=trans.getOutput();
		
		Image *out=RGBa2RGB(temp);
		//delete temp;
		//temp=out->getROI(50,200,250,400);
		writeBMP(outputF,out);
		fclose(inputF);
		fclose(outputF);
		delete out;
		delete in;
		delete temp;
	}catch(char const *e){
		printf("catch Exception: %s\n",e);
	}catch(string e){
		printf("cath Exception with info: %s\n",e.c_str());
	}catch(...){
		printf("Unexpected error occur!");
	}
}

