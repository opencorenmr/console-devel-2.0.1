#include "replaceRealWithAbsolute.h"

TReplaceRealWithAbsolute::TReplaceRealWithAbsolute()
{

}

bool TReplaceRealWithAbsolute::process(TFID *fid)
{
    fid->updateAbs();
    for(int k=0; k<fid->al(); k++)
    {
        fid->real->sig[k]=fid->abs->sig[k];
        fid->imag->sig[k]=0;
    }
    return true;
}

bool TReplaceRealWithAbsolute::process(TFID_2D *fid_2d, int k)
{
    return process(fid_2d->FID[k]);
}

bool TReplaceRealWithAbsolute::process(TFID_2D *fid_2d)
{
    bool ok=true;
    for(int k=0; k<fid_2d->FID.size(); k++) ok=process(fid_2d,k);
    return ok;
}
