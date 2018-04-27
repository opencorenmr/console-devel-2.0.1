#ifndef TPHASE_H
#define TPHASE_H

#include "processElement.h"

class TPhaseRotation : public TProcessElement
{
public:
    TPhaseRotation();
    ~TPhaseRotation();

    bool process(TFID *fid);
    bool process(TFID_2D *fid_2d, int k);
    bool process(TFID_2D *fid_2d);

    QStringList processInformation();
    QString command();

    double phase0() {return FPhase0;}
    double phase1() {return FPhase1;}
    void setPhase0(double p0) {FPhase0=p0;}
    void setPhase1(double p1) {FPhase1=p1;}
    int pivot() {return FPivot;}

public slots:
    void setPivot(int p);

private:
    double FPhase0;
    double FPhase1;
    int FPivot;
};

#endif // TPHASE_H
