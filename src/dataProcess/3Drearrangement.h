#ifndef S3DREARRANGEMENT_H
#define S3DREARRANGEMENT_H

#include "processElement.h"

class STranslation : public TProcessElement
{
public:
    STranslation();

    bool process(TFID_2D *fid_2D,int size1,int size2);

    int d1Displacement,d2Displacement,abscissaDisplacement;
};

class SxyzRotation : public TProcessElement
{
public:
    SxyzRotation();

    bool process(TFID_2D *fid_2D,int size1,int size2);
};

#endif // 3DREARRANGEMENT_H
