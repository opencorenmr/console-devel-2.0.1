#ifndef FFT_H
#define FFT_H

#include "processElement.h"
//#include "fidDomain.h"

class TFFT: public TProcessElement
{

public:
   // using TProcessElement::process;
    TFFT();
    ~TFFT();

    enum TDomainOption {ToggleDomain, KeepDomain, SetTime, SetFrequency};
    int axisDomain() {return FAxisDomain;}
    void setAxisDomain(int ad) {FAxisDomain=ad;}

    bool process(TFID *fid);
    bool process(TFID_2D *fid_2d, int k);
    bool process(TFID_2D *fid_2d);
    QStringList processInformation();
    QString command();

    bool Laplace() {return FLaplace;}
    void setLaplace(bool b) {FLaplace=b;}
    double LaplaceWidth() {return FLaplaceWidth;}
    void setLaplaceWidth(double d) {FLaplaceWidth=d;}

private:
    bool FFTProcess(TFID *fid);
    int FAxisDomain;
    bool FLaplace;
    double FLaplaceWidth;
    void processDomain(TFID *fid);
};


class TFFT3D: public TProcessElement
{
public:
    TFFT3D();
    TFFT3D(int n1, int n2);

    ~TFFT3D();

    bool process(TFID_2D *fid_2d);
    QStringList processInformation();
    QString command();
\
    void FFT3D_setLengths(int n1, int n2) {setLengths(n1,n2);}
    void FFT3D_setN1(int n1) {setN1(n1);}
    void FFT3D_setN2(int n2) {setN2(n2);}
    int FFT3D_n1() {return n1();}
    int FFT3D_n2() {return n2();}

private:
    void setLengths(int n1, int n2) {FN1=n1; FN2=n2;}
    void setN1(int n1) {FN1=n1;}
    void setN2(int n2) {FN2=n2;}
    int n1() {return FN1;}
    int n2() {return FN2;}

    int FN1;
    int FN2;

};


#endif // FFT_H
