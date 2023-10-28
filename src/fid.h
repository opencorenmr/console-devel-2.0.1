#ifndef FID_H
#define FID_H

#include <QIODevice>
#include <QVector>
#include <QPointF>
#include <QString>
#include <QStringList>
#include <QMutex>
#include "metricPrefix.h"

class THalfFID {
  public:
//    QVector<float> sig;
    QVector<double> sig;

    THalfFID() {sig.clear();}
    THalfFID(int al) {sig.fill(0.0, al);}
    ~THalfFID() {sig.clear();}

    void initialize() {sig.fill(0.0,al());}
    //void initialize(int al) {sig.fill(0.0, al);}
    int al() {return sig.size();}

    double sum();
    double sum(int ini, int fin);
    double average();
    double average(int ini, int fin);
    double standardDeviation();
    double standardDeviation(int ini, int fin);
    double max();
    int maxAt();
    double max(int ini, int fin);
    int maxAt(int ini, int fin);
    double min();
    double min(int ini, int fin);
    double absMax();
    double absMax(int ini, int fin);


  protected:

};  // THalfFID

class TFIDXUnit{
  public:
  enum xUnit {Second, Hz, ppm, Tesla, QuantumNumber, NoUnit};
};

class TFID
{
  public:
    enum TDomain {TimeDomain,FrequencyDomain,NoDomain};
    TFID(int al);
    ~TFID() {delete real; delete imag; delete abs;}

    THalfFID *real,*imag,*abs;
    int domain() {return FDomain;}
    void setDomain(int domain) {FDomain=domain;}

    TFIDXUnit::xUnit xunit() {return FXUnit;}
    void setXUnit(TFIDXUnit::xUnit xu) {FXUnit=xu;}

    bool isEmpty() {return FisEmpty;}
    void setEmpty(bool b) {FisEmpty=b;}

    TMetricPrefix metricPrefix() {return FMetricPrefix;}
    void setMetricPrefix(TMetricPrefix mp) {FMetricPrefix=mp;}
    TMetricPrefix::prefixes prefix() {return metricPrefix().prefix();}
    void setPrefix(TMetricPrefix::prefixes pf) {FMetricPrefix.setPrefix(pf);}
    void setPrefix(QString qs) {FMetricPrefix.setPrefix(qs);}

    TMetricPrefix plotMetricPrefix() {return FPlotMetricPrefix;}
    void setPlotMetricPrefix(TMetricPrefix mp) {FPlotMetricPrefix=mp;}
    TMetricPrefix::prefixes plotPrefix() {return plotMetricPrefix().prefix();}
    void setPlotPrefix(TMetricPrefix::prefixes pf) {FPlotMetricPrefix.setPrefix(pf);}
    void setPlotPrefix(QString qs) {FPlotMetricPrefix.setPrefix(qs);}

    void setXInitialValue(double d) {FxInitialValue=d;}
    double xInitialValue() {return FxInitialValue;} // no metric prefix!
    void setDx(double d) {Fdx=d;}
    double dx() {return Fdx;}            // no metric prefix!

    QString xAxisLabel() {return FXAxisLabel;}
    void setXAxisLabel(QString qs) {FXAxisLabel=qs;}
    QString xAxisUnitSymbol() {return FXAxisUnitSymbol;}
    void setXAxisUnitSymbol(QString qs) {FXAxisUnitSymbol=qs;}
    bool isXAxisCustom() {return FIsXAxisCustom;}
    void setCustomXAxis(bool b) {FIsXAxisCustom=b;}

    double xValue(int k);
    int xIndex(double x);
    QString xAxisUnitString();
    // int pivot;
    int na() {return FNA;}
    void setNA(int na) {FNA=na;}
    int nd(){return FND;}
    void setND(int nd) {FND=nd;}
    int actualNA;
    int dummyCount;

    void initialize() {
        real->initialize(); imag->initialize(); abs->initialize();
        actualNA=0;
    }

    double dw() {return FDW;}
    void setDW(double dw);
    int al() {return FAL;}
    void setAL(int al);
    double sf1() {return FSF1;}
    void setSF1(double sf1);

    double phase(int k);
    void rotate(double angle);
    void phaseReverse();
    void fft();
    double phase0() {return FPhase0;}
    void updateAbs();
    void swapDomain();

    QStringList comment() {return FComment;}
    void setComment(QStringList com) {FComment=com;}
    // text data
    bool ReadsmdFile(QString fn);
    bool WritesmdFile(QString fn);

    // binary data (float)
    bool Writesm2pFile(QString fn);
    bool Writesm2dFile(QString fn, QIODevice::OpenModeFlag flag=QIODevice::WriteOnly);
    bool Writesm2Files(QString fn, QIODevice::OpenModeFlag flag=QIODevice::WriteOnly);

    // binary data (double)
    bool WriteoppFile(QString fn);
    bool WriteopdFile(QString fn, QIODevice::OpenModeFlag flag=QIODevice::WriteOnly);
    bool WriteopFiles(QString fn, QIODevice::OpenModeFlag flag=QIODevice::WriteOnly);

    // ascii
    bool WriteopaFile(QString fn, QIODevice::OpenModeFlag flag=QIODevice::WriteOnly);

    bool exportAscii(QString fn);
    bool exportAscii(QString fn,int xini, int xfin);

    QString errorMessage;

    // NonUniformSampling (Kobayashi)
    bool NUS;
    QVector<double> nusTimes;
    double indirectTime;
    // end


  protected:
    TMetricPrefix FMetricPrefix;
    TMetricPrefix FPlotMetricPrefix;

    TFIDXUnit::xUnit FXUnit;
    bool FisEmpty;
    int FAL;
    double FDW;
    double FSF1;
    double FPhase0;
    int FNA;
    int FND;
    QStringList FComment;

    bool FIsXAxisCustom;
    QString FXAxisLabel;
    QString FXAxisUnitSymbol;

    double FxInitialValue; // no metric prefix!
    double Fdx;            // no metric prefix!

    QMutex mutex;

    int FDomain;

};


class TFID_2D
{
  public:
    TFID_2D()
    {
      FID.clear();

      // NonUniformSampling(Kobayashi)
      NUS = false;
      indirectTime = 0.0;
      hyperComplex = false;
      // end
    }
    TFID_2D(TFID_2D &) {FID.clear();}
    ~TFID_2D() {FID.clear();}

    QVector<TFID*> FID;
    QString errorMessage;
    QStringList comments;
    QStringList parameters;
    //bool error;

    TFIDXUnit::xUnit xunit() {return FXUnit;}
    void setXUnit(TFIDXUnit::xUnit xu) {FXUnit=xu; for(int k=0; k<FID.size(); k++) FID[k]->setXUnit(xu);}



    TMetricPrefix metricPrefix() {return FMetricPrefix;}
    void setMetricPrefix(TMetricPrefix mp) {FMetricPrefix=mp; for(int k=0; k<FID.size(); k++) FID[k]->setMetricPrefix(mp);}
    TMetricPrefix::prefixes prefix() {return metricPrefix().prefix();}
    void setPrefix(TMetricPrefix::prefixes pf) {FMetricPrefix.setPrefix(pf); for(int k=0; k<FID.size(); k++) FID[k]->setPrefix(pf);}
    void setPrefix(QString qs) {FMetricPrefix.setPrefix(qs); for(int k=0; k<FID.size(); k++) FID[k]->setPrefix(qs);}

    TMetricPrefix plotMetricPrefix() {return FPlotMetricPrefix;}
    void setPlotMetricPrefix(TMetricPrefix mp) {FPlotMetricPrefix=mp; for(int k=0; k<FID.size(); k++) FID[k]->setMetricPrefix(mp);}
    TMetricPrefix::prefixes plotPrefix() {return plotMetricPrefix().prefix();}
    void setPlotPrefix(TMetricPrefix::prefixes pf) {FPlotMetricPrefix.setPrefix(pf); for(int k=0; k<FID.size(); k++) FID[k]->setPrefix(pf);}
    void setPlotPrefix(QString qs) {FPlotMetricPrefix.setPrefix(qs); for(int k=0; k<FID.size(); k++) FID[k]->setPrefix(qs);}


    void setXInitialValue(double d) {FxInitialValue=d; for(int k=0; k<FID.size(); k++) FID[k]->setXInitialValue(d);}
    double xInitialValue() {return FxInitialValue;} // no metric prefix!
    void setDx(double d) {Fdx=d; for(int k=0; k<FID.size(); k++) FID[k]->setDx(d);}
    double dx() {return Fdx;}            // no metric prefix!


    bool isXAxisCustom() {return FIsXAxisCustom;}
    void setCustomXAxis(bool b) {FIsXAxisCustom=b;}

    QString xAxisLabel() {return FXAxisLabel;}
    void setXAxisLabel(QString qs) {FXAxisLabel=qs; for(int j=0; j<FID.size(); j++) {FID[j]->setXAxisLabel(qs);} }
    QString xAxisUnitSymbol() {return FXAxisUnitSymbol;}
    void setXAxisUnitSymbol(QString qs) {FXAxisUnitSymbol=qs;}

    bool ReadjdfFile(QString fn);
    bool ReadsmdFile(QString fn);

    bool Readsm2pFile(QString fn);
    bool Readsm2dFile(QString fn);
    bool Readsm2Files(QString fn);
    bool Writesm2pFile(QString fn);
    bool Writesm2dFile(QString fn, QIODevice::OpenModeFlag flag=QIODevice::WriteOnly);
    bool Writesm2dFile(QStringList fnList, QIODevice::OpenModeFlag flag=QIODevice::WriteOnly);
    bool Writesm2Files(QString fn, QIODevice::OpenModeFlag flag=QIODevice::WriteOnly);

    bool ReadopaFile(QString fn); // called by ReadopaFile"s", declared below.
    bool ReadopaFiles(QString fn); // ascii data, parameter is supposed to be stored in .opp, just like .opd (Jul 2020 KT)
    bool ReadoppFile(QString fn);
    bool ReadopdFile(QString fn);
    bool ReadopFiles(QString fn);
    bool WriteoppFile(QString fn);
    bool WriteopdFile(QString fn, QIODevice::OpenModeFlag flag=QIODevice::WriteOnly);
    bool WriteopdFile(QStringList fnList, QIODevice::OpenModeFlag flag=QIODevice::WriteOnly);
    bool WriteopFiles(QString fn, QIODevice::OpenModeFlag flag=QIODevice::WriteOnly);
    bool WriteopaFile(QStringList fnList, QIODevice::OpenModeFlag flag=QIODevice::WriteOnly); // asci
    bool WriteopaFile(QString fn, QIODevice::OpenModeFlag flag=QIODevice::WriteOnly); // asci

    int currentFID() {return FCurrentFID;}
    void setCurrentFID(int k) {if((k>=0) && (k<FID.size())) FCurrentFID=k; else FCurrentFID=0;}
    void toggleCurrentFID() {if(FCurrentFID>=FID.size()-1) FCurrentFID=0; else FCurrentFID++;}
    int na() {return FNA;}
    void setNA(int na) {FNA=na; for(int j=0; j<FID.size(); j++) FID[j]->setNA(na);}
    int nd() {return FND;}
    void setND(int nd) {FND=nd; for(int j=0; j<FID.size(); j++) FID[j]->setND(nd);}
    int defaultAL() {
//      if(!FID.isEmpty()) return FID.at(0)->al();
//      else return 0;
      return FDefaultAL;

    }
//    int al() {return FAL;}
    void setDefaultAL(int al) {
        FDefaultAL=al;
//        if(!FID.isEmpty()) {FID[0]->setAL(al);}
    }
//    void setAl(int al) {FAL=al; for(int j=0; j<FID.size(); j++) FID[j]->setAL(al);}
    double dw() {return FDW;}
    void setDW(double dw) {FDW=dw; for(int j=0; j<FID.size(); j++) FID[j]->setDW(dw);}
    double sf1() {return FSF1;}
    void setSF1(double sf1) {FSF1=sf1; for(int j=0; j<FID.size(); j++) FID[j]->setSF1(sf1);}


    void initialize() {for(int j=0; j<FID.size(); j++) FID[j]->initialize();}

    bool setPlotData(int k, QVector<QPointF> *re, QVector<QPointF> *im, QVector<QPointF> *abs);

    // NonUniformSampling(Kobayashi)
    bool NUS;
    double indirectTime;
    bool hyperComplex;
    // end

  protected:
    TFIDXUnit::xUnit FXUnit;
    TMetricPrefix FMetricPrefix;
    TMetricPrefix FPlotMetricPrefix;

    int FNA,FND;
    int FDefaultAL;
    double FDW;
    double FSF1;
    int FCurrentFID;

    bool FIsXAxisCustom;
    QString FXAxisLabel;
    QString FXAxisUnitSymbol;
    double FxInitialValue; // no metric prefix!
    double Fdx;            // no metric prefix!


    QMutex mutex;

};


#endif // FID_H
