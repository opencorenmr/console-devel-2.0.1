#include "variables.h"
#include "math.h"


void TVariable::initialize(){
    FisConst=false;
    setArrayMode(LinearArray);
    setArrayDimension(0);
    setArrayCount(1);
    FUnit=Unity; FValueType=Double; Ferror=false; FerrorMessage.clear();
    setNOfChannels();
}

int TVariable::NOfChannels=0;  // initialization of a static member

void TVariable::setNOfChannels() {
    expressions.clear();
    addresses.clear();
    gateIndex.clear();
    AD9858ProfileIndex.clear();
    for (int j=0; j<TVariable::NOfChannels; j++)
    {
        addresses.append(QList<int>());
        expressions.append(QList<QString>());
        gateIndex.append(QList<int>());
        AD9858ProfileIndex.append(QList<int>());
    }
}

QVariant TVariable::numeric() {return Fnumeric;}
QVariant TVariable::numeric(TVariable::Unit u) {
  if(u==unit()) return numeric();
  int e;
    switch(u) {
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

  int e2 = exponent() - e ;

  QVariant qv;
  switch(valueType()){
    case Integer:
      qv=QVariant(Fnumeric.toInt() * ((int)pow(10,e2)));
      break;

    case Double:
    default:
      qv=QVariant(Fnumeric.toDouble() * pow(10,e2));
      break;
  }

  return qv;

}


void TVariable::changeUnit(TVariable::Unit un){
    setNumeric(numeric(un));
    setUnit(un);
}

QVariant TVariable::value(){
  QVariant qv;
    switch(valueType()){
      case Integer:
        qv=QVariant(Fnumeric.toInt() * ((int)pow(10,exponent())));
        break;

      case Double:
      default:
        qv=QVariant(Fnumeric.toDouble() * pow(10,exponent()));
        break;
    }
  return qv;
}

QVariant TVariable::arrayNumeric(int k)
{
    if(k<0 || k>arrayCount()-1) return iniValue();
    else
    {
        QVariant qv;
        switch(valueType()){
          case Integer:
            qv=QVariant(iniValue().toInt() + k*incValue().toInt());
            break;

          case Double:
          default:
            qv=QVariant(iniValue().toDouble() + k*incValue().toDouble());
            break;
        }
        return qv;
    }
}


QVariant TVariable::arrayValue(int k)
{
    QVariant num=arrayNumeric(k);
    QVariant qv;
      switch(valueType()){
        case Integer:
          qv=QVariant(num.toInt() * ((int)pow(10,exponent())));
          break;

        case Double:
        default:
          qv=QVariant(num.toDouble() * pow(10,exponent()));
          break;
      }
    return qv;
  }

QString TVariable::unitString(Unit unit)
{
    QString qs;
    switch(unit) {
      case Pico: qs="p"; break;
      case Nano: qs="n"; break;
      case Micro: qs="u"; break;
      case Milli: qs="m"; break;
      case Unity: qs=""; break;
      case Kilo: qs="k"; break;
      case Mega: qs="M"; break;
      case Giga: qs="G"; break;
      case Tera: qs="T"; break;
      default : qs=""; break;
    }
  return qs;
}


QString TVariable::typeString(Type type)
{
    QString qs;
    switch(type) {
      case timeVariable: qs="time"; break;
      case loopVariable: qs="loop"; break;
      case naVariable: qs="aux"; break;
      case ndVariable: qs="aux"; break;
      case ampVariable: qs="amp"; break;
      case phaseVariable: qs="phase"; break;
      case intVariable: qs="int"; break;
      case freqVariable: qs="freq"; break;
      case dwVariable: qs="aux"; break;
      case alVariable: qs="aux"; break;
      case doubleVariable: qs="double"; break;
      case pdVariable: qs="aux"; break;
      default: qs="unknown type"; break;
    }
    return qs;
}



bool TVariable::checkRange(Type type, QVariant numeric, TVariable::Unit unit)
{
    bool result=true;
    double d=0;
    int i=0;
    switch(type)
    {
      case timeVariable:
      case pdVariable:
        d=numeric.toDouble()*pow(10,TVariable::exponent(unit));
        if((d<MIN_TIME_VALUE) || (d>MAX_TIME_VALUE)) {result=false;}
        break;

      case dwVariable:
        d=numeric.toDouble()*pow(10,TVariable::exponent(unit));
        if((d<MIN_DW_VALUE) || (d>MAX_DW_VALUE)) {result=false;}
        break;

      case loopVariable:
        i=numeric.toInt()*pow(10,TVariable::exponent(unit));
        if((i<MIN_LOOP_VALUE) || (d>MAX_LOOP_VALUE)) {result=false;}
        break;

      case naVariable:
      case ndVariable:
        i=numeric.toInt()*pow(10,TVariable::exponent(unit));
        if((d<MIN_NA_VALUE) || (d>MAX_NA_VALUE)) {result=false;}
        break;

      case alVariable:
        i=numeric.toInt()*pow(10,TVariable::exponent(unit));
        if((d<MIN_AL_VALUE) || (d>MAX_AL_VALUE)) {result=false;}
        break;

      case ampVariable:
        d=numeric.toDouble()*pow(10,TVariable::exponent(unit));
        if((d<MIN_AMP_VALUE) || (d>MAX_AMP_VALUE)) {result=false;}
        break;

    case phaseVariable:
    case intVariable:
    case freqVariable:
        break;    // No check! Users are responsible for setting these variables!

    default:
        result=false;
        break;

    } // switch

    return result;

}

void TVariable::setArray(int k)
{
    if(arrayMode()==LinearArray)
    {
        if(valueType()==Double)
        {
           double d=iniValue().toDouble() + k * incValue().toDouble();
           setNumeric(QVariant(d));
        }
        else  // integer
        {
           int i=iniValue().toInt() + k * incValue().toInt();
           setNumeric(QVariant(i));
        }

    } //
    else // random array
    {
      setNumeric(randomArrayValues.at(k));
    }
}

QString TVariable::arrayInfo()
{
    QString qs;
    if(arrayMode()==LinearArray)
    {
        qs= "ini: " + iniNumericString()
                + ", inc: " + incNumericString() + ", "
                + QString::number(arrayCount()) + " steps";
    } //
    else if(arrayMode()==RandomArray) // random array
    {
        qs=randomArrayNumericString();
    }

return qs;

}


void TVariable::setNumeric(QVariant qv)
{
    FPreviousNumeric=Fnumeric; FPreviousUnit=FUnit; Fnumeric=qv;

}
void TVariable::setNumeric(QString qs, bool *ok)
{
    if(valueType()==Integer)
    {
      int i=qs.toInt(ok);
      if(ok) setNumeric(QVariant(i));
    }
    else if(valueType()==Double)
    {
        double d=qs.toDouble(ok);
        if(ok) setNumeric(QVariant(d));
    }
}

QString TVariable::randomArrayNumericString(int k)
{
    QString s; s.clear();
    if(randomArrayValues.size()<1) return s;
    if(k<0 || k>randomArrayValues.size()-1) return s;

    if(valueType()==Integer)
    {
      s=QString::number(randomArrayValues.at(k).toInt());
    }
    else if(valueType()==Double)
    {
      s=QString::number(randomArrayValues.at(k).toDouble(),'g',13);
    }
    return s;
}

QString TVariable::randomArrayNumericString()
{
    QString s; s.clear();
    if(randomArrayValues.size()<1) return s;

    for(int k=0; k<randomArrayValues.size(); k++)
    {
        s.append(randomArrayNumericString(k));
        s.append(" ");
    }
    return s.trimmed();
}

QString TVariable::iniNumericString()
{
   QString s;
   if(valueType()==Integer)
   {
     s=QString::number(iniValue().toInt());
   }
   else if(valueType()==Double)
   {
     s=QString::number(iniValue().toDouble(),'g',13);
   }
   else s="";
   return s;

}

QString TVariable::incNumericString()
{
   QString s;
   if(valueType()==Integer)
   {
     s=QString::number(incValue().toInt());
   }
   else if(valueType()==Double)
   {
     s=QString::number(incValue().toDouble(),'g',13);
   }
   else s="";
   return s;

}

QString TVariable::numericString()
{
   QString s;
   if(valueType()==Integer)
   {
     s=QString::number(numeric().toInt());
   }
   else if(valueType()==Double)
   {
     s=QString::number(numeric().toDouble(),'g',13);
   }
   else s="";
   return s;

}


bool TVariable::setRandomArrayValues(QStringList sl)
{
  bool ok=true;
  randomArrayValues.clear();
  for(int k=0; k<sl.size(); k++)
  {
    if(valueType()==Integer)
    {
      int i=sl.at(k).toInt(&ok);
      if(ok) randomArrayValues.append(QVariant(i));
      else
      {
          setErrorMessage("Invalid expression: " + sl.at(k));
          break;
      }
    }
    else if(valueType()==Double)
    {
        double d=sl.at(k).toDouble(&ok);
        if(ok) randomArrayValues.append(QVariant(d));
        else
        {
            setErrorMessage("Invalid expression: " + sl.at(k));
            break;
        }
    }
  }//k

  if(!ok) randomArrayValues.clear();
  setArrayCount(sl.size());
  return ok;

}

void TVariable::setIniValue(QString qs, bool *ok)
{
    if(valueType()==Integer)
    {
      int i=qs.toInt(ok);
      if(ok) setIniValue(QVariant(i));
    }
    else if(valueType()==Double)
    {
        double d=qs.toDouble(ok);
        if(ok) setIniValue(QVariant(d));
    }
}

void TVariable::setIncValue(QString qs, bool *ok)
{
    if(valueType()==Integer)
    {
      int i=qs.toInt(ok);
      if(ok) setIncValue(QVariant(i));
    }
    else if(valueType()==Double)
    {
        double d=qs.toDouble(ok);
        if(ok) setIncValue(QVariant(d));
    }
}
