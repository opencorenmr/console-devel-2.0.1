#include "axisStyle.h"
#include "float.h"
#include "math.h"

TAxisStyle::TAxisStyle()
{
  setProcessType(TProcessElement::AxisStyle);
  setDomain(AxisDomain::TimeDomain);
  setAxisStyle(AxisStyle::NormalStyle);
  setUnit(AxisUnit::msec);
  setLabel("");
  setReferencePoint(0);
  setReferenceValue(0);
}

void TAxisStyle::setDomain(int ad) {FDomain=ad;}
void TAxisStyle::setDomain(QString qs)
{
    if(0==QString::compare(qs,"time",Qt::CaseInsensitive)) setDomain(TimeDomain);
    else if(0==QString::compare(qs,"frequency",Qt::CaseInsensitive)) setDomain(FrequencyDomain);
    else setDomain(Other);
}
QString TAxisStyle::domainString()
{
    QString qs;
    if(domain()==TimeDomain) {qs="time";}
    else if(domain()==FrequencyDomain) {qs="frequency";}
    else {qs="other";}
    return qs;
}

void TAxisStyle::setUnit(int au) {FUnit=au;}
void TAxisStyle::setUnit(QString qs)
{
    if(0==QString::compare(qs,"usec",Qt::CaseInsensitive)) setUnit(usec);
    else if(0==QString::compare(qs,"msec",Qt::CaseInsensitive)) setUnit(msec);
    else if(0==QString::compare(qs,"sec",Qt::CaseInsensitive)) setUnit(sec);
    else if(0==QString::compare(qs,"kHz",Qt::CaseInsensitive)) setUnit(kHz);
    else if(0==QString::compare(qs,"Hz",Qt::CaseInsensitive)) setUnit(Hz);
    else if(0==QString::compare(qs,"ppm",Qt::CaseInsensitive)) setUnit(ppm);
    else setUnit(msec); // ad hoc default

}
QString TAxisStyle::unitString()
{
    QString qs;
    if(unit()==nsec) {qs="nsec";}
    else if(unit()==usec) {qs="usec";}
    else if(unit()==msec) {qs="msec";}
    else if(unit()==sec) {qs="sec";}
    else if(unit()==GHz) {qs="GHz";}
    else if(unit()==MHz) {qs="MHz";}
    else if(unit()==kHz) {qs="kHz";}
    else if(unit()==Hz) {qs="Hz";}
    else if(unit()==ppm) {qs="ppm";}

    return qs;

}

QStringList TAxisStyle::processInformation()
{
    // QString
    return QStringList();
}

QString TAxisStyle::command()
{
    QString qs1,qs2,qs3;

    qs1="ref. point: ";
    qs2="ref. value: ";

    qs3= "axisStyle ("
            + domainString() + ", "
            + unitString() + ", "
            + label() + ", "
            + qs1 + QString::number(referencePoint()) + ", "
            + qs2 + QString::number(referenceValue());
    return qs3;
}

bool TAxisStyle::process(TFID_2D *fid_2d, int k)
{
    if(k<0 || k>fid_2d->FID.size()-1)
    {
        errorQ=true;
        setErrorMessage(QString(Q_FUNC_INFO)+": index is out of range.");
        return false;
    }
    else return process(fid_2d->FID[k]);

}

bool TAxisStyle::process(TFID_2D *fid_2d)
{
    errorQ=false;
    switch(applyMode())
    {
      default:
      case ApplyToAll:

        for(int c=0; c<fid_2d->FID.size(); c++)
        {
          errorQ=!process(fid_2d->FID[c]);

          if(errorQ) break;
        }
        break;
      case ApplyToOne:
        if(applyIndex()<0 || applyIndex()>fid_2d->FID.size()-1)
        {
          errorQ=true;
          setErrorMessage(QString(Q_FUNC_INFO) + ": Index out of range.");
        }
        else
        {
          errorQ=!process(fid_2d->FID[applyIndex()]);
        }
        break;
      case ApplyToOthers:

        if(applyIndex()<0 || applyIndex()>fid_2d->FID.size()-1)
        {
          errorQ=true;
          setErrorMessage(QString(Q_FUNC_INFO) + ": Index out of range.");
        }
        else
        {
          for(int k=0; k<fid_2d->FID.size(); k++)
          {
            if(k!=applyIndex())
            {
              errorQ=!process(fid_2d->FID[k]);
              if(errorQ) break;
            }
          } // k
        }
        break;
    } // switch

    return !errorQ;

}


bool TAxisStyle::process(TFID *fid)
{
   // qDebug() << QString(Q_FUNC_INFO) << "0";
    if(referencePoint()<0 || referencePoint()>fid->al()-1)
    {
        setErrorMessage("Reference point is out of range.");
        return false;
    }
//    qDebug() << QString(Q_FUNC_INFO) << "sf1 " << fid->sf1();

    if(axisStyle()==PPMStyle && fabs(fid->sf1()) < DBL_EPSILON)
    {
        setErrorMessage("sf1 is set to zero, and division cannot be performed.");
        return false;
    }

    fid->setDomain(domain());
    fid->setXAxisLabel(label());
//    qDebug() << QString(Q_FUNC_INFO) << "a";
//    qDebug() << QString(Q_FUNC_INFO) << "axisStyle " << axisStyle();

    if(domain()==TimeDomain)
    {
        // We assume that the time domain signal is stored in usec.
        fid->setPrefix(TMetricPrefix::Micro);
        switch(unit())
        {
          default:
          case AxisUnit::usec:
            fid->setPlotPrefix(TMetricPrefix::Micro);
            break;
          case AxisUnit::msec:
            fid->setPlotPrefix(TMetricPrefix::Milli);
            break;
          case AxisUnit::sec:
            fid->setPlotPrefix(TMetricPrefix::None);
            break;
        }
    }
    else if(domain()==FrequencyDomain)
    {
        fid->setPrefix(TMetricPrefix::None);
        switch(unit())
        {
          default:
          case AxisUnit::kHz:
            fid->setPlotPrefix(TMetricPrefix::Kilo);
            break;
          case AxisUnit::Hz:
            fid->setPlotPrefix(TMetricPrefix::None);
            break;

          case AxisUnit::ppm:
            fid->setPlotPrefix(TMetricPrefix::None);
            break;
        }

    }
    else
    {
        fid->setPrefix(TMetricPrefix::None);
    }

    if(domain()==FrequencyDomain)
    {
    switch(axisStyle())
    {
      case PPMStyle:
       // fid->xunit=TFID::Hz;
//        qDebug() << QString(Q_FUNC_INFO) << "1";
        fid->setPrefix(TMetricPrefix::None);
        fid->setPlotPrefix(TMetricPrefix::None);

        fid->setDx(
                    -1.0/(fid->dw()*TMetricPrefix::Decimal(TMetricPrefix::Micro)
                                    *fid->al()*fid->sf1())
                    );
        fid->setXInitialValue(referenceValue()-referencePoint()*fid->dx());

        fid->setXAxisUnitSymbol("ppm");
     //   fid->setDomain(TFID::FrequencyDomain);
     //   qDebug() << QString(Q_FUNC_INFO) << "2";

        break;

      case NormalStyle:
       // fid->xunit=TFID::Second;
        fid->setPrefix(TMetricPrefix::None);
        fid->setPlotPrefix(TMetricPrefix::Kilo);
        fid->setDx(
                    -1.0/(fid->dw()*TMetricPrefix::Decimal(TMetricPrefix::Micro)
                                    *fid->al())
                    );
        fid->setXInitialValue(
                    0.5/(fid->dw()*TMetricPrefix::Decimal(TMetricPrefix::Micro))
                    );
        fid->setXAxisUnitSymbol("Hz");
       // fid->domain=TFID::TimeDomain;
        break;


      default:
        break;
    }
    }
    return true;

}
