#ifndef TFIDMATH_H
#define TFIDMATH_H

#include "processElement.h"


class TFIDMath : public TProcessElement
{
public:

// ---- moved to processElement.h (20 Feb 2023, KT) ----
//
//    enum TFIDMathOperation {Add,Subtract,Multiply,Divide,
//                            Normalize,CorrectOffset,CorrectPhaseOffset,
//                            ReversePhase, Invalid};
//    enum TFIDMathOperationWith {Number,File,Buffer};
//-------------------------------------------------------
    enum TFileType {sm2d,opd,smd};
    TFIDMath();

    TFIDMathOperation FIDMathOperation() {return FFIDMathOperation;}
    QString FIDMathOperationStr();
    void setFIDMathOperationStr(QString qs);

    TFIDMathOperationWith FIDMathOperationWith() {return FOperationWith;}
    QString FIDMathOperationWithStr();
    void setFIDMathOperationWithStr(QString qs);

    double FIDMathReal() {return FNumber1;}
    void setFIDMathReal(double d) {FNumber1=d;}
    double FIDMathImag() {return FNumber2;}
    void setFIDMathImag(double d) {FNumber2=d;}
    int FIDMathXIni() {return FXIni;}
    int FIDMathXFin() {return FXFin;}
    void setFIDMathXIni(int k) {FXIni=k;}
    void setFIDMathXFin(int k) {FXFin=k;}
    QString FIDMathDirName() {return FDirName;}
    QString FIDMathFileName() {return FFileName;}
    void setFIDMathDirName(QString qs) {FDirName=qs;}
    void setFIDMathFileName(QString qs) {FFileName=qs;}
    QString absoluteFileName() {return FDirName+"/"+FFileName;}



    bool process(TFID *fid);
    bool process(TFID_2D *fid_2d, int k);
    bool process(TFID_2D *fid_2d);

    QStringList processInformation();
    QString command();

public slots:
    void setFIDMathOperation(TFIDMathOperation k) {FFIDMathOperation=k;}
    void setFIDMathOperationWith(TFIDMathOperationWith t) {FOperationWith=t;}

private:
    TFIDMathOperation FFIDMathOperation;
    TFIDMathOperationWith FOperationWith;
    double FNumber1,FNumber2;
    int FXIni,FXFin;
    QString FDirName;
    QString FFileName;
    TFileType FFileType;
    TFID_2D *afid2=nullptr;
  //  bool loadFlag=true;

    TFileType fileType() {return FFileType;}
    void setFileType(TFileType ft) {FFileType=ft;}

    bool loadFile();

    bool operationWithNumber(TFID *fid);
    bool operationWithFile(TFID *fid);
    bool operationWithFile(TFID_2D *fid_2D);
    bool operationWith2DFile(TFID_2D *fid_2D);
   // bool operationWithBuffer(TFID *fid);
};

#endif // TFIDMATH_H
