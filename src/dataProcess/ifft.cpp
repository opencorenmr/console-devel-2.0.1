#include "ifft.h"
#include <math.h>

#include <QDebug>


TIFFT::TIFFT()
{
    setProcessType(TProcessType::IFFT);
    setAxisDomain(fidDomain::ToggleDomain);
    setLaplace(false);  // By default, we do not Laplace transformation.
    setLaplaceWidth(0.0);

}

QStringList TIFFT::processInformation()
{
  if(!Laplace())
  {
    return QStringList() << "process=Inversion of FFT"
                         << "AxisDomain="+QString::number(axisDomain());
  }
  else
  {
    return QStringList() << "process=Inversion of Laplace transformation"
                         << "width=" + QString::number(LaplaceWidth())
                         << "AxisDomain="+QString::number(axisDomain());
  }
}

QString TIFFT::command()
{
    return "ifft";
}


//bool TIFFT::process(TFID_2D *fid_2d)
//{
//    bool r;
//    for(int c=0; c<fid_2d->FID.size(); c++) r=process(fid_2d->FID[c]);
//    return r;
//}

bool TIFFT::process(TFID_2D *fid_2d)
{
    errorQ=false;
    switch(applyMode())
    {
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
          errorQ=false;
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
          errorQ=false;
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
    default:
        errorQ=true;
        setErrorMessage(QString(Q_FUNC_INFO) + ": Invalid operation.");
        break;
    } // switch
    return !errorQ;

}

bool TIFFT::process(TFID_2D *fid_2d, int k)
{
    return process(fid_2d->FID[k]);
}

bool TIFFT::process(TFID *fid)
{
  //  qDebug() << QString(Q_FUNC_INFO) << "0";
    bool res=iFFTProcess(fid);
  //  qDebug() << QString(Q_FUNC_INFO) << "1" << res;
    if(res==false) return res;
    else
    {
       processDomain(fid);
    }

    // In the case of Laplace transformation,
    // we apply exponential "GROW" in advance.
    // GROW -> Note the absense of the minus sign in the following "exp" command.
      if(Laplace())
      {
        for(int k=0; k<fid->al(); k++)
        {
          double w=exp(k*(fid->dw()/1000000.0)*3.141592*LaplaceWidth());
          fid->real->sig[k] *= w;
          fid->imag->sig[k] *= w;
        }  // for(int k=0; l<fid->al(); k++)
        fid->updateAbs();
      }



    return res;

  //  return iFFTProcess(fid);

}



void TIFFT::processDomain(TFID *fid)
{

    switch(axisDomain())
    {
      case SetFrequency:
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

      case SetTime:
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

}




bool TIFFT::iFFTProcess(TFID *fid)
{
    double theta;
    double yr,yi,xr,xi,wr,wi;
    int i,j,k,m,mh,irev,n,nh;

    // We get such n that 2^n=AL
    n=1;
    while(n<fid->al()) n = n << 1;

    // We zero-fill, if necessary
    if(fid->al()<n)
    {
      k=fid->al();
      fid->setAL(n);
      for(int m=k; m<fid->al(); m++)
      {
          fid->real->sig[m]=0.0;
          fid->imag->sig[m]=0.0;
          fid->abs->sig[m]=0.0;
      }
      warningQ=true;
      setWarningMessage("Automatic zero-fill to "
                       + QString::number(fid->al())
                       + " point has been performed.");
    }

    theta=TWOPI/n;

    // ----  IFFT main ----

    // STEP 1: Data arrangement for NMR
    nh = n/2;
    for(k=0; k<nh; k++)
    {
        xr = fid->real->sig.at(k);
        xi = fid->imag->sig.at(k);
        fid->real->sig[k] = fid->real->sig.at(nh+k);
        fid->imag->sig[k] = fid->imag->sig.at(nh+k);
        fid->real->sig[nh+k] = xr;
        fid->imag->sig[nh+k] = xi;
    }

    // STEP 2
    m = n;
    mh = m >> 1;
    while(m>=2)
    {
        irev = 0;
        i = 0;
        while(i<n)
        {
            wr = cos(theta * irev);
            wi = sin(theta * irev);
            k = n >> 2;
            irev = irev ^ k;   // irev xor k
            while(k>irev)
            {
                k = k >> 1;
                irev = irev ^ k;
            }
            for(j=i; j<mh+i; j++)
            {
                k = j + mh;
                xr = wr * fid->real->sig.at(k) + wi * fid->imag->sig.at(k);
                xi = wr * fid->imag->sig.at(k) - wi * fid->real->sig.at(k);
                yr = fid->real->sig.at(j);
                yi = fid->imag->sig.at(j);
                fid->real->sig[j] = yr + xr;
                fid->imag->sig[j] = yi + xi;
                fid->real->sig[k] = yr - xr;
                fid->imag->sig[k] = yi - xi;
            }
            i=i+m;
        } // while(i<n)
        m = mh;
        mh = m >> 1;
    } // while(m>=2)

    // STEP 3
    i=0;
    for(j=1; j<n-1; j++)
    {
      k = n >> 1;
      i = i ^ k;   // i xor k
      while(k>i)
      {
        k = k >> 1;
        i = i ^ k;
      }
      if(j<i)
      {
        xr = fid->real->sig.at(j);
        xi = fid->imag->sig.at(j);
        fid->real->sig[j] = fid->real->sig.at(i);
        fid->imag->sig[j] = fid->imag->sig.at(i);
        fid->real->sig[i] = xr;
        fid->imag->sig[i] = xi;
      }
    }

    for(k=0; k<fid->al(); k++)
    {
        fid->real->sig[k] = fid->real->sig[k];
        fid->imag->sig[k] = fid->imag->sig[k];
    }
    fid->updateAbs();

    //if(fid->domain==TFID::TimeDomain) {fid->domain=TFID::FrequencyDomain;}
    //else if(fid->domain==TFID::FrequencyDomain) {fid->domain=TFID::TimeDomain;}

    return true;
}
