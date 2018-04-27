#ifndef RCVRCOM_H
#define RCVRCOM_H

#include "digitalfilter.h"
#include "math.h"
//#include "pulseProgram.h"
#include <QString>

#define defaultADCSamplingRate 80.0 // in MHz

//class TpulseProgram;

class TDSPOption
{
public:
    enum digitalFilterOption {FIR_On,FIR_Off};
    enum QDOption {QD_On,QD_Off};
    // enum cutOffSetting {AutoSetCutoff,ManualSetCutoff};
};

class TreceiverCommand
{
public:
    static QString updateND(int nd); // number of dummy scans
    static QString updateAR(int ar); // array length
    static QString updateNA(int na); // number of acquisitions
    static QString updateAL(int al); // aquisition length
    static QString updateDW(double dw); // dwell time
    static QString updateDW(double dw, double samplingRateinMHz);
    static QString updateAF(TDSPOption::QDOption qd, TDSPOption::digitalFilterOption df); // DSP setting
    static QString updateST(int st); //
    static QString updateCO(double dw); // FIR coefficients
    static QString updateDO(int i); // DC offset
};

class TreceiverInfo
{
public:
    TreceiverInfo()
    {
      digitalFilterOption=TDSPOption::FIR_On;
      qdOption=TDSPOption::QD_On;
      setStep(1);
      setNA(1);
      setAL(8);
      setDW(1.0e-6);
      setND(0);
      setAR(1);
      setNC(1);
      setPhaseReverseEnabled(false);
      setPhaseRotationEnabled(false);
      setFFTEnabled(false);
      setPhaseRotationAngle(0.0);
      setSpectralDensityAccumEnabled(false);
    }

    TDSPOption::digitalFilterOption digitalFilterOption;
    TDSPOption::QDOption qdOption;

    int al() {return FAL;}
    void setAL(int al) {FAL=al; FACQTIME=FDW*FAL;}
    double dw() {return FDW;}
    void setDW(double dw) {
        FDWN=round(1000000*dw*0.5*defaultADCSamplingRate);
        FDW=FDWN/(0.5*defaultADCSamplingRate);
        FACQTIME=FDW*FAL;


        double d=dw;
        if(d==0) d=0.025; //
        d = 1.1*0.5/d;
        d = d/4.0;   //  (4 -> 40/10)    40: 80/2    80: ADC sampling rate
        if (d>0.5) d=0.5;
      //  FDFScale=2*d;
    }

    double acqTime() {return FACQTIME;}
    int dwn() {return FDWN; }
    void setDWN(int dwn) {if(dwn<=0) dwn=1; FDWN=dwn; FDW=FDWN/(0.5*defaultADCSamplingRate);FACQTIME=FDW*FAL;}
    int na() {return FNA; }
    void setNA(int na) {FNA=na;}
    int step() {return FSTEP; }
    void setStep(int st) {FSTEP=st;}
    int taps() {return FTaps; }
    void setTaps(int taps) {FTaps=taps; digitalFilter.setTaps(taps);}

    //void setOrigNA(int i) {ForigNA=i;}
    //int origNA() {return ForigNA;}

    int nd() {return FND;}
    void setND(int nd) {FND=nd;}
    int ar() {return FArrayLength;}
    void setAR(int ar) {FArrayLength=ar;}

    int nc() {return FNComplex;}
    void setNC(int nc) {FNComplex=nc;}

    int DCOffset() {return FDO;}
    void setDCOffset(int i) {FDO=i;}

    double dfScale() {return FDFScale;}
  // void importReceiverParameters(TpulseProgram *ppg);

    QStringList receiverCommands();

    bool updated;
    TdigitalFilter digitalFilter;

    bool isPhaseReverseEnabled() {return FisPhaseReverseEnabled;}
    bool isPhaseRotationEnabled() {return FisPhaseRotationEnabled;}
    bool isFFTEnabled() {return FisFFTEnabled;}
    void setPhaseReverseEnabled(bool b) {FisPhaseReverseEnabled=b;}
    void setPhaseRotationEnabled(bool b) {FisPhaseRotationEnabled=b;}
    void setFFTEnabled(bool b) {FisFFTEnabled=b;}
    void setPhaseRotationAngle(double d) {FphaseRotationAngle=d;}
    double phaseRotationAngle() {return FphaseRotationAngle;}
    bool isSpectralDensityAccumEnabled() {return FisSpectralDensityAccumEnabled;}
    void setSpectralDensityAccumEnabled(bool b) {FisSpectralDensityAccumEnabled=b;}

protected:
    int FAL;
    double FDW;
    double FACQTIME;
    int FDWN;
    int FNA;
    int FND;
    int FArrayLength; // FAR <--- error in mingw (reserved word??  20150327 Takeda)
    int FSTEP;
    int FTaps;
    int FDO;

    double FDFScale;
   // int ForigNA;

    int FNComplex; // hypercomplex option

    bool FisPhaseReverseEnabled;
    bool FisPhaseRotationEnabled; double FphaseRotationAngle;
    bool FisFFTEnabled;
    bool FisSpectralDensityAccumEnabled;

};

#endif // RCVRCOM_H
