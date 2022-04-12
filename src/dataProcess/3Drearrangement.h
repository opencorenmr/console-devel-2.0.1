#ifndef S3DREARRANGEMENT_H
#define S3DREARRANGEMENT_H

#include "processElement.h"

class SSetCenterattheOrigin : public TProcessElement
{
public:
    SSetCenterattheOrigin();

    bool process(TFID_2D *fid_2D,int size1,int size2);
};

class SxyzRotation : public TProcessElement
{
public:
    SxyzRotation();

    bool process(TFID_2D *fid_2D,int size1,int size2);
};

#endif // 3DREARRANGEMENT_H
