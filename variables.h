#ifndef VARIABLES_H
#define VARIABLES_H

#include <QString>
#include <QList>
#include <QVariant>
#include <QDebug>

#include "math.h"

#define MAX_LOOP_VALUE pow(2,40)-1
//#define MAX_LOOP_VALUE 65535
#define MIN_LOOP_VALUE 1
#define MAX_NA_VALUE pow(2,40)-1
#define MIN_NA_VALUE 1
#define MAX_TIME_VALUE 10000
#define MIN_TIME_VALUE 25E-9
#define MAX_AL_VALUE 16384
#define MIN_AL_VALUE 1
#define MAX_DW_VALUE 1E-3
#define MIN_DW_VALUE MIN_TIME_VALUE
#define MAX_AMP_VALUE 100
#define MIN_AMP_VALUE 0


//enum TKindOfArray {Fixed, Linear, Random};
//enum TUnit {Femto,Pico,Nano,Micro,Milli,Unity,Kilo,Mega,Giga,Tera};

class TVariable {
  public:
    enum Unit {Femto,Pico,Nano,Micro,Milli,Unity,Kilo,Mega,Giga,Tera};
    enum Type {timeVariable, loopVariable, naVariable,
               ampVariable, phaseVariable, intVariable,
               freqVariable, dwVariable, alVariable, doubleVariable, ndVariable,pdVariable};
    enum ValueType {Double, Integer};
    enum ArrayMode {LinearArray, RandomArray};

    TVariable() {initialize();}
    TVariable(QString name) {initialize(); setName(name);}
    TVariable(QString name, Type type) {initialize(); setName(name); setType(type);}
    TVariable(QString name, Type type, QVariant num, TVariable::Unit uni) {
      initialize(); setName(name); setType(type); setNumeric(num); setUnit(uni);
    }

    virtual ~TVariable() { addresses.clear(); expressions.clear(); gateIndex.clear(); AD9858ProfileIndex.clear();}

    QString errorMessage() {return FerrorMessage;}
    void setErrorMessage(QString qs) {FerrorMessage=qs;}
    bool hasError() {return Ferror;}

    QString name() {return FName;}
    void setName(QString s) {FName=s;}
    QString caption() {return FCaption;}
    void setCaption(QString s) {FCaption=s;}

    Type type() {return Ftype;}
    void setType(Type ty) {
        Ftype=ty;
        switch(Ftype) {
          case loopVariable:
          case naVariable:
          case ndVariable:
          case intVariable:
            setValueType(Integer); break;
          default:
            setValueType(Double); break;
        }
    }

    ValueType valueType() {return FValueType;}
    void setValueType(ValueType vt) {FValueType=vt;}
    Unit unit() {return FUnit;}
    void setUnit(Unit un) {FPreviousNumeric=Fnumeric; FPreviousUnit=FUnit; FUnit=un;}
    void changeUnit(Unit un);

    void undoUpdate() {FUnit=FPreviousUnit;Fnumeric=FPreviousNumeric;}

    QVariant numeric();
    QVariant numeric(Unit u);
    void setNumeric(QVariant qv);
    void setNumeric(QString qs, bool *ok);

    QString numericString();
    QString iniNumericString();
    QString incNumericString();
    QString randomArrayNumericString(int k);
    QString randomArrayNumericString();

    QVariant value();

    static int exponent(Unit unit)
    {
        int e;
          switch(unit) {
            case Pico: e=-12; break;
            case Nano: e=-9; break;
            case Micro: e=-6; break;
            case Milli: e=-3; break;
            case Unity: e=0; break;
            case Kilo: e=3; break;
            case Mega: e=6; break;
            case Giga: e=9; break;
            case Tera: e=12; break;
            default: e=0; break;
          }
        return e;
    }
    int exponent() {return TVariable::exponent(FUnit);}


    bool isConst() {return FisConst;}
    void setConst(bool b) {FisConst=b;}

    static QString unitString(Unit unit);

    QString unitString() {
      QString qs=TVariable::unitString(FUnit);
      if(type()==timeVariable || type()==dwVariable || type()==pdVariable)
        if(FUnit==TVariable::Unity)  qs+="s";
      return qs;
    }

    QString unitString2() {
      QString qs=TVariable::unitString(FUnit);
      if(type()==timeVariable || type()==dwVariable) qs+="s";
      else if(type()==freqVariable) qs+="MHz";
      else if(type()==phaseVariable) qs+="deg.";
      return qs;
    }


    static QString typeString(Type type);
    QString typeString() {return typeString(Ftype);}

    static int NOfChannels;

    QList<QList<QString> > expressions;
    QList<QList<int> > addresses;
      // It retains the address(es) where the variable is used in the combiled (binary) ppg.
      // row: channel no (0,1,2,...)
      // column: address(es)

    QList<QList<int> > gateIndex;
      // gateIndex carries the index of the gate that use the variable.
    QList<QList<int> > AD9858ProfileIndex;
      // This stores the AD9858 profile index. This is used only by freq. variable

    int declarationPosition() {return FdeclarationPosition;}
    void setDeclarationPosition(int p) {FdeclarationPosition=p;}

    static bool checkRange(Type type, QVariant numeric, TVariable::Unit unit);
    static bool checkRange(Type type, QVariant numeric) {return TVariable::checkRange(type,numeric,Unity);}
    bool checkRange() {return TVariable::checkRange(Ftype,Fnumeric,FUnit);}

    QVariant iniValue() {return FIniValue;}
    void setIniValue(QVariant v) {FIniValue=v;}
    void setIniValue(QString qs, bool *ok);
    QVariant incValue() {return FIncValue;}
    void setIncValue(QVariant v) {FIncValue=v;}
    void setIncValue(QString qs, bool *ok);
    int arrayCount() {return FArrayCount;}
    void setArrayCount(int c) {FArrayCount=c;}
    QVariant arrayNumeric(int k);
    QVariant arrayValue(int k);
    int arrayDimension() {return FArrayDimension;}
    void setArrayDimension(int dim) {FArrayDimension=dim;}

    QList<QVariant> randomArrayValues;
    bool setRandomArrayValues(QStringList sl);

    QString arrayInfo();
    void setArray(int k);
    ArrayMode arrayMode() {return FArrayMode;}
    void setArrayMode(ArrayMode am) {FArrayMode=am; if(am==LinearArray) randomArrayValues.clear();}
    QString arrayModeString()
    {
       if(arrayMode()==LinearArray) return "Linear";
       else if(arrayMode()==RandomArray) return "Random";
       else return "";
    }

protected:
    void initialize();
    void setNOfChannels();


    QString FName;
    QString FCaption;
    QString FerrorMessage;
    bool Ferror;
    Type Ftype;    // Time, Loop, ...
    ValueType FValueType;  // Double, Integer
    TVariable::Unit FUnit, FPreviousUnit;
    QVariant Fnumeric,FPreviousNumeric;

    bool FisConst;
    QVariant FIniValue;
    QVariant FIncValue;
    int FArrayCount;
    int FArrayDimension;// 0: no array 1: 2D, 2: 3D, ...
    ArrayMode FArrayMode;
    int FdeclarationPosition;
      // This carries the line number in which the variable was declared in the pulse program.

};  // TVariable


#endif // VARIABLES_H
