#ifndef KSVD_H
#define KSVD_H

#include "processElement.h"
#include "fidDomain.h"

class KSVD : public TProcessElement
{
public:

    using TProcessElement::process;

    KSVD();
    ~KSVD();

    fidDomain::AxisDomain axisDomain() {return FAxisDomain;}
    void setAxisDomain(fidDomain::AxisDomain ad) {FAxisDomain=ad;}

    bool process(TFID_2D *fid_2d);
    QStringList processInformation();
    QString command();

    bool innerProduct(){return IPActive;}
    void setInnerProduct(bool ip){IPActive=ip;}

    bool imagKeep(){return IKActive;}
    void setImagKeep(bool ik){IKActive=ik;}

private:
    bool svdProcess(TFID_2D *fid_2d);
    fidDomain::AxisDomain FAxisDomain;

    bool IPActive;
    bool IKActive;
};

#endif // KSVD_H
