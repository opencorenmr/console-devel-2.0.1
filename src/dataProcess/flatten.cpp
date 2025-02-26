#include "flatten.h"
#include <cmath>

TFlatten::TFlatten()
{
  setProcessType(TProcessType::Flatten);
}
TFlatten::~TFlatten()
{
    ;
}
QStringList TFlatten::processInformation()
{
    return QStringList() << "Flatten";
}
QString TFlatten::command()
{
    return "Flatten";
}

bool TFlatten::process(TFID_2D *fid_2d)
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

    for(int k=1; k<fid_2d->FID.size(); k++)
    {
        for(int i=0; i<fid_2d->FID.at(k)->al(); i++)
        {
            int re=fid_2d->FID.at(k)->real->sig.at(i);
            int im=fid_2d->FID.at(k)->imag->sig.at(i);
            fid_2d->FID[0]->real->sig.append(re);
            fid_2d->FID[0]->imag->sig.append(im);
            fid_2d->FID[0]->abs->sig.append(sqrt(re*re+im*im));
        }
        fid_2d->FID[0]->setAL(fid_2d->FID.at(0)->al()+fid_2d->FID.at(k)->al());
    }
    // we call setAL in order to let TFID update its private member FAL.
    fid_2d->FID[0]->setAL(fid_2d->FID[0]->real->sig.size());

    fid_2d->setDefaultAL(fid_2d->FID.at(0)->al());

    while(fid_2d->FID.size()>1) fid_2d->FID.removeLast();
   // fid_2d->FID.first()->updateAbs();
   // it has been commented out,
   // because we have already updated abs in the above loop.

    return true;

}
