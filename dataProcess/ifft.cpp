#include "ifft.h"
#include <math.h>

#include <QDebug>


TIFFT::TIFFT()
{
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

bool TIFFT::process(TFID_2D *fid_2d, int k)
{
    return process(fid_2d->FID[k]);
}

bool TIFFT::process(TFID_2D *fid_2d)
{
    bool r;
    for(int c=0; c<fid_2d->FID.size(); c++) r=process(fid_2d->FID[c]);
    return r;
}

bool TIFFT::process(TFID *fid)
{

    bool res=iFFTProcess(fid);
    if(res==false) return res;
    else
    {
       fidDomain::process(fid,axisDomain());
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
      warningMessage = "Automatic zero-fill to "
                       + QString::number(fid->al())
                       + " point has been performed.";
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
