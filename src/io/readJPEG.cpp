#include <cmath>
#include "readJPEG.h"
#include "colorTrans.h"
#include "bitOpt.h"

using namespace readJPEG_ns;
using std::cout;
using std::endl;

Image* readJPEG(FILE *JPEG){

}

void writeJPEG(FILE *output,Image *img){
    generateStdHuffmanDict();
    generateStdQuantTbl();
    
    //copy an image for output.
    Image *outsrc=NULL;
    //transform color mode.
    if(img->mode==RGB || img->mode==BGR){
        outsrc=new Image(*img);
        RGB2YUV(outsrc);
    }else if(img->mode==GRAY){
        //gray to YUV by extend UV(Cr,Cb) as zero
        outsrc=GRAY2YUV(img);
    }
    if(!outsrc || outsrc->mode!=YUV){
        char ret[70];
        sprintf(ret,"writeJPEG: can't resolve color space %d to YUV!",outsrc->mode);
        throw (string)ret;
    }
    //delete by 128
    outsrc->iterateAll(del128_pixel);
    
    //trim to 8*8 block.
    Byte padding[3]={0,0,0};
    RegionSet *regions=outsrc->getAllRegions(8,8,padding);

    //time to rearrange and split every channel from one region.
    regions->iterateAll(channelAssemble_pixel);
    //begin to compress,for every 8*8 region
    regions->iterateAll(DCT_pixel);
    regions->iterateAll(Zigzag_pixel);
    //every channel follow the sequence:
    //1(DC),2( (bits of 0) + AC),2,...,2,2((0+0))
    memset(lastDC,0,sizeof(lastDC));
    //VLI encode, change data to vli structure
    writeJPEGHeader(output,img);
    //write img data.
    //generate output buffer.
    BitBuffer tempBuffer(64,output);
    buffer = &tempBuffer;
    regions->iterateAll(runCode_pixel);
    buffer->flush();
    buffer=NULL;
    //end of image
    uint16_t EOI=0xD9FF;
    fwrite(&EOI,2,1,output);
    delete outsrc;
    delete regions;
}


namespace readJPEG_ns{
    SByte lastDC[BCount]={0,0,0};
    vector<VLICode> vliVec;
    map<Byte,HuffCode> huffmanDict[4];
    BitBuffer *buffer=NULL;
    uint8_t std_huffman_tbl_cache[std_huffman_tbl_len];
    uint8_t std_luminance_quant_tbl[DCTSIZE2];
    uint8_t std_chrominance_quant_tbl[DCTSIZE2];
}

//all pixel operation need to transfer uint32_to signed.
void readJPEG_ns::del128_pixel(Byte *_pixel,ColorMode mode){
    SByte *pixel=(SByte *)_pixel;
    pixel[0]=(SByte)((int)(_pixel[0])-128);
    pixel[1]=(SByte)((int)(_pixel[1])-128);
    pixel[2]=(SByte)((int)(_pixel[1])-128);
    // pixel[0]-=128;
    // pixel[1]-=128;
    // pixel[3]-=128;
}

void readJPEG_ns::channelAssemble_pixel(Byte *_region,ColorMode mode){
    SByte *region=(SByte *)_region;
    const int hwsize=edge*edge;
    const int size=hwsize*BCount;

    SByte temp[size];
    memcpy(temp,region,size);
    for(int cha=0;cha<BCount;cha++){
        //locate specifit channel zone.
        SByte *destBase=region+hwsize*cha;
        for(int pi=0;pi<hwsize;pi++){
            //encode the same channel together
            *(destBase+pi)=*(temp+pi*BCount+cha);
            //cout<<"pixel: "<<(int)*(destBase+pi)<<endl;
        }
    }
}

void readJPEG_ns::DCT_pixel(Byte *_region,ColorMode mode){
    SByte *region=(SByte *)_region;
    const int hwsize=edge*edge;
    //copy this 8*8 region(three for every pixel)
    const int size=hwsize*BCount;
    const float zeroC=1.0/sqrt(2);
    SByte temp[size];
    memcpy(temp,region,size);
    //count every channel
    for(int cha=0;cha<BCount;cha++){
        SByte *destBase=region+hwsize*cha, *srcBase=temp+hwsize*cha;
        //count every pixel.
        for(int u=0;u<edge;u++){
            for(int v=0;v<edge;v++){
                SByte *pix=destBase+(u*edge+v);
                //do the colvo
                float colvo=0;
                for(int y=0;y<edge;y++){
                for(int x=0;x<edge;x++){
                    colvo+=(*(srcBase+y*edge+x))*
                    cos((2*y+1)*u*PID16)*
                    cos((2*x+1)*v*PID16);
                }
                }
                if(u==0)
                    colvo*=zeroC;
                if(v==0)
                    colvo*=zeroC;
                colvo/=4.0;
                //qualify also,for Y channel:
                if(cha==0){
                    colvo/=std_luminance_quant_tbl[u*edge+v];
                }else{//for C channel
                    colvo/=std_chrominance_quant_tbl[u*edge+v];
                }
                *pix=(SByte)colvo;
                //if(u==0 && v==0) cout<<colvo<<"\t"<<(int)*pix<<endl;
            }
        }
        
    }
}

void readJPEG_ns::Zigzag_pixel(Byte *_region,ColorMode mode){
    SByte *region=(SByte *)_region;
    
    const int hwsize=edge*edge;
    const int size=hwsize*BCount;
    SByte temp[size];
    memcpy(temp,region,size);
    for(int cha=0;cha<BCount;cha++){
        SByte *srcBase=temp+hwsize*cha, *destBase=region+hwsize*cha;
        for(int pi=0;pi<hwsize;pi++){
            //jump to specifit byte,only operate one channel per time
            *(destBase+pi)=*(srcBase+std_zigzag_tbl[pi]);
            //if(pi==0) cout<<" ori: "<<(int)(*(destBase+pi))<<endl;
        }
    }
}

void readJPEG_ns::runCode_pixel(Byte *_region,ColorMode mode){
    SByte *region=(SByte *)_region;
    const int hwsize=edge*edge;
    //three channels seperately
    for(int cha=0;cha<BCount;cha++){
        //clear vector
        vliVec.clear();

        SByte *src=region+cha*hwsize;
        //count DC
        SByte newDC=src[0]-lastDC[cha];
        //every time create a new segment, add it to ali vector
        vliEncode(0,newDC);
        lastDC[cha]=src[0];
        //count AC
        int k=1,sum=0;
        while(k<hwsize){
            //encounter a non-zero number,
            if(src[k]!=0){
                //if there is zero ahead, count zero first
                while(sum>=16){
                    vliEncode(15,0);
                    sum-=16;
                }
                vliEncode(sum,src[k]);
                sum=0;
            }else sum++;
            k++;
        }
        //end with 0, add ELB
        if(src[63]==0){
            //cout<<"cur is End"<<endl;
            vliEncode(0,0);
        }
        //instantly go Huffman when this channel is finished.
        huffmanEncode(cha);
    }
}

SByte readJPEG_ns::countBits(SByte number){
    if(number==0) return 0;
    number=abs(number);
    int ret=1;
    while((number/=2,number!=0)){
        ret++;
    }
    return ret;
}

void readJPEG_ns::vliEncode(int runlen,SByte data){
    VLICode code;
    code.codeLen=countBits(data);
    code.runlen=runlen;
    if(data<0){
        //get 1' complement code.
        code.codedata=(1<<code.codeLen)-1+data;
    }else{
        code.codedata=data;
    }
    vliVec.push_back(code);
}

void readJPEG_ns::huffmanEncode(int channel){
    bool isUV=0;
    if(channel!=0) isUV=1;
    //channel 0, Y
    int size=vliVec.size();
    for(int q=0;q<size;q++){
        Byte key=((vliVec[q].runlen<<4) | vliVec[q].codeLen);
        int offset=2; //for AC code.
        if(q==0) // for DC code.
            offset=0;
        map<Byte,HuffCode>::iterator iter = huffmanDict[offset+isUV].find(key);
        
        if(iter == huffmanDict[offset+isUV].end()){
            char temp[100];
            sprintf(temp,"jpeg: huffman dictionary is missing the encoding of %#x,\n\tcombine from (%d,%d) in %s mode!"
                ,key,vliVec[q].runlen,vliVec[q].codeLen,(q==0?"DC":"AC"));
            throw (string)temp;
        }
        if(buffer==NULL){
            throw "jpeg: missing bit buffer when output binary!";
        }
        //std::cout<<(int)(iter->second).codeLen<<"  "<<(iter->second).codedata<<"  ";
        //std::cout<<(int)vliVec[q].codeLen<<"  "<<vliVec[q].codedata<<std::endl;
        
        buffer->push(iter->second.codedata,iter->second.codeLen);
        buffer->push(vliVec[q].codedata,vliVec[q].codeLen);
    }
    
}






void readJPEG_ns::generateHuffmanDict(FILE *src){
    int tblCount=0,index=0;
    HuffHeader header;
    HuffCode code;
    Byte nums[16],symbol;
    //current huff code data.
    uint16_t codedata=0x0000;
    while(1){
        if(tblCount==4) break;
        fread(&header,sizeof(HuffHeader),1,src);
        if(header.tagSign1!=0xff || header.tagSign2!=0xc4)
            break;
        codedata=0x0000;
        for(index=0;index<4;index++)
            if(huffmanDictIndex[index]==header.tableSign) break;
        fread(nums,sizeof(Byte),16,src);
        for(int q=0;q<16;q++){
            int codeLen=q+1;
            //generate "nums[q]" of huffman codes.
            for(int w=0;w<nums[q];w++){
                fread(&symbol,1,1,src);
                code.codeLen=codeLen;
                code.codedata=codedata;
                huffmanDict[index][symbol]=code;
                codedata++;
            }
            codedata<<=1;
        }
        tblCount++;
    }
    if(tblCount==0){
        char err[100];
        sprintf(err,"try to generate a huffman table with header %#x%#x ,which is not DHT signed(oxffc4)!",header.tagSign1,header.tagSign2);
        throw (string)err;
    }
}

void readJPEG_ns::generateStdHuffmanDict(){
    FILE *stdio=fopen(std_huffman_tbl_path,"rb");
    if(stdio==NULL)
        throw "could not open standard huffman table in public/stdio !";
    
    fread(std_huffman_tbl_cache,std_huffman_tbl_len,1,stdio);
    fseek(stdio,0,SEEK_SET);
    generateHuffmanDict(stdio);
    fclose(stdio);
}

void readJPEG_ns::generateQuantTbl(FILE *src){
    DQTHeader header;
    for(int q=0;q<2;q++){
        fread(&header,sizeof(DQTHeader),1,src);
        if(header.marker!=0xDBFF){
            throw "try to generate quantify tabel with illegal marker!";
        }
        if(header.qTableID==0x00)
            fread(std_luminance_quant_tbl,1,DCTSIZE2,src);
        else
            fread(std_chrominance_quant_tbl,1,DCTSIZE2,src);
    }
}

void readJPEG_ns::generateStdQuantTbl(){
    FILE *stdio=fopen(std_quant_tbl_path,"rb");
    if(stdio==NULL)
        throw "could not open standard quantify table in public/stdio !";
    generateQuantTbl(stdio);
}









void readJPEG_ns::writeJPEGHeader(FILE *output,Image* img){
    //write header
    loadJFIFHeader(output);
    //write DQT
    loadStdDQT(output);
    //write SOF0
    loadStdSOF0(output,img);
    //write DHT
    fwrite(std_huffman_tbl_cache,1,std_huffman_tbl_len,output);
    loadSOS(output);
}

void readJPEG_ns::loadJFIFHeader(FILE *output,uint8_t unit,uint16_t density){
    JFIFHeader header;
    header.marker = 0xD8FF;
    header.app0Marker = 0xE0FF;

    header.app0Length = swap16(sizeof(JFIFHeader) - 4);

    strcpy(header.identifier, "JFIF");
    header.version = 0x0101; // JFIF version 1.01
    header.units = unit; // No units
    header.xDensity = swap16(density); // You can set your pixel density values here
    header.yDensity = swap16(density);
    header.xThumbnail = 0;
    header.yThumbnail = 0;
    fwrite(&header,sizeof(JFIFHeader),1,output);
}


void readJPEG_ns::loadStdSOF0(FILE* output,Image *img){
    SOF0Header sofh;
    sofh.marker=0xC0FF;
    sofh.length=swap16(sizeof(SOF0Header)-2);
    sofh.dataPrecision=8;
    sofh.imageHeight=swap16(img->height());
    sofh.imageWidth=swap16(img->width());
    sofh.numComponents=3;
    for(int q=0;q<3;q++){
        sofh.components[q].componentID=q+1;
        sofh.components[q].samplingFactors=(q==0?0x11:0x11);
        sofh.components[q].quantizationTableID=(q==0?0x00:0x01);
    }
    fwrite(&sofh,sizeof(SOF0Header),1,output);
}

void readJPEG_ns::loadStdDQT(FILE *output){
    DQTHeader dqth;
    dqth.marker=0xDBFF;
    //dqth.precision=0;
    //2+1+1+64
    dqth.length=swap16(sizeof(DQTHeader)-2+64);
    dqth.qTableID=0;
    fwrite(&dqth,sizeof(DQTHeader),1,output);
    fwrite(std_luminance_quant_tbl,1,DCTSIZE2,output);
    dqth.qTableID=1;
    fwrite(&dqth,sizeof(DQTHeader),1,output);
    fwrite(std_chrominance_quant_tbl,1,DCTSIZE2,output);
}

void readJPEG_ns::loadSOS(FILE *output){
    SOSHeader sosh;
    sosh.marker=0xDAFF;
    sosh.length=swap16(sizeof(SOSHeader)-2);
    sosh.numComponents=3;
    for(int q=0;q<3;q++){
        sosh.components[q].componentID=q+1;
        sosh.components[q].huffmanTable=((q!=0)<<4) | (q!=0);
    }
    sosh.spectralSelectionStart=0;
    sosh.spectralSelectionEnd=63;
    sosh.successiveApproximation=0;
    fwrite(&sosh,sizeof(SOSHeader),1,output);
}

uint16_t readJPEG_ns::swap16(uint16_t value){
    return ((value & 0xFF00) >> 8) | ((value & 0x00FF) << 8);
}