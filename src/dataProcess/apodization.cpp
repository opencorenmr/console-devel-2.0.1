#include "apodization.h"
#include "math.h"

TApodization::TApodization()
{
  setProcessType(TProcessType::Apodization);
  setApodizationType(Gaussian); // default type
  setWidth(0);
  setInverse(false);
}


TApodization::~TApodization()
{

}

QString TApodization::apodizationType_str()
{
    QString qs;
    switch(apodizationType())
    {
      case Gaussian:
        qs="gaussian";
        break;
    case Exponential:
      qs="exponential";
      break;
    case Sinc:
      qs="sinc";
      break;
    default:
      qs="undefined";
      break;
    }

    return qs;
}

QString TApodization::command()
{
    QString qs;
    qs="apodization: ";
    qs += apodizationType_str();
    if(inverse()) {qs += " (inv)";}
    qs += " ";
    qs += QString::number(width());
    return qs;
}

QStringList TApodization::processInformation()
{
    return QStringList() << "process=apodizaition";
}

bool TApodization::process(TFID_2D *fid_2d)
{
    errorQ=false;
    switch(applyMode())
    {
      default:
      case ApplyToAll:

        for(int c=0; c<fid_2d->FID.size(); c++)
        {
          errorQ=!process(fid_2d->FID[c]);
          if(errorQ) break;
        }
        break;
      case ApplyToOne:
        if(applyIndex()<0 || applyIndex()>fid_2d->FID.size()-1)
        {
          errorQ=true;
          setErrorMessage(QString(Q_FUNC_INFO) + ": Index out of range.");
        }
        else
        {
          errorQ=!process(fid_2d->FID[applyIndex()]);
        }
        break;
      case ApplyToOthers:

        if(applyIndex()<0 || applyIndex()>fid_2d->FID.size()-1)
        {
          errorQ=true;
          setErrorMessage(QString(Q_FUNC_INFO) + ": Index out of range.");
        }
        else
        {
          for(int k=0; k<fid_2d->FID.size(); k++)
          {
            if(k!=applyIndex())
            {
              errorQ=!process(fid_2d->FID[k]);
              if(errorQ) break;
            }
          } // k
        }
        break;
    } // switch

    return !errorQ;

}

bool TApodization::process(TFID_2D *fid_2d, int k)
{
    if(k<0 || k>fid_2d->FID.size()-1)
    {
        errorQ=true;
        setErrorMessage(QString(Q_FUNC_INFO)+": index is out of range.");
        return false;
    }
    else return process(fid_2d->FID[k]);

}

bool TApodization::process(TFID *fid)
{
    if(width()<0)
    {
        errorQ=true;
        setErrorMessage("Width cannot be negative.");
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
            setErrorMessage("Inverse operation is not allowed for sinc apodization");
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
        setErrorMessage("Undefined apodization type.");
        return false;
    } // switch

  return true;
}
