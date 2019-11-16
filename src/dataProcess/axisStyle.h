#ifndef TAXISSTYLE_H
#define TAXISSTYLE_H


#include "processElement.h"

class TAxisStyle : public TProcessElement
{
public:
    TAxisStyle();
    enum AxisStyle {NormalStyle,PPMStyle};
    int axisStyle() {return FAxisStyle;}
    void setAxisStyle(int axisStyle) {FAxisStyle=axisStyle;}

    enum AxisDomain {TimeDomain,FrequencyDomain,Other};
    int domain() {return FDomain;}
    void setDomain(int ad);
    void setDomain(QString qs);

    enum AxisUnit {nsec,usec,msec,sec,GHz,MHz,kHz,Hz,ppm};
    int unit() {return FUnit;}
    void setUnit(int au);
    void setUnit(QString qs);

    QString label() {return FLabel;}
    void setLabel(QString qs) {FLabel=qs;}

    int referencePoint() {return FReferencePoint;}
    void setReferencePoint(int rp) {FReferencePoint=rp;}
    double referenceValue() {return FReferenceValue;}
    void setReferenceValue(double d) {FReferenceValue=d;}

    bool process(TFID *fid);
    bool process(TFID_2D *fid_2d, int k);
    bool process(TFID_2D *fid_2d);
    QString command();
    QStringList processInformation();

private:
    int FAxisStyle;
    int FReferencePoint;
    double FReferenceValue;
    int FDomain;
    int FUnit;
    QString FLabel;

};
#endif // TAXISSTYLE_H
