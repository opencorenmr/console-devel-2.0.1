#include "addCutPoints.h"

TAddCutPoints::TAddCutPoints()
{
  setProcessType(TProcessElement::CutAdd);
  FHeadPoints=0;
  FTailPoints=0;
  FAveragePoints=1;
  errorQ=false;
  warningQ=false;

}

TAddCutPoints::~TAddCutPoints()
{


}

bool TAddCutPoints::process(TFID *fid)
{
    if(headTail()==headTailUndefined || operation()==operationUndefined)
    {
//        qDebug()<<QString(Q_FUNC_INFO)<<operation() << " " << headTail();
        setErrorMessage("Operation is not defined");
        return false;
    }

    bool ok;
    switch(operation()){
    case Cut:
      if(headTail()==Head) {ok=cutHead(fid);}
      else if(headTail()==Tail) {ok=cutTail(fid);}
      else if(headTail()==Both) {ok=cutHeadTail(fid);}
      else {
        setErrorMessage("Unsupported cut operation.");
        ok=false;
      }
      break;
    case Leave:
        if(headTail()==Head) {ok=leaveHead(fid);}
        else if(headTail()==Tail) {ok=leaveTail(fid);}
        else if(headTail()==Both) {ok=leaveMiddle(fid);}
        else {
          setErrorMessage("Unsupported cut operation.");
          ok=false;
        }
        break;

    case ZeroFill:
      if(headTail()==Head) {ok=zeroFillHead(fid);}
      else if(headTail()==Tail) {ok=zeroFillTail(fid);}
      else if(headTail()==Both) {ok=zeroFillHeadTail(fid);}
      else {
        setErrorMessage("Unsupported zero-fill operation.");
        ok=false;
      }
      break;
    case Extrapolate:
        if(headTail()==Head) {ok=extrapolateHead(fid);}
        else if(headTail()==Tail) {ok=extrapolateTail(fid);}
        else if(headTail()==Both) {ok=extrapolateHeadTail(fid);}
        else {
          setErrorMessage("Unsupported extrapolation operation.");
          ok=false;
        }

      break;
    default:
      setErrorMessage("Unsupported operation");
      ok=false;
      break;
    }

    if(!ok) return false;

    return true;
}

bool TAddCutPoints::process(TFID_2D *fid_2d, int k)
{
    return process(fid_2d->FID[k]);
}


bool TAddCutPoints::process(TFID_2D *fid_2d)
{
    errorQ=false;
    switch(applyMode())
    {
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
          errorQ=false;
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
          errorQ=false;
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
    default:

        errorQ=true;
        setErrorMessage(QString(Q_FUNC_INFO) + ": Invalid operation.");
        break;
    } // switch
    return !errorQ;
/*
    bool ok=true;
    for(int k=0; k<fid_2d->FID.size(); k++)
    {
       // qDebug()<<fid_2d->FID.at(0)->al();
      ok=process(fid_2d,k);
      if(!ok) {  //qDebug() << "NG: " << errorMessage;
               return false;}
       // qDebug()<<fid_2d->FID.at(0)->al();

    }

    //  commented out (2 Mar 2023 KT)
    //  fid_2d->setDefaultAl(fid_2d->FID.at(0)->al());

    return true;
    */
}




bool TAddCutPoints::cutHead(TFID *fid)
{
  if(headPoints()==0)
  {
      return false;
  }
  if(headPoints()>fid->al()-1)
  {
      setErrorMessage("You cannot cut more points than " + QString::number(fid->al()) + ".");
      return false;
  }

  double newIniX=fid->xValue(headPoints());
//  double newIniX=fid->xValue(headPoints())
//          *TMetricPrefix::Decimal(fid->prefix());
//  qDebug() << QString(Q_FUNC_INFO) << " newIniX: " << newIniX;

  fid->real->sig.remove(0,headPoints());
  fid->imag->sig.remove(0,headPoints());
  fid->abs->sig.remove(0,headPoints());

  fid->setAL(fid->al()-headPoints());
  fid->setXInitialValue(newIniX);

  return true;
}
bool TAddCutPoints::cutTail(TFID *fid)
{
    if(tailPoints()==0)
    {
        return false;
    }

    if(tailPoints()>fid->al()-1)
    {
        setErrorMessage("You cannot cut more points than " + QString::number(fid->al()) + ".");
        return false;
    }

    fid->setAL(fid->al()-tailPoints());

  return true;
}
bool TAddCutPoints::cutHeadTail(TFID *fid)
{
    if(headPoints()+tailPoints()==0)
    {
        return false;
    }

    if(headPoints()+tailPoints() > fid->al()-1)
    {
        setErrorMessage("[cutHeadTail error] You cannot cut more points than " + QString::number(fid->al()) + ".");
        return false;
    }

    bool ok;
    ok=cutTail(fid);
    if(!ok) return false;
    ok=cutHead(fid);
    if(!ok) return false;
    return true;
}

bool TAddCutPoints::leaveHead(TFID *fid)
{
  if(headPoints()==fid->al())
  {
      setErrorMessage("Nothing to be done.");
      //warningQ=true;
      return false;
  }

  if(headPoints()<1)
  {
      setErrorMessage("[leaveHead error] You cannot cut more points than " + QString::number(fid->al()-1) + ".");
      return false;
  }

  if(headPoints()>fid->al())
  {
      setErrorMessage("[leaveHead error] You cannot leave more points than " + QString::number(fid->al()) + ".");
      return false;
  }

  fid->setAL(headPoints());
  return true;
}

bool TAddCutPoints::leaveTail(TFID *fid)
{
    if(tailPoints()==fid->al())
    {
        setErrorMessage("Nothing to be done.");
        return false;
    }

    if(tailPoints()<1)
    {
        setErrorMessage("[leaveTail error] You cannot cut more points than " + QString::number(fid->al()-1) + ".");
        return false;
    }
    if(tailPoints()>fid->al())
    {
        setErrorMessage("[leaveTail error] You cannot leave more points than " + QString::number(fid->al()) + ".");
        return false;
    }

    double newIniX=fid->xValue(headPoints());

    fid->real->sig.remove(0,fid->al()-tailPoints());
    fid->imag->sig.remove(0,fid->al()-tailPoints());
    fid->abs->sig.remove(0,fid->al()-tailPoints());

    fid->setAL(tailPoints());

    fid->setXInitialValue(newIniX);

    return true;
}

bool TAddCutPoints::leaveMiddle(TFID *fid)
{
  if(headPoints()+tailPoints()==0)
  {
     setErrorMessage("Nothing to be done.");
     return false;
  }

  if(headPoints()>tailPoints())
  {
      setErrorMessage("[leaveMiddle error] Head Points needs to be smaller than the Tail Points.");
      return false;
  }

  if(headPoints() > fid->al()-1)
  {
      setErrorMessage("[leaveMiddle error] You cannot cut more Head Points than " + QString::number(fid->al()-1) + ".");
      return false;
  }

  if(tailPoints() > fid->al()-1)
  {
      setErrorMessage("[leaveMiddle error] Tail Points need to be smaller than " + QString::number(fid->al()-1) + ".");
      return false;
  }

  bool ok;
  // tail points needs to be cut BEFORE head points are cut,
  // because the tailPoints will lose consistency by cutHead!
  fid->setAL(tailPoints());
  ok=cutHead(fid);
  if(!ok) return false;
  return true;
}

bool TAddCutPoints::zeroFillHead(TFID *fid)
{
    if(headPoints()==0)
    {
        return false;
    }

   int prevAL=fid->al();
   fid->setAL(prevAL+headPoints());
     /*
      fid->setAL(newAL) does:
        FAL=newAL
        fid->real->sig.resize(FAL);
        fid->imag->sig.resize(FAL);
        fid->abs->sig.resize(FAL);
     */

   for(int k=0; k<prevAL; k++)
   {
       fid->real->sig[fid->al()-1-k] = fid->real->sig.at(prevAL-1-k);
       fid->imag->sig[fid->al()-1-k] = fid->imag->sig.at(prevAL-1-k);
       fid->abs->sig[fid->al()-1-k] = fid->abs->sig.at(prevAL-1-k);
   }
   for(int k=headPoints()-1; k>=0; k--)
   {
       fid->real->sig[k]=0.0;
       fid->imag->sig[k]=0.0;
       fid->abs->sig[k]=0.0;
   }

   fid->setXInitialValue(fid->xInitialValue()-headPoints()*fid->dx());

  return true;
}
bool TAddCutPoints::zeroFillTail(TFID *fid)
{
    if(tailPoints()==0)
    {
        return false;
    }

    int prevAL=fid->al();
    fid->setAL(prevAL+tailPoints());
      /*
       fid->setAL(newAL) does:
         FAL=newAL
         fid->real->sig.resize(FAL);
         fid->imag->sig.resize(FAL);
         fid->abs->sig.resize(FAL);
      */

    for(int k=0; k<tailPoints(); k++)
    {
        fid->real->sig[prevAL+k]=0.0;
        fid->imag->sig[prevAL+k]=0.0;
        fid->abs->sig[prevAL+k]=0.0;
    }

  return true;
}
bool TAddCutPoints::zeroFillHeadTail(TFID *fid)
{
    if(headPoints()+tailPoints()==0)
    {
        return false;
    }

  zeroFillHead(fid);
  zeroFillTail(fid);
  return true;
}

bool TAddCutPoints::extrapolateHeadTail(TFID *fid)
{
    if(headPoints()+tailPoints()==0)
    {
        return false;
    }

  extrapolateHead(fid);
  extrapolateTail(fid);
  return true;

}

bool TAddCutPoints::extrapolateHead(TFID *fid)
{
    if(headPoints()==0)
    {
        return false;
    }

    int prevAL=fid->al();
    fid->setAL(prevAL+headPoints());
      /*
       fid->setAL(newAL) does:
         FAL=newAL
         fid->real->sig.resize(FAL);
         fid->imag->sig.resize(FAL);
         fid->abs->sig.resize(FAL);
      */

    for(int k=0; k<prevAL; k++)
    {
        fid->real->sig[fid->al()-1-k] = fid->real->sig.at(prevAL-1-k);
        fid->imag->sig[fid->al()-1-k] = fid->imag->sig.at(prevAL-1-k);
        fid->abs->sig[fid->al()-1-k] = fid->abs->sig.at(prevAL-1-k);
    }

    for(int k=headPoints()-1; k>=0; k--)
    {
        fid->real->sig[k]=fid->real->sig.at(headPoints());
        fid->imag->sig[k]=fid->imag->sig.at(headPoints());
        fid->abs->sig[k]=fid->abs->sig.at(headPoints());
    }

    fid->setXInitialValue(fid->xInitialValue()-headPoints()*fid->dx());

   return true;
}

bool TAddCutPoints::extrapolateTail(TFID *fid)
{
    if(tailPoints()==0)
    {
        return false;
    }

    int prevAL=fid->al();
    fid->setAL(prevAL+tailPoints());
      /*
       fid->setAL(newAL) does:
         FAL=newAL
         fid->real->sig.resize(FAL);
         fid->imag->sig.resize(FAL);
         fid->abs->sig.resize(FAL);
      */

    for(int k=0; k<tailPoints(); k++)
    {
        fid->real->sig[prevAL+k]=fid->real->sig.at(prevAL-1);
        fid->imag->sig[prevAL+k]=fid->imag->sig.at(prevAL-1);
        fid->abs->sig[prevAL+k]=fid->abs->sig.at(prevAL-1);
    }

  return true;
}

QString TAddCutPoints::headTail_str()
{
  QString qs;
  switch (headTail())
  {
    case Head:
      qs="head";
      qs+=" ";
      qs+=QString::number(headPoints());
      break;
    case Tail:
      qs="tail";
      qs+=" ";
      qs+=QString::number(tailPoints());
      break;
    case Both:
      qs="both";
      qs+=" ";
      qs+=QString::number(headPoints());
      qs+=" ";
      qs+=QString::number(tailPoints());
      break;
    default:
      qs="undefined";
      break;
  }

  return qs;
}

QString TAddCutPoints::operation_str()
{
    QString qs;
    switch (operation()) {
    case operationUndefined:
        qs="undefined";
        break;
    case Add:
        qs="add";
        break;
    case Cut:
        qs="cut";
        break;
    case Leave:
        qs="leave";
        break;
    case ZeroFill:
        qs="zero fill";
        break;
    case Extrapolate:
        qs="extrapolate";
        break;
    default:
        qs="undefined";
        break;
    }
    return qs;
}

QStringList TAddCutPoints::processInformation()
{
    QString qs;
    switch (FOperation) {
    case operationUndefined:
        qs="Undefined";
        break;
    case Add:
        qs="Add";
        break;
    case Cut:
        qs="Cut";
        break;
    case ZeroFill:
        qs="Zero fill";
        break;
    case Extrapolate:
        qs="Extrapolate";
        break;
    default:
        qs="Undefined";
        break;
    }
    return QStringList() << "process=addcut(" << qs << ")";
}
QString TAddCutPoints::command()
{
    QString qs;
    qs="addcut";
    qs+=": ";
    qs+=operation_str();
    qs+=" ";
    qs+=headTail_str();
    return qs;
}


/*
 *
          processSettings->setValue("operation", processElements.at(index)->operation());
          processSettings->setValue("headTail", processElements.at(index)->headTail());
          processSettings->setValue("headPoints", processElements.at(index)->headPoints());
          processSettings->setValue("tailPoints", processElements.at(index)->tailPoints());

 *
 */
