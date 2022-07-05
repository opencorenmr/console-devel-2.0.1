#ifndef CARTESIANMAPWEIGHT_H
#define CARTESIANMAPWEIGHT_H

#include "cartesianMap.h"

class SCartesianMapWeight3D : public TCartesianMap3D
{
    Q_OBJECT
public:
    SCartesianMapWeight3D();
    ~SCartesianMapWeight3D();

    bool process(TFID_2D *fid_2d);

    QStringList processInformation();
    QString command();

    int iteration;

protected:
    void run();
};

#endif // CARTESIANMAPWEIGHT_H
