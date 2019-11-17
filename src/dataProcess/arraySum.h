#ifndef ARRAYSUM_H
#define ARRAYSUM_H

#include "processElement.h"

class TArraySum : public TProcessElement
{
public:
    TArraySum();
    ~TArraySum();

    bool process(TFID_2D *fid_2d);
    QStringList processInformation();
    QString command();
};

#endif // ARRAYSUM_H
