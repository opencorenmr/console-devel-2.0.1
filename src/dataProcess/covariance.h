#ifndef KCOVARIANCE_H
#define KCOVARIANCE_H

#include "processElement.h"
#include "fidDomain.h"

class KCovariance : public TProcessElement
{
public:

    using TProcessElement::process;

    KCovariance();
    ~KCovariance();

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
    bool covarianceProcess(TFID_2D *fid_2d);
    int FAxisDomain;

    bool IPActive;
    bool IKActive;
};

#endif // KCOVARIANCE_H
