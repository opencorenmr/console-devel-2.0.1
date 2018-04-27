#include "covariance.h"
#include <math.h>

#include <QDebug>


KCovariance::KCovariance()
{
    setAxisDomain(fidDomain::ToggleDomain);
}

KCovariance::~KCovariance()
{
    ;
}

QStringList KCovariance::processInformation() {return QStringList() << "process=covariance";}
QString KCovariance::command() {return "covariance";}

bool KCovariance::process(TFID_2D *fid_2d)
{
    bool r;
    r= covarianceProcess(fid_2d);
    return r;
}

bool KCovariance::covarianceProcess(TFID_2D *fid_2d)
{
  double real1, real2, imag1, imag2, re=0.0, im=0.0;
  int i, j, k, n_al, n_ind;

  n_al = fid_2d->al(); //number of points DIRECT FID
  n_ind = fid_2d->FID.size(); //number of INDIRECT FID

  double avr[n_al], avi[n_al];

  for(i=0;i<n_al;i++)
  {
      avr[i] = 0.0;
      avi[i] = 0.0;
  }

  // -----save original FID------

    TFID_2D *originalFID2D = new TFID_2D;
    originalFID2D->FID.clear();

    for(k=0; k<n_ind; k++)
    {
        originalFID2D->FID.append(new TFID(n_al));
        for(i=0; i<n_al; i++)
        {
            originalFID2D->FID[k]->real->sig[i] = fid_2d->FID.at(k)->real->sig.at(i);
            originalFID2D->FID[k]->imag->sig[i] = fid_2d->FID.at(k)->imag->sig.at(i);
        }
    }

   // clock_t start, end;


    qDebug() << "imagKeep" << imagKeep();

    qDebug() << "Inner-Product" << innerProduct();

    qDebug() << "copied" << originalFID2D->FID.size();

   // start = clock();

    // -----Covariance main -----

    // ----- expand fid to n_al*n_al matrix
    if(n_ind<n_al)
    {
      for(i=0;i<n_al-n_ind;i++)
      {
          fid_2d->FID.append(new TFID(n_al));
          fid_2d->FID.last()->setEmpty(false);
          fid_2d->FID.last()->domain=fid_2d->FID.first()->domain;
          fid_2d->FID.last()->setXAxisLabel(fid_2d->FID.first()->xAxisLabel());
          fid_2d->FID.last()->setXAxisUnitSymbol(fid_2d->FID.first()->xAxisUnitSymbol());
          fid_2d->FID.last()->setDW(fid_2d->FID.first()->dw());
          fid_2d->FID.last()->setDx(fid_2d->FID.first()->dx());
          fid_2d->FID.last()->plotMetricPrefix.setPrefix(fid_2d->FID.first()->plotMetricPrefix.prefix());
          fid_2d->FID.last()->setXInitialValue(fid_2d->FID.first()->xInitialValue());
      }
    }

    for(k=0; k<n_al; k++)
    {
        for(i=0; i<n_al; i++)
        {
            fid_2d->FID[k]->real->sig[i] = 0.0;
            fid_2d->FID[k]->imag->sig[i] = 0.0;
        }
    }

    qDebug() << fid_2d->al() << fid_2d->FID.size();

    // -----calculate average-----

    if(!innerProduct()) //if (innerProduct=true) then skip this part
    {
        for(i=0;i<n_al;i++)
        {
            for(k=0;k<n_ind;k++)
            {
                avr[i] += originalFID2D->FID.at(k)->real->sig.at(i);
                avi[i] += originalFID2D->FID.at(k)->imag->sig.at(i);
            }

            avr[i] /= n_ind;
            avi[i] /= n_ind;
        }
    } // until here

    qDebug() << "average calculated";

    // -----calculate covariance-----

    for(i=0;i<n_al;i++)
    {
        for(j=0;j<=i;j++)
        {
            for(k=0;k<n_ind;k++)
            {
                real1 = originalFID2D->FID.at(k)->real->sig.at(i) - avr[i];
                imag1 = originalFID2D->FID.at(k)->imag->sig.at(i) - avi[i];
                real2 = originalFID2D->FID.at(k)->real->sig.at(j) - avr[j];
                imag2 = originalFID2D->FID.at(k)->imag->sig.at(j) - avi[j];
                re +=  real1*real2 + imag1*imag2;
                if(imagKeep()){im += -real1*imag2 + imag1*real2;}
            }

            fid_2d->FID[i]->real->sig[j] = re / n_ind;
            if(imagKeep()){fid_2d->FID[i]->imag->sig[j] = im / n_ind;}

            if(j<i)
            {
                fid_2d->FID[j]->real->sig[i] =  re / n_ind;
                if(imagKeep()){fid_2d->FID[j]->imag->sig[i] = -im / n_ind;}
            }

            re = 0.0;
            if(imagKeep()){im = 0.0;}
        }

        qDebug() << "processing..." << i+1 << "/" << n_al ;//<< "value" << fid_2d->FID[i]->real->sig.at(1000);
    }

    qDebug() << "covariance calculated";

    for(i=0;i<n_al;i++)
    {
      fid_2d->FID[i]->updateAbs();
    }

 //   end = clock();

    //qDebug() << "calculation time is" << end - start;

  return true;

}
