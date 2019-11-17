#include "arraySum.h"

TArraySum::TArraySum()
{
  setProcessType(TProcessType::ArraySum);
}

TArraySum::~TArraySum()
{
    ;
}

QStringList TArraySum::processInformation()
{
    return QStringList() << "Summation over arrayed data";
}

QString TArraySum::command()
{
    return "array sum";
}

bool TArraySum::process(TFID_2D *fid_2d)
{
    if(fid_2d->FID.isEmpty())
    {
        errorQ=true;
        setErrorMessage(QString(Q_FUNC_INFO) + ": Data is empty.");
        return false;
    }
    if(fid_2d->FID.size()<2)
    {
        errorQ=true;
        setErrorMessage(QString(Q_FUNC_INFO) + ": array length must be larger than 2.");
        return false;
    }

    int al0=fid_2d->FID.at(0)->al();
    for(int k=1; k<fid_2d->FID.size(); k++)
    {
        if(al0!=fid_2d->FID.at(k)->al())
        {
            errorQ=true;
            setErrorMessage(QString(Q_FUNC_INFO)+": Data length must be the same for all arrayed data.");
            return false;
        }
    }

    while(fid_2d->FID.size()>1)
    {
        for(int i=0; i<al0; i++)
        {
            fid_2d->FID[0]->real->sig[i] += fid_2d->FID.last()->real->sig[i];
            fid_2d->FID[0]->imag->sig[i] += fid_2d->FID.last()->imag->sig[i];
        }
        fid_2d->FID.removeLast();
    }

    fid_2d->FID.first()->updateAbs();

    return true;

}
