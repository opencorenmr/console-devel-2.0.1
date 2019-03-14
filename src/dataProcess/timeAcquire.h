#ifndef KTIMEACQUIRE_H
#define KTIMEACQUIRE_H

#include "processElement.h"
#include "fidDomain.h"

class KTimeAcquire : public TProcessElement
{
public:

    using TProcessElement::process;

    KTimeAcquire();
    ~KTimeAcquire();

    fidDomain::AxisDomain axisDomain() {return FAxisDomain;}
    void setAxisDomain(fidDomain::AxisDomain ad) {FAxisDomain=ad;}

    bool process(TFID_2D *fid_2d, QStringList *sl);
    QStringList processInformation();
    QString command();

private:
    bool timeAcquireProcess(TFID *fid, QStringList *sl);
    fidDomain::AxisDomain FAxisDomain;
};

#endif // KTIMEACQUIRE_H
