#include "fft.h"
#include <math.h>

#include <QDebug>


TFFT::TFFT()
{
    //qDebug() << "TFFT constructor";
    setAxisDomain(fidDomain::ToggleDomain);
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

bool TFFT::process(TFID_2D *fid_2d, int k)
{
    return process(fid_2d->FID[k]);
}

bool TFFT::process(TFID_2D *fid_2d)
{
    bool r;
    for(int c=0; c<fid_2d->FID.size(); c++) r=process(fid_2d->FID[c]);
    return r;
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
    if(res==false) return res;
    else
    {
      fidDomain::process(fid,axisDomain());
      //qDebug()<<ad;
    }
    return res;
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
      warningMessage = "Automatic zero-fill to "
                       + QString::number(fid->al())
                       + " point has been performed.";
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

   // if(fid->domain==TFID::TimeDomain) {fid->domain=TFID::FrequencyDomain;}
   // else if(fid->domain==TFID::FrequencyDomain) {fid->domain=TFID::TimeDomain;}

    return true;
}
