#include "flatten.h"

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

 //   int al0=fid_2d->FID.at(0)->al();
 //   for(int k=1; k<fid_2d->FID.size(); k++)
 //   {
 //       if(al0!=fid_2d->FID.at(k)->al())
 //       {
 //           errorQ=true;
 //           setErrorMessage(QString(Q_FUNC_INFO)+": Data length must be the same for all arrayed data.");
 //           return false;
 //       }
 //   }

    for(int k=1; k<fid_2d->FID.size(); k++)
    {
        for(int i=0; i<fid_2d->FID.at(k)->al(); i++)
        {
            fid_2d->FID[0]->real->sig.append(
                        fid_2d->FID.at(k)->real->sig.at(i));
            fid_2d->FID[0]->imag->sig.append(
                        fid_2d->FID.at(k)->imag->sig.at(i));
        }
        fid_2d->FID[0]->setAL(fid_2d->FID.at(0)->al()+fid_2d->FID.at(k)->al());
    }

    fid_2d->setAl(fid_2d->FID.at(0)->al());

    while(fid_2d->FID.size()>1) fid_2d->FID.removeLast();
    fid_2d->FID.first()->updateAbs();

    return true;




}
