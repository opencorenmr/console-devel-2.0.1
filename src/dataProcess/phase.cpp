#include "phase.h"
#include "math.h"

TPhaseRotation::TPhaseRotation()
{
  //qDebug() << "TPhaseRotation constructor.";
  setProcessType(TProcessType::Phase);
  setPreviousPhase0(0);
  setPreviousPhase1(0);
  resetPhase();
  resetInitialPhase();
  resetAccumPhase();
}

TPhaseRotation::~TPhaseRotation()
{
  //qDebug() << "TPhaseRotation destructor.";
}

void TPhaseRotation::incPhase0(double dp0)
{
    setPreviousPhase0(phase0());
    setPhase0(dp0);
    setAccumPhase0(accumPhase0()+dp0);
}

void TPhaseRotation::incPhase1(double dp1)
{
    setPreviousPhase1(phase1());
    setPhase1(dp1);
    setAccumPhase1(accumPhase1()+dp1);
}

void TPhaseRotation::changePhase0To(double p0)
{
    setPreviousPhase0(phase0());
    double d=p0-accumPhase0();
    setPhase0(d);
    setAccumPhase0(accumPhase0()+d);
}

void TPhaseRotation::changePhase1To(double p1)
{
    setPreviousPhase1(phase1());
    double d=p1-accumPhase1();
    setPhase1(d);
    setAccumPhase1(accumPhase1()+d);
}

void TPhaseRotation::setPivot(int p)
{
    FPivot=p;
}

QStringList TPhaseRotation::processInformation() {return QStringList() << "process=phase";}
QString TPhaseRotation::command()
{
    QString qs;
    qs= "phase (0th order: " + QString::number(accumPhase0())
            + ", 1st order: " + QString::number(accumPhase1())
            + ", pivot: " + QString::number(pivot())
            + ")";

    return qs;
}


bool TPhaseRotation::process(TFID_2D *fid_2d)
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
//    default:
//        qDebug() << QString(Q_FUNC_INFO) << " Default";

//        errorQ=true;
//        setErrorMessage(QString(Q_FUNC_INFO) + ": Invalid operation.");
//        break;
    } // switch


    return !errorQ;

}

bool TPhaseRotation::process(TFID_2D *fid_2d, int k)
{
    if(k<0 || k>fid_2d->FID.size()-1)
    {
        errorQ=true;
        setErrorMessage(QString(Q_FUNC_INFO)+": index is out of range.");
        return false;
    }
    else return process(fid_2d->FID[k]);
}


bool TPhaseRotation::process(TFID *fid)
{
    fid->rotate(phase0());
   // qDebug() << QString(Q_FUNC_INFO) << phase0();
    if(phase1()==0.0) return true;

    if(pivot()<0 || pivot()>fid->al()-1)
    {
        errorQ=true;
        setErrorMessage(QString(Q_FUNC_INFO)+": Invalid pivot index.");
        return false;
    }

    double phi;
    double c;
    double s;
    double xr,xi;

    for(int k=0; k<fid->al(); k++){
        phi=phase1()*3.14159265358979*(pivot()-k)/180.0;

        c=cos(phi);
        s=sin(phi);

        xr=fid->real->sig.at(k); xi=fid->imag->sig.at(k);
        fid->real->sig[k]=c*xr-s*xi;
        fid->imag->sig[k]=c*xi+s*xr;
    }
    return true;
}

