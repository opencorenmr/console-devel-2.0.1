#include "phase.h"
#include "math.h"

TPhaseRotation::TPhaseRotation()
{
  //qDebug() << "TPhaseRotation constructor.";
}

TPhaseRotation::~TPhaseRotation()
{
  //qDebug() << "TPhaseRotation destructor.";
}

void TPhaseRotation::setPivot(int p)
{
    FPivot=p;
}

QStringList TPhaseRotation::processInformation() {return QStringList() << "process=phase";}
QString TPhaseRotation::command() {return "phase";}

bool TPhaseRotation::process(TFID_2D *fid_2d, int k)
{
    if(k<0 || k>fid_2d->FID.size()-1)
    {
        errorQ=true;
        errorMessage=QString(Q_FUNC_INFO)+": index is out of range.";
        return false;
    }
    else return process(fid_2d->FID[k]);
}

bool TPhaseRotation::process(TFID_2D *fid_2d)
{
    bool r;
    for(int c=0; c<fid_2d->FID.size(); c++)
    {
        r=process(fid_2d->FID[c]);
    }
    return r;
}

bool TPhaseRotation::process(TFID *fid)
{
    fid->rotate(phase0());
   // qDebug() << QString(Q_FUNC_INFO) << phase0();
    if(phase1()==0.0) return true;

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

