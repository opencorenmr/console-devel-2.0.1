#include "reshape.h"

TReshape::TReshape()
{
  setProcessType(TProcessElement::Reshape);
  FReshapeOption = TProcessElement::ReshapeInvalid;
}

TReshape::~TReshape()
{
    ;
}


void TReshape::setReshapeOptionStr(QString qs)
{
    if(0==QString::compare(qs,RESHAPE_AL,Qt::CaseInsensitive))
        setReshapeOption(TProcessElement::ReshapeAL);
    else if(0==QString::compare(qs,RESHAPE_NOD,Qt::CaseInsensitive))
        setReshapeOption(TProcessElement::ReshapeNumberOfDivisions);
    else setReshapeOption(TProcessElement::ReshapeInvalid);
    return;
}

QString TReshape::reshapeOptionStr()
{
    QString qs;
    switch(reshapeOption())
    {
      case TProcessElement::ReshapeAL:
        qs=RESHAPE_AL;
        break;
      case TProcessElement::ReshapeNumberOfDivisions:
        qs=RESHAPE_NOD;
        break;
      default:
        qs=RESHAPE_INVALID;
        break;
    } // switch

    return qs;
}

bool TReshape::process(TFID_2D *fid_2d)
{
    int netAL=0;
    for(int k=0; k<fid_2d->FID.size(); k++) netAL+=fid_2d->FID.at(k)->al();

    switch(reshapeOption())
    {
      case TProcessElement::ReshapeAL:
          if(netAL%reshapeAL())
          {
            errorQ=true;
            setErrorMessage("Cannot divide.");
            return false;
          }
          setReshapeNOfDivisions(netAL/reshapeAL());
          break;
      case TProcessElement::ReshapeNumberOfDivisions:
        if(netAL%reshapeNOfDivisions())
        {
          errorQ=true;
          setErrorMessage("Cannot divide.");
          return false;
        }
        setReshapeAL(netAL/reshapeNOfDivisions());
        break;
      default:
        setErrorMessage("Invalid reshape operation.");
        errorQ=true;
        return false;

    } // switch

    TFID helpFID(netAL);

    int c=0;
    for(int k=0; k<fid_2d->FID.size(); k++)
    {
        for(int m=0; m<fid_2d->FID.at(k)->al(); m++)
        {
            helpFID.real->sig[c]=fid_2d->FID.at(k)->real->sig[m];
            helpFID.imag->sig[c]=fid_2d->FID.at(k)->imag->sig[m];
            helpFID.abs->sig[c]=fid_2d->FID.at(k)->abs->sig[m];
            c++;
        } // m
    }  //k

    if(c!=netAL)
    {
        //
        setErrorMessage("Counter mismatched during reshape operation.");
        return false;
    }

    fid_2d->setDefaultAL(reshapeAL());
    fid_2d->FID[0]->setAL(fid_2d->defaultAL());
    while(fid_2d->FID.size()>1) fid_2d->FID.removeLast();

    c=0;
    for(int m=0; m<reshapeAL(); m++)
    {
      fid_2d->FID.first()->real->sig[m]=helpFID.real->sig.at(c);
      fid_2d->FID.first()->imag->sig[m]=helpFID.imag->sig.at(c);
      fid_2d->FID.first()->abs->sig[m]=helpFID.abs->sig.at(c);
      c++;
    }

    for(int k=1; k<reshapeNOfDivisions(); k++)
    // we start with k=1,
    // because the case of k=0 has already been dealt with above.
    {
      fid_2d->FID.append(new TFID(reshapeAL()));
      fid_2d->FID.last()->setDomain(fid_2d->FID.first()->domain());
      fid_2d->FID.last()->setXUnit(fid_2d->FID.first()->xunit());
      fid_2d->FID.last()->setPrefix(fid_2d->FID.first()->prefix());
      fid_2d->FID.last()->setPlotPrefix(fid_2d->FID.first()->plotPrefix());
      fid_2d->FID.last()->setCustomXAxis(fid_2d->FID.first()->isXAxisCustom());
      fid_2d->FID.last()->setDW(fid_2d->FID.first()->dw());

      for(int m=0; m<reshapeAL(); m++)
      {
        fid_2d->FID.last()->real->sig[m]=helpFID.real->sig.at(c);
        fid_2d->FID.last()->imag->sig[m]=helpFID.imag->sig.at(c);
        fid_2d->FID.last()->abs->sig[m]=helpFID.abs->sig.at(c);
        c++;
      }
    }

    return true;
}

QString TReshape::command()
{
    QString qs;
    qs="reshape: " + reshapeOptionStr();
    if(reshapeOption()==TProcessElement::ReshapeAL)
    {
        qs+=" to ";
        qs+=QString::number(reshapeAL());
    }
    else if(reshapeOption()==TProcessElement::ReshapeNumberOfDivisions)
    {
        qs+=" to ";
        qs+=QString::number(reshapeNOfDivisions());
    }
    else if(reshapeOption()==TProcessElement::ReshapeInvalid)
    {
        ;
    }
    return qs;
}
