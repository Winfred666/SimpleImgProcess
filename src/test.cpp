#include <iostream>
#include <cmath>
#include <vector>

#include "./io/readBMP.h"

#include "image.h"
#include "binaryImage.h"

using namespace std;



int main(){
	char fileName[20]="test.bmp";
	
	//wrapped by try-catch to collect and print string exception.
	try{
		FILE *fp=fopen_s(fileName,"rb");
		
		Image *gray=readBMP(fp);

		BinaryImage *binary=gray2Binary(gray);
		Image *binaGray=binary2Gray(binary);
		FILE *out=fopen_s("binary.bmp","wb");
		writeBMP(out,binaGray);
		cout<<"successfully output binary image!"<<endl;

		BinaryImage *erose=binary->erose(9);
		Image *erosGray=binary2Gray(erose);
		FILE *eroOut=fopen_s("binary_erosed.bmp","wb");
		writeBMP(eroOut,erosGray);
		delete erose;
		delete erosGray;
		cout<<"successfully do the erosion to binary image!"<<endl;
		
		BinaryImage *dilate=binary->dilate(9);
		Image *dilaGray=binary2Gray(dilate);
		FILE *dilOut=fopen_s("binary_dilate.bmp","wb");
		writeBMP(dilOut,dilaGray);
		delete dilate;
		delete dilaGray;
		cout<<"successfully do the dilation to binary image!"<<endl;
		
		BinaryImage *opente=binary->open(5);
		Image *openGray=binary2Gray(opente);
		FILE *openOut=fopen_s("binary_open.bmp","wb");
		writeBMP(openOut,openGray);
		delete opente;
		delete openGray;
		cout<<"successfully do the open operation to binary image!"<<endl;
		
		BinaryImage *closete=binary->close(5);
		Image *closeGray=binary2Gray(closete);
		FILE *closeOut=fopen_s("binary_close.bmp","wb");
		writeBMP(closeOut,closeGray);
		delete closete;
		delete closeGray;
		cout<<"successfully do the close operation to binary image!"<<endl;
		
		delete gray;
		delete binary;
		delete binaGray;
		
		
	}catch(char const *e){
		printf("catch Exception: %s\n",e);
	}catch(...){
		printf("Unexpected error occur!");
	}
}

