#include  "bitOpt.h"



BitBuffer::BitBuffer(int buffer,FILE *output,bool padding):bits(),output(output),padding(padding){}

void BitBuffer::push(const int data,int len){
    while(len>0){
        len--;
        bits.push((data & (1<<len)));
    }
    //dequeue a byte and output it.
    if(bits.size()<buffer) return;
    outputBytes();
}

int BitBuffer::bitCount(){
    return bits.size();
}
void BitBuffer::outputBytes(){
    if(output==NULL) return;
    uint8_t out=0;
    while(bits.size()>=8){
        out=0;
        for(int q=0;q<8;q++){
            out<<=1;
            out+=bits.front();
            bits.pop();
        }
        fwrite(&out,1,1,output);
        //here is the post effect of output, from jpeg
        if(out==0xFF){
            out=0x00;
            fwrite(&out,1,1,output);
        }
    }
}

void BitBuffer::flush(){
    outputBytes();
    int remain=bits.size();
    if(remain==0) return;
    uint8_t out=0;
    for(int q=0;q<8;q++){
        out<<=1;
        if(q<remain){
            out|=bits.front();
            bits.pop();
        }else{
            out|=padding;
        }
    }
    fwrite(&out,1,1,output);
}