#include "3Drearrangement.h"

SSetCenterattheOrigin::SSetCenterattheOrigin()
{

}

bool SSetCenterattheOrigin::process(TFID_2D *fid_2d, int size1, int size2)
{
    int al=fid_2d->FID[0]->al();
    int fidsize=fid_2d->FID.size();
    if(fidsize!=size1*size2)
    {
        setErrorMessage("length does not match.");
        return false;
    }
    if(al%2)
    {
        setErrorMessage("FID length is odd.");
        return false;
    }
    if(size1%2)
    {
        setErrorMessage("Size1 is odd.");
        return false;
    }
    if(size2%2)
    {
        setErrorMessage("Size2 is odd.");
        return false;
    }

    double tmp_r,tmp_i;
    int s1m,s2m,lm;
    for(int s1=0;s1<size1/2;s1++){
        s1m=s1+size1/2;
        for(int s2=0;s2<size2;s2++){
            s2m=(s2+size2/2)%size2;
            for(int l=0;l<al;l++){
                lm=(l+al/2)%al;
                tmp_r=fid_2d->FID.at(s1*size2+s2)->real->sig.at(l);
                tmp_i=fid_2d->FID.at(s1*size2+s2)->imag->sig.at(l);
                fid_2d->FID[s1*size2+s2]->real->sig[l]=fid_2d->FID.at(s1m*size2+s2m)->real->sig.at(lm);
                fid_2d->FID[s1*size2+s2]->imag->sig[l]=fid_2d->FID.at(s1m*size2+s2m)->imag->sig.at(lm);
                fid_2d->FID[s1m*size2+s2m]->real->sig[lm]=tmp_r;
                fid_2d->FID[s1m*size2+s2m]->imag->sig[lm]=tmp_i;
            }
            fid_2d->FID[s1*size2+s2]->updateAbs();
            fid_2d->FID[s1m*size2+s2m]->updateAbs();
        }
    }

    fid_2d->setCurrentFID(0);

    return true;
}

SxyzRotation::SxyzRotation()
{

}

bool SxyzRotation::process(TFID_2D *fid_2d, int size1, int size2)
{
    int al=fid_2d->FID[0]->al();
    int fidsize=fid_2d->FID.size();
    if(fidsize!=size1*size2)
    {
        setErrorMessage("length does not match.");
        return false;
    }

    TFID_2D *helpfid_2d = new TFID_2D;
    helpfid_2d->FID.clear();

    for(int s2=0;s2<size2;s2++){
        for(int l=0;l<al;l++){
            helpfid_2d->FID.append(new TFID(size1));
            for(int s1=0;s1<size1;s1++){
                helpfid_2d->FID[s2*al+l]->real->sig[s1] = fid_2d->FID.at(s1*size2+s2)->real->sig.at(l);
                helpfid_2d->FID[s2*al+l]->imag->sig[s1] = fid_2d->FID.at(s1*size2+s2)->imag->sig.at(l);
            }
            helpfid_2d->FID.last()->updateAbs();
        }
    }

    fid_2d->FID.clear();
    for(int i=0;i<helpfid_2d->FID.length();i++){
        fid_2d->FID.append(new TFID(size1));
        for(int j=0;j<size1;j++){
            fid_2d->FID[i]->real->sig[j]=helpfid_2d->FID.at(i)->real->sig.at(j);
            fid_2d->FID[i]->imag->sig[j]=helpfid_2d->FID.at(i)->imag->sig.at(j);
        }
        fid_2d->FID[i]->updateAbs();
        fid_2d->FID.last()->setEmpty(false);
    }

    fid_2d->setCurrentFID(0);

    delete helpfid_2d;

    return true;
}
