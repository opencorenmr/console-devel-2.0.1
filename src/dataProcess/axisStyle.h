#ifndef TAXISSTYLE_H
#define TAXISSTYLE_H


#include "processElement.h"

class TAxisStyle : public TProcessElement
{
public:
    TAxisStyle();
    enum AxisStyle {NormalStyle,PPMStyle};
    AxisStyle axisStyle() {return FAxisStyle;}
    void setAxisStyle(AxisStyle as) {FAxisStyle=as;}

    enum AxisDomain {TimeDomain,FrequencyDomain,Other};
    AxisDomain domain() {return FDomain;}

    void setDomain(AxisDomain ad);
    void setDomain(QString qs);

    enum AxisUnit {usec,msec,sec,kHz,Hz,ppm};
    AxisUnit unit() {return FUnit;}
    void setUnit(AxisUnit au);
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
    AxisStyle FAxisStyle;
    int FReferencePoint;
    double FReferenceValue;
    AxisDomain FDomain;
    AxisUnit FUnit;
    QString FLabel;

};
#endif // TAXISSTYLE_H
