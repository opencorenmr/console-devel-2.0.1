#include "digitalfilter.h"
#include "intToHex.h"
#include "math.h"
#include <QDebug>

//-----------------------------------------------------------------------------
TdigitalFilter::TdigitalFilter()
{
//    setTaps(135);
    setTaps(51);
    setCutOff(0.5);
    setBitLength(10);
    setMode(0);
}
//-----------------------------------------------------------------------------
TdigitalFilter::TdigitalFilter(int n)
{
   setTaps(n);
   setCutOff(0.5);
   setBitLength(10);
   setMode(0);
}
//-----------------------------------------------------------------------------
int TdigitalFilter::mode()
{
  return FMode;
}
//-----------------------------------------------------------------------------
void TdigitalFilter::setMode(int m)
{
  FMode=m;
}
//-----------------------------------------------------------------------------
int TdigitalFilter::taps()
{
    return FTaps;
}
//-----------------------------------------------------------------------------
void TdigitalFilter::setTaps(int k)
{
    FTaps=k;
}
//-----------------------------------------------------------------------------
double TdigitalFilter::cutOff()
{
    return FCutOff;
}
//-----------------------------------------------------------------------------
void TdigitalFilter::setCutOff(double r)
{
    FCutOff=r;
}
//-----------------------------------------------------------------------------
int TdigitalFilter::bitLength()
{
    return FBitLength;
}
//-----------------------------------------------------------------------------
void TdigitalFilter::setBitLength(int k)
{
    FBitLength=k;
}
//-----------------------------------------------------------------------------
double TdigitalFilter::h(int k)
{
    double th=0,hh;
    if(k==0) {hh=2.0*FCutOff;}
    else
    {
        th=6.283184*FCutOff*k;
        hh=2.0*FCutOff*sin(th)/th;
    }
    return hh;
}
//-----------------------------------------------------------------------------
double TdigitalFilter::HammingWindow(int k)
{
    double w;
    if(FTaps==1) {w=1.0;}
    else {w=0.54+0.46*cos(6.283184*k/(FTaps-1));}
    return w;
}
//-----------------------------------------------------------------------------
double TdigitalFilter::frequencyResponse(double wt)
{
    int center=(FTaps-1)/2;
    double r=coeff(center);
    for(int k=1; k<=center; k++)
    {
       r+=2*coeff(center-k)*cos(k*wt);
    }
    if(r==0) return -60.0;
    else return 20*log10(fabs(r));
}
//-----------------------------------------------------------------------------
double TdigitalFilter::truncatedFrequencyResponse(double wt)
{
    int center=(FTaps-1)/2;
    double r=truncatedCoeff(center);
    for(int k=1; k<=center; k++)
    {
       r+=2*truncatedCoeff(center-k)*cos(k*wt);
    }
    if(r==0) return -60.0;
    else return 20*log10(fabs(r));
}
//-----------------------------------------------------------------------------
double TdigitalFilter::coeff(int k)
{
    double result;
    int shift=(FTaps-1)/2;
    if(FMode==0) result=HammingWindow(k-shift)*h(k-shift);
    else result=HammingWindow(k-shift)*h(k-shift)/(2*FCutOff);
    return result;
}
//-----------------------------------------------------------------------------
QString TdigitalFilter::coeffArray()
{
    QString s;
    s.clear();
    for(int i=0; i<FTaps; i++) s+=QString::number(coeff(i));
    return s;
}
//-----------------------------------------------------------------------------
double TdigitalFilter::truncatedCoeff(int k)
{
    return round(coeff(k)*(pow(2,FBitLength-1)-1))/(pow(2,FBitLength-1)-1);
}
//-----------------------------------------------------------------------------
QString TdigitalFilter::HexCoeff(int k)
{
    int p;
    if((FBitLength % 4)==0) {p=0;} else {p=1;}
    int digits=(FBitLength/4)+p;
    int i=round(coeff(k)*(pow(2,FBitLength-1)-1));

 //qDebug() << "frequencyResponse: " << frequencyResponse(0.0);

    return intToHex(i,digits);

//    QString s=QString::number(i, 16);
//    while(s.length()<digits) s='0'+s;
//    return s.right(digits);
}
//-----------------------------------------------------------------------------
QString TdigitalFilter::HexCoeffArray()
{
    QString s;
    s.clear();
    for(int i=0; i<FTaps; i++) s+=HexCoeff(i)+" ";
    return s;
}
//-----------------------------------------------------------------------------
