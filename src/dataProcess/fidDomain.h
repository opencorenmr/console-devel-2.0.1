#ifndef FIDDOMAIN_H
#define FIDDOMAIN_H

#include "processElement.h"

class fidDomain : public TProcessElement
{
public:

    enum AxisDomain {ToggleDomain,KeepDomain,SetTime,SetFrequency};
    fidDomain(int axisDomain=KeepDomain);
    int axisDomain() {return FAxisDomain;}
    void setAxisDomain(int ad) {FAxisDomain=ad;}

    //using TProcessElement::process;
    bool process(TFID *fid);
    bool process(TFID_2D *fid_2d, int k);
    bool process(TFID_2D *fid_2d);
    //static bool process(TFID *fid, int axisDomain);
    //static bool process(TFID_2D *fid_2d, int k, int axisDomain);
    //static bool process(TFID_2D *fid_2d, int axisDomain);

    QStringList processInformation();
    QString command();

private:
    int FAxisDomain;
   // void setDomain(TFID *fid, AxisDomain ad);
};

#endif // FIDDOMAIN_H
