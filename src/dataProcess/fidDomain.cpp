#include "fidDomain.h"

fidDomain::fidDomain(AxisDomain ad)
{
  setAxisDomain(ad);
}

QStringList fidDomain::processInformation() {return QStringList() << "fidDomain";}
QString fidDomain::command() {return "xAxisDomain";}

bool fidDomain::process(TFID_2D *fid_2d,AxisDomain ad)
{
    bool result=true;
    for(int i=0;i<fid_2d->FID.size(); i++)
    {
        result=process(fid_2d->FID[i],ad);
    }
    return result;
}

bool fidDomain::process(TFID_2D *fid_2d)
{
    bool result=true;
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
        setErrorMessage(QString(Q_FUNC_INFO)+": index (" +QString::number(k)
                +") is out of range.");
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
        fid->setXUnit(TFIDXUnit::Hz);
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
        fid->setDomain(TFID::FrequencyDomain);


        break;

      case TimeDomain:
        fid->setXUnit(TFIDXUnit::Second);
        fid->setPrefix(TMetricPrefix::Micro);
        fid->setPlotPrefix(TMetricPrefix::Milli);
        fid->setXInitialValue(0.0);
        fid->setDx(fid->dw()*TMetricPrefix::Decimal(TMetricPrefix::Micro));
        fid->setXAxisUnitSymbol("sec");
        fid->setDomain(TFID::TimeDomain);
        break;

      case ToggleDomain:
        if(fid->domain()==TFID::TimeDomain)
        {
            fid->setXUnit(TFIDXUnit::Hz);
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
            fid->setDomain(TFID::FrequencyDomain);

       //     qDebug() << fid->dw() << fid->al() << fid->dx() << fid->xInitialValue();

        }
        else if(fid->domain()==TFID::FrequencyDomain)
        {
            fid->setXUnit(TFIDXUnit::Second);
            fid->setPrefix(TMetricPrefix::Micro);
            fid->setPlotPrefix(TMetricPrefix::Milli);
            fid->setXInitialValue(0.0);
            fid->setDx(fid->dw()*TMetricPrefix::Decimal(TMetricPrefix::Micro));
            fid->setXAxisUnitSymbol("sec");
            fid->setDomain(TFID::TimeDomain);
        }
        break;

      case KeepDomain:
        break;
    }


    return true;

}

