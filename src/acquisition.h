#ifndef TACQUISITION_H
#define TACQUISITION_H



class TAcqInfo
{
public:
//    enum Unit {Femto,Pico,Nano,Micro,Milli,Unity,Kilo,Mega,Giga,Tera};

//     enum QD {QD_On, QD_Off};
//     enum FIR {FIR_On, FIR_Off};
     enum SpinNoise {SpinNoise_On, SpinNoise_Off};

//     QD qd() {return qDemodulation;}
//     void setQD(QD q) {if(q==qd()) {return;} qDemodulation=q;}
//     FIR fir() {return firFilter;}
//     void setFIR(FIR f) {if(f==fir()) {return;} firFilter=f;}
     int offset() {return FOffset;}
     void setOffset(int k) {FOffset=k;}
     SpinNoise spinNoise() {return FspinNoise;}
     void setSpinNoise(SpinNoise sn) {FspinNoise=sn;}

//TODO  postdigitization phase offset, phase reversal, states

    TAcqInfo();



private:
//    QD qDemodulation;
//    FIR firFilter;
    int FOffset;
    SpinNoise FspinNoise;

};

#endif // TACQUISITION_H
