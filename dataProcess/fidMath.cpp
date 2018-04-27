#include "fidMath.h"
#include "math.h"

TFIDMath::TFIDMath()
{
  // default values
    setFIDMathOperation(Add);
    setOperationWith(Number);
    setNumber1(0.0);
    setNumber2(0.0);
    setXIni(0);
    setXFin(0);
    setFileName("");
    setFileType(opd);
}

QString TFIDMath::command()
{
    return "math";
}

QString TFIDMath::mathOperationString()
{
    QString qs;
    switch(FIDMathOperation())
    {
      case Add: qs="add"; break;
      case Subtract: qs="subtract"; break;
      case Multiply: qs="multiply"; break;
      case Divide: qs="divide"; break;
      case Normalize: qs="normalize"; break;
      case CorrectOffset: qs="correct_offset";break;
      case CorrectPhaseOffset: qs="correct_phaseoffset"; break;
      case ReversePhase: qs="reverse_phase";break;
     // case SwapReIm: qs="swap_re_im"; break;
      default: qs="invalid"; break;
    } // switch
    return qs;
}

QString TFIDMath::operationWithString()
{
    QString qs;
    switch(operationWith())
    {
      case Number: qs="number"; break;
      case File: qs="file"; break;
     // case Buffer: qs="buffer"; break;
      default: qs="invalid"; break;
    } // switch

    return qs;
}


QStringList TFIDMath::processInformation()
{
    return QStringList() << "process=math"
                         << "operation="+mathOperationString()
                         << "with="+operationWithString();
}
bool TFIDMath::process(TFID_2D *fid_2d)
{
    bool r;
    for(int c=0; c<fid_2d->FID.size(); c++)
    {
        r=process(fid_2d->FID[c]);
    }
    return r;
}

bool TFIDMath::process(TFID_2D *fid_2d, int k)
{
    if(k<0 || k>fid_2d->FID.size()-1)
    {
        errorQ=true;
        errorMessage=QString(Q_FUNC_INFO)+": index is out of range.";
        return false;
    }
    else return process(fid_2d->FID[k]);

}

bool TFIDMath::process(TFID *fid)
{
    bool ok=true;

    switch(operationWith())
    {
      case Number:
        ok=operationWithNumber(fid);
        break;
      case File:
        ok=operationWithFile(fid);
        break;
   //   case Buffer:
   //     ok=operationWithBuffer(fid);
   //     break;
      default:
        errorMessage="Invalid math operation";
        ok=false;
        break;
    }

    return ok;
}

bool TFIDMath::operationWithNumber(TFID *fid)
{
    double amax=0,ph=0;
    double re1=0,im1=0,re2=0,im2=0,ab2Squared=0;

    switch(FIDMathOperation())
    {
      case Add:
        for(int k=0; k<fid->al(); k++)
        {
            fid->real->sig[k]+=number1();
            fid->imag->sig[k]+=number2();
        }
        fid->updateAbs();
        break;
      case Subtract:
        for(int k=0; k<fid->al(); k++)
        {
            fid->real->sig[k]-=number1();
            fid->imag->sig[k]-=number2();
        }
        fid->updateAbs();
        break;
      case Multiply:
        for(int k=0; k<fid->al(); k++)
        {
            re1=fid->real->sig.at(k);
            im1=fid->imag->sig.at(k);
            re2=number1();
            im2=number2();
            fid->real->sig[k] = re1*re2-im1*im2;
            fid->imag->sig[k] = re1*im2+im1*re2;
        }
        fid->updateAbs();
        break;
      case Divide:
        if(number1()==0 && number2()==0)
        {
            errorQ=true;
            errorMessage="Division by 0 is not allowed.";
            return false;
        }
        for(int k=0; k<fid->al(); k++)
        {
            re1=fid->real->sig.at(k);
            im1=fid->imag->sig.at(k);
            re2=number1();
            im2=number2();
            ab2Squared=re2*re2+im2*im2;
            fid->real->sig[k] = (re1*re2+im1*im2)/ab2Squared;
            fid->imag->sig[k] = (-re1*im2+im1*re2)/ab2Squared;
        }
        fid->updateAbs();
        break;

      case Normalize:
        amax=fid->abs->max();
        if(amax==0)
        {
            errorQ=true;
            errorMessage="Division by 0 is not allowed.";
            return false;
        }
        for(int k=0; k<fid->al(); k++)
        {
            fid->real->sig[k]/=amax;
            fid->imag->sig[k]/=amax;
        }
        fid->updateAbs();
        break;

      case CorrectOffset:
        if(xini()<0 || xfin()>fid->al()-1)
        {
            errorQ=true;
            errorMessage="Index out of range.";
            return false;
        }

        re1=fid->real->average(xini(),xfin());
        im1=fid->imag->average(xini(),xfin());
        for(int k=0; k<fid->al(); k++)
        {
            fid->real->sig[k]-=re1;
            fid->imag->sig[k]-=im1;
        }
        fid->updateAbs();

        break;

      case CorrectPhaseOffset:
        if(xini()<0 || xfin()>fid->al()-1)
        {
            errorQ=true;
            errorMessage="Index out of range.";
            return false;
        }
        re1=fid->real->average(xini(),xfin());
        im1=fid->imag->average(xini(),xfin());

        ph=-180/3.141592*atan2(im1,re1);
        fid->rotate(ph);

        break;

      case ReversePhase:
        fid->phaseReverse();
        return true;

      default:
        errorQ=true;
        errorMessage="Invalid math operation";
        return false;

    }

    return true;
}

bool TFIDMath::operationWithFile(TFID *fid)
{
    TFID_2D *afid2=new TFID_2D();
    // try to open file
    bool ok;

   // qDebug () << QString(Q_FUNC_INFO) << fileType();

    switch (fileType())
    {
      case opd:
        ok=afid2->ReadopFiles(fileName());
        break;
      case sm2d:
        ok=afid2->Readsm2Files(fileName());
        break;
      case smd:
        ok=afid2->ReadsmdFile(fileName());
        break;
      default:
        ok=false;
    }
    // qDebug() << QString(Q_FUNC_INFO) << "2" << ok;

    if(!ok) {delete afid2; return false;}

    // At this moment we can implement operation with
    // a single (i.e., non-arrayed) data
    if(afid2->FID.size()!=1)
    {
        errorQ=true;
        errorMessage="Math operation with arrayed data is not supported.";
        delete afid2;
        return false;
    }

    if(afid2->al()!=fid->al())
    {
        errorQ=true;
        errorMessage="Math operation of two data with different data length is not allowed.";
        delete afid2;
        return false;
    }


    double re1=0,im1=0,re2=0,im2=0,ab2=0;
    switch(FIDMathOperation())
    {
      case Add:
        for(int k=0; k<fid->al(); k++)
        {
            fid->real->sig[k]+=afid2->FID.first()->real->sig.at(k);
            fid->imag->sig[k]+=afid2->FID.first()->imag->sig.at(k);
        }
        fid->updateAbs();
        break;
      case Subtract:
        for(int k=0; k<fid->al(); k++)
        {
            fid->real->sig[k]-=afid2->FID.first()->real->sig.at(k);
            fid->imag->sig[k]-=afid2->FID.first()->imag->sig.at(k);
        }
        fid->updateAbs();
        break;
      case Multiply:
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
      case Divide:
        for(int k=0; k<afid2->al(); k++)
        {
           if(afid2->FID.first()->abs->sig.at(k)==0)
           {
               errorQ=true;
               errorMessage="Division by zero is not allowed.";
               delete afid2;
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
        errorMessage="Invalid operation";
        delete afid2;
        return false;

    }
    delete afid2;
    return true;
}

/*
bool TFIDMath::operationWithBuffer(TFID *fid)
{
    return true;
}
*/
