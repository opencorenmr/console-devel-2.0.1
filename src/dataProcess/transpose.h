#ifndef TTRANSPOSE_H
#define TTRANSPOSE_H


#include "processElement.h"

class TTranspose : public TProcessElement
{
public:
    TTranspose();

   // bool process(TFID *fid) {return true;}
   // bool process(TFID_2D *fid_2d, int k) {return true;}
    bool process(TFID_2D *fid_2d);

    QStringList processInformation();
    QString command();

};

#endif // TTRANSPOSE_H
