#include "timeAcquire.h"
#include <math.h>

#include <QDebug>
#include <QMessageBox>

KTimeAcquire::KTimeAcquire()
{
    //qDebug() << "KSlowFT constructor";
    setAxisDomain(fidDomain::ToggleDomain);
}

KTimeAcquire::~KTimeAcquire()
{
    ;
}

QStringList KTimeAcquire::processInformation() {return QStringList() << "process=TimeAcquire";}
QString KTimeAcquire::command() {return "TimeAcquire";}


bool KTimeAcquire::process(TFID_2D *fid_2d, QStringList *sl)
{
    bool r;
    for(int c=0; c<fid_2d->FID.size(); c++) {r=timeAcquireProcess(fid_2d->FID[c], sl);}
    return r;
}


bool KTimeAcquire::timeAcquireProcess(TFID *fid, QStringList *sl)
{
  int i,n,k,t;

  n = sl->size();
  t = 0;

  //-------error-------
  if(n>fid->al())
  {
      //QMessageBox::warning(this,"error!","Number of time data is inappropriate.",QMessageBox::Ok);
      return false;
  }

  fid->nusTimes.resize(n);
  fid->NUS = true;

  //-------convert to double-------
  if(t==0)
  {
      for(i=0;i<n;i++)
      {
          k = sl->at(i).toDouble();
          fid->nusTimes[i] = k;
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
