#include "hyperComplex.h"

THyperComplex::THyperComplex()
{
    setProcessType(TProcessType::HyperComplexCompression);
    FPhaseReverse=false;
}

bool THyperComplex::process(TFID_2D *fid_2d)
{
    int nRow=fid_2d->FID.size();  //qDebug() << QString(Q_FUNC_INFO) << "nRow: " <<nRow;
    if(nRow < 2 || nRow % 2 > 0)
    {
        errorQ=true;
        setErrorMessage("Length of the arrayed data must be even and larger than 2.");
        return false;
    }

    int nCol=fid_2d->FID.at(0)->al();  //qDebug() << QString(Q_FUNC_INFO) << "nCol: " <<nCol;

    int sgn;
    if(phaseReverse()) {sgn=-1;} else {sgn=1;}

    for(int k=0; k<nRow; k+=2)
    {
        for(int i=0; i<nCol; i++)
        {
            fid_2d->FID[k]->imag->sig[i] = sgn * fid_2d->FID[k+1]->real->sig.at(i);
           //if(!phaseReverse())
           // {
           //     fid_2d->FID[k]->imag->sig[i]=fid_2d->FID[k+1]->real->sig.at(i);
           // }
           // else // phaseReverse() == true
           // {
           //     fid_2d->FID[k]->imag->sig[i]=fid_2d->FID[k]->real->sig.at(i);
           //     fid_2d->FID[k]->real->sig[i]=fid_2d->FID[k+1]->real->sig.at(i);
           // }
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
    return QStringList() << "process=HyperComplexCompression";
}

QString THyperComplex::command()
{
    return "HyperComplexCompression";
}
