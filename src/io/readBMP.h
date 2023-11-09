#ifndef READBMP_H
#define BMP_H


#include "image.h"

typedef struct _BMPFileHeader{
	//because of Struct completion, skip typeTag.
	//uint16_t typeTag; //type tag of bmp( 2 bytes,must be 0x4d42)
	uint32_t fileSize; // file size of bmp, whose unit is byte(4 bytes)
	uint16_t reserved1;// zero reserve(2 bytes)
	uint16_t reserved2;// zero reserve(2 bytes)
	uint32_t offsetBytes;// offset of pixel(2 bytes)
} BMPFileHeader;

typedef struct _BMPImgHeader{
	uint32_t headerSize; //size of img header, whose unit is byte
	uint32_t width; //width of this img,(4 bytes)
	uint32_t height; //height of this img,(4 bytes)
	uint16_t planes; // only one plane for each monitor, so equals to 1;
	uint16_t bitCount;//important.bits per pixel(2 bytes)
	uint32_t compression;
	uint32_t imgSize; //should be fileSize-offsetBytes
	int XPixelsPerMeter;
	int YPixelsPerMeter;
	uint32_t colorUsed; // number of used reference colors
	uint32_t colorImportant;
} BMPImgHeader;

//color table of BMP file, 4 bytes per color
typedef struct _ColorInfo{
	Byte b;
	Byte g;
	Byte r;
	Byte reserved;
} ColorInfo;


Image* readBMP(FILE *BMP);

void storeBMPHeader(FILE *output,BMPFileHeader *fHeader,BMPImgHeader *iHeader);

void writeBMP(FILE *output,Image *img);

FILE *fopen_s(const char *name,const char *mode);


#endif