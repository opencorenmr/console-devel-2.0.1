#ifndef DIGITALFILTER_H
#define DIGITALFILTER_H

#include <QString>
#include "intToHex.h"

class TdigitalFilter
{
public:
    TdigitalFilter();
    TdigitalFilter(int n);
    virtual ~TdigitalFilter() { ;}

    double h(int k);
    double HammingWindow(int k);
    double coeff(int k);
    double truncatedCoeff(int k);
    QString HexCoeff(int k);
    QString HexCoeffArray();
    QString coeffArray();

    double frequencyResponse(double wt);
    double truncatedFrequencyResponse(double wt);

    int mode();
    void setMode(int m);
    int taps();
    void setTaps(int k);
    double cutOff();
    void setCutOff(double r);
    int bitLength();
    void setBitLength(int k);

  // plotCoeff
  // plotResponse

  private:
    int FTaps;
    double FCutOff;
    int FBitLength;
    int FMode;


};

#endif // DIGITALFILTER_H
