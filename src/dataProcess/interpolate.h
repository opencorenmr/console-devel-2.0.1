#ifndef KINTERPOLATE_H
#define KINTERPOLATE_H


#include "processElement.h"

class KInterpolate : public TProcessElement
{
public:
    KInterpolate();

    bool processInterpolate(TFID_2D *fid_2d, double delta);

    QStringList processInformation();
    QString command();

};

#endif // TTRANSPOSE_H
