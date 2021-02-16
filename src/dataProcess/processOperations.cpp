#include "processOperations.h"
#include <QDebug>


TProcessOperations::TProcessOperations(QObject *parent) : QObject(parent)
{
   while(!processElements.isEmpty()) processElements.removeLast();
}

TProcessOperations::~TProcessOperations()
{
    while(!processElements.isEmpty()) processElements.removeLast();
}

bool TProcessOperations::saveToFile(QString filename)
{

    QSettings *processSettings = new QSettings(filename, QSettings::IniFormat);

    processSettings->beginGroup("main");
      processSettings->setValue("numberOfOperations", processElements.size());
    processSettings->endGroup();
    processSettings->sync();

    for(int index=0; index<processElements.size(); index++)
    {
      processSettings->beginGroup(QString::number(index));
      processSettings->setValue("type", processElements.at(index)->processType());
      processSettings->setValue("command", processElements.at(index)->command());
      processSettings->setValue("applyMode",processElements.at(index)->applyMode());
      // We only need to write applyIndex only when applyMode() is not zero (apply to all)
      if(processElements.at(index)->applyMode()>0)
      {
        processSettings->setValue("applyIndex",processElements.at(index)->applyIndex());
      }
//      assert(processElements.at(index)->processType() == CutAdd
//             || processElements.at(index)->processType() == Apodization
//             || processElements.at(index)->processType() == Phase
//             || processElements.at(index)->processType() == FFT
//             || processElements.at(index)->processType() == IFFT
//             );
      switch(processElements.at(index)->processType())
      {
        case TProcessElement::CutAdd:

          processSettings->setValue("operation", processElements.at(index)->operation());
          processSettings->setValue("headTail", processElements.at(index)->headTail());
          processSettings->setValue("headPoints", processElements.at(index)->headPoints());
          processSettings->setValue("tailPoints", processElements.at(index)->tailPoints());

          break;

        case TProcessElement::Apodization:
          processSettings->setValue("apodizationType", processElements.at(index)->apodizationType());
          processSettings->setValue("width", processElements.at(index)->width());
          processSettings->setValue("inverse", processElements.at(index)->inverse());

          break;

        case TProcessElement::Phase:
          processSettings->setValue("initialPhase0", processElements.at(index)->initialPhase0());
          processSettings->setValue("initialPhase1", processElements.at(index)->initialPhase1());
          processSettings->setValue("finalPhase0", processElements.at(index)->accumPhase0());
          processSettings->setValue("finalPhase1", processElements.at(index)->accumPhase1());
          processSettings->setValue("pivot", processElements.at(index)->pivot());
          break;
        case TProcessElement::FFT:
        case TProcessElement::IFFT:
          processSettings->setValue("laplace", processElements.at(index)->Laplace());
          processSettings->setValue("laplaceWidth", processElements.at(index)->LaplaceWidth());
          processSettings->setValue("axisDomain",processElements.at(index)->axisDomain());
          break;

        case TProcessElement::AxisStyle:
          processSettings->setValue("domain",processElements.at(index)->domain());
          processSettings->setValue("axisStyle",processElements.at(index)->axisStyle());
          processSettings->setValue("unit",processElements.at(index)->unit());
          processSettings->setValue("label",processElements.at(index)->label());
          processSettings->setValue("referencePoint",processElements.at(index)->referencePoint());
          processSettings->setValue("referenceValue",processElements.at(index)->referenceValue());
          break;

        case TProcessElement::Transpose: break;
        case TProcessElement::ArraySum: break;
        case TProcessElement::Flatten: break;

        case TProcessElement::CartesianMap3D:
          processSettings->setValue("polarAngles",processElements.at(index)->cartesianMap3DPolarAnglesStr());

          break;


        default:
          break;
      }
      processSettings->endGroup();
    } // index
    processSettings->sync();
    delete processSettings;
    return true;
}

bool TProcessOperations::loadFromFile(QString filename)
{
    QSettings settings(filename, QSettings::IniFormat);

    settings.beginGroup("main");
     int n=settings.value("numberOfOperations",0).toInt();
    settings.endGroup();

    if(n==0) return false;
    while (!processElements.isEmpty()) processElements.removeLast();

    int pType;
    for(int k=0; k<n; k++)
    {
      settings.beginGroup(QString::number(k));
        pType=settings.value("type",0).toInt();
        switch(pType)
        {
          case TProcessElement::CutAdd:
            processElements.append(new TAddCutPoints);
            processElements.last()->setOperation(settings.value("operation",0).toInt());
            processElements.last()->setHeadTail(settings.value("headTail",0).toInt());
            processElements.last()->setHeadPoints(settings.value("headPoints",0).toInt());
            processElements.last()->setTailPoints(settings.value("tailPoints",0).toInt());
            processElements.last()->setAveragePoints(settings.value("averagePoints",0).toInt());
            break;

          case TProcessElement::Apodization:
            processElements.append(new TApodization);
            processElements.last()->setApodizationType(settings.value("apodizationType",0).toInt());
            processElements.last()->setWidth(settings.value("width",0).toDouble());
            processElements.last()->setInverse(settings.value("inverse",false).toBool());
            break;

          case TProcessElement::FFT:
            processElements.append(new TFFT);
            processElements.last()->setLaplace(settings.value("laplace",false).toBool());
            processElements.last()->setLaplaceWidth(settings.value("laplaceWidth",0).toDouble());
            processElements.last()->setAxisDomain(settings.value("axisDomain",0).toInt());
            break;

          case TProcessElement::IFFT:
            processElements.append(new TIFFT);
            processElements.last()->setLaplace(settings.value("laplace",false).toBool());
            processElements.last()->setLaplaceWidth(settings.value("laplaceWidth",0).toDouble());
            processElements.last()->setAxisDomain(settings.value("axisDomain",0).toInt());
            break;

          case TProcessElement::Phase:
            processElements.append(new TPhaseRotation);
            processElements.last()->setInitialPhase0(settings.value("initialPhase0",0).toDouble());
            processElements.last()->setInitialPhase1(settings.value("initialPhase1",0).toDouble());
            processElements.last()->setAccumPhase0(settings.value("finalPhase0",0).toDouble());
            processElements.last()->setAccumPhase1(settings.value("finalPhase1",0).toDouble());
            processElements.last()->setPivot(settings.value("pivot",0).toInt());
            processElements.last()->setPhase0(processElements.last()->accumPhase0()-processElements.last()->initialPhase0());
            processElements.last()->setPhase1(processElements.last()->accumPhase1()-processElements.last()->initialPhase1());
            break;

          case TProcessElement::AxisStyle:
            processElements.append(new TAxisStyle);
            processElements.last()->setDomain(settings.value("domain",0).toInt());
            processElements.last()->setUnit(settings.value("unit",0).toInt());
            processElements.last()->setAxisStyle(settings.value("axisStyle",0).toInt());
            processElements.last()->setLabel(settings.value("label","").toString());
            processElements.last()->setReferencePoint(settings.value("referencePoint",0).toInt());
            processElements.last()->setReferenceValue(settings.value("referenceValue",0).toDouble());
            break;

          case TProcessElement::Transpose:
            processElements.append(new TTranspose);
            break;

          case TProcessElement::ArraySum:
            processElements.append(new TArraySum);
            break;

          case TProcessElement::Flatten:
            processElements.append(new TFlatten);
            break;

          case TProcessElement::CartesianMap3D:
            processElements.append(new TCartesianMap3D);
            processElements.last()->setCartesianMap3DPolarAnglesStr(settings.value("polarAngles","").toString());
            break;

        }

      settings.endGroup();
    } // k
    return true;
}

bool TProcessOperations::applyTo(TFID_2D *fid_2d)
{
  bool result=false;

  if(processElements.isEmpty())
  {
    setErrorMessage("Process is empty.");
    return false;
  }

  for(int k=0; k<processElements.size(); k++)
  {
    result=processElements[k]->process(fid_2d);

    if(!result)
    {
        setErrorMessage(processElements[k]->errorMessage());
        break;
    }

  }
  return result;
}
