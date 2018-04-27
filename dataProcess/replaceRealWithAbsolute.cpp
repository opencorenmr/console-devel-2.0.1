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

    return true;
}

bool TReplaceRealWithAbsolute::process(TFID_2D *fid_2d)
{

    return true;
}
