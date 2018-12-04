#include "hyperComplex.h"

THyperComplex::THyperComplex()
{
  setPhaseReverse(false);
}

bool THyperComplex::process(TFID_2D *fid_2d)
{
    int nRow=fid_2d->FID.size();  //qDebug() << QString(Q_FUNC_INFO) << "nRow: " <<nRow;
    if(nRow < 2 || nRow % 2 > 0)
    {
        errorQ=true;
        errorMessage="Length of the arrayed data must be even and larger than 2.";
        return false;
    }

    int nCol=fid_2d->FID.at(0)->al();  //qDebug() << QString(Q_FUNC_INFO) << "nCol: " <<nCol;

    for(int k=0; k<nRow; k+=2)
    {
        for(int i=0; i<nCol; i++)
        {
            if(!phaseReverse())
            {
                fid_2d->FID[k]->imag->sig[i]=fid_2d->FID[k+1]->real->sig.at(i);
            }
            else // phaseReverse() == true
            {
                fid_2d->FID[k]->imag->sig[i]=fid_2d->FID[k]->real->sig.at(i);
                fid_2d->FID[k]->real->sig[i]=fid_2d->FID[k+1]->real->sig.at(i);
            }
        }
    }

    for(int k=2; k<nRow; k+=2)
    {
      for(int i=0; i<nCol; i++)
      {
        fid_2d->FID[k/2]->real->sig[i]=fid_2d->FID[k]->real->sig.at(i);
        fid_2d->FID[k/2]->imag->sig[i]=fid_2d->FID[k]->imag->sig.at(i);
      }
      fid_2d->FID[k/2]->updateAbs();
    }

    while(fid_2d->FID.size()>nRow/2) fid_2d->FID.removeLast();

    return true;
}

QStringList THyperComplex::processInformation()
{
    return QStringList() << "process=hypercomplex";
}

QString THyperComplex::command()
{
    return "hypercomplex";
}
