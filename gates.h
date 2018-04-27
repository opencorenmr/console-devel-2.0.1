#ifndef GATES_H
#define GATES_H

#include <QList>
#include <QSet>
#include <QVector>
#include <QString>
#include <QStringList>
#include <QDebug>

#include "intToHex.h"
#include "variables.h"  // from which MAX and MIN values are read

#define BITLENGTH 64



enum TKindOfGate {GAmplitude, GPhase, GLogic, GLogicVector,
                  GAD9858, GPhaselist, GInteger, GRFIQ};

const int AD9858_PROFILE_SELECT_0=0;
const int AD9858_PROFILE_SELECT_1=1;
const int AD9858_PROFILE_SELECT_2=2;
const int AD9858_PROFILE_SELECT_3=3;
const int AD9858_FREQ_SWEEP=-1;

//const double AMPWORD_MIN=0.0;
//const double AMPWORD_MAX=100.0;



class TGateControlWord {
  public:
    TGateControlWord(int bLength) {
        //qDebug() << "constructor of TGateControlWord";
        transformFunction="#";
        FBits.resize(bLength); FBits.fill(0);}  // create an int vector (0,0,...,0)
    TGateControlWord() {FBits.resize(1); FBits.fill(0);}
    TGateControlWord(TGateControlWord &)  // copy constructor
      { } //qDebug() << "copy constructor of TGateControlWord";}
    virtual ~TGateControlWord() { } // qDebug() << "destructor of TGateControlWord";}
    QString name;
    QString caption;
    bool used; // for checking multiple usage in a single line

    QString errorMessage;
    bool error;

    QString transformFunction;

    virtual double offset() {return 0;}
    virtual void setOffset(double) { ; }
    virtual bool isLogicVector() {return false;}
    virtual void setLogicVector(bool) { ; }
    virtual double sweepStartFreq() {return 0;}
    virtual void setSweepStartFreq(double) {;}
    virtual double sweepStopFreq() {return 0;}
    virtual void setSweepStopFreq(double) {;}

    virtual void setValue(double) {;}
    virtual void setValue(int) {;}
    virtual int order() {return 0;}
    virtual void setOrder(int) { }
    virtual QStringList genInitialSettingSource() {return QStringList();}
    virtual QStringList genInitialSettingBin() {return QStringList();}
    virtual void setPhaseListLength(int ) { }
    virtual int phaseListLength() {return 0;}
    virtual void setPhaseList(int , double ) { }
    virtual double phaseList(int ) { return 0.0;}
    virtual void setProfile(int) { }
    virtual int profile() {return 0;}
    virtual void setAmpGate(QString) {}
    virtual void setPhaseGate(QString) {}
    virtual QString translate(double,double) {return QString();}  // for IQ gate
    virtual QString translate(double,double,double) {return QString();} // 3rd argument: phase offset (2016Dec13)

    int bitLength() {return FBits.size();}
    void setBitLength(int k) {FBits.resize(k);}
    int channel() {return FChannel;}
    void setChannel(int k) {FChannel=k;}
    TKindOfGate KindOfGate() {return FKindOfGate;}
    QString KindOfGate_str();
    void setKindOfGate(TKindOfGate kog) {FKindOfGate=kog;}

    void assign(int m, int j) {FBits[m]=j;}  // m-th bit  <---->  gate number j
    void assign(QVector<int> v) {
      if(bitLength()==v.size()) {for(int i = 0; i<v.size(); ++i) assign(i,v[i]);}
    }
    int getAssignment(int m) { return FBits.at(m); }

    virtual QSet<int> output()=0;  // pure virtual function (virtual .... =0)

  protected:
    int FChannel;
    TKindOfGate FKindOfGate;


    QVector<int> FBits;


}; // TGateControlWord


class TphaseListWord: public TGateControlWord {
  public:
    TphaseListWord(int k) : TGateControlWord(3) {  // constructor  NOTE: size=3 (not k!)
         // qDebug()<<"constructor of TphaseListWord";
        FKindOfGate=GPhaselist;
        setOrder(k);  // 1 to 7
                  // Which (1st, 2nd, .., 7th) list of the relevant channel?
        Fphases.clear();
    }
    virtual ~TphaseListWord() { }// qDebug()<<"destructor of TphaseListWord"; }
    virtual QSet<int> output();
    virtual int order() {return FOrder;}
    virtual void setOrder(int k) {FOrder=k;}

    virtual QStringList genInitialSettingSource();
       // ppg source code for initial setting...

    virtual void setPhaseListLength(int k) {Fphases.resize(k);}
    virtual int phaseListLength() {return Fphases.size();}
    virtual void setPhaseList(int k, double d) {if (k<Fphases.size()) Fphases[k]=d;}
    virtual double phaseList(int k) {if(k<Fphases.size()) return Fphases.at(k); else return 0.0;}

  protected:
    int FOrder;
    QVector<double> Fphases;

};



class TampWord: public TGateControlWord {
  public:
    TampWord(int k) : TGateControlWord(k) // constructor
    {
        // qDebug()<<"constructor of TampWord";
        FKindOfGate=GAmplitude;
    }
    virtual ~TampWord() { } // qDebug()<<"destructor of TampWord";}
    double value() {return Fvalue;}
    void setValue(double d);
    virtual QSet<int> output();
    static bool checkRange(double d)
    {
        if ((d<MIN_AMP_VALUE) || (d>MAX_AMP_VALUE)) return false; else return true;
    }

  protected:
    double Fvalue;

};


class TphaseWord: public TGateControlWord {
  public:
    TphaseWord(int k) : TGateControlWord(k)
    { // qDebug() << "constructor of TphaseWord";
        FKindOfGate=GPhase;}
    virtual ~TphaseWord() { } // qDebug()<<"destructor of TphaseWord"; }
    double value() {return Fvalue;}
    void setValue(double d)  {Fvalue=d; error=false;}
    virtual QSet<int> output();

  protected:
    double Fvalue;
};

class TRFIQWord: public TGateControlWord
{
public:
    TRFIQWord(): TGateControlWord()
    {
       FKindOfGate=GRFIQ;
    }
    virtual ~TRFIQWord() { }
    QString translate(double inPhase, double quad)
    {
        double amp=sqrt(inPhase*inPhase + quad*quad);
        double ph=180*atan2(quad,inPhase)/3.141592;
        QString qs;
        qs.append(ampGateName+"("+QString::number(amp)+"),");
        qs.append(phaseGateName+"("+QString::number(ph)+")");
        return qs;
    }
    QString translate(double inPhase, double quad, double phase_offset)
    {
        double amp=sqrt(inPhase*inPhase + quad*quad);
        double ph = phase_offset
                  + 180*atan2(quad,inPhase)/3.141592;
        QString qs;
        qs.append(ampGateName+"("+QString::number(amp)+"),");
        qs.append(phaseGateName+"("+QString::number(ph)+")");
        return qs;
    }

    virtual QSet<int> output() {return QSet<int>();}

    virtual void setAmpGate(QString s) {ampGateName=s;}
    virtual void setPhaseGate(QString s) {phaseGateName=s;}

protected:
    QString ampGateName;
    QString phaseGateName;
};


class TlogicWord: public TGateControlWord {
  public:
    TlogicWord() : TGateControlWord(1) {FKindOfGate=GLogic;}
    virtual ~TlogicWord(){  }
    virtual QSet<int> output() {return QSet<int>() << getAssignment(0);}
       // This should be equivalent to {QSet<int> q; q.clear(); q << getAssignment(0); return q;}
};


class TlogicVectorWord : public TGateControlWord {
  public:
    TlogicVectorWord(int k) : TGateControlWord(k) {FKindOfGate=GLogicVector;}
    virtual ~TlogicVectorWord() {  }
    int value() {return Fvalue;}
    void setValue(int i)  {Fvalue=i; error=false;}
    virtual QSet<int> output();

  protected:
    int Fvalue;
};

class TintWord : public TGateControlWord {
  public:
    TintWord(int k) : TGateControlWord(k) {FKindOfGate=GInteger;}
    virtual ~TintWord() {  }
    int value() {return Fvalue;}
    void setValue(int i)  {Fvalue=i; error=false;}
    virtual QSet<int> output();

  protected:
    int Fvalue;
};

class TAD9858Word : public TGateControlWord {
  public:
    TAD9858Word() : TGateControlWord(16) {FKindOfGate=GAD9858;FLogicVector=false;}
    virtual ~TAD9858Word() {  }

    virtual double offset() {return Foffset;}
    virtual void setOffset(double d) {Foffset=d;}
    virtual bool isLogicVector() {return FLogicVector;}
    virtual void setLogicVector(bool b) {FLogicVector=b;}
    virtual double sweepStartFreq() {return FSweepStartFreq;}
    virtual void setSweepStartFreq(double d) {FSweepStartFreq=d;}
    virtual double sweepStopFreq() {return FSweepStopFreq;}
    virtual void setSweepStopFreq(double d) {FSweepStopFreq=d;}
    double value() {return Fvalue;}
    void setValue(double d) {Fvalue=d;}
    int profile() {return Fprofile;}
    void setProfile(int k) {Fprofile=k;}
    int order(){return FOrder;}
    void setOrder(int k) {FOrder=k;}

    virtual QSet<int> output();

  protected:
    double Fvalue;
    double Foffset;
    int Fprofile;
    int FOrder;
    bool FLogicVector;
    double FSweepStartFreq;
    double FSweepStopFreq;

};


class TGates {
  public:
    TGates();
    TGates(TGates &);
    ~TGates();
    void initialize();
    QList<TGateControlWord *> Items;
    bool error;
    QString errorMessage;
//    QStringList messages;
    QStringList info;
    QStringList AD9858GatesInfo();
//    QString definition;
    bool genPhaseCycle(int ch, QStringList *sl);

    bool readFromFile(QString fn);
    QVector<int> FNOfPhaseListGates;
    int nameIndex(QString gateName);

  protected:

};

#endif // GATES_H
