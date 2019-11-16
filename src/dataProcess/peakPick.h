#ifndef KPEAKPICK_H
#define KPEAKPICK_H

#include "processElement.h"
#include "processBase.h"
#include "fidDomain.h"

class KPeakPick : public TProcessElement
{
public:

    using TProcessElement::process;

    KPeakPick();
    ~KPeakPick();

    int axisDomain() {return FAxisDomain;}
    void setAxisDomain(int axisDomain) {FAxisDomain=axisDomain;}

    QString dataFilePath() {return FDataFilePath;}

    bool process(TFID_2D *fid_2d, double tgPeak, int tgFID);
    bool process2D(TFID_2D *fid_2d, double tgPeak, int tgSt, int tgEn);
    QStringList processInformation();
    QString command();

private:
    bool peakPickProcess(TFID_2D *fid_2d, double tgPeak, int tgFID);
    bool peakPickProcess2D(TFID_2D *fid_2d, double tgPeak, int tgSt, int tgEn);
    int FAxisDomain;

    QString FDataFilePath;

};

#endif // KPEAKPICK_H
