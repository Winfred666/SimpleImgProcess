#include "geometry.h"
#include "colorTrans.h"
#include <cmath>

#ifndef PI
#define PI acos(-1)
#endif

namespace TransTool_ns{
    TransZone *context=NULL;
    TransMatrix *reverseCtx=NULL;
    RegionSet *kernalCtx=NULL;
    int KerSize=5;
    float sigmaOfGaussian=1.0f;
    float *weightKer=NULL;
    const float DSqrt2PI=1.0f/sqrtf(2*PI);
}

using namespace TransTool_ns;

const TransMatrix TransZone::UniMat={{1,0,0},{0,1,0},{0,0,1}};
const int TransZone::Padding=5;

TransZone::TransZone(Image *source,float oriX,float oriY){
    if(source->bytePerPixel()<4)
        _src=RGB2RGBA(source);
    //a bigger canvas,have opacity channel, and size is the expanding block from original point.
    const int oldW=_src->width();
    const int oldH=_src->height();
    //set the origin
    _origin[0]=oriX*oldW; _origin[1]=oriY*oldH;
    
    //calculate the farthest point distance and set it as stretch;
    Point2 farP={(float)((oriX<0.5f)*oldW) , (float)((oriY<0.5f)*oldH)};
    int dis=(int)distance(_origin,farP)+Padding;
    
    //if needed, cover this piece back to original bigger image on the same position.
    _offset[0]=_origin[0]-dis;
    _offset[1]=_origin[1]-dis;
    //for potential rotate, TransZone must be square+XXXA
    _output=new Image(dis*2,dis*2,_src->bytePerPixel()*8,_src->mode);
    _output->clear();
    //set image piece to center.
    _output->setROI(-_offset[0],-_offset[1],_src);
    
    memcpy(_mat,UniMat,sizeof(TransMatrix));
}

//must be considerably bigger than size of current output.
void TransZone::_expandZone(int expandPad){
    if(expandPad<0) return;
    expandPad+=Padding;
    _offset[0]-=expandPad;
    _offset[1]-=expandPad;
    int newSize=_output->width()+2*expandPad;
    Image *newZone=new Image(newSize,
        newSize ,_output->bytePerPixel()*8 ,_output->mode);
    newZone->clear();
    newZone->setROI(expandPad,expandPad,_output);
    delete _output;
    _output=newZone;
}

void TransZone::_trimPadding(){
    BoundingBox bbox;
    getBoudingBox(bbox);
    Image *temp = _output->getROI(bbox[0][0],bbox[0][1],bbox[2][0],bbox[2][1]);
    delete _output;
    _output=temp;
    _offset[0]+=bbox[0][0];
    _offset[1]+=bbox[0][1];
}

//copy _output and return.
Image *TransZone::getOutput(){
    _trimPadding();
    Image *res=new Image(*_output);
    return res;
}

void TransZone::src2output(Point2 dest,int srcX,int srcY){
    dest[0]=srcX-_origin[0];
    dest[1]=srcY-_origin[1];
    TransTool_ns::transPoint(dest,_mat);
    dest[0]+=(_origin[0] - _offset[0]);
    dest[1]+=(_origin[1] - _offset[1]);
}

void TransZone::output2src(Point2 dest,int outputX,int outputY){
    if(reverseCtx==NULL) throw "output2src: do not have context, fail to get reversed matrix!!";
    dest[0]=outputX+_offset[0]-_origin[0];
    dest[1]=outputY+_offset[1]-_origin[1];
    TransTool_ns::transPoint(dest,*reverseCtx);
    dest[0]+=_origin[0];
    dest[1]+=_origin[1];
}


void TransZone::getTransCorners(BoundingBox &dest){
    const int Xlist[4]={0,_src->width()-1,_src->width()-1,0};
    const int Ylist[4]={0,0,_src->height()-1,_src->height()-1};
    for(int q=0;q<4;q++){
        src2output(dest[q],Xlist[q],Ylist[q]);
    }
}

void TransZone::getBoudingBox(BoundingBox &dest){
    getTransCorners(dest);
    int minX=dest[0][0],maxX=minX,minY=dest[0][1],maxY=minY;
    for(int q=1;q<4;q++){
        if(dest[q][0]<minX) minX=dest[q][0];
        else if(dest[q][0]>maxX) maxX=dest[q][0];
        if(dest[q][1]<minY) minY=dest[q][1];
        else if(dest[q][1]>maxY) maxY=dest[q][1];
    }
    dest[0][0]=dest[3][0]=minX;
    dest[1][0]=dest[2][0]=maxX;
    dest[0][1]=dest[1][1]=minY;
    dest[2][1]=dest[3][1]=maxY;
}

void TransZone::applyTrans(const TransMatrix apply,bool isleft,TransMethod method){
    //renew transMatrix.
    transMatrix(_mat,apply,isleft);
    //check whether need to expand canvas.
    BoundingBox bb;
    getBoudingBox(bb);
    int max=_output->width()-1;
    int excess=0;
    float bigger[4]={0-bb[0][0],0-bb[0][1],bb[2][0]-max,bb[2][1]-max};
    for(int q=0;q<4;q++)
        if(bigger[q]>excess) excess=(int)bigger[q];
    //expand canvas
    if(excess>0)
        _expandZone(excess);
    context=this;
    TransMatrix rev;
    getReverseMat(rev,_mat);
    reverseCtx=&rev;
    //to fill hole, we iterate _output instead of input
    switch(method){
        case TRANS_RAW:
            _output->clear();
            _src->iterateAll(transRaw_pixel);
            break;
        case TRANS_LINEAR:
            _output->iterateAll(transLiearItp_pixel);
            break;
        case TRANS_NEAREST:
            _output->iterateAll(transNearestItp_pixel);
            break;
        case TRANS_RBF:
            Byte padd[5];
            memset(padd,0,sizeof(padd));
            kernalCtx=_src->getAllKernals(KerSize,padd);//need to get all kernal first.            
            //make kernal.
            generateKernal(gaussian2D,sigmaOfGaussian);
            _output->iterateAll(transRBFItp_pixel);
            
            //delete context;
            free(weightKer);
            weightKer=NULL;
            delete kernalCtx;
            kernalCtx=NULL;
            break;
    }
    reverseCtx=NULL;
    context=NULL;
}

void TransZone::resetTrans(){
    memcpy(_mat,UniMat,sizeof(TransMatrix));
}

//kill _output.
TransZone::~TransZone(){
    delete _output;
    delete _src;
}








//generate useful transform matrix
void getScaleMat(TransMatrix &dest,float scaleX,float scaleY){
    memset(&dest,0,sizeof(TransMatrix));
    dest[0][0]=scaleX;
    dest[1][1]=scaleY;
    dest[2][2]=1;
}

//anticlock-wise for positive angle
void getRotateMat(TransMatrix &dest,float angle){
    memset(&dest,0,sizeof(TransMatrix));
    float sine=sinf(angle*PI/180);
    float cose=cosf(angle*PI/180);
    dest[0][0]=dest[1][1]=cose;
    dest[0][1]=-sine;
    dest[1][0]=sine;
    dest[2][2]=1;
}

void getTranslateMat(TransMatrix &dest,float tx,float ty){
    memcpy(dest,TransZone::UniMat,sizeof(TransMatrix));
    dest[0][2]=tx;
    dest[1][2]=ty;
}

void getSkewMat(TransMatrix &dest,float sx,float sy){
    memcpy(dest,TransZone::UniMat,sizeof(TransMatrix));
    dest[0][1]=sx;
    dest[1][0]=sy;
}


void TransTool_ns::transRBFItp_pixel(Byte *pixel,int x,int y,Image *output){
    Point2 opt;
    const int BPP=output->bytePerPixel();
    context->output2src(opt,x,y);
    Image *src=context->getSrc4Read();
    memset(pixel,0,BPP);
    if(opt[0]<=0 || opt[0]>=src->width()-1 || opt[1]<=0 || opt[1]>=src->height()-1)
        return;
    //get the nearest kernal and begin to calculate.
    
    Byte *imgKer=kernalCtx->getPixel(roundf(opt[0]),roundf(opt[1]));
    
    float result[5];
    memset(result,0,sizeof(result));
    for(int ky=0;ky<KerSize;ky++){
        for(int kx=0;kx<KerSize;kx++){
             float weight=weightKer[ky*KerSize+kx];
             Byte *onePix=imgKer+(ky*KerSize+kx)*BPP;
             for(int cha=0;cha<BPP;cha++)
                result[cha]+=onePix[cha]*weight;
        }
    }

    for(int cha=0;cha<BPP;cha++)
        pixel[cha]=(Byte)result[cha];
}

void TransTool_ns::transLiearItp_pixel(Byte *pixel,int x,int y,Image *output){
    Point2 opt;
    context->output2src(opt,x,y);
    Image *src=context->getSrc4Read();
    if(opt[0]<=0 || opt[0]>=src->width()-1 || opt[1]<=0 || opt[1]>=src->height()-1)
        memset(pixel,0,output->bytePerPixel());
    else{
        //calculate near four pixel.
        const int channels=src->bytePerPixel();
        Point2 xran={floorf(opt[0]),ceilf(opt[0])};
        Point2 yran={floorf(opt[1]),ceilf(opt[1])};
        Point2 offset={opt[0]-xran[0],opt[1]-yran[0]};
        Byte *nearBy[4];
        for(int q=0;q<4;q++){
            nearBy[q]=src->getPixel(xran[(q==1||q==2)],yran[(q>1)]);
        }
        //every channel
        int a,b,c,d;
        for(int cha=0;cha<channels;cha++){
            d=nearBy[0][cha];
            a=nearBy[1][cha]-d;
            b=nearBy[3][cha]-d;
            c=nearBy[2][cha]-a-b-d;
            pixel[cha]=a*offset[0]+b*offset[1]+c*offset[0]*offset[1]+d;
        }
    }
}

void TransTool_ns::transNearestItp_pixel(Byte *pixel,int x,int y,Image *output){
    if(reverseCtx==NULL) return;
    Point2 opt;
    context->output2src(opt,x,y);
    Image *src=context->getSrc4Read();
    if(opt[0]<=0 || opt[0]>=src->width()-1 || opt[1]<=0 || opt[1]>=src->height()-1)
        memset(pixel,0,output->bytePerPixel());
    else
        memcpy(pixel,
            src->getPixel(round(opt[0]),round(opt[1])),
            output->bytePerPixel());
}

void TransTool_ns::transRaw_pixel(Byte* pixel,int x,int y,Image *src){
    if(context==NULL) return;
    Point2 opt;
    context->src2output(opt,x,y);
    //use offset to write this pixel.
    memcpy(context->getOutput4Write()->getPixel(opt[0],opt[1])
        ,pixel
        ,src->bytePerPixel());
}


void TransTool_ns::transPoint(Point2 ori,const TransMatrix mat){
    //make point agen first.
    float agen[3]={ori[0],ori[1],1},
    dest[3];
    //apply ori to the left
    for(int r=0;r<3;r++){
        dest[r]=0;
        for(int it=0;it<3;it++){
            dest[r]+=mat[r][it]*agen[it];
        }
    }
    ori[0]=dest[0]/dest[2];
    ori[1]=dest[1]/dest[2];
}

void TransTool_ns::transMatrix(TransMatrix ori,const TransMatrix apply,bool isLeft){
    TransMatrix copy;
    memcpy(copy,ori,sizeof(TransMatrix));
    for(int r=0;r<3;r++){
        for(int c=0;c<3;c++){
            ori[r][c]=0;
            for(int it=0;it<3;it++){
                if(isLeft)
                    ori[r][c]+=apply[r][it]*copy[it][c];
                else
                    ori[r][c]+=copy[r][it]*apply[it][c];
            }
        }
    }

}

float TransTool_ns::distance(const Point2 p1,const Point2 p2){
    return sqrtf((p1[0]-p2[0])*(p1[0]-p2[0])+(p1[1]-p2[1])*(p1[1]-p2[1]));
}


float TransTool_ns::getDet(const TransMatrix mat){
    float ret=0;
    int flag=1;
    for(int q=0;q<3;q++){
        int a=(q+1)%3;
        int b=(q+2)%3;
        ret+=flag*mat[q][q]*
            (mat[a][a]*mat[b][b]-mat[a][b]*mat[b][a]);
        flag*=-1;
    }
    return ret;
}

void TransTool_ns::getReverseMat(TransMatrix &dest,const TransMatrix ori){
    float det=getDet(ori);
    for(int r=0;r<3;r++){
        int smr=(r==0 ? 1:0);
        int lgr=(r==2 ? 1:2);
        for(int c=0;c<3;c++){
            int smc=(c==0 ? 1:0);
            int lgc=(c==2 ? 1:2);
            dest[c][r] = ((r+c)%2==0 ? 1 : -1)
            *(ori[smr][smc] * ori[lgr][lgc] - ori[smr][lgc] * ori[lgr][smc])
            /det;
        }
    }
}

float *TransTool_ns::generateKernal(KernalFunc func,float param){
    weightKer=(float *)malloc(sizeof(float)*KerSize*KerSize);
    float sum=0.0f,temp;
    for(int y=0;y<KerSize;y++){
        for(int x=0;x<KerSize;x++){
            temp=func(abs(KerSize/2-x),abs(KerSize/2-y),param);
            *(weightKer+y*KerSize+x)=temp;
            sum+=temp;
        }
    }
    for(int y=0;y<KerSize;y++){
        for(int x=0;x<KerSize;x++){
            *(weightKer+y*KerSize+x)/=sum;
        }
    }
}

float TransTool_ns::gaussian2D(float disx,float disy,float sigma){
    float A=DSqrt2PI/sigma;
    sigma=sigma*sigma*2;
    return A*expf(-(1.0f*disx*disx/sigma)-
        (1.0f*disy*disy/sigma));
}