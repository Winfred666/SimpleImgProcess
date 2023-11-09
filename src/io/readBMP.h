#ifndef READBMP_H
#define BMP_H


#include "image.h"

typedef struct _BMPFileHeader{
	//because of Struct completion, skip typeTag.
	//unsigned short typeTag; //type tag of bmp( 2 bytes,must be 0x4d42)
	unsigned int fileSize; // file size of bmp, whose unit is byte(4 bytes)
	unsigned short reserved1;// zero reserve(2 bytes)
	unsigned short reserved2;// zero reserve(2 bytes)
	unsigned int offsetBytes;// offset of pixel(2 bytes)
} BMPFileHeader;

typedef struct _BMPImgHeader{
	unsigned int headerSize; //size of img header, whose unit is byte
	unsigned int width; //width of this img,(4 bytes)
	unsigned int height; //height of this img,(4 bytes)
	unsigned short planes; // only one plane for each monitor, so equals to 1;
	unsigned short bitCount;//important.bits per pixel(2 bytes)
	unsigned int compression;
	unsigned int imgSize; //should be fileSize-offsetBytes
	int XPixelsPerMeter;
	int YPixelsPerMeter;
	unsigned int colorUsed; // number of used reference colors
	unsigned int colorImportant;
} BMPImgHeader;

//color table of BMP file, 4 bytes per color
typedef struct _ColorInfo{
	unsigned char b;
	unsigned char g;
	unsigned char r;
	unsigned char reserved;
} ColorInfo;


Image* readBMP(FILE *BMP);

void storeBMPHeader(FILE *output,BMPFileHeader &fHeader,BMPImgHeader &iHeader);

void writeBMP(FILE *output,Image *img);

FILE *fopen_s(const char *name,const char *mode);


#endif