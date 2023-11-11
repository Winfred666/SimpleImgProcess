#include <iostream>
#include <cmath>

#include "readBMP.h"

#include "image.h"
#include "colorTrans.h"

using namespace std;

#define BMPStdHead 0x4d42

//read a BMP file into Image class.
Image* readBMP(FILE *BMP){
	BMPFileHeader fHeader;
	BMPImgHeader iHeader;
	//read file head
	uint16_t typeTag=0;
	fread(&typeTag,1,sizeof(uint16_t),BMP);
	if(typeTag!=BMPStdHead){
		throw "BMP type tag error!";
	}
	fread(&fHeader,1,sizeof(BMPFileHeader),BMP);
	
	//read image head
	fseek(BMP,14,SEEK_SET);
	fread(&iHeader,1,sizeof(BMPImgHeader),BMP);

	int per=iHeader.bitCount;
	int w=iHeader.width;
	int h=iHeader.height;

	//prepare color table.
	ColorInfo *colorTable=NULL;
	int colorBytes=fHeader.offsetBytes-sizeof(BMPFileHeader)-sizeof(BMPImgHeader);
	//read color info only when bitCount<8, which has colorTable;
	if(per<8 && colorBytes>0){
		//handle to binary system because byte-wise image can't operate it.
		//colorTable=(ColorInfo *)malloc(colorBytes);
		throw "Reading a BMP file whose bits count is less than 8! The program hasn't supportted it yet.";
	}
	//begin to read data.
	fseek(BMP,fHeader.offsetBytes,SEEK_SET);
	//prepare content for reading.
	ColorMode mode=GRAY;
	if(per==3*8) mode=BGR;
	else if(per==4*8) mode=BGRA;
	Image *bmpImg=new Image(w,h,per,mode);
	bmpImg->readFile(BMP);

	return bmpImg;
}


//write a BMPHeader to file.
void storeBMPHeader(FILE *output,BMPFileHeader *fHeader,BMPImgHeader *iHeader){
	if(output==NULL) return;
	fseek(output,0,SEEK_SET);
	uint16_t typeHead=BMPStdHead;
	fwrite(&typeHead,sizeof(uint16_t),1,output);
	fwrite(fHeader,sizeof(BMPFileHeader),1,output);
	fwrite(iHeader,sizeof(BMPImgHeader),1,output);
}


//generate a BMPHeader and write BMP(byte-wise) according to the information of image.
void writeBMP(FILE *output,Image *img){
	if(output==NULL) throw "try to output BMP image to a NULL file pointer!";
	uint16_t typeHead=BMPStdHead;
	fwrite(&typeHead,sizeof(uint16_t),1,output);
	BMPFileHeader fHeader;
	BMPImgHeader iHeader;
	iHeader.bitCount=img->bytePerPixel()*8;
	iHeader.headerSize=sizeof(BMPImgHeader);
	iHeader.height=img->height();
	iHeader.width=img->width();
	iHeader.imgSize=img->size();
	iHeader.planes=1;
	iHeader.XPixelsPerMeter=iHeader.YPixelsPerMeter=3780;
	
	iHeader.colorImportant=iHeader.colorUsed=iHeader.compression=0;
	
	//for bitCount==8, build color plattes(gray picture).
	int useColorNum=0;
	ColorInfo *plattee=NULL;
	if(iHeader.bitCount<=8){
		iHeader.colorUsed=pow(2,iHeader.bitCount);
		plattee=(ColorInfo *)malloc(sizeof(ColorInfo)*iHeader.colorUsed);
		//set color for it.
		for(int q=0;q<iHeader.colorUsed;q++){
			(*(plattee+q)).b=(*(plattee+q)).g=(*(plattee+q)).r=(Byte)(1.0*q/iHeader.colorUsed*255);
			(*(plattee+q)).reserved=0;
		}
	}

	fHeader.offsetBytes=sizeof(uint16_t)+sizeof(BMPFileHeader)+iHeader.headerSize + sizeof(ColorInfo)*useColorNum;
	
	fHeader.fileSize=fHeader.offsetBytes+iHeader.imgSize;
	fHeader.reserved1=fHeader.reserved2=0;
	
	fwrite(&fHeader,sizeof(BMPFileHeader),1,output);
	fwrite(&iHeader,sizeof(BMPImgHeader),1,output);
	
	//for bitCount under 8, apply color plattee.
	if(iHeader.bitCount<=8){
		fwrite(plattee,sizeof(ColorInfo),iHeader.colorUsed,output);
		free(plattee);
	}

	if(iHeader.width%4==0)
		fwrite(img->getDataPtr(),iHeader.imgSize,1,output);
	else{
		//not divided by 4.
		int mol=iHeader.width%4;
		int bytes=img->bytePerPixel();
		Byte Pad=0x00;
		for(int q=0;q<iHeader.height;q++){
			fwrite(img->getPixel(0,q),iHeader.width*bytes,1,output);
			for(int w=0;w<mol;w++)
				fwrite(&Pad,1,1,output);
		}
	}
}

//safely open a file,overwrite.
FILE *fopen_s(const char *name,const char *mode){
	char path[30];
	sprintf((char *)path,"../public/%s",name);
	FILE *fp=fopen(path,mode);
	if(fp==NULL){
		throw "fail to open the image!";
	}
	return fp;
}
