#include "indirectTimeAcquire.h"
#include <math.h>

#include <QDebug>
#include <QMessageBox>



KIndirectTimeAcquire::KIndirectTimeAcquire()
{
    //qDebug() << "KSlowFT constructor";
    setAxisDomain(fidDomain::ToggleDomain);
}

KIndirectTimeAcquire::~KIndirectTimeAcquire()
{
    ;
}

QStringList KIndirectTimeAcquire::processInformation() {return QStringList() << "process=indirectTimeAcquire";}
QString KIndirectTimeAcquire::command() {return "indirectTimeAcquire";}


bool KIndirectTimeAcquire::process(TFID_2D *fid_2d, QStringList *sl, int *timeType)
{
    bool r;
    r = indirectTimeAcquireProcess(fid_2d, sl, timeType);
    return r;
}


bool KIndirectTimeAcquire::indirectTimeAcquireProcess(TFID_2D *fid_2d, QStringList *sl, int *timeType)
{
  int i,n,k,t;

  n = sl->size();
  t = 0;

  qDebug() << timeType;

  fid_2d->NUS = true;

  //-------error-------
  if(n>fid_2d->FID.size())
  {
      //QMessageBox::warning(this,"error!","Number of time data is inappropriate.",QMessageBox::Ok);
      return false;
  }

  //-------convert to integer or double-------
  if(t==0)
  {
      for(i=0;i<n;i++)
      {
          k = sl->at(i).toDouble();
          fid_2d->FID.at(i)->indirectTime = k;
      }
  }
  /*
  else
  {
      double d;
      for(i=0;i<n;i++)
      {
          d = sl->at(i).toDouble();
          fid_2d->FID.at(i)->indirectTime = d;
      }
  }
  */
  return true;
}
