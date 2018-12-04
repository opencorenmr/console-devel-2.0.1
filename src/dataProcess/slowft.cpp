#include "slowft.h"
#include <math.h>

#include <QDebug>


KSlowFT::KSlowFT()
{
    //qDebug() << "KSlowFT constructor";
    setAxisDomain(fidDomain::ToggleDomain);
}

KSlowFT::~KSlowFT()
{
    ;
}

QStringList KSlowFT::processInformation() {return QStringList() << "process=slowft";}
QString KSlowFT::command() {return "slowft";}


bool KSlowFT::process(TFID_2D *fid_2d, int k)
{
    return process(fid_2d->FID[k]);
}

bool KSlowFT::process(TFID_2D *fid_2d)
{
    bool r;
    for(int c=0; c<fid_2d->FID.size(); c++) r=process(fid_2d->FID[c]);
    return r;
}

bool KSlowFT::process(TFID *fid)
{
   bool res=slowFTProcess(fid);
   if(res==false) return res;
   else
   {
      fidDomain::process(fid,axisDomain());
      //qDebug()<<ad;
   }
   return res;

}

bool KSlowFT::slowFTProcess(TFID *fid)
{
    double theta,omega;
    double dw,er,ei,t;
    int i,j,n;

    n=fid->al();
    dw=fid->dw();

    double fr[n],fi[n],specr[n],speci[n];

    // theta=TWOPI/n;
    theta=6.28318530717958647692528676656/n;

    for(i=0; i<n; i++)
    {
      fr[i] = fid->real->sig.at(i);
      fi[i] = fid->imag->sig.at(i);

      specr[i] = 0;
      speci[i] = 0;
    }

    // ----  slowFT main (NUS=false)----

    if(fid->NUS==false)
    {

        for(i=0; i<n; i++)
        {
          omega = 1 /(dw * pow(10,-3)) - 2 * i/(dw * pow(10,-3) * (n - 1));

          for (j=0; j<n; j++)
          {
            t = j * dw;

            er = cos(theta * omega * t);
            ei = sin(theta * omega * t);

            specr[i] += fr[j] * er - fi[j] * ei;
            speci[i] += fr[j] * ei + fi[j] * er;
          }

        }
    }

    // ----  slowFT main (NUS=true)----

    else {

        for(i=0; i<n; i++)
        {
          //!check! NUS do not have dw.
          omega = 1 /(dw * pow(10,-3)) - 2 * i/(dw * pow(10,-3) * (n - 1));

          for (j=0; j<n; j++)
          {
            t = fid->nusTimes.at(j);

            er = cos(theta * omega * t);
            ei = sin(theta * omega * t);

            specr[i] += fr[j] * er - fi[j] * ei;
            speci[i] += fr[j] * ei + fi[j] * er;
          }

        }
    }


    // Data arrangement for NMR (left: positive freq., right: negative freq.)
    for(i=0; i<n; i++)
    {
      fid->real->sig[n-1-i]=specr[i] / n;
      fid->imag->sig[n-1-i]=speci[i] / n;
    }

    fid->updateAbs();

   // if(fid->domain==TFID::TimeDomain) {fid->domain=TFID::FrequencyDomain;}
   // else if(fid->domain==TFID::FrequencyDomain) {fid->domain=TFID::TimeDomain;}

    return true;
}
