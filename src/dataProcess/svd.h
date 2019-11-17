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

    int axisDomain() {return FAxisDomain;}
    void setAxisDomain(int axisDomain) {FAxisDomain=axisDomain;}

    bool process(TFID_2D *fid_2d);
    QStringList processInformation();
    QString command();

    bool innerProduct(){return IPActive;}
    void setInnerProduct(bool ip){IPActive=ip;}

    bool imagKeep(){return IKActive;}
    void setImagKeep(bool ik){IKActive=ik;}

private:
    bool svdProcess(TFID_2D *fid_2d);
    int FAxisDomain;

    bool IPActive;
    bool IKActive;
};

#endif // KSVD_H
