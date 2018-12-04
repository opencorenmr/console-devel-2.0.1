#include "fidDomain.h"

fidDomain::fidDomain(AxisDomain ad)
{
  setAxisDomain(ad);
}

QStringList fidDomain::processInformation() {return QStringList() << "fidDomain";}
QString fidDomain::command() {return "xAxisDomain";}

bool fidDomain::process(TFID_2D *fid_2d,AxisDomain ad)
{
    bool result;
    for(int i=0;i<fid_2d->FID.size(); i++)
    {
        result=process(fid_2d->FID[i],ad);
    }
    return result;
}

bool fidDomain::process(TFID_2D *fid_2d)
{
    bool result;
    for(int i=0;i<fid_2d->FID.size(); i++)
    {
        result=process(fid_2d->FID[i],axisDomain());
    }
    return result;
}

bool fidDomain::process(TFID_2D *fid_2d, int k, AxisDomain ad)
{
    if(k<0 || k>(fid_2d->FID.size())-1)
    {
        // The followings cannot be used for static functions, and have been commented out (KT)
      //  errorQ=true;
      //  errorMessage=QString(Q_FUNC_INFO)+": index (" +QString::number(k)
      //          +") is out of range.";
        return false;
    }
    return process(fid_2d->FID[k],ad);
}

bool fidDomain::process(TFID_2D *fid_2d, int k)
{
    if(k<0 || k>(fid_2d->FID.size())-1)
    {
        errorQ=true;
        errorMessage=QString(Q_FUNC_INFO)+": index (" +QString::number(k)
                +") is out of range.";
        return false;
    }
    return process(fid_2d->FID[k],axisDomain());
}

bool fidDomain::process(TFID *fid)
{
    return process(fid,axisDomain());
}

bool fidDomain::process(TFID *fid, AxisDomain ad)
{

    switch(ad)
    {
      case FrequencyDomain:
        fid->xunit=TFID::Hz;
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
        fid->domain=TFID::FrequencyDomain;


        break;

      case TimeDomain:
        fid->xunit=TFID::Second;
        fid->metricPrefix.setPrefix(TMetricPrefix::Micro);
        fid->plotMetricPrefix.setPrefix(TMetricPrefix::Milli);
        fid->setXInitialValue(0.0);
        fid->setDx(fid->dw()*TMetricPrefix::Decimal(TMetricPrefix::Micro));
        fid->setXAxisUnitSymbol("sec");
        fid->domain=TFID::TimeDomain;
        break;

      case ToggleDomain:

        if(fid->domain==TFID::TimeDomain)
        {
            fid->xunit=TFID::Hz;
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
            fid->domain=TFID::FrequencyDomain;

        }
        else if(fid->domain==TFID::FrequencyDomain)
        {
            fid->xunit=TFID::Second;
            fid->metricPrefix.setPrefix(TMetricPrefix::Micro);
            fid->plotMetricPrefix.setPrefix(TMetricPrefix::Milli);
            fid->setXInitialValue(0.0);
            fid->setDx(fid->dw()*TMetricPrefix::Decimal(TMetricPrefix::Micro));
            fid->setXAxisUnitSymbol("sec");
            fid->domain=TFID::TimeDomain;
        }
        break;

      case KeepDomain:
        break;
    }


    return true;

}

