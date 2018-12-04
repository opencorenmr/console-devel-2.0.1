#include "metricPrefix.h"
#include "math.h"

TMetricPrefix::TMetricPrefix()
{
  setPrefix(None); // default
}


double TMetricPrefix::Decimal(prefixes p)
{
    double d;
    switch(p)
    {
      case Yotta: d=pow(10,24); break;
      case Zetta: d=pow(10,21); break;
      case Exa: d=pow(10,18); break;
      case Peta: d=pow(10,15); break;
      case Tera: d=pow(10,12); break;
      case Giga: d=pow(10,9); break;
      case Mega: d=pow(10,6); break;
      case Kilo: d=pow(10,3); break;
      case Hecto: d=pow(10,2); break;
      case Deca: d=pow(10,1); break;
      case None: d=pow(10,0); break;
      case Deci: d=pow(10,-1); break;
      case Centi: d=pow(10,-2); break;
      case Milli: d=pow(10,-3); break;
      case Micro: d=pow(10,-6); break;
      case Nano: d=pow(10,-9); break;
      case Pico: d=pow(10,-12); break;
      case Femto: d=pow(10,-15); break;
      case Atto: d=pow(10,-18); break;
      case Zepto: d=pow(10,-21); break;
      case Yocto: d=pow(10,-24); break;
      default: d=1.0; break;
    }
    return d;
}

QString TMetricPrefix::Symbol(prefixes p)
{
    QString qs;
    switch(p)
    {
      case Yotta: qs="Y"; break;
      case Zetta: qs="Z"; break;
      case Exa: qs="E"; break;
      case Peta: qs="P"; break;
      case Tera: qs="T"; break;
      case Giga: qs="G"; break;
      case Mega: qs="M"; break;
      case Kilo: qs="k"; break;
      case Hecto: qs="h"; break;
      case Deca: qs="da"; break;
      case None: qs=""; break;
      case Deci: qs="d"; break;
      case Centi: qs="c"; break;
      case Milli: qs="m"; break;
      case Micro: qs="u"; break;
      case Nano: qs="n"; break;
      case Pico: qs="p"; break;
      case Femto: qs="f"; break;
      case Atto: qs="a"; break;
      case Zepto: qs="z"; break;
      case Yocto: qs="y"; break;
      default: qs=""; break;
    }
    return qs;
}


QString TMetricPrefix::Name(prefixes p)
{
    QString qs;
    switch(p)
    {
      case Yotta: qs="yotta"; break;
      case Zetta: qs="zetta"; break;
      case Exa: qs="exa"; break;
      case Peta: qs="peta"; break;
      case Tera: qs="tera"; break;
      case Giga: qs="giga"; break;
      case Mega: qs="mega"; break;
      case Kilo: qs="kilo"; break;
      case Hecto: qs="hecto"; break;
      case Deca: qs="deca"; break;
      case None: qs=""; break;
      case Deci: qs="deci"; break;
      case Centi: qs="centi"; break;
      case Milli: qs="milli"; break;
      case Micro: qs="micro"; break;
      case Nano: qs="nano"; break;
      case Pico: qs="pico"; break;
      case Femto: qs="femto"; break;
      case Atto: qs="atto"; break;
      case Zepto: qs="zepto"; break;
      case Yocto: qs="yocto"; break;
      default: qs=""; break;
    }
    return qs;
}

int TMetricPrefix::prefixIndex(QString name)
{
    int i;
    if(QString::compare(name,"yotta"),Qt::CaseInsensitive) i=0;
    else if(QString::compare(name,"zetta",Qt::CaseInsensitive)) i=1;
    else if(QString::compare(name,"exa",Qt::CaseInsensitive)) i=2;
    else if(QString::compare(name,"tera",Qt::CaseInsensitive)) i=3;
    else if(QString::compare(name,"giga",Qt::CaseInsensitive)) i=4;
    else if(QString::compare(name,"mega",Qt::CaseInsensitive)) i=5;
    else if(QString::compare(name,"kilo",Qt::CaseInsensitive)) i=6;
    else if(QString::compare(name,"hecto",Qt::CaseInsensitive)) i=7;
    else if(QString::compare(name,"deca",Qt::CaseInsensitive)) i=8;
    else if(QString::compare(name,"",Qt::CaseInsensitive)) i=9;    // "" -> 9!
    else if(QString::compare(name,"none",Qt::CaseInsensitive)) i=9; // "none" -> alse 9!
    else if(QString::compare(name,"deci",Qt::CaseInsensitive)) i=10;
    else if(QString::compare(name,"centi",Qt::CaseInsensitive)) i=11;
    else if(QString::compare(name,"milli",Qt::CaseInsensitive)) i=12;
    else if(QString::compare(name,"micro",Qt::CaseInsensitive)) i=13;
    else if(QString::compare(name,"nano",Qt::CaseInsensitive)) i=14;
    else if(QString::compare(name,"pico",Qt::CaseInsensitive)) i=15;
    else if(QString::compare(name,"femto",Qt::CaseInsensitive)) i=16;
    else if(QString::compare(name,"atto",Qt::CaseInsensitive)) i=17;
    else if(QString::compare(name,"zepto",Qt::CaseInsensitive)) i=18;
    else if(QString::compare(name,"yocto",Qt::CaseInsensitive)) i=19;
    else i=9;

    return i;
}

void TMetricPrefix::setPrefix(QString name)
{
    if(0==QString::compare(name,"yotta",Qt::CaseInsensitive)) setPrefix(Yotta);
    else if(0==QString::compare(name,"zetta",Qt::CaseInsensitive)) setPrefix(Zetta);
    else if(0==QString::compare(name,"exa",Qt::CaseInsensitive)) setPrefix(Exa);
    else if(0==QString::compare(name,"tera",Qt::CaseInsensitive)) setPrefix(Tera);
    else if(0==QString::compare(name,"giga",Qt::CaseInsensitive)) setPrefix(Giga);
    else if(0==QString::compare(name,"mega",Qt::CaseInsensitive)) setPrefix(Mega);
    else if(0==QString::compare(name,"kilo",Qt::CaseInsensitive)) setPrefix(Kilo);
    else if(0==QString::compare(name,"hecto",Qt::CaseInsensitive)) setPrefix(Hecto);
    else if(0==QString::compare(name,"deca",Qt::CaseInsensitive)) setPrefix(Deca);
    else if(0==QString::compare(name,"",Qt::CaseInsensitive)) setPrefix(None);    // "" -> None
    else if(0==QString::compare(name,"none",Qt::CaseInsensitive)) setPrefix(None); // "none" -> None
    else if(0==QString::compare(name,"deci",Qt::CaseInsensitive)) setPrefix(Deci);
    else if(0==QString::compare(name,"centi",Qt::CaseInsensitive)) setPrefix(Centi);
    else if(0==QString::compare(name,"milli",Qt::CaseInsensitive)) setPrefix(Milli);
    else if(0==QString::compare(name,"micro",Qt::CaseInsensitive)) setPrefix(Micro);
    else if(0==QString::compare(name,"nano",Qt::CaseInsensitive)) setPrefix(Nano);
    else if(0==QString::compare(name,"pico",Qt::CaseInsensitive)) setPrefix(Pico);
    else if(0==QString::compare(name,"femto",Qt::CaseInsensitive)) setPrefix(Femto);
    else if(0==QString::compare(name,"atto",Qt::CaseInsensitive)) setPrefix(Atto);
    else if(0==QString::compare(name,"zepto",Qt::CaseInsensitive)) setPrefix(Zepto);
    else if(0==QString::compare(name,"yocto",Qt::CaseInsensitive)) setPrefix(Yocto);
    else setPrefix(None);

}

QStringList TMetricPrefix::prefixList()
{
    QStringList sl;
    sl.clear();

    sl.append("yotta");
    sl.append("zetta");
    sl.append("exa");
    sl.append("peta");
    sl.append("tera");
    sl.append("giga");
    sl.append("mega");
    sl.append("kilo");
    sl.append("hecto");
    sl.append("deca");
    sl.append("none");
    sl.append("deci");
    sl.append("centi");
    sl.append("milli");
    sl.append("micro");
    sl.append("nano");
    sl.append("pico");
    sl.append("femto");
    sl.append("atto");
    sl.append("zepto");
    sl.append("yocto");

    return sl;
}

