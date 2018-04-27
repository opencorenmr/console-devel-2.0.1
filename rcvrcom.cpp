#include <QStringList>
#include <QDebug>

#include "rcvrcom.h"
#include "intToHex.h"


QStringList TreceiverInfo::receiverCommands()
{
    if(digitalFilterOption==TDSPOption::FIR_Off) FDFScale=1.0;

    QStringList sl;
    //sl.append(TreceiverCommand::updateCO(dw()));

      double d=dw(); // s -> us conversion
      if(d==0) d=0.025; //
      d = 1.1*0.5/d;
      d = d/4.0;   //  (4 -> 40/10)    40: 80/2    80: ADC sampling rate
      if (d>0.5) d=0.5;

      TdigitalFilter *dFilter = new TdigitalFilter();
      dFilter->setMode(1);
      dFilter->setCutOff(d);
      QString s = dFilter->HexCoeffArray().toUpper();

      if(digitalFilterOption==TDSPOption::FIR_On)
      {
         FDFScale=1.0/(511*10/16); // (2^9-1)*(10 accum.)/(4 bit trimming)  (2016.5.31 Takeda)
         if(dFilter->mode()==1) FDFScale*=2*dFilter->cutOff();
      }

      delete dFilter;


    sl.append("set CO " + s);


    sl.append(TreceiverCommand::updateAL(al()));
    sl.append(TreceiverCommand::updateDW(dw()));
    sl.append(TreceiverCommand::updateNA(na()*nc()));  // nc()=2 for States (hypercomplex) acquisition
    sl.append(TreceiverCommand::updateND(nd()));
    sl.append(TreceiverCommand::updateST(step()));
    sl.append(TreceiverCommand::updateAR(ar()));
    sl.append(TreceiverCommand::updateAF(qdOption,digitalFilterOption));
    sl.append(TreceiverCommand::updateDO(DCOffset()));
    return sl;
}

//----------------------------------------------------------------------------
QString TreceiverCommand::updateND(int nd)
{
  if(nd<0) nd=0;
  return "set ND " + intToHex(nd,8);
}
//----------------------------------------------------------------------------
QString TreceiverCommand::updateAR(int ar)
{
  return "set AR " + intToHex(ar,8);
}
//----------------------------------------------------------------------------
QString TreceiverCommand::updateDO(int i)
{
   return "set DO " + intToHex(i,8);
}
//----------------------------------------------------------------------------
QString TreceiverCommand::updateNA(int na)
{
  if(na<=0) na=1;
  return "set NA " + intToHex(na,8);
}
//----------------------------------------------------------------------------
QString TreceiverCommand::updateAL(int al)
{
  if(al<1) al=1;
  return "set AL " + intToHex(al-1,8);
      // Do not forget to subtract 1, because the FPGA counts from 0 to 2^2-1.
}
//----------------------------------------------------------------------------
QString TreceiverCommand::updateDW(double dw)
{
   return TreceiverCommand::updateDW(dw,defaultADCSamplingRate);
}
QString TreceiverCommand::updateDW(double dw, double samplingRateinMHz)
{
    //  int a=round(1000000*dw/0.025); // 0.025us=1/40MHz
    int a=round(dw*0.5*samplingRateinMHz);
    return "set DW " + intToHex(a,8);

}
//----------------------------------------------------------------------------
QString TreceiverCommand::updateAF(TDSPOption::QDOption qd, TDSPOption::digitalFilterOption df)
{
  //  first bit   0: Quad demodulation On, 1: QD off
  //  second bit  0: no FIR, 1: FIR
  int a;
  if (qd==TDSPOption::QD_On && df==TDSPOption::FIR_On) a=2;
  else if (qd==TDSPOption::QD_On && df==TDSPOption::FIR_Off) a=0;
  else if (qd==TDSPOption::QD_Off && df==TDSPOption::FIR_On) a=3;
  else if (qd==TDSPOption::QD_Off && df==TDSPOption::FIR_Off) a=1;
  else a=0;

  return "set AF " + intToHex(a,8);
}
//----------------------------------------------------------------------------
QString TreceiverCommand::updateST(int st)
{
  return "set ST " + intToHex(st,8);
}
//----------------------------------------------------------------------------
QString TreceiverCommand::updateCO(double dw)
{
  double d=dw; // s -> us conversion
  if(d==0) d=0.025; //
  d = 1.1*0.5/d;
  d = d/4.0;   //  (4 -> 40/10)    40: 80/2    80: ADC sampling rate
  if (d>0.5) d=0.5;

  TdigitalFilter *dFilter = new TdigitalFilter();
  dFilter->setMode(1);
  dFilter->setCutOff(d);
  QString s = dFilter->HexCoeffArray().toUpper();
  delete dFilter;


  return "set CO " + s;

}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/*
 *
    static QString updateND(int nd); // number of dummy scans
    static QString updateAR(int ar); // array length
    static QString updateNA(int na); // number of acquisitions
    static QString updateAL(int al); // aquisition length
    static QString updateDW(double dw); // dwell time
    static QString updateAF(TDSPOption::QDOption qd, TDSPOption::digitalFilterOption df); // DSP setting
    static QString updateST(int st); //
    static QString updateCO(double co); // FIR coefficients

 */
/*
void TreceiverInfo::importReceiverParameters(TpulseProgram *ppg)
{
    setNA(ppg->variables.at(ppg->variableIndex("NA"))->value().toInt());
    setND(ppg->variables.at(ppg->variableIndex("ND"))->value().toInt());
    setAL(ppg->variables.at(ppg->variableIndex("AL"))->value().toInt());
    setDW(ppg->variables.at(ppg->variableIndex("DW"))->value().toDouble());
}

*/


