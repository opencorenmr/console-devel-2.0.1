#ifndef KINDIRECTTIMEACQUIRE_H
#define KINDIRECTTIMEACQUIRE_H

#include "processElement.h"
#include "fidDomain.h"

class KIndirectTimeAcquire : public TProcessElement
{
public:

    using TProcessElement::process;

    KIndirectTimeAcquire();
    ~KIndirectTimeAcquire();

    fidDomain::AxisDomain axisDomain() {return FAxisDomain;}
    void setAxisDomain(fidDomain::AxisDomain ad) {FAxisDomain=ad;}

    bool process(TFID_2D *fid_2d, QStringList *sl, int *timeType);
    QStringList processInformation();
    QString command();

private:
    bool indirectTimeAcquireProcess(TFID_2D *fid_2d, QStringList *sl, int *timeType);
    fidDomain::AxisDomain FAxisDomain;
};

#endif // KINDIRECTTIMEACQUIRE_H
