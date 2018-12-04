#ifndef SLOWIFT_H
#define SLOWIFT_H

#include "processElement.h"
#include "fidDomain.h"

class KSlowIFT : public TProcessElement
{
public:

    using TProcessElement::process;

    KSlowIFT();
    ~KSlowIFT();

    fidDomain::AxisDomain axisDomain() {return FAxisDomain;}
    void setAxisDomain(fidDomain::AxisDomain ad) {FAxisDomain=ad;}

    bool process(TFID *fid);
    bool process(TFID_2D *fid_2d, int k);
    bool process(TFID_2D *fid_2d);
    QStringList processInformation();
    QString command();

private:
    bool slowIFTProcess(TFID *fid);
    fidDomain::AxisDomain FAxisDomain;
};

#endif // SLOWIFT_H
