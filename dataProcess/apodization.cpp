#include "apodization.h"
#include "math.h"

TApodization::TApodization()
{
  setApodizationType(Gaussian); // default type
  setInverse(false);
}


TApodization::~TApodization()
{

}

QString TApodization::command()
{
  return "apodization";
}

QStringList TApodization::processInformation()
{
    return QStringList() << "process=apodizaition";
}

bool TApodization::process(TFID_2D *fid_2d)
{
    bool r;
    for(int c=0; c<fid_2d->FID.size(); c++)
    {
        r=process(fid_2d->FID[c]);
    }
    return r;
}

bool TApodization::process(TFID_2D *fid_2d, int k)
{
    if(k<0 || k>fid_2d->FID.size()-1)
    {
        errorQ=true;
        errorMessage=QString(Q_FUNC_INFO)+": index is out of range.";
        return false;
    }
    else return process(fid_2d->FID[k]);

}

bool TApodization::process(TFID *fid)
{
    if(width()<0)
    {
        errorQ=true;
        errorMessage="Width cannot be negative.";
        return false;
    }
    switch(apodizationType())
    {
       case Gaussian:
         for(int k=0; k<fid->al(); k++)
         {
           double w=exp(-k*k*(fid->dw()/1000000.0)*(fid->dw()/1000000.0)*width()*width()*3.55971);
                                    // (2pi) ^2/(16 ln2) = 3.55971
           if(inverse()==true) w = 1.0/w;
           fid->real->sig[k] *= w;
           fid->imag->sig[k] *= w;
         }  // for(int k=0; l<fid->al(); k++)
         fid->updateAbs();
         break;

       case Exponential:
         for(int k=0; k<fid->al(); k++)
         {
           double w=exp(-k*(fid->dw()/1000000.0)*3.141592*width());
           if(inverse()==true) w = 1.0/w;
           fid->real->sig[k] *= w;
           fid->imag->sig[k] *= w;
         }  // for(int k=0; l<fid->al(); k++)
         fid->updateAbs();

        break;
       case Sinc:
        if(inverse()==true)
        {
            errorQ=true;
            errorMessage="Inverse operation is not allowed for sinc apodization";
            return false;
        }
        for(int k=1; k<fid->al(); k++) // For sinc, we start from k=1 (second point!)
        {
          double w=sin(k*(fid->dw()/1000000.0)*3.141592*width())/
                    (k*(fid->dw()/1000000.0)*3.141592*width());
          fid->real->sig[k] *= w;
          fid->imag->sig[k] *= w;
        }  // for(int k=0; l<fid->al(); k++)
        fid->updateAbs();
        break;
       default:
        errorQ=true;
        errorMessage="Undefined apodization type.";
        return false;
    } // switch

  return true;
}
