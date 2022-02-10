#include "fft.h"
#include <math.h>

#include <QDebug>


TFFT::TFFT()
{
    setProcessType(TProcessType::FFT);
    //qDebug() << "TFFT constructor";
    setAxisDomain(ToggleDomain);
    setLaplace(false);
    setLaplaceWidth(0.0);
}

TFFT::~TFFT()
{
    ;
}


QStringList TFFT::processInformation()
{
    if(!Laplace())
    {
      return QStringList() << "process=FFT"
                           << "AxisDomain="+QString::number(axisDomain());

    }
    else
    {
      return QStringList() << "process=Laplace transformation"
                           << "width=" + QString::number(LaplaceWidth())
                           << "AxisDomain="+QString::number(axisDomain());

    }

}
QString TFFT::command()
{
      return "fft";
}


bool TFFT::process(TFID_2D *fid_2d)
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

bool TFFT::process(TFID_2D *fid_2d, int k)
{
    return process(fid_2d->FID[k]);
}

bool TFFT::process(TFID *fid)
{
  // In the case of Laplace transformation,
  // we apply exponential decay in advance.
    if(Laplace())
    {
      for(int k=0; k<fid->al(); k++)
      {
        double w=exp(-k*(fid->dw()/1000000.0)*3.141592*LaplaceWidth());
        fid->real->sig[k] *= w;
        fid->imag->sig[k] *= w;
      }  // for(int k=0; l<fid->al(); k++)
      fid->updateAbs();
    }

    bool res=FFTProcess(fid);
    if(res==false)
    {
        return res;
    }
    else
    {
      processDomain(fid);
    }
    return res;
}


void TFFT::processDomain(TFID *fid)
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

bool TFFT::FFTProcess(TFID *fid)
{
    double theta;
    double xr,xi,wr,wi;
    int i,j,k,mh,irev,m,n,nh;

    // We get such n that 2^n=AL
    n=1;
    while(n<fid->al()) n = n << 1;  // qDebug() << "n: " << n;

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

    // theta=TWOPI/n;
    theta=6.28318530717958647692528676656/n;
    // ----  FFT main ----

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
    } // j

    mh=1;
    m = mh << 1;
    while(m<=n)
    {
      irev=0;
      i=0;
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
          xr = fid->real->sig.at(j) - fid->real->sig.at(k);
          xi = fid->imag->sig.at(j) - fid->imag->sig.at(k);

          fid->real->sig[j] += fid->real->sig.at(k);
          fid->imag->sig[j] += fid->imag->sig.at(k);

          fid->real->sig[k] = wr*xr - wi*xi;
          fid->imag->sig[k] = wr*xi + wi*xr;
        }  // j

        i=i+m;

      } // while(i<n)

      mh = m;
      m = mh << 1;
    } // while(m<=n)

    // Data arrangement for NMR (left: positive freq., right: negative freq.)
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

    for(k=0; k<fid->al(); k++)
    {
        fid->real->sig[k] = fid->real->sig[k]/fid->al();
        fid->imag->sig[k] = fid->imag->sig[k]/fid->al();
    }
    fid->updateAbs();

    return true;
}


TFFT3D::TFFT3D()
{
  TFFT3D(4,4);
}

TFFT3D::TFFT3D(int n1, int n2)
{
    setProcessType(TProcessType::FFT3D);
    setApplyMode(0);
    FN1=n1;
    FN2=n2;
}

TFFT3D::~TFFT3D()
{
    ;
}

QStringList TFFT3D::processInformation()
{
    return QStringList() << "process=3D FFT";
}

QString TFFT3D::command() { return "fft3d";}

bool TFFT3D::process(TFID_2D *fid_2d)
{
    bool ok=false;
    int al=fid_2d->FID[0]->al();
    int n1n2=fid_2d->FID.size();
    if(n1n2!=n1()*n2())
    {
        setErrorMessage("length does not match.");
        return false;
    }

    TFFT *FFT = new TFFT;
    TFID *helpFID = new TFID(4);

    ok=FFT->process(fid_2d);
    if(!ok)
    {
        setErrorMessage(FFT->errorMessage());
    }
    else
    {
      helpFID->setAL(n1());
      for(int c3=0; c3<al; c3++)
      {
         for(int c2=0; c2<n2(); c2++)
         {
            for(int c1=0; c1<n1(); c1++)
            {
                helpFID->real->sig[c1]=fid_2d->FID.at(c1*n1()+c2)->real->sig.at(c3);
                helpFID->imag->sig[c1]=fid_2d->FID.at(c1*n1()+c2)->imag->sig.at(c3);
            }
            ok=FFT->process(helpFID);
            if(!ok) {setErrorMessage(FFT->errorMessage()); delete helpFID; delete FFT; return false;}
            for(int c1=0; c1<n1(); c1++)
            {
                fid_2d->FID[c1*n1()+c2]->real->sig[c3]=helpFID->real->sig.at(c1);
                fid_2d->FID[c1*n1()+c2]->imag->sig[c3]=helpFID->imag->sig.at(c1);
            }
         } // c2
      } // c3

      helpFID->setAL(n2());
      for(int c3=0; c3<al; c3++)
      {
         for(int c1=0; c1<n1(); c1++)
         {
            for(int c2=0; c2<n2(); c2++)
            {
                helpFID->real->sig[c2]=fid_2d->FID.at(c1*n1()+c2)->real->sig.at(c3);
                helpFID->imag->sig[c2]=fid_2d->FID.at(c1*n1()+c2)->imag->sig.at(c3);
            }
            ok=FFT->process(helpFID);
            if(!ok) {setErrorMessage(FFT->errorMessage()); delete helpFID; delete FFT; return false;}
            for(int c2=0; c2<n2(); c2++)
            {
                fid_2d->FID[c1*n1()+c2]->real->sig[c3]=helpFID->real->sig.at(c2);
                fid_2d->FID[c1*n1()+c2]->imag->sig[c3]=helpFID->imag->sig.at(c2);
            }
         } // c2
      } // c3

    }

    delete helpFID;
    delete FFT;

    return ok;
}
