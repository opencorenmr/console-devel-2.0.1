#ifndef THYPERCOMPLEX_H
#define THYPERCOMPLEX_H


#include "processElement.h"

class THyperComplex : public TProcessElement
{
public:
    THyperComplex();

   // bool process(TFID *fid) {return true;}
   // bool process(TFID_2D *fid_2d, int k) {return true;}
    bool process(TFID_2D *fid_2d);

    QStringList processInformation();
    QString command();

    bool phaseReverse() {return FPhaseReverse;}
    void setPhaseReverse(bool pr) {FPhaseReverse=pr;}

private:
    bool FPhaseReverse;

};

#endif // THYPERCOMPLEX_H
