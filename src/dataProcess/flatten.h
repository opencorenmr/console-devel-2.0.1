#ifndef FLATTEN_H
#define FLATTEN_H

#include "processElement.h"

class TFlatten : public TProcessElement
{
public:
    TFlatten();
    ~TFlatten();

    bool process(TFID_2D *fid_2d);
    QStringList processInformation();
    QString command();

};

#endif // FLATTEN_H
