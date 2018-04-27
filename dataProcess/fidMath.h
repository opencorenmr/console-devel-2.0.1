#ifndef TFIDMATH_H
#define TFIDMATH_H

#include "processElement.h"


class TFIDMath : public TProcessElement
{
public:
    enum TFIDMathOperation {Add,Subtract,Multiply,Divide,
                            Normalize,CorrectOffset,CorrectPhaseOffset,
                            ReversePhase};
    enum TOperationWith {Number,File,Buffer};
    enum TFileType {sm2d,opd,smd};
    TFIDMath();

    TFIDMathOperation FIDMathOperation() {return FFIDMathOperation;}
    TOperationWith operationWith() {return FOperationWith;}
    double number1() {return FNumber1;}
    void setNumber1(double d) {FNumber1=d;}
    double number2() {return FNumber2;}
    void setNumber2(double d) {FNumber2=d;}
    int xini(){return FXIni;}
    void setXIni(int k) {FXIni=k;}
    void setXFin(int k) {FXFin=k;}
    int xfin(){return FXFin;}
    QString fileName() {return FFileName;}
    void setFileName(QString qs) {FFileName=qs;}
    TFileType fileType() {return FFileType;}
    void setFileType(TFileType ft) {FFileType=ft;}

    bool process(TFID *fid);
    bool process(TFID_2D *fid_2d, int k);
    bool process(TFID_2D *fid_2d);

    QStringList processInformation();
    QString command();

public slots:
    void setFIDMathOperation(TFIDMathOperation k) {FFIDMathOperation=k;}
    void setOperationWith(TOperationWith t) {FOperationWith=t;}

private:
    TFIDMathOperation FFIDMathOperation;
    TOperationWith FOperationWith;
    double FNumber1,FNumber2;
    int FXIni,FXFin;
    QString FFileName;
    TFileType FFileType;

    QString mathOperationString();
    QString operationWithString();

    bool operationWithNumber(TFID *fid);
    bool operationWithFile(TFID *fid);
   // bool operationWithBuffer(TFID *fid);
};

#endif // TFIDMATH_H
