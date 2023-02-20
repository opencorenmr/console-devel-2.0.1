#include "processElement.h"

//TProcessElement::TProcessElement(QObject *parent) : QObject(parent)
TProcessElement::TProcessElement()
{
    errorQ=false;
    warningQ=false;
    FErrorMessage.clear();
    FWarningMessage.clear();
    setApplyMode(0);
    setApplyIndex(0);

}

QString TProcessElement::processTypeStr()
{
    QString qs;
    switch(processType())
    {
      case TProcessType::CutAdd: qs="CutAdd"; break;
      case TProcessType::Apodization: qs="Apodization"; break;
      case TProcessType::FFT: qs="FFT"; break;
      case TProcessType::IFFT: qs="IFFT"; break;
      case TProcessType::Phase: qs="Phase"; break;
      case TProcessType::PhaseReverse: qs="PhaseReverse"; break;
      case TProcessType::AxisStyle: qs="AxisStyle"; break;
      case TProcessType::ArraySum: qs="ArraySum"; break;
      case TProcessType::Transpose: qs="Transpose"; break;
      case TProcessType::Flatten: qs="Flatten"; break;
      case TProcessType::CartesianMap3D: qs="CartesianMap3D"; break;
      case TProcessType::FFT3D: qs="FFT3D"; break;
      case TProcessType::Math: qs="Math"; break;
      default: qs="InvalidType"; break;
    }

    return qs;
}

void TProcessElement::setProcessTypeStr(QString qs)
{
    if(0==QString::compare("CutAdd",qs,Qt::CaseInsensitive)) setProcessType(TProcessType::CutAdd);
    else if(0==QString::compare("Apodization",qs,Qt::CaseInsensitive)) setProcessType(TProcessType::Apodization);
    else if(0==QString::compare("FFT",qs,Qt::CaseInsensitive)) setProcessType(TProcessType::FFT);
    else if(0==QString::compare("IFFT",qs,Qt::CaseInsensitive)) setProcessType(TProcessType::IFFT);
    else if(0==QString::compare("Phase",qs,Qt::CaseInsensitive)) setProcessType(TProcessType::Phase);
    else if(0==QString::compare("PhaseReverse",qs,Qt::CaseInsensitive)) setProcessType(TProcessType::PhaseReverse);
    else if(0==QString::compare("AxisStyle",qs,Qt::CaseInsensitive)) setProcessType(TProcessType::AxisStyle);
    else if(0==QString::compare("ArraySum",qs,Qt::CaseInsensitive)) setProcessType(TProcessType::ArraySum);
    else if(0==QString::compare("Transpose",qs,Qt::CaseInsensitive)) setProcessType(TProcessType::Transpose);
    else if(0==QString::compare("Flatten",qs,Qt::CaseInsensitive)) setProcessType(TProcessType::Flatten);
    else if(0==QString::compare("CartesianMap3D",qs,Qt::CaseInsensitive)) setProcessType(TProcessType::CartesianMap3D);
    else if(0==QString::compare("FFT3D",qs,Qt::CaseInsensitive)) setProcessType(TProcessType::FFT3D);
    else if(0==QString::compare("Math",qs,Qt::CaseInsensitive)) setProcessType(TProcessType::Math);
    else setProcessType(TProcessType::InvalidType);


    return;
}
