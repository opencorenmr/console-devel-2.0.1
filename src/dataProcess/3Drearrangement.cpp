#include "3Drearrangement.h"

STranslation::STranslation()
{

}

bool STranslation::process(TFID_2D *fid_2d, int size1, int size2)
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

    int s1s,s2s,ls;
    for(int s1=0;s1<size1;s1++){
        s1s = (s1+d1Displacement*(size1-1))%size1;
        for(int s2=0;s2<size2;s2++){
            s2s = (s2+d2Displacement*(size2-1))%size2;
            helpfid_2d->FID.append(new TFID(al));
            for(int l=0;l<al;l++){
                ls = (l+abscissaDisplacement*(al-1))%al;
                helpfid_2d->FID[s1*size2+s2]->real->sig[l] = fid_2d->FID.at(s1s*size2+s2s)->real->sig.at(ls);
                helpfid_2d->FID[s1*size2+s2]->imag->sig[l] = fid_2d->FID.at(s1s*size2+s2s)->imag->sig.at(ls);
            }
            helpfid_2d->FID.last()->updateAbs();
        }
    }

    for(int s=0;s<helpfid_2d->FID.size();s++){
        for(int l=0;l<helpfid_2d->FID.at(0)->al();l++){
            fid_2d->FID[s]->real->sig[l] = helpfid_2d->FID.at(s)->real->sig.at(l);
            fid_2d->FID[s]->imag->sig[l] = helpfid_2d->FID.at(s)->imag->sig.at(l);
        }
        fid_2d->FID[s]->updateAbs();
    }

    delete helpfid_2d;

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
