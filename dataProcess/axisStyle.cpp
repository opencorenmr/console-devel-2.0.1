#include "axisStyle.h"

TAxisStyle::TAxisStyle()
{
  setAxisStyle(NormalStyle);
  setReferencePoint(0);
}

void TAxisStyle::setDomain(AxisDomain ad) {FDomain=ad;}
void TAxisStyle::setDomain(QString qs)
{
    if(0==QString::compare(qs,"time",Qt::CaseInsensitive)) setDomain(TimeDomain);
    else if(0==QString::compare(qs,"time",Qt::CaseInsensitive)) setDomain(FrequencyDomain);
    else setDomain(Other);
}

void TAxisStyle::setUnit(AxisUnit au) {FUnit=au;}
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

QStringList TAxisStyle::processInformation()
{
    // QString
    return QStringList();
}

QString TAxisStyle::command() {return "axisStyle";}

bool TAxisStyle::process(TFID_2D *fid_2d, int k) { return process(fid_2d->FID[k]);}

bool TAxisStyle::process(TFID_2D *fid_2d)
{
    bool ok=true;
    for(int k=0; k<fid_2d->FID.size(); k++)
    {
        if(!process(fid_2d->FID[k])) ok=false;
    }

    return ok;
}


bool TAxisStyle::process(TFID *fid)
{
    if(referencePoint()<0 || referencePoint()>fid->al()-1)
    {
        errorMessage="Reference point is out of range.";
        return false;
    }

    if(axisStyle()==PPMStyle && fid->sf1()==0)
    {
        errorMessage="sf1 is set to zero, and division cannot be performed.";
        return false;
    }

    switch(axisStyle())
    {
      case PPMStyle:
       // fid->xunit=TFID::Hz;
        fid->metricPrefix.setPrefix(TMetricPrefix::None);
        fid->plotMetricPrefix.setPrefix(TMetricPrefix::None);

        fid->setDx(
                    -1.0/(fid->dw()*TMetricPrefix::Decimal(TMetricPrefix::Micro)
                                    *fid->al()*fid->sf1())
                    );
        fid->setXInitialValue(referenceValue()-referencePoint()*fid->dx());

        fid->setXAxisUnitSymbol("ppm");
       // fid->domain=TFID::FrequencyDomain;

        break;

      case NormalStyle:
       // fid->xunit=TFID::Second;
        fid->metricPrefix.setPrefix(TMetricPrefix::None);
        fid->plotMetricPrefix.setPrefix(TMetricPrefix::Kilo);
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

    return true;

}
