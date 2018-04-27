#include "slow2dft.h"
#include <math.h>

#include <QDebug>


KSlow2DFT::KSlow2DFT()
{
    //qDebug() << "KSlowFT constructor";
    setAxisDomain(fidDomain::ToggleDomain);
}

KSlow2DFT::~KSlow2DFT()
{
    ;
}

QStringList KSlow2DFT::processInformation() {return QStringList() << "process=slow2dft";}
QString KSlow2DFT::command() {return "slow2dft";}

/*
bool KSlow2DFT::process(TFID_2D *fid_2d, int k)
{
    return process(fid_2d->FID[k]);
}
*/  // commented out by Takeda (20170926)

bool KSlow2DFT::ftprocess(TFID_2D *fid_2d, double *init, double *inter, int *pts)
{
    bool r;
    r= slow2DFTProcess(fid_2d, init, inter, pts);  // Added by Takeda (20170926)
      //  for(int c=0; c<fid_2d->FID.size(); c++) r=process(fid_2d->FID[c]); // commented out by Takeda (20170926)
    return r;
}

/*
bool KSlow2DFT::process(TFID *fid)
{
   bool res=slow2DFTProcess(fid);
   if(res==false) return res;
   else
   {
      fidDomain::process(fid,axisDomain());
      //qDebug()<<ad;
   }
   return res;

}

*/   // commented out by Takeda (20170926)

bool KSlow2DFT::slow2DFTProcess(TFID_2D *fid_2d, double *init, double *inter, int *pts)
{
  double theta, omega;
  double dw, er, ei, t;
  int i, j, k, n_dir, n_ind, pt;

  n_dir = fid_2d->al(); //number of points DIRECT FID
  n_ind = fid_2d->FID.size(); //number of INDIRECT FID
  dw = fid_2d->dw(); // dw of *DIRECT* fid

  pt = *pts;

  double fr[n_ind],fi[n_ind];
  double specr[pt], speci[pt];

  // theta=TWOPI/n;
  theta=6.28318530717958647692528676656/n_ind;

  /*
  qDebug() << *init;
  qDebug() << *inter;
  qDebug() << *pts;
  */

  if(*pts<n_ind){return false;}

  // -----slow2DFT main -----
/*
    for(i=0; i<n_dir; i++)
    {
        for(j=0; j<n_ind; j++)
        {
          fr[j] = fid_2d->FID.at(j)->real->sig.at(i);
          fi[j] = fid_2d->FID.at(j)->imag->sig.at(i);
        }

        for(j=0; j<pt; j++)
        {
          specr[j] = 0.0;
          speci[j] = 0.0;
        }

        for(j=0; j<n_ind; j++)
        {
          omega = *init + double(j) * *inter;

          for (k=0; k<n_ind; k++)
          {

            t = fid_2d->FID.at(k)->indirectTime;

            er = cos(theta * omega * t);
            ei = sin(theta * omega * t);

            specr[j] += fr[k] * er - fi[k] * ei;
            speci[j] += fr[k] * ei + fi[k] * er;
          }

        }

        for(j=0; j<n_ind; j++)
        {
          fid_2d->FID[j]->real->sig[i] = specr[j];
          fid_2d->FID[j]->imag->sig[i] = speci[j];
        }
    }
*/
    TFID_2D *originalFID2D = new TFID_2D;
    originalFID2D->FID.clear();

    for(int k=0; k<n_ind; k++)
    {
        originalFID2D->FID.append(new TFID(n_dir));
        for(int i=0; i<n_dir; i++)
        {
            originalFID2D->FID[k]->real->sig[i] = fid_2d->FID.at(k)->real->sig.at(i);
            originalFID2D->FID[k]->imag->sig[i] = fid_2d->FID.at(k)->imag->sig.at(i);
        }
    }

    qDebug() << "copied" << originalFID2D->FID.size();

    if(n_ind<pt){
      for(i=0;i<pt-n_ind;i++){fid_2d->FID.append(new TFID(n_dir));}
    }

    qDebug() << "fid_2d->FID.size() = " << fid_2d->FID.size();

    for(i=0; i<n_dir; i++)
    {
        for(j=0; j<n_ind; j++)
        {
          fr[j] = originalFID2D->FID[j]->real->sig[i];
          fi[j] = originalFID2D->FID[j]->imag->sig[i];
        }

        //qDebug() << "0";

        for(j=0; j<pt; j++)
        {
          specr[j] = 0.0;
          speci[j] = 0.0;
        }

        //qDebug() << "1";

        for(j=0; j<pt; j++)
        {
          omega = *init + double(j) * *inter;

          for (k=0; k<n_ind; k++)
          {

            t = fid_2d->FID.at(k)->indirectTime;

            er = cos(theta * omega * t);
            ei = sin(theta * omega * t);

            specr[j] += fr[k] * er - fi[k] * ei;
            speci[j] += fr[k] * ei + fi[k] * er;
          }
        }

        //qDebug() << "2";

        for(j=0; j<pt; j++)
        {
          if(j<n_ind){
            fid_2d->FID[j]->real->sig[i] = specr[j];
            fid_2d->FID[j]->imag->sig[i] = speci[j];
            //qDebug() << "a" << j;
          } else {
            fid_2d->FID[j]->real->sig[i] = specr[j];
            fid_2d->FID[j]->imag->sig[i] = speci[j];
            //qDebug() << "b" << j;
          }
        }

        //qDebug() << "yoko" << i;
    }

    for(i=0;i<pt;i++)
    {
      fid_2d->FID[i]->updateAbs();
    }

  return true;
}
