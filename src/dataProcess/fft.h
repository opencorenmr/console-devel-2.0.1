#ifndef FFT_H
#define FFT_H

#include "processElement.h"
#include "fidDomain.h"

class TFFT: public TProcessElement
{

public:
    using TProcessElement::process;
    TFFT();
    ~TFFT();

    fidDomain::AxisDomain axisDomain() {return FAxisDomain;}
    void setAxisDomain(fidDomain::AxisDomain ad) {FAxisDomain=ad;}

    bool process(TFID *fid);
    bool process(TFID_2D *fid_2d, int k);
    bool process(TFID_2D *fid_2d);
    QStringList processInformation();
    QString command();

    bool Laplace() {return FLaplace;}
    void setLaplace(bool b) {FLaplace=b;}
    double LaplaceWidth() {return FLaplaceWidth;}
    void setLaplaceWidth(double d) {FLaplaceWidth=d;}

private:
    bool FFTProcess(TFID *fid);
    fidDomain::AxisDomain FAxisDomain;
    bool FLaplace;
    double FLaplaceWidth;
};

#endif // FFT_H
