#ifndef BITOPT_H
#define BITOPT_H

#include <iostream>
#include <queue>
using std::queue;
class BitBuffer{
private:
    queue<bool> bits;
    FILE *output;
    int buffer;
    bool padding;
public:
    BitBuffer(int buffer,FILE *output,bool padding=1);
    void push(int data,int len);
    int bitCount();
    void flush();
    void outputBytes();
};

#endif