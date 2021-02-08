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

    double initialPhase0() {return FInitialPhase0;}
    double initialPhase1() {return FInitialPhase1;}
    void setInitialPhase0(double ip0) {FInitialPhase0=ip0;}
    void setInitialPhase1(double ip1) {FInitialPhase1=ip1;}
    void resetInitialPhase0() {setInitialPhase0(0);}
    void resetInitialPhase1() {setInitialPhase1(0);}
    void resetInitialPhase() {resetInitialPhase0(); resetInitialPhase1();}
    double phase0() {return FPhase0;}
    double phase1() {return FPhase1;}
    void setPhase0(double p0) {FPhase0=p0;}
    void setPhase1(double p1) {FPhase1=p1;}
    void setPreviousPhase0(double pp0) {FPreviousPhase0=pp0;}
    void setPreviousPhase1(double pp1) {FPreviousPhase1=pp1;}
    double previousPhase0() {return FPreviousPhase0;}
    double previousPhase1() {return FPreviousPhase1;}
    void resetPhase0() {setPhase0(0);}
    void resetPhase1() {setPhase1(0);}
    void resetPhase() {resetPhase0(); resetPhase1();}
    int pivot() {return FPivot;}

    void incPhase0(double dp0);
    void incPhase1(double dp1);
    void changePhase0To(double p0);
    void changePhase1To(double p1);

    double accumPhase0() {return FAccumPhase0;}
    double accumPhase1() {return FAccumPhase1;}
    void setAccumPhase0(double ap0) {FAccumPhase0=ap0;}
    void setAccumPhase1(double ap1) {FAccumPhase1=ap1;}
    void resetAccumPhase0() {setAccumPhase0(0);}
    void resetAccumPhase1() {setAccumPhase1(0);}
    void resetAccumPhase() {resetAccumPhase0(); resetAccumPhase1();}

public slots:
    void setPivot(int p);

private:
    double FPhase0;
    double FPhase1;
    double FInitialPhase0;
    double FInitialPhase1;
    double FAccumPhase0;
    double FAccumPhase1;
    double FPreviousPhase0;
    double FPreviousPhase1;
    int FPivot;
};

#endif // TPHASE_H
