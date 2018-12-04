#include "slowift.h"
#include <math.h>

#include <QDebug>


KSlowIFT::KSlowIFT()
{
    //qDebug() << "KSlowFT constructor";
    setAxisDomain(fidDomain::ToggleDomain);
}

KSlowIFT::~KSlowIFT()
{
    ;
}

QStringList KSlowIFT::processInformation() {return QStringList() << "process=slowift";}
QString KSlowIFT::command() {return "slowift";}


bool KSlowIFT::process(TFID_2D *fid_2d, int k)
{
    return process(fid_2d->FID[k]);
}

bool KSlowIFT::process(TFID_2D *fid_2d)
{
    bool r;
    for(int c=0; c<fid_2d->FID.size(); c++) r=process(fid_2d->FID[c]);
    return r;
}

bool KSlowIFT::process(TFID *fid)
{
   bool res=slowIFTProcess(fid);
   if(res==false) return res;
   else
   {
      fidDomain::process(fid,axisDomain());
      //qDebug()<<ad;
   }
   return res;

}

bool KSlowIFT::slowIFTProcess(TFID *fid)
{
    double theta,omega;
    double dw,er,ei,t;
    int i,j,n;

    n=fid->al();
    dw=fid->dw();

    double ifr[n],ifi[n],ispecr[n],ispeci[n];

    // theta=TWOPI/n;
    theta=6.28318530717958647692528676656/n;
    // ----  KSlowFT main ----

    for(i=0; i<n; i++)
    {
      ispecr[i] = fid->real->sig.at(i);
      ispeci[i] = fid->imag->sig.at(i);

      ifr[i] = 0;
      ifi[i] = 0;

    }


    for(i=0; i<n; i++)
    {

      t = i * dw;

      for (j=0; j<n; j++)
      {

        omega = 1 /(dw * pow(10,-3)) - 2 * j/(dw * pow(10,-3) * (n - 1));

        er = cos(theta * omega * t);
        ei = sin(theta * omega * t);

        ifr[i] += ispecr[j] * er + ispeci[j] * ei;
        ifi[i] += ispeci[j] * er - ispecr[j] * ei;
      }

    }


    // Data arrangement for NMR (left: positive freq., right: negative freq.)
    for(i=0; i<n; i++)
    {
      fid->real->sig[n-1-i]=ifr[i];
      fid->imag->sig[n-1-i]=ifi[i];
    }

    fid->updateAbs();

   // if(fid->domain==TFID::TimeDomain) {fid->domain=TFID::FrequencyDomain;}
   // else if(fid->domain==TFID::FrequencyDomain) {fid->domain=TFID::TimeDomain;}

    return true;
}
