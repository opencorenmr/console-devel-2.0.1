#ifndef CARTESIANMAPWEIGHT_H
#define CARTESIANMAPWEIGHT_H

#include "cartesianMap.h"

class SCartesianMapWeight3D : public TCartesianMap3D
{
    Q_OBJECT
public:
    SCartesianMapWeight3D();
    ~SCartesianMapWeight3D();

    bool process(TFID_2D *fid_2D);

    QStringList processInformation();
    QString command();

private:
    void run();
};

#endif // CARTESIANMAPWEIGHT_H
