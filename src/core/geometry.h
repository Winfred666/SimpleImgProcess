#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "image.h"
#include "colorTrans.h"
#include <cmath>


//transform matrix, locate with row and column.
typedef float TransMatrix[3][3];
//transform original point.
typedef float Point2[2];
//upleft-upright-bottomright-bottomleft
typedef Point2 BoundingBox[4];

typedef float (*KernalFunc)(float disx,float disy,float param);

enum TransMethod{TRANS_RAW,TRANS_NEAREST,TRANS_LINEAR,TRANS_RBF};
/**
 * zones that generate when select a image or piece of it
 * ready to make geometry transform on it.
*/
class TransZone{
private:
    //src of transform.
    Image *_src;
    //output after transform, change according to target, dynamically, and bigger size, adjustable.
    Image *_output;
    static const int Padding;
    TransMatrix _mat;
    /**no use within TransZone Coordinate system
    * because center point is transform origin
    * this is a offset to world coordinate, that is, 
    * get this zone from (0,0), but to return this zone to world,we need a nagative offset.
    */
    Point2 _offset;
    //don't change origin,it's binded to _src.
    Point2 _origin;
    //expandZone and keep original point the same position.
    void _expandZone(int expandPad);

public:
    //transfrom matrix
    static const TransMatrix UniMat;
	void _trimPadding();
    TransZone(Image *src,float oriX=0.5f,float oriY=0.5f);
    //copy _output.
    Image *getOutput();
	inline const Image *TransZone::getOriOutput() const {
		return _output;
	}
    /**
     * could apply any matrixes.
     * @note most transforms are suggest to multiply to the left.
    */
    void applyTrans(const TransMatrix apply,bool isleft=true,TransMethod trans=TRANS_LINEAR);
    void resetTrans();
    //kill _output.
    ~TransZone();
    
    
    void getBoudingBox(BoundingBox &dest);
    void getTransCorners(BoundingBox &dest);
    
    //reflect _src point to _output point.
    void src2output(Point2 dest,int srcX,int srcY);
    //reverse way
    void output2src(Point2 dest,int outputX,int outputY);

    //expose to _src iterator.
    inline Image *getOutput4Write(){return _output;}
    inline Image *getSrc4Read(){return _src;}
    inline const TransMatrix &getMatrix4Read(){return _mat;};
    //need it when cover this zone back to bigger image.
    inline const Point2 &getCanvasOffset(){return _offset;}

};



void getScaleMat(TransMatrix &dest,float scaleX,float scaleY);
//anticlock-wise for positive angle
void getRotateMat(TransMatrix &dest,float angle);
void getTranslateMat(TransMatrix &dest,float tx,float ty);
void getSkewMat(TransMatrix &dest,float sx,float sy);

namespace TransTool_ns{
    float distance(const Point2 p1,const Point2 p2);
    /**
     * here value of ori TransMatrix will change!
    */
    void transMatrix(TransMatrix ori,const TransMatrix apply,bool isLeft=1);
    /**
     * here value of ori point will change!
    */
    void transPoint(Point2 ori,const TransMatrix mat);

    extern TransZone *context;

    extern TransMatrix *reverseCtx;
    void transRaw_pixel(Byte* pixel,int x,int y,Image *src);
    void transNearestItp_pixel(Byte *pixel,int x,int y,Image *output);
    void transLiearItp_pixel(Byte *pixel,int x,int y,Image *output);
    void transRBFItp_pixel(Byte *pixel,int x,int y,Image *output);

    float getDet(const TransMatrix mat);
    void getReverseMat(TransMatrix &dest,const TransMatrix ori);

    float gaussian2D(float disx,float disy,float sigma=1);
    float *generateKernal(KernalFunc func,float param);
}

#endif