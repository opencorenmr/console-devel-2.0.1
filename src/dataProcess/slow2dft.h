#ifndef KSLOW2DFT_H
#define KSLOW2DFT_H

#include "processElement.h"
#include "fidDomain.h"

class KSlow2DFT : public TProcessElement
{
public:

    using TProcessElement::process;

    KSlow2DFT();
    ~KSlow2DFT();

    fidDomain::AxisDomain axisDomain() {return FAxisDomain;}
    void setAxisDomain(fidDomain::AxisDomain ad) {FAxisDomain=ad;}

    bool ftprocess(TFID_2D *fid_2d, double *init, double *inter, int *pts);
    QStringList processInformation();
    QString command();

private:
    bool slow2DFTProcess(TFID_2D *fid_2d, double *init, double *inter, int *pts);
    fidDomain::AxisDomain FAxisDomain;
};

#endif // KSLOW2DFT_H
