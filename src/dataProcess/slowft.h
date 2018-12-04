#ifndef SLOWFT_H
#define SLOWFT_H

#include "processElement.h"
#include "fidDomain.h"

class KSlowFT : public TProcessElement
{
public:

    using TProcessElement::process;

    KSlowFT();
    ~KSlowFT();

    fidDomain::AxisDomain axisDomain() {return FAxisDomain;}
    void setAxisDomain(fidDomain::AxisDomain ad) {FAxisDomain=ad;}

    bool process(TFID *fid);
    bool process(TFID_2D *fid_2d, int k);
    bool process(TFID_2D *fid_2d);
    QStringList processInformation();
    QString command();

private:
    bool slowFTProcess(TFID *fid);
    fidDomain::AxisDomain FAxisDomain;
};

#endif // SLOWFT_H
