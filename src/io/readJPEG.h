#ifndef READJPEG_H
#define READJPEG_H

#include "image.h"
#include "string.h"
#include "colorTrans.h"
#include "bitOpt.h"
#include <vector>
#include <map>
using std::map;
using std::vector;
using std::pair;
using std::string;

typedef signed char SByte;

Image* readJPEG(FILE *JPEG);

void writeJPEG(FILE *output,Image *img);


namespace readJPEG_ns{
    const float PID16=3.14/16;
    const int DCTSIZE2=64;
    //标准亮度分量量化表
    extern uint8_t std_luminance_quant_tbl[DCTSIZE2];
    //标准色度分量量化表
    extern uint8_t std_chrominance_quant_tbl[DCTSIZE2];
    
    const uint8_t std_zigzag_tbl[DCTSIZE2] = {
        0, 1, 5, 6, 14, 15, 27, 28,
        2, 4, 7, 13, 16, 26, 29, 42,
        3, 8, 12, 17, 25, 30, 41, 43,
        9, 11, 18, 24, 31, 40, 44, 53,
        10, 19, 23, 32, 39, 45, 52, 54,
        20, 22, 33, 38, 46, 51, 55, 60,
        21, 34, 37, 47, 50, 56, 59, 61,
        35, 36, 48, 49, 57, 58, 62, 63
    };



    //basic information of a 8*8 region.
    const int edge=8;
    const int BCount=3;
    const int std_huffman_tbl_len=432;
    //last DC value, give runcode process to make substraction to DC. 
    extern SByte lastDC[BCount];

    void del128_pixel(Byte *pixel,ColorMode mode);
    void DCT_pixel(Byte *region,ColorMode mode);
    void Zigzag_pixel(Byte *region,ColorMode mode);
    void channelAssemble_pixel(Byte *region,ColorMode mode);
    void runCode_pixel(Byte *region,ColorMode mode);

    SByte countBits(SByte number);

    typedef struct{
        //number of zero ahead,use half byte instead.
        Byte runlen;
        //bit number of this vli code,
        Byte codeLen;
        //bit data of this vli code,16 bits(2 byte) mostly, and length is indicated by codeLen.
        // use smaller storage.(float right)
        uint16_t codedata;
    } VLICode;

    
    #pragma pack(1)
    typedef struct{
        uint8_t tagSign1;
        uint8_t tagSign2;
        uint16_t partSize;
        uint8_t tableSign;
    } HuffHeader;

    typedef struct{
        Byte codeLen;
        // use smaller storage.(float right)
        uint16_t codedata;
    } HuffCode;

    extern vector<VLICode> vliVec;
    //order, first DC second AC.
    const Byte huffmanDictIndex[4]={0x00,0x01,0x10,0x11};
    
    //one standart huffman generator, as an example of DHT.
    const char std_huffman_tbl_path[]="../public/stdio/huffman-tbl.bin";
    const char std_quant_tbl_path[]="../public/stdio/quant-tbl.bin";
    

    extern uint8_t std_huffman_tbl_cache[std_huffman_tbl_len];
    //from byte symbol to binary code(use padding)
    extern map<Byte,HuffCode> huffmanDict[4];
    extern BitBuffer *buffer;
    //read from x\ffx\c4, and generate four table.
    void generateHuffmanDict(FILE *src);
    void generateStdHuffmanDict();

    void generateQuantTbl(FILE *src);
    void generateStdQuantTbl();

    void vliEncode(int runlen,SByte data);
    void huffmanEncode(int channel);


    #pragma pack(1)
    typedef struct {
        uint16_t marker; // 0xFFD8: SOI (Start of Image)
        uint16_t app0Marker; // 0xFFE0: APP0 marker
        uint16_t app0Length; // Length of APP0 segment (including this field)
        char identifier[5]; // "JFIF\0"
        uint16_t version;
        uint8_t units; // Units: 0 - none, 1 - inch, 2 - cm
        uint16_t xDensity; // Horizontal pixel density
        uint16_t yDensity; // Vertical pixel density
        uint8_t xThumbnail; // X Thumbnail size
        uint8_t yThumbnail; // Y Thumbnail size
        // (The rest of the structure can include optional thumbnail data)
    } JFIFHeader;

    #pragma pack(1)
    typedef struct {
        uint16_t marker; // Marker for DQT (0xFFDB)
        uint16_t length; // Length of DQT segment
        //uint8_t precision; // Precision (0: 8-bit, 1: 16-bit)
        uint8_t qTableID; // Quantization table ID (0-3)
        //uint8_t quantizationTable[64]; // Quantization table data
    } DQTHeader;


    #pragma pack(1)
    typedef struct{
        uint16_t marker; // Marker for SOF0 (0xFFC0)
        uint16_t length; // Length of SOF0 segment
        uint8_t dataPrecision; // Data precision (8 or 12 bits)
        uint16_t imageHeight; // Image height
        uint16_t imageWidth; // Image width
        uint8_t numComponents; // Number of image components (usually 3: Y, Cb, Cr)
        // Information for each component
        struct ComponentInfo {
            uint8_t componentID;
            uint8_t samplingFactors; // Sampling factors (bit 0-3: vertical, bit 4-7: horizontal)
            uint8_t quantizationTableID; // Quantization table ID (0-3)
        } components[3]; // 3 components for typical YCbCr color space
    } SOF0Header;

    #pragma pack(1)
    typedef struct {
    uint16_t marker; // Marker for SOS (0xFFDA)
    uint16_t length; // Length of SOS segment
    uint8_t numComponents; // Number of image components in the scan (usually 3: Y, Cb, Cr)
    // Information for each component
    struct ComponentInfo {
        uint8_t componentID; // Component ID
        uint8_t huffmanTable; // Huffman table (bit 0-3: DC table, bit 4-7: AC table)
    } components[3]; // 3 components for typical YCbCr color space
    uint8_t spectralSelectionStart; // Start of spectral selection (usually 0)
    uint8_t spectralSelectionEnd; // End of spectral selection (usually 63)
    uint8_t successiveApproximation; // Successive approximation bit position (usually 0)
    } SOSHeader;

    void loadJFIFHeader(FILE *output,uint8_t unit=0,uint16_t density=96);
    void loadStdDQT(FILE *output);
    void loadSOS(FILE *output);
    void writeJPEGHeader(FILE *output,Image* img);
    void loadStdSOF0(FILE* output,Image *img);

    uint16_t swap16(uint16_t value);
}



#endif