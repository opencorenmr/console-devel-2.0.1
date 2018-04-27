#ifndef FIDDOMAIN_H
#define FIDDOMAIN_H

#include "processElement.h"

class fidDomain : public TProcessElement
{
public:

    enum AxisDomain {TimeDomain,FrequencyDomain,ToggleDomain,KeepDomain};
    fidDomain(AxisDomain ad=KeepDomain);
    AxisDomain axisDomain() {return FAxisDomain;}
    void setAxisDomain(AxisDomain ad)
    {FAxisDomain=ad;}

    //using TProcessElement::process;
    bool process(TFID *fid);
    bool process(TFID_2D *fid_2d, int k);
    bool process(TFID_2D *fid_2d);
    static bool process(TFID *fid, AxisDomain ad);
    static bool process(TFID_2D *fid_2d, int k, AxisDomain ad);
    static bool process(TFID_2D *fid_2s, AxisDomain ad);

    QStringList processInformation();
    QString command();

private:
    AxisDomain FAxisDomain;
    void setDomain(TFID *fid, AxisDomain ad);
};

#endif // FIDDOMAIN_H
