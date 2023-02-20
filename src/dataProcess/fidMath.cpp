#include <QFileInfo>
#include <QDir>
#include "fidMath.h"
#include "math.h"
#include "float.h"

TFIDMath::TFIDMath()
{
    afid2 = new TFID_2D();
    setProcessType(TProcessType::Math);
  // default values
    // Never call virtual methods/functions in constructors or destructors
    // That is why the following virtual functions have been commented out!
    // (20 Feb 2023 KT)
    //setFIDMathOperation(TFIDMathOperation::Add);
    //setFIDMathOperationWith(TFIDMathOperationWith::Number);
}

TFIDMath::~TFIDMath()
{
    if (afid2 != nullptr) delete afid2;
}

QString TFIDMath::command()
{
    QString qs;
    qs ="math: ";
    if(FIDMathOperationWith()==TFIDMath::Number)
    {
      qs += FIDMathOperationStr();
      switch(FIDMathOperation())
      {
        case TFIDMathOperation::Add:
        case TFIDMathOperation::Subtract:
        case TFIDMathOperation::Multiply:
        case TFIDMathOperation::Divide:
          qs += " by ("
                    + QString::number(FIDMathReal())
                    + ","
                    + QString::number(FIDMathImag())
                    + ")";
          break;
        case TFIDMathOperation::Offset:
        case TFIDMathOperation::PhaseOffset:
          qs += " with average value between points (";
          qs += QString::number(FIDMathXIni());
          qs += ", ";
          qs += QString::number(FIDMathXFin());
          qs += ")";
          break;
        default:
          break;
      }
    }
    else if(FIDMathOperationWith()==TFIDMath::File)
    {
        qs += FIDMathOperationStr();
        switch(FIDMathOperation())
        {
          case Add:
            qs +=" with ";
            break;
          default:
            qs += " by ";
            break;
        }
        qs += FIDMathDirName() + "/" + FIDMathFileName();
    }
    return qs;
}


void TFIDMath::setFIDMathOperationStr(QString qs)
{
  if(0==QString::compare(qs,"add",Qt::CaseInsensitive)) setFIDMathOperation(TFIDMathOperation::Add);
  else if(0==QString::compare(qs,"subtract",Qt::CaseInsensitive)) setFIDMathOperation(TFIDMathOperation::Subtract);
  else if(0==QString::compare(qs,"multiply",Qt::CaseInsensitive)) setFIDMathOperation(TFIDMathOperation::Multiply);
  else if(0==QString::compare(qs,"divide",Qt::CaseInsensitive)) setFIDMathOperation(TFIDMathOperation::Divide);
  else if(0==QString::compare(qs,"normalize",Qt::CaseInsensitive)) setFIDMathOperation(TFIDMathOperation::Normalize);
  else if(0==QString::compare(qs,"offset",Qt::CaseInsensitive)) setFIDMathOperation(TFIDMathOperation::Offset);
  else if(0==QString::compare(qs,"phase_offset",Qt::CaseInsensitive)) setFIDMathOperation(TFIDMathOperation::PhaseOffset);
  else if(0==QString::compare(qs,"reverse_phase",Qt::CaseInsensitive)) setFIDMathOperation(TFIDMathOperation::ReversePhase);
  else setFIDMathOperation(TFIDMathOperation::InvalidMathOperation);
}

QString TFIDMath::FIDMathOperationStr()
{
    QString qs;
    switch(FIDMathOperation())
    {
      case TFIDMathOperation::Add: qs="add"; break;
      case TFIDMathOperation::Subtract: qs="subtract"; break;
      case TFIDMathOperation::Multiply: qs="multiply"; break;
      case TFIDMathOperation::Divide: qs="divide"; break;
      case TFIDMathOperation::Normalize: qs="normalize"; break;
      case TFIDMathOperation::Offset: qs="offset";break;
      case TFIDMathOperation::PhaseOffset: qs="phase_offset"; break;
      case TFIDMathOperation::ReversePhase: qs="reverse_phase";break;
     // case SwapReIm: qs="swap_re_im"; break;
      default: qs="invalid"; break;
    } // switch
    return qs;
}

void TFIDMath::setFIDMathOperationWithStr(QString qs)
{
    if(0==QString::compare(qs,"number",Qt::CaseInsensitive)) setFIDMathOperationWith(TFIDMathOperationWith::Number);
    else if(0==QString::compare(qs,"file",Qt::CaseInsensitive)) setFIDMathOperationWith(TFIDMathOperationWith::File);
    else setFIDMathOperationWith(TFIDMathOperationWith::Number);
}

QString TFIDMath::FIDMathOperationWithStr()
{
    QString qs;
    switch(FIDMathOperationWith())
    {
      case TFIDMathOperationWith::Number: qs="number"; break;
      case TFIDMathOperationWith::File: qs="file"; break;
     // case Buffer: qs="buffer"; break;
      default: qs="invalid"; break;
    } // switch

    return qs;
}


QStringList TFIDMath::processInformation()
{
    return QStringList() << "process=math"
                         << "operation="+FIDMathOperationStr()
                         << "with="+FIDMathOperationWithStr();
}

bool TFIDMath::process(TFID_2D *fid_2d)
{
    bool r=true;

    switch(FIDMathOperationWith())
    {
      case TFIDMathOperationWith::Number:
        for(int c=0; c<fid_2d->FID.size(); c++)
        {
            r=process(fid_2d->FID[c]);
            if(!r)break;
        }
        break;
      case TFIDMathOperationWith::File:
        r=loadFile();
        if(!r)return false;
        if(afid2->FID.size()==1)
        {
            r=operationWithFile(fid_2d);
        }
        else
        {
            r=operationWith2DFile(fid_2d);
        }
//        else
//        {
//            for(int c=0; c<fid_2d->FID.size(); c++)
//            {
//                r=process(fid_2d->FID[c]);
//                loadFlag = false;
//                if(!r)break;
//            }
//        }
        afid2->FID.clear();
        break;
      default:
        setErrorMessage("Invalid math operation");
        r=false;
        break;
    }

    return r;
}

bool TFIDMath::process(TFID_2D *fid_2d, int k)
{
    if(k<0 || k>fid_2d->FID.size()-1)
    {
        errorQ=true;
        setErrorMessage(QString(Q_FUNC_INFO)+": index is out of range.");
        return false;
    }
    else return process(fid_2d->FID[k]);

}

bool TFIDMath::process(TFID *fid)
{
    bool ok=true;

    switch(FIDMathOperationWith())
    {
      case TFIDMathOperationWith::Number:
        ok=operationWithNumber(fid);
        break;
      case TFIDMathOperationWith::File:
        ok=loadFile();
        if(!ok) break;
        if(afid2->FID.size()!=1)
        {
            ok=false;
            setErrorMessage("math operation with a file with a different size is not allowed.");
            break;
        }

        ok=operationWithFile(fid);
        afid2->FID.clear();
        break;
   //   case Buffer:
   //     ok=operationWithBuffer(fid);
   //     break;
      default:
        setErrorMessage("Invalid math operation");
        ok=false;
        break;
    }

    return ok;
}

bool TFIDMath::loadFile()
{
    // try to open file
    bool ok;
    QFileInfo qfi;

    qfi.setFile(QDir(FIDMathDirName()),FIDMathFileName());
    if(!qfi.exists())
    {
        setErrorMessage("File does not exist.");
        return false;
    }
    if(0==QString::compare(qfi.suffix(),"sm2p",Qt::CaseInsensitive)
        || 0==QString::compare(qfi.suffix(),"sm2d",Qt::CaseInsensitive))
    {
        setFileType(TFIDMath::sm2d);
    }
    else if (0==QString::compare(qfi.suffix(),"opp",Qt::CaseInsensitive)
             || 0==QString::compare(qfi.suffix(),"opd",Qt::CaseInsensitive))
    {
        setFileType(TFIDMath::opd);
        // qDebug() << QString(Q_FUNC_INFO) << "opd";
        // qDebug() << QString(Q_FUNC_INFO) << FIDMath->fileType();
    }
    else if (0==QString::compare(qfi.suffix(),"smd",Qt::CaseInsensitive))
    {
        setFileType(TFIDMath::smd);
    }
    else
    {
        setErrorMessage("Invalid file type.");
        return false;
    }

//    TFID_2D *helpfid_2D=new TFID_2D();

    switch (fileType())
    {
      case TFIDMath::opd:
        ok=afid2->ReadopFiles(absoluteFileName());
        break;
      case TFIDMath::sm2d:
        ok=afid2->Readsm2Files(absoluteFileName());
        break;
      case TFIDMath::smd:
        ok=afid2->ReadsmdFile(absoluteFileName());
        break;
      default:
        ok=false;
    }
    // qDebug() << QString(Q_FUNC_INFO) << "2" << ok;

    if(!ok)
    {
        QString errmsg;
        errmsg = "Failed to load file: ";
        errmsg += absoluteFileName();
        errmsg += ".";
        setErrorMessage(errmsg);
 //       delete helpfid_2D;
        return false;
    }

 //   if(afid2!=nullptr)delete afid2;
 //   afid2 = helpfid_2D;
    return true;
}

bool TFIDMath::operationWithNumber(TFID *fid)
{
    double amax=0,ph=0;
    double re1=0,im1=0,re2=0,im2=0,ab2Squared=0;

    switch(FIDMathOperation())
    {
      case TFIDMathOperation::Add:
        for(int k=0; k<fid->al(); k++)
        {
            fid->real->sig[k]+=FIDMathReal();
            fid->imag->sig[k]+=FIDMathImag();
        }
        fid->updateAbs();
        break;
      case TFIDMathOperation::Subtract:
        for(int k=0; k<fid->al(); k++)
        {
            fid->real->sig[k]-=FIDMathReal();
            fid->imag->sig[k]-=FIDMathImag();
        }
        fid->updateAbs();
        break;
      case TFIDMathOperation::Multiply:
        for(int k=0; k<fid->al(); k++)
        {
            re1=fid->real->sig.at(k);
            im1=fid->imag->sig.at(k);
            re2=FIDMathReal();
            im2=FIDMathImag();
            fid->real->sig[k] = re1*re2-im1*im2;
            fid->imag->sig[k] = re1*im2+im1*re2;
        }
        fid->updateAbs();
        break;
      case TFIDMathOperation::Divide:
        if(fabs(FIDMathReal())<DBL_EPSILON && fabs(FIDMathImag())<DBL_EPSILON)
        {
            errorQ=true;
            setErrorMessage("Division by 0 is not allowed.");
            return false;
        }
        for(int k=0; k<fid->al(); k++)
        {
            re1=fid->real->sig.at(k);
            im1=fid->imag->sig.at(k);
            re2=FIDMathReal();
            im2=FIDMathImag();
            ab2Squared=re2*re2+im2*im2;
            fid->real->sig[k] = (re1*re2+im1*im2)/ab2Squared;
            fid->imag->sig[k] = (-re1*im2+im1*re2)/ab2Squared;
        }
        fid->updateAbs();
        break;

      case TFIDMathOperation::Normalize:
        amax=fid->abs->max();
        if(fabs(amax)<DBL_EPSILON)
        {
            errorQ=true;
            setErrorMessage("Division by 0 is not allowed.");
            return false;
        }
        for(int k=0; k<fid->al(); k++)
        {
            fid->real->sig[k]/=amax;
            fid->imag->sig[k]/=amax;
        }
        fid->updateAbs();
        break;

      case TFIDMathOperation::Offset:
        if(FIDMathXIni()<0 || FIDMathXFin()>fid->al()-1)
        {
            errorQ=true;
            setErrorMessage("Index out of range.");
            return false;
        }

        re1=fid->real->average(FIDMathXIni(),FIDMathXFin());
        im1=fid->imag->average(FIDMathXIni(),FIDMathXFin());
        for(int k=0; k<fid->al(); k++)
        {
            fid->real->sig[k]-=re1;
            fid->imag->sig[k]-=im1;
        }
        fid->updateAbs();

        break;

      case TFIDMathOperation::PhaseOffset:
        if(FIDMathXIni()<0 || FIDMathXFin()>fid->al()-1)
        {
            errorQ=true;
            setErrorMessage("Index out of range.");
            return false;
        }
        re1=fid->real->average(FIDMathXIni(),FIDMathXFin());
        im1=fid->imag->average(FIDMathXIni(),FIDMathXFin());

        ph=-180/3.141592*atan2(im1,re1);
        fid->rotate(ph);

        break;

      case TFIDMathOperation::ReversePhase:
        fid->phaseReverse();
        return true;

      default:
        errorQ=true;
        setErrorMessage("Invalid math operation");
        return false;

    }

    return true;
}

bool TFIDMath::operationWithFile(TFID *fid)
{
    // In this function we can implement operation with
    // a single (i.e., non-arrayed) data
    if(afid2->FID.size()!=1)
    {
        errorQ=true;
        setErrorMessage("Math operation of two data with different data size is not allowed.");
        return false;
    }

    if(afid2->al()!=fid->al())
    {
        errorQ=true;
        setErrorMessage("Math operation of two data with different data length is not allowed.");
        return false;
    }

    double re1=0,im1=0,re2=0,im2=0,ab2=0;
    switch(FIDMathOperation())
    {
      case TFIDMathOperation::Add:
        for(int k=0; k<fid->al(); k++)
        {
            fid->real->sig[k]+=afid2->FID.first()->real->sig.at(k);
            fid->imag->sig[k]+=afid2->FID.first()->imag->sig.at(k);
        }
        fid->updateAbs();
        break;
      case TFIDMathOperation::Subtract:
        for(int k=0; k<fid->al(); k++)
        {
            fid->real->sig[k]-=afid2->FID.first()->real->sig.at(k);
            fid->imag->sig[k]-=afid2->FID.first()->imag->sig.at(k);
        }
        fid->updateAbs();
        break;
      case TFIDMathOperation::Multiply:
        for(int k=0; k<fid->al(); k++)
        {
           re1=fid->real->sig.at(k);
           im1=fid->imag->sig.at(k);
           re2=afid2->FID.first()->real->sig.at(k);
           im2=afid2->FID.first()->imag->sig.at(k);
           fid->real->sig[k] = (re1*re2)-(im1*im2);
           fid->imag->sig[k] = (im1*re2)+(re1*im2);
        }
        fid->updateAbs();
        break;
      case TFIDMathOperation::Divide:
        for(int k=0; k<afid2->al(); k++)
        {
           if(fabs(afid2->FID.first()->abs->sig.at(k))<DBL_EPSILON)
           {
               errorQ=true;
               setErrorMessage("Division by zero is not allowed.");
               return false;
           }
        }
        for(int k=0; k<afid2->al(); k++)
        {
            re1=fid->real->sig.at(k);
            im1=fid->imag->sig.at(k);
            re2=afid2->FID.first()->real->sig.at(k);
            im2=afid2->FID.first()->imag->sig.at(k);
            ab2=afid2->FID.first()->abs->sig.at(k);

            //complex division
            fid->real->sig[k] = (re1*re2+im1*im2)/(ab2*ab2);
            fid->imag->sig[k] = (im1*re2-re1*im2)/(ab2*ab2);

        }
        fid->updateAbs();
        break;
      default:
        errorQ=true;
        setErrorMessage("Invalid operation");
        return false;

    }
    return true;
}

bool TFIDMath::operationWith2DFile(TFID_2D *fid_2D)
{
    if(afid2->al()!=fid_2D->al())
    {
        errorQ=true;
        setErrorMessage("Math operation of two data with different data length is not allowed.");
//        delete afid2;
        return false;
    }

    double re1=0,im1=0,re2=0,im2=0,ab2=0;
    switch(FIDMathOperation())
    {
      case TFIDMathOperation::Add:
        for(int l=0; l<fid_2D->FID.size(); l++){
            for(int k=0; k<fid_2D->al(); k++)
            {
                fid_2D->FID[l]->real->sig[k]+=afid2->FID.at(l)->real->sig.at(k);
                fid_2D->FID[l]->imag->sig[k]+=afid2->FID.at(l)->imag->sig.at(k);
            }
            fid_2D->FID.at(l)->updateAbs();
        }
        break;
      case TFIDMathOperation::Subtract:
        for(int l=0; l<fid_2D->FID.size(); l++){
            for(int k=0; k<fid_2D->al(); k++)
            {
                fid_2D->FID[l]->real->sig[k]-=afid2->FID.at(l)->real->sig.at(k);
                fid_2D->FID[l]->imag->sig[k]-=afid2->FID.at(l)->imag->sig.at(k);
            }
            fid_2D->FID.at(l)->updateAbs();
        }
        break;
      case TFIDMathOperation::Multiply:
        for(int l=0; l<fid_2D->FID.size(); l++){
            for(int k=0; k<fid_2D->al(); k++)
            {
               re1=fid_2D->FID.at(l)->real->sig.at(k);
               im1=fid_2D->FID.at(l)->imag->sig.at(k);
               re2=afid2->FID.at(l)->real->sig.at(k);
               im2=afid2->FID.at(l)->imag->sig.at(k);
               fid_2D->FID[l]->real->sig[k] = (re1*re2)-(im1*im2);
               fid_2D->FID[l]->imag->sig[k] = (im1*re2)+(re1*im2);
            }
            fid_2D->FID.at(l)->updateAbs();
        }
        break;
      case TFIDMathOperation::Divide:
        for(int l=0; l<afid2->FID.size(); l++){
            for(int k=0; k<afid2->al(); k++)
            {
               if(fabs(afid2->FID.at(l)->abs->sig.at(k))<DBL_EPSILON)
               {
                   errorQ=true;
                   setErrorMessage("Division by zero is not allowed.");
//                   delete afid2;
                   return false;
               }
            }
        }
        for(int l=0; l<fid_2D->FID.size(); l++){
            for(int k=0; k<fid_2D->al(); k++)
            {
                re1=fid_2D->FID.at(l)->real->sig.at(k);
                im1=fid_2D->FID.at(l)->imag->sig.at(k);
                re2=afid2->FID.at(l)->real->sig.at(k);
                im2=afid2->FID.at(l)->imag->sig.at(k);
                ab2=afid2->FID.at(l)->abs->sig.at(k);

                //complex division
                fid_2D->FID[l]->real->sig[k] = (re1*re2+im1*im2)/(ab2*ab2);
                fid_2D->FID[l]->imag->sig[k] = (im1*re2-re1*im2)/(ab2*ab2);

            }
            fid_2D->FID.at(l)->updateAbs();
        }
        break;
      default:
        errorQ=true;
        setErrorMessage("Invalid operation");
        return false;

    }
    return true;
}


bool TFIDMath::operationWithFile(TFID_2D *fid_2D)
{
    if(afid2->al()!=fid_2D->al())
    {
        errorQ=true;
        setErrorMessage("Math operation of two data with different data length is not allowed.");
//        delete afid2;
        return false;
    }

    double re1=0,im1=0,re2=0,im2=0,ab2=0;
    switch(FIDMathOperation())
    {
      case TFIDMathOperation::Add:
        for(int l=0; l<fid_2D->FID.size(); l++){
            for(int k=0; k<fid_2D->al(); k++)
            {
                fid_2D->FID[l]->real->sig[k]+=afid2->FID.at(0)->real->sig.at(k);
                fid_2D->FID[l]->imag->sig[k]+=afid2->FID.at(0)->imag->sig.at(k);
            }
            fid_2D->FID.at(l)->updateAbs();
        }
        break;
      case TFIDMathOperation::Subtract:
        for(int l=0; l<fid_2D->FID.size(); l++){
            for(int k=0; k<fid_2D->al(); k++)
            {
                fid_2D->FID[l]->real->sig[k]-=afid2->FID.at(0)->real->sig.at(k);
                fid_2D->FID[l]->imag->sig[k]-=afid2->FID.at(0)->imag->sig.at(k);
            }
            fid_2D->FID.at(l)->updateAbs();
        }
        break;
      case TFIDMathOperation::Multiply:
        for(int l=0; l<fid_2D->FID.size(); l++){
            for(int k=0; k<fid_2D->al(); k++)
            {
               re1=fid_2D->FID.at(l)->real->sig.at(k);
               im1=fid_2D->FID.at(l)->imag->sig.at(k);
               re2=afid2->FID.at(0)->real->sig.at(k);
               im2=afid2->FID.at(0)->imag->sig.at(k);
               fid_2D->FID[l]->real->sig[k] = (re1*re2)-(im1*im2);
               fid_2D->FID[l]->imag->sig[k] = (im1*re2)+(re1*im2);
            }
            fid_2D->FID.at(l)->updateAbs();
        }
        break;
      case TFIDMathOperation::Divide:
//        for(int l=0; l<afid2->FID.size(); l++){
            for(int k=0; k<afid2->al(); k++)
            {
               if(fabs(afid2->FID.at(0)->abs->sig.at(k))<DBL_EPSILON)
               {
                   errorQ=true;
                   setErrorMessage("Division by zero is not allowed.");
                   return false;
               }
            }
//        }
        for(int l=0; l<fid_2D->FID.size(); l++){
            for(int k=0; k<fid_2D->al(); k++)
            {
                re1=fid_2D->FID.at(l)->real->sig.at(k);
                im1=fid_2D->FID.at(l)->imag->sig.at(k);
                re2=afid2->FID.at(0)->real->sig.at(k);
                im2=afid2->FID.at(0)->imag->sig.at(k);
                ab2=afid2->FID.at(0)->abs->sig.at(k);

                //complex division
                fid_2D->FID[l]->real->sig[k] = (re1*re2+im1*im2)/(ab2*ab2);
                fid_2D->FID[l]->imag->sig[k] = (im1*re2-re1*im2)/(ab2*ab2);

            }
            fid_2D->FID.at(l)->updateAbs();
        }
        break;
      default:
        errorQ=true;
        setErrorMessage("Invalid operation");
        return false;

    }
    return true;
}
/*
bool TFIDMath::operationWithBuffer(TFID *fid)
{
    return true;
}
*/
