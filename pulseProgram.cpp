#include "pulseProgram.h"

#include <math.h>
#include <QDebug>
#include <QString>
#include <QFileInfo>

#define halfSamplingRate 40.0 // (80 MHz/2)

//-----------------------------------------------------------------------------
QString TcompiledPPG::comArgLineWord(TcompiledPPG *tc)
{
   return command(tc) + " " + arg(tc) + " " + lineWord(tc);
}
//-----------------------------------------------------------------------------
QString TcompiledPPG::command(TcompiledPPG *tc)
{
    QString s;
    switch(tc->binCommand)
    {
    case com_END_PPG: s="00"; break;
    case com_LOOP0: s="10"; break;
    case com_LOOP1: s="11"; break;
    case com_LOOP2: s="12"; break;
    case com_END_LOOP0: s="18"; break;
    case com_END_LOOP1: s="19"; break;
    case com_END_LOOP2: s="1A"; break;
    case com_JUMP: s="B0"; break;
    case com_JUMP_IF: s="B1"; break;
//    case com_OUT_PD: s="A0"; break;
    case com_OUT_PD: s="A1"; break;
    case com_OUT: s="A1"; break;
    case com_BURST: s="BA"; break;
    case com_WAIT_HIGH: s="A2"; break;
    case com_WAIT_LOW: s="A3"; break;
    case com_ASYNC: s="AA"; break;
    case com_ALL_SYNC: s="FF"; break;
    default: s="00"; break;
    }
    return s;
}
//-----------------------------------------------------------------------------
QString TcompiledPPG::arg(TcompiledPPG *tc)
{
   return tc->argument;
}
//-----------------------------------------------------------------------------
QString TcompiledPPG::lineWord(TcompiledPPG *tc)
{
  quint64 line=0;
  quint64 one64=1;
  for(int k=0; k<BITLENGTH; k++) if (tc->lineSet.contains(k)) line = (line | (one64 << k));
  return myHex(line,16);
}
//-----------------------------------------------------------------------------
    bool TppgLines::getStr() {
      while (   (currentLine.size()==0)
             && (currentPosition < source.size()-1)   )
        {
          currentPosition++;
          currentLine=source.at(currentPosition).trimmed();
        }
      if (currentLine.size()==0) return false; // empty
      else return true;
    }

    bool TppgLines::getStr(QChar qc) {
        while (   (!currentLine.contains(qc,Qt::CaseInsensitive))
               && (currentPosition < source.size()-1)   )
        {
          currentPosition++;
          currentLine += source.at(currentPosition).trimmed();
        }
      if ((!currentLine.contains(qc,Qt::CaseInsensitive))) return false; // empty
      else return true;
    }

    bool TppgLines::getStr(QChar qc, int*p) {
        *p=-1;
        while (   (!currentLine.contains(qc,Qt::CaseInsensitive))
               && (currentPosition < source.size()-1)   )
        {
          currentPosition++;
          currentLine += source.at(currentPosition).trimmed();
        }
      if ((!currentLine.contains(qc,Qt::CaseInsensitive))) return false; // empty
      else
      {
          *p=currentLine.indexOf(qc,0);
          return true;
      }
    }

    bool TppgLines::getStr(QChar qc1, QChar qc2) {
        while (   (!currentLine.contains(qc1,Qt::CaseInsensitive))
                  && (!currentLine.contains(qc2,Qt::CaseInsensitive))
                  && (currentPosition < source.size()-1)   )
        {
          currentPosition++;
          currentLine += source.at(currentPosition).trimmed();
        }
      if ((!currentLine.contains(qc1,Qt::CaseInsensitive))
              && (!currentLine.contains(qc2,Qt::CaseInsensitive))) return false; // empty
      else return true;
    }

    bool TppgLines::getStr(QChar qc1, QChar qc2, int *p1, int *p2) {
        *p1=-1; *p2=-1;
        while (   (!currentLine.contains(qc1,Qt::CaseInsensitive))
                  && (!currentLine.contains(qc2,Qt::CaseInsensitive))
                  && (currentPosition < source.size()-1)   )
        {
          currentPosition++;
          currentLine += source.at(currentPosition).trimmed();
        }
      if ((!currentLine.contains(qc1,Qt::CaseInsensitive))
              && (!currentLine.contains(qc2,Qt::CaseInsensitive))) return false; // empty
      else {
          *p1=currentLine.indexOf(qc1,0);
          *p2=currentLine.indexOf(qc2,0);
          return true;
      }
    }

    bool TppgLines::getStr(QChar qc1, QChar qc2, int *p)
    {
        int p1=-1; int p2=-1;
        while (   (!currentLine.contains(qc1,Qt::CaseInsensitive))
                  && (!currentLine.contains(qc2,Qt::CaseInsensitive))
                  && (currentPosition < source.size()-1)   )
        {
          currentPosition++;
          currentLine += source.at(currentPosition).trimmed();
        }
      if ((!currentLine.contains(qc1,Qt::CaseInsensitive))
              && (!currentLine.contains(qc2,Qt::CaseInsensitive))) return false; // empty
      else {
          p1=currentLine.indexOf(qc1,0);
          p2=currentLine.indexOf(qc2,0);
          if(p1<0) *p=p2;
          else if(p2<0) *p=p1;
          else if(p1<p2) *p=p1;
          else *p=p2;

          return true;
      }


    }

    bool TppgLines::getStr(QString qs) {
        while (   (!currentLine.contains(qs,Qt::CaseInsensitive))
               && (currentPosition < source.size()-1)   )
        {
          currentPosition++;
          currentLine += source.at(currentPosition).trimmed();
        }
      if (!currentLine.contains(qs,Qt::CaseInsensitive)) return false; // empty
      else return true;
    }

    bool TppgLines::getStr(QString qs1, QString qs2) {
        while (   (!currentLine.contains(qs1,Qt::CaseInsensitive))
               && (!currentLine.contains(qs2,Qt::CaseInsensitive))
               && (currentPosition < source.size()-1)   )
        {
          currentPosition++;
          currentLine += source.at(currentPosition).trimmed();
        }
      if ((!currentLine.contains(qs1,Qt::CaseInsensitive))
              && (!currentLine.contains(qs2,Qt::CaseInsensitive))) return false; // empty
      else return true;
    }
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
TpulseProgram::TpulseProgram(int channels) {
//  qDebug() << "constructor of TpulseProgram";

  CLK=160.0;  // default clock frequency in MHz

  FNOfChannels=channels;

  preambleInfo.clear(); // infomation generated by the compiler
  preambleCommands << "uses" << "clk" << "time" << "loop" << "amp"
                   << "int" << "phaselist" << "freq" << "aux" << "phase"
                   << "acqphase" << "const" << "function" << "version"
                   << "transform" << "double";

  //
  //  IMPORTANT: note that both phase and phaselist start with "phase"!!
  //    We therefore must check "phaselist" before "phase".


  ppgCommands << "pulse" << "delay" << "burst" << "ready" << "loop"
              << "endlp" << "endLoop" << "async" << "sync" << "wait_h" << "wait_l"
              << "exttrig" << "import" << "init" << "relax" << "sweep" << "endSweep";

  asyncPPGCommands << ppgCommands << "continue" << "standby";

  specialFunctionNames << "sin" << "cos" << "sinc" << "exp" << "tan" << "atan"
                       << "tanh" << "atan2" << "cosh" << "sinh"
                       << "log" << "log10" << "sech";


  reservedWords << preambleCommands << asyncPPGCommands << specialFunctionNames
                << "ACQTIME" << "NA" << "ND" << "AL" << "PD" << "DW" << "PHASELIST"
                << "TIME" << "LOOP" << "AUX" << "ACQ"
                << "INIT" << "PULSE" << "DELAY" << "LOOP" << "ENDLP"
                << "BURST" << "ASYNC" << "SYNC" << "RELAX"
                << "CONTINUE" << "STANDBY"
                << "FILE" << "GENERAL" << "ACQUISITION"
                << "TRANSFER" << "DIGITALFILTER"
                << "TABLE" << "SETUP";


  userDefinedFunctions.clear();

  AD9858Buffer.clear();
  for(int j=0; j<5; j++)
  {
      AD9858Buffer.append(QSet<int>());
      AD9858Buffer[j].clear();
  }

  gates.FNOfPhaseListGates.resize(channels);

  TVariable::NOfChannels=channels;  // initialization of the static data member of TVariable class.

  acqPhaseList.clear(); acqPhaseList.append(0);


  while (!compiledPPG_str.isEmpty()) delete compiledPPG_str.takeFirst();
  for(int ch=0; ch<channels; ch++) compiledPPG_str.append(new QStringList);

  while (!compiledPhaseCycle.isEmpty()) delete compiledPhaseCycle.takeFirst();
  for(int ch=0; ch<channels; ch++) compiledPhaseCycle.append(new QStringList);



//  while (!compiledPPG.isEmpty()) delete compiledPPG.takeFirst();
  compiledPPG.clear();
  QList<TcompiledPPG*> a;
  for (int ch=0; ch<channels; ch++) compiledPPG.append(a);
//  for (int ch=0; ch<channels; ch++) compiledPPG.append(new QList<TcompiledPPG>);

  //insertionCompiledPPG = new QList<TcompiledPPG>;


  while (!NOfLinesPerCommand.isEmpty()) delete NOfLinesPerCommand.takeFirst();
  for (int ch=0; ch<channels; ch++) NOfLinesPerCommand.append(new QList<int>);

  toFPGA.clear();
  updatedPPG.clear();

  errorCheckOnly=false;
  loopZero=false;

  isInsertActive=false;
}

//-----------------------------------------------------------------------------
TpulseProgram::~TpulseProgram() {

  while (!compiledPPG_str.isEmpty()) delete compiledPPG_str.takeFirst();
  while (!compiledPhaseCycle.isEmpty()) delete compiledPhaseCycle.takeFirst();
  //while (!compiledPPG.isEmpty()) delete compiledPPG.takeFirst();
  for(int ch=0; ch<channels(); ch++) while(!compiledPPG.at(ch).isEmpty()) delete compiledPPG[ch].takeLast();
  compiledPPG.clear();
  while (!NOfLinesPerCommand.isEmpty()) delete NOfLinesPerCommand.takeFirst();
  while (!asyncPPG.isEmpty()) delete asyncPPG.takeFirst();
  while (!variables.isEmpty()) delete variables.takeFirst();
  while (!insertionCompiledPPG.isEmpty()) delete insertionCompiledPPG.takeFirst();

  loadedFiles.clear();
  gateFilePaths.clear();
  lineSet.clear();
  AD9858Buffer.clear();

  toFPGA.clear();
  updatedPPG.clear();

  // qDebug() << "destructor of TpulseProgram";
}
//-----------------------------------------------------------------------------
TpulseProgram& TpulseProgram::operator=(const TpulseProgram& src)
{
  if(this==&src) return *this;

  while (!compiledPPG_str.isEmpty()) delete compiledPPG_str.takeFirst();
  while (!compiledPhaseCycle.isEmpty()) delete compiledPhaseCycle.takeFirst();
  //while (!compiledPPG.isEmpty()) delete compiledPPG.takeFirst();
  compiledPPG.clear();
  while (!NOfLinesPerCommand.isEmpty()) delete NOfLinesPerCommand.takeFirst();
  while (!asyncPPG.isEmpty()) delete asyncPPG.takeFirst();
  while (!variables.isEmpty()) delete variables.takeFirst();

  loadedFiles.clear();
  gateFilePaths.clear();
  lineSet.clear();
  AD9858Buffer.clear();

  toFPGA.clear();
  updatedPPG.clear();

  sourcePPG=src.sourcePPG;
  preamble=src.preamble;
  mainPPG=src.mainPPG;

  for(int k=0; k<src.asyncPPG.size(); k++)
  {
    asyncPPG.append(new TasyncPPG);
    asyncPPG[k]=src.asyncPPG[k];
  }

  //TODO: variable
  //TODO: gate
  //TODO: array

  return *this;
}
//-----------------------------------------------------------------------------
void removeComment(QString &s) {
  int index=s.indexOf('{',0);
  if (index>-1) {
    s.truncate(index);
  }
  s=s.trimmed();

  index=s.indexOf("//",0);
  if(index>-1)
  {
      s.truncate(index);
  }
  s=s.trimmed();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool TpulseProgram::p_acqPhase() {

  if(constVar) {error=true;errorMessage=QString(Q_FUNC_INFO)+ ": Cannot use \"const\" for \"acqphase\""; return false;}

  bool e=processPreambleCommand2();
  if (!e) {return false;}

  QStringList list1 = argument.split(",");

  if (list1.size()<1) {
     error=true;
     errorMessage=QString(Q_FUNC_INFO) + ": Phase-list is empty.";
     return false;
  }
  if (list1.size()>MAX_PHASELIST_LENGTH) {
     error=true;
     errorMessage=QString(Q_FUNC_INFO) + ": Phase-list length cannot exceed 128.";
     return false;
  }

  double d;
  //bool ok;

  for (int i=0; i<list1.size(); i++) {
      list1[i]=list1[i].trimmed();
      list1[i].replace(QString("-x"), QString("180.0"), Qt::CaseInsensitive);
      list1[i].replace(QString("+x"), QString("0.0"), Qt::CaseInsensitive);
      list1[i].replace(QString("x"), QString("0.0"), Qt::CaseInsensitive);
      list1[i].replace(QString("-y"), QString("270.0"), Qt::CaseInsensitive);
      list1[i].replace(QString("+y"), QString("90.0"), Qt::CaseInsensitive);
      list1[i].replace(QString("y"), QString("90.0"), Qt::CaseInsensitive);
      //d=list1[i].toDouble(&ok);
      //if (!ok) {
      //  error=true;
      //  errorMessage=QString(Q_FUNC_INFO) + ": Invalid expression: \""+ list1.at(i) + "\".";
      //  return false;
      //}

  }


  QList<double> list2;

  for (int i=0; i<list1.size(); i++) {
      QString qs0=list1.at(i);
      if(!evalArg(qs0)) {errorMessage=QString(Q_FUNC_INFO)+": Invalid expression: " + list1.at(i); return false;}
      d=evalArgResult;

      //d=list1.at(i).toDouble(&ok);

      while (d<0) d+=360.0;
      while (d>=360) d-=360.0;

      if (!(d==0.0 || d==90.0 || d==180.0 || d==270.0)) {
        error=true;
        errorMessage=QString(Q_FUNC_INFO) +": acqphase = " + QString::number(d)
                      + ", but Only x (0), y (90), -x (180), or -y (270) are allowed for acqphaselist.";
        return false;
      }
      list2.append(d);

  }


  // No error. Let us proceed!
  acqPhaseList.clear();
  for (int i=0; i<list2.size(); i++) {
      if(list2.at(i)==0.0) acqPhaseList.append(0);
      else if (list2.at(i)==90.0) acqPhaseList.append(1);
      else if (list2.at(i)==180.0) acqPhaseList.append(2);
      else if (list2.at(i)==270.0) acqPhaseList.append(3);

//      qDebug() << list1.at(i).toDouble();

  }

  QString qs;
  for (int k=0; k<acqPhaseList.size(); k++) {
      qs+= QString::number(acqPhaseList.at(k),'g',12);
      if (k<acqPhaseList.size()-1) qs += ", ";
  }
  preambleInfo.append("acqphase: "
                      + QString::number(acqPhaseList.size()) + " items. ("
                      + qs + ")"
                      );

  acqPhaseDefined=true;

  return true;
}
//-----------------------------------------------------------------------------
bool TpulseProgram::p_phaseList() {

  if(constVar) {error=true;errorMessage=QString(Q_FUNC_INFO)+ ": Cannot use \"const\" for \"phaselist\""; return false;}

  if (!preamble.getStr())//  end of preamble without finding "=" !
    {errorMessage="Declaration is missing."; return false;}

  if (!preamble.getStr('=')) {errorMessage="\"=\" was not found."; return false;}
  int index=preamble.currentLine.indexOf('=',0);
  varName=preamble.currentLine.left(index).trimmed();
  if (!checkReservedWordDeclaration(varName)) return false;
  if (!checkMultipleDeclaration(varName)) return false;
  if (!checkVariableNameExpression(varName)) return false;

  preamble.currentLine=preamble.currentLine.remove(0,index+1).trimmed();

  if(!preamble.getStr())  // end of preamble without finding an argument after '='.
    {error=true; errorMessage=QString(Q_FUNC_INFO)+": Argument is expected after \"=\"."; return false;}

  if(!preamble.currentLine.startsWith('(')) {
      errorMessage=QString(Q_FUNC_INFO)+": \"(\" is expected after \"=\"."; return false;}
  preamble.currentLine=preamble.currentLine.remove(0,1).trimmed();
  if(!preamble.getStr(';'))
     {errorMessage=QString(Q_FUNC_INFO)+": \";\" is expected before phase list."; return false;}
  index=preamble.currentLine.indexOf(";",0);

  QString t=preamble.currentLine.left(index).trimmed();   // t should be "ch*"
  if (!t.startsWith("ch", Qt::CaseInsensitive)) {
    error=true;
    errorMessage=QString(Q_FUNC_INFO) + ": \"ch\" is expected after \"(\"";
    return false;
  }

  t.remove(0,2);
  bool ok;
  int ch = t.toInt(&ok, 10);
  if (!ok) {error=true; errorMessage=QString(Q_FUNC_INFO) + ": Invalid channel number."; return false;}

  ch--; // IMPORTANT! ch=1..3 --> 0..2

  if (gates.FNOfPhaseListGates.at(ch)+1>MAX_PHASELIST_ITEMS) { // --> ERROR!!
    error=true;
    errorMessage=QString(Q_FUNC_INFO)+ ": Phase-list items cannot exceed " + QString::number(MAX_PHASELIST_ITEMS);
    return false;
  }

  preamble.currentLine=preamble.currentLine.remove(0,index+1);

  /*
    bool parenthesisClosed=false;
    while (!parenthesisClosed && (preample.currentPosition<preamble.source.size()))
    {
        if(preample.currentLine.count('(') <= preample.currentLine.count(')')) parenthesisClosed=true;
        else
        {
            preample.currentPosition++;
            if(preample.currentPosition<preample.source.size()-1)
            {
               preample.currentLine+=preample.source.at(preample.currentPosition).trimmed();
            }
        }
    }

    //qDebug() << QString(Q_FUNC_INFO) << "1";

    if(preample.currentPosition>preample.source.size()-1) preample.currentPosition=preample.source.size()-1;

    if(!parenthesisClosed)
    {
          errorMessage=QString(Q_FUNC_INFO) + "Parenthesis is not closed.";
          return false;
    }

   */

  if(!preamble.getStr(')'))
    {errorMessage=QString(Q_FUNC_INFO) + ": \")\" is expected at the end of phaselist."; return false;}


  index=preamble.currentLine.indexOf(')',0);
  t=preamble.currentLine.left(index);  // t should be *,*,....

  preamble.currentLine.remove(0,index+1);
  if(!preamble.getStr())
    { errorMessage=QString(Q_FUNC_INFO)+": \";\" is expected."; return false;}
  if(!preamble.currentLine.startsWith(';'))
    { errorMessage=QString(Q_FUNC_INFO)+": \";\" is expected."; return false;}

  preamble.currentLine.remove(0,1); // remove ";"



  QStringList list1 = t.split(",");

  if (list1.size()<1) {
     error=true;
     errorMessage=QString(Q_FUNC_INFO) + ": Phase-list is empty.";
     return false;
  }

  if (list1.size()>MAX_PHASELIST_LENGTH) {
     error=true;
     errorMessage=QString(Q_FUNC_INFO) + ": Phase-list length cannot exceed "+QString::number(MAX_PHASELIST_LENGTH)+".";
     return false;
  }

  double d;

  for (int i=0; i<list1.size(); i++) {
      list1[i]=list1[i].trimmed();
      list1[i].replace(QString("-x"), QString("180.0"), Qt::CaseInsensitive);
      list1[i].replace(QString("+x"), QString("0.0"), Qt::CaseInsensitive);
      list1[i].replace(QString("x"), QString("0.0"), Qt::CaseInsensitive);
      list1[i].replace(QString("-y"), QString("270.0"), Qt::CaseInsensitive);
      list1[i].replace(QString("+y"), QString("90.0"), Qt::CaseInsensitive);
      list1[i].replace(QString("y"), QString("90.0"), Qt::CaseInsensitive);
     // d=list1[i].toDouble(&ok);
     // if (!ok) {
     //   error=true;
     //   errorMessage=QString(Q_FUNC_INFO) + ": Invalid expression: \""+ list1.at(i) + "\".";
     //   return false;
     // }
  }

  // no error!
  gates.FNOfPhaseListGates[ch]++;
  gates.Items.append(new TphaseListWord(gates.FNOfPhaseListGates[ch]));
  gates.Items[gates.Items.size()-1]->setChannel(ch);
  gates.Items[gates.Items.size()-1]->setOrder(gates.FNOfPhaseListGates[ch]);
  gates.Items[gates.Items.size()-1]->name=varName;

  gates.Items[gates.Items.size()-1]->setPhaseListLength(list1.size());
  for (int i=0; i<list1.size(); ++i) {
      QString qs0=list1.at(i);
      if(!evalArg(qs0)) {errorMessage=QString(Q_FUNC_INFO)+": Invalid expression: " + list1.at(i); return false;}
      d=evalArgResult;
      //d=list1.at(i).toDouble();
      gates.Items[gates.Items.size()-1]->setPhaseList(i,d);
  }

  QString qs;
  for (int k=0; k<gates.Items.at(gates.Items.size()-1)->phaseListLength(); k++) {
      qs+= QString::number(gates.Items.at(gates.Items.size()-1)->phaseList(k),'g',12);
      if (k<gates.Items.at(gates.Items.size()-1)->phaseListLength()-1) qs += ", ";
  }

  preambleInfo.append("phaselist "  +  varName + "(ch" + QString::number(ch+1) +"): "
                      + QString::number(gates.FNOfPhaseListGates[ch]) + "-th item."
                      + "("
                       + qs + ")");

  return true;
}
//-----------------------------------------------------------------------------
bool TpulseProgram::processPreambleCommand2()
{
    int index=preamble.currentLine.indexOf('=',0);
    if(index<0) // = was not found!
      {error=true; errorMessage="\"=\" was not found."; return false;}
    varName=preamble.currentLine.left(index).trimmed();
    preamble.currentLine=preamble.currentLine.remove(0,index+1).trimmed();

    if(!preamble.getStr())  // end of preamble without finding an argument after '='.
      {error=true; errorMessage="Argument is expected after \"=\"."; return false;}

    //qDebug()<<QString(Q_FUNC_INFO)<<preamble.currentLine;

    int indexC=preamble.currentLine.indexOf(';',0); // look for a colon

    if(indexC<0) // ';' was not found.
      {error=true; errorMessage="\";\" is missing."; return false;}
    if(indexC==0) // Came to ';' or '(' without an argument.
    {
        error=true;
        errorMessage="Argument is expected before \"" + QString(preamble.currentLine.at(0))+"\".";
        return false;
    }

    index=indexC;

    argument=preamble.currentLine.left(index).trimmed().remove(' '); // it should contain the argument (value)
    preamble.currentLine=preamble.currentLine.remove(0,index+1).trimmed();
    //qDebug()<<QString(Q_FUNC_INFO)<<argument;

    return true;

}
//-----------------------------------------------------------------------------
/*
bool TpulseProgram::processPreambleCommand()
{
  int index=preamble.currentLine.indexOf('=',0);
  if(index<0) // = was not found!
    {error=true; errorMessage="\"=\" was not found."; return false;}
  varName=preamble.currentLine.left(index).trimmed();
  preamble.currentLine=preamble.currentLine.remove(0,index+1).trimmed();

  if(!preamble.getStr())  // end of preamble without finding an argument after '='.
    {error=true; errorMessage="Argument is expected after \"=\"."; return false;}

  int indexC=preamble.currentLine.indexOf(';',0); // look for a colon
  int indexP=preamble.currentLine.indexOf('(',0); // look for a left parenthesis

  if(indexC<0) // ';' was not found.
    {error=true; errorMessage="\";\" is missing."; return false;}
  if(indexC==0 || indexP==0) // Came to ';' or '(' without an argument.
  {
      error=true;
      errorMessage="Argument is expected before \"" + QString(preamble.currentLine.at(0))+"\".";
      return false;
  }

  if(indexP<0) index=indexC;  // no caption
  else if(indexP<indexC) index=indexP;  // caption
  else {error=true; errorMessage="\"(\" should come before \";\"."; return false;}

  argument=preamble.currentLine.left(index).trimmed(); // it should contain the argument (value)
  preamble.currentLine=preamble.currentLine.remove(0,index+1).trimmed();
  //qDebug()<<argument;

  caption.clear();
  if (indexP>0) {  // get a string between '(' and ')'
    int indexQ=preamble.currentLine.indexOf(')',0);
    if (indexQ<0) {error=true; errorMessage="\")\" is missing."; return false;}
    caption=preamble.currentLine.left(indexQ);
    preamble.currentLine=preamble.currentLine.remove(0,indexQ+1).trimmed();

    if (!preamble.currentLine.startsWith(';'))
      {error=true; errorMessage="\";\" is expected."; return false;}
    preamble.currentLine=preamble.currentLine.remove(0,1).trimmed(); // remove ';'
  }
  return true;
}
*/
bool TpulseProgram::processPreambleCommand()
{
  int index=preamble.currentLine.indexOf('=',0);
  if(index<0) // = was not found!
    {error=true; errorMessage="\"=\" was not found."; return false;}
  varName=preamble.currentLine.left(index).trimmed();
  preamble.currentLine=preamble.currentLine.remove(0,index+1).trimmed();

  if(!preamble.getStr())  // end of preamble without finding an argument after '='.
    {error=true; errorMessage="Argument is expected after \"=\"."; return false;}

  int indexC=preamble.currentLine.indexOf(';',0); // look for a colon
  int indexP=preamble.currentLine.indexOf('(',0); // look for a left parenthesis
      // Note that parentheses could exist in the next variable declaration in the same line!

  if(indexC<0) // ';' was not found.
    {error=true; errorMessage="\";\" is missing."; return false;}
  if(indexC==0 || indexP==0) // Came to ';' or '(' without an argument.
  {
      error=true;
      errorMessage="Argument is expected before \"" + QString(preamble.currentLine.at(0))+"\".";
      return false;
  }

  if(indexP>0 && indexP<indexC) index=indexP; // captions comes first
  else index=indexC;   // no caption

  argument=preamble.currentLine.left(index).trimmed(); // it should contain the argument (value)
  preamble.currentLine=preamble.currentLine.remove(0,index+1).trimmed();

  caption.clear();
  if (index==indexP) {  // get a string between '(' and ')'
    int indexQ=preamble.currentLine.indexOf(')',0);
    if (indexQ<0) {error=true; errorMessage="\")\" is missing."; return false;}
    caption=preamble.currentLine.left(indexQ);
    preamble.currentLine=preamble.currentLine.remove(0,indexQ+1).trimmed();

    if (!preamble.currentLine.startsWith(';'))
      {error=true; errorMessage="\";\" is expected."; return false;}
    preamble.currentLine=preamble.currentLine.remove(0,1).trimmed(); // remove ';'
  }
  return true;
}

//-----------------------------------------------------------------------------
bool TpulseProgram::p_transform()
{
    if(constVar) {error=true;errorMessage=QString(Q_FUNC_INFO)+ ": Cannot use \"const\" for \"version\""; return false;}
    bool e=processPreambleCommand2();
    if (!e) {return false;}

    QString gName=varName.trimmed();
    int gIndex=gates.nameIndex(gName);
    if(gIndex<0)
    {
        error=true;
        errorMessage=QString(Q_FUNC_INFO)+": Gate " + gName + " was not found."; return false;
    }
    gates.Items[gIndex]->transformFunction=argument;

    if(gates.Items[gIndex]->KindOfGate()==GLogic)
    {
        error=true;
        errorMessage=QString(Q_FUNC_INFO)+": Cannot transform the logic gate " + gName + "."; return false;
    }
    if(gates.Items[gIndex]->KindOfGate()==GPhase)
    {
        error=true;
        errorMessage=QString(Q_FUNC_INFO)+": Cannot transform the phase gate " + gName + "."; return false;
    }
    if(gates.Items[gIndex]->KindOfGate()==GPhaselist)
    {
        error=true;
        errorMessage=QString(Q_FUNC_INFO)+": Cannot transform the phaselist gate " + gName + "."; return false;
    }
    if(gates.Items[gIndex]->KindOfGate()==GRFIQ)
    {
        error=true;
        errorMessage=QString(Q_FUNC_INFO)+": Cannot transform the rfiq gate " + gName + "."; return false;
    }

    preambleInfo.append("transform: " + gName + "=" + argument);
    return true;
}
//-----------------------------------------------------------------------------
bool TpulseProgram::p_version()
{
    if(constVar) {error=true;errorMessage=QString(Q_FUNC_INFO)+ ": Cannot use \"const\" for \"version\""; return false;}

    bool e=processPreambleCommand2();
    if (!e) {return false;}

    bool ok;
    double d;

    d = argument.toDouble(&ok);
    if (!ok) {error=true; errorMessage=QString(Q_FUNC_INFO)+ ": Invalid version number."; return false;}

    if(d>CURRENT_VERSION)
    {error=true; errorMessage=QString(Q_FUNC_INFO)+ ": Incompatible version number."; return false;}

    setVersion(d);

    preambleInfo.append("version: " + QString::number(d,'g') );

    return true;


}
//-----------------------------------------------------------------------------
bool TpulseProgram::p_function()
{
    if(constVar) {error=true;errorMessage=QString(Q_FUNC_INFO)+ ": Cannot use \"const\" for \"function\""; return false;}

    bool e=processPreambleCommand2();
    if (!e) {return false;}

    argument.remove(QRegExp("\\s+"));  // Important! We remove any space (or tab) characters.
    userDefinedFunctions.append(varName + "=" + argument);
    preambleInfo.append("function: " + varName + "=" + argument );

 // qDebug() << preambleInfo.last();

    return true;

}
//-----------------------------------------------------------------------------
bool TpulseProgram::p_uses() {

  if(constVar) {error=true;errorMessage=QString(Q_FUNC_INFO)+ ": Cannot use \"const\" for \"uses\""; return false;}

  bool e=processPreambleCommand2();
  if (!e) {return false;}

  bool found=false;
  QString fn;
  QFileInfo fi;
  for(int k=0; k<gateFilePaths.size(); ++k) {
    fn=gateFilePaths.at(k);
    if(!fn.endsWith('/')) {fn.append('/');}
    fn += argument;
    fi.setFile(fn);
    if (fi.exists()) {found=true; break;}
  }

  if (!found) {
    error=true; errorMessage=QString(Q_FUNC_INFO)+": Gate file " + argument + " was not found."; return false;
  }


  setGateFileName(fn);
  if (!gates.readFromFile(fn)) {
      error=true;
      errorMessage = "In the gate file " + fn + ": " + gates.errorMessage;
      return false;
  }

  preambleInfo.append("uses :" + fn );

  return true;
}
//-----------------------------------------------------------------------------
bool TpulseProgram::p_clk() {

  if(constVar) {error=true;errorMessage=QString(Q_FUNC_INFO)+ ": \"const\" cannot be used for \"clk\""; return false;}

  bool e=processPreambleCommand2();
  if (!e) {return false;}

  bool ok;
  double d;

  d = argument.toDouble(&ok);
  if (!ok) {error=true; errorMessage=QString(Q_FUNC_INFO)+ ": Invalid number."; return false;}

  CLK=d;

  preambleInfo.append("clk: " + QString::number(d,'g') );

  return true;
}
//-----------------------------------------------------------------------------
bool TpulseProgram::p_time() {

  if (!processPreambleCommand()) return false;
  if (!checkReservedWordDeclaration(varName)) return false;
  if (!checkMultipleDeclaration(varName)) return false;
  if (!checkVariableNameExpression(varName)) return false;

  TVariable::Unit uni;
  QChar unitOfTime=argument.at(argument.size()-1);
  switch (unitOfTime.toLatin1()) {
    case 'n':
    case 'N': uni=TVariable::Nano; break;
    case 'u':
    case 'U': uni=TVariable::Micro; break;
    case 'm':
    case 'M': uni=TVariable::Milli; break;
    case 's':
    case 'S': uni=TVariable::Unity; break;
    default:
      error=true; errorMessage=QString(Q_FUNC_INFO)+": Unit of time (n,u,m, or s) is required.";return false;
      break;
  }

  // remove the unit string;
  argument.chop(1);
  argument=argument.trimmed();

  bool ok; double d;
  d=argument.toDouble(&ok);
  if(!ok) {error=true; errorMessage=QString(Q_FUNC_INFO)+": Invalid number."; return false;}

  variables.append(new TVariable(varName,TVariable::timeVariable,QVariant(d),uni));
  d=variables.at(variables.size()-1)->value().toDouble();
 // if(d<MIN_TIME_VALUE || d>MAX_TIME_VALUE)
 //   {error=true; errorMessage=QString(Q_FUNC_INFO)+": Out of range."; return false;}

  variables[variables.size()-1]->setDeclarationPosition(preamble.currentPosition);
  variables[variables.size()-1]->setCaption(caption);

  variables[variables.size()-1]->setConst(constVar);
  if (constVar) constVar=false;


  preambleInfo.append("time "  +  varName + "(" + caption +"): "
                      + QString::number(variables.at(variables.size()-1)->numeric().toDouble(),'g')
                      + variables.at(variables.size()-1)->unitString() );

  return true;
}
//-----------------------------------------------------------------------------
bool TpulseProgram::p_amp() {

  if (!processPreambleCommand()) return false;
  if (!checkReservedWordDeclaration(varName)) return false;
  if (!checkMultipleDeclaration(varName)) return false;
  if (!checkVariableNameExpression(varName)) return false;

  bool ok; double d;
  d=argument.toDouble(&ok);
  if(!ok) {error=true; errorMessage=QString(Q_FUNC_INFO)+": Invalid number."; return false;}
//  if(d<MIN_AMP_VALUE || d>MAX_AMP_VALUE)
//    {error=true; errorMessage=QString(Q_FUNC_INFO)+": Out of range."; return false;}

  variables.append(new TVariable(varName,TVariable::ampVariable,QVariant(d),TVariable::Unity));
  variables[variables.size()-1]->setDeclarationPosition(preamble.currentPosition);
  variables[variables.size()-1]->setCaption(caption);

  if (constVar) {
    variables[variables.size()-1]->setConst(true);
    constVar=false;
  }
  else variables[variables.size()-1]->setConst(false);

  preambleInfo.append("amp "  +  varName + "(" + caption +"): " + QString::number(d,'g') );

  return true;
}
//-----------------------------------------------------------------------------
bool TpulseProgram::p_phase() {

  if (!processPreambleCommand()) return false;
  if (!checkReservedWordDeclaration(varName)) return false;
  if (!checkMultipleDeclaration(varName)) return false;
  if (!checkVariableNameExpression(varName)) return false;

  bool ok; double d;
  d=argument.toDouble(&ok);
  if(!ok) {error=true; errorMessage=QString(Q_FUNC_INFO)+": Invalid number."; return false;}

  variables.append(new TVariable(varName,TVariable::phaseVariable,QVariant(d),TVariable::Unity));
  variables[variables.size()-1]->setDeclarationPosition(preamble.currentPosition);
  variables[variables.size()-1]->setCaption(caption);

  if (constVar) {
    variables[variables.size()-1]->setConst(true);
    constVar=false;
  }
  else variables[variables.size()-1]->setConst(false);

  preambleInfo.append("phase "  +  varName + "(" + caption +"): " + QString::number(d,'g') );

  return true;
}
//-----------------------------------------------------------------------------
bool TpulseProgram::p_freq() {

  if (!processPreambleCommand()) return false;
  if (!checkReservedWordDeclaration(varName)) return false;
  if (!checkMultipleDeclaration(varName)) return false;
  if (!checkVariableNameExpression(varName)) return false;

  bool ok; double d;
  d=argument.toDouble(&ok);
  if(!ok) {error=true; errorMessage=QString(Q_FUNC_INFO)+": Invalid number."; return false;}

//qDebug() << argument <<  "   freq:" << QString("%1").arg(d,0,'g',13);

//  variables.append(new TVariable(varName,TVariable::freqVariable,QVariant(d),TVariable::Mega));
  variables.append(new TVariable(varName,TVariable::freqVariable,QVariant(d),TVariable::Unity));
  variables[variables.size()-1]->setDeclarationPosition(preamble.currentPosition);
  variables[variables.size()-1]->setCaption(caption);

  // qDebug() << "d: " << d;
  // qDebug() << "v: " << variables[variables.size()-1]->value().toDouble();

  if (constVar) {
    variables[variables.size()-1]->setConst(true);
    constVar=false;
  }
  else variables[variables.size()-1]->setConst(false);

  preambleInfo.append("freq "  +  varName + "(" + caption +"): " + QString::number(d,'g') );

  return true;
}
//-----------------------------------------------------------------------------
bool TpulseProgram::p_int() {

  if (!processPreambleCommand()) return false;
  if (!checkReservedWordDeclaration(varName)) return false;
  if (!checkMultipleDeclaration(varName)) return false;
  if (!checkVariableNameExpression(varName)) return false;

  bool ok; int i;
  i=argument.toInt(&ok);
  if(!ok) {error=true; errorMessage=QString(Q_FUNC_INFO)+": Invalid number."; return false;}

  variables.append(new TVariable(varName,TVariable::intVariable,QVariant(i),TVariable::Unity));
  variables[variables.size()-1]->setDeclarationPosition(preamble.currentPosition);
  variables[variables.size()-1]->setCaption(caption);

  if (constVar) {
    variables[variables.size()-1]->setConst(true);
    constVar=false;
  }
  else variables[variables.size()-1]->setConst(false);

  preambleInfo.append("int "  +  varName + "(" + caption +"): " + QString::number(i) );

  return true;
}
//-----------------------------------------------------------------------------
bool TpulseProgram::p_double()
{
    if (!processPreambleCommand()) return false;
    if (!checkReservedWordDeclaration(varName)) return false;
    if (!checkMultipleDeclaration(varName)) return false;
    if (!checkVariableNameExpression(varName)) return false;

    bool ok; double d;
    d=argument.toDouble(&ok);
    if(!ok) {error=true; errorMessage=QString(Q_FUNC_INFO)+": Invalid number."; return false;}

    variables.append(new TVariable(varName,TVariable::doubleVariable,QVariant(d),TVariable::Unity));
    variables[variables.size()-1]->setDeclarationPosition(preamble.currentPosition);
    variables[variables.size()-1]->setCaption(caption);

    if (constVar) {
      variables[variables.size()-1]->setConst(true);
      constVar=false;
    }
    else variables[variables.size()-1]->setConst(false);

    preambleInfo.append("int "  +  varName + "(" + caption +"): " + QString::number(d) );

    return true;


  return true;
}
//-----------------------------------------------------------------------------
bool TpulseProgram::p_loop() {

  if (!processPreambleCommand()) return false;
  if (!checkReservedWordDeclaration(varName)) return false;
  if (!checkMultipleDeclaration(varName)) return false;
  if (!checkVariableNameExpression(varName)) return false;

  bool ok; int i;
  i=argument.toInt(&ok);
  if(!ok) {error=true; errorMessage=QString(Q_FUNC_INFO)+": Invalid number."; return false;}

  variables.append(new TVariable(varName,TVariable::loopVariable,QVariant(i),TVariable::Unity));
  variables[variables.size()-1]->setDeclarationPosition(preamble.currentPosition);
  variables[variables.size()-1]->setCaption(caption);

  if (constVar) {
    variables[variables.size()-1]->setConst(true);
    constVar=false;
  }
  else variables[variables.size()-1]->setConst(false);

  preambleInfo.append("loop "  +  varName + "(" + caption +"): " + QString::number(i) );

  return true;
}
//-----------------------------------------------------------------------------
bool TpulseProgram::p_aux() {
//  if(constVar) {errorMessage="\"const\" cannot be used for \"aux\"."; return false;}
  if (!processPreambleCommand()) return false;
  if (!checkMultipleDeclaration(varName)) return false;
  if (!checkVariableNameExpression(varName)) return false;
  bool ok;
  if (QString::compare(varName,"NA",Qt::CaseInsensitive)==0)
    { int i;
      i=argument.toInt(&ok);
      if(!ok) {errorMessage="Invalid number."; return false;}
      if (i<1) {errorMessage="NA must be larger than 0."; return false;}
      variables.append(new TVariable(varName,TVariable::naVariable,QVariant(i),TVariable::Unity));
      variables[variables.size()-1]->setDeclarationPosition(preamble.currentPosition);
      variables[variables.size()-1]->setCaption(caption);
      NADefined=true;
      setOrigNA(i);
        // We keep the value, so that we can restore it whenever it is modified
        // for multi-dimensional experiments (hypercomplex, etc.)
    }
  else if (QString::compare(varName,"ND",Qt::CaseInsensitive)==0) // Number of Dummy scans (New 2015 Jan)
    { int i;
      i=argument.toInt(&ok);
      if(!ok) {errorMessage="Invalid number."; return false;}
      if (i<0) {errorMessage="ND must not be negative."; return false;}
      variables.append(new TVariable(varName,TVariable::ndVariable,QVariant(i),TVariable::Unity));
      variables[variables.size()-1]->setDeclarationPosition(preamble.currentPosition);
      variables[variables.size()-1]->setCaption(caption);
      NDDefined=true;
    }
  else if (QString::compare(varName,"AL",Qt::CaseInsensitive)==0)
    {
      int i;
      i=argument.toInt(&ok);
      if(!ok) {errorMessage="Invalid number."; return false;}
      if (i<MIN_AL_VALUE) {errorMessage="AL cannot be smaller than " + QString::number(MIN_AL_VALUE); return false;}
      if (i>MAX_AL_VALUE) {errorMessage="AL cannot exceed " + QString::number(MAX_AL_VALUE); return false;}
      variables.append(new TVariable(varName,TVariable::alVariable,QVariant(i),TVariable::Unity));
      variables[variables.size()-1]->setDeclarationPosition(preamble.currentPosition);
      variables[variables.size()-1]->setCaption(caption);
      ALDefined=true;
  }
  else if (QString::compare(varName,"DW",Qt::CaseInsensitive)==0)
  {
      TVariable::Unit uni;
      QChar unitOfTime=argument.at(argument.size()-1);
      switch (unitOfTime.toLatin1()) {
        case 'n':
        case 'N': uni=TVariable::Nano; break;
        case 'u':
        case 'U': uni=TVariable::Micro; break;
        case 'm':
        case 'M': uni=TVariable::Milli; break;
        case 's':
        case 'S': uni=TVariable::Unity; break;
        default:
          errorMessage=QString(Q_FUNC_INFO)+": Unit of time (n,u,m, or s) is required.";return false;
          break;
      }

      // remove the unit string;
      argument.chop(1);
      argument=argument.trimmed();

      double d;
      d=argument.toDouble(&ok);
      if(!ok) {errorMessage=QString(Q_FUNC_INFO)+": Invalid number."; return false;}

      variables.append(new TVariable(varName,TVariable::dwVariable,QVariant(d),uni));
      d=variables.at(variables.size()-1)->value().toDouble();
      if(d<MIN_DW_VALUE || d>MAX_DW_VALUE) {errorMessage=QString(Q_FUNC_INFO)+": Out of range."; return false;}
      variables[variables.size()-1]->setDeclarationPosition(preamble.currentPosition);
      variables[variables.size()-1]->setCaption(caption);
      DWDefined=true;

  }
  else if (QString::compare(varName,"PD",Qt::CaseInsensitive)==0)
  {

      TVariable::Unit uni;
      QChar unitOfTime=argument.at(argument.size()-1);
      switch (unitOfTime.toLatin1()) {
        case 'n':
        case 'N': uni=TVariable::Nano; break;
        case 'u':
        case 'U': uni=TVariable::Micro; break;
        case 'm':
        case 'M': uni=TVariable::Milli; break;
        case 's':
        case 'S': uni=TVariable::Unity; break;
        default:
          errorMessage=QString(Q_FUNC_INFO)+": Unit of time (n,u,m, or s) is required.";return false;
          break;
      }

      // remove the unit string;
      argument.chop(1);
      argument=argument.trimmed();

      double d;
      d=argument.toDouble(&ok);
      if(!ok) {errorMessage=QString(Q_FUNC_INFO)+": Invalid number."; return false;}

      if(!TVariable::checkRange(TVariable::pdVariable,QVariant(d),uni))
        {errorMessage=QString(Q_FUNC_INFO)+": Out of range."; return false;}

   // d=variables.at(variables.size()-1)->value().toDouble();
   // if(d<MIN_TIME_VALUE || d>MAX_TIME_VALUE) {errorMessage=QString(Q_FUNC_INFO)+": Out of range."; return false;}

      variables.append(new TVariable(varName,TVariable::pdVariable,QVariant(d),uni));
      variables[variables.size()-1]->setDeclarationPosition(preamble.currentPosition);
      variables[variables.size()-1]->setCaption(caption);
      PDDefined=true;

  }
  else
    {error=true; errorMessage= varName +" is an unknown aux parameter."; return false;}

  if (constVar) {
    variables[variables.size()-1]->setConst(true);
    constVar=false;
  }
  else variables[variables.size()-1]->setConst(false);

  //qDebug()<<variables[variables.size()-1]->name()<<variables[variables.size()-1]->isConst();

  return true;

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool TpulseProgram::checkVariableNameExpression(QString vn)
{
    if(vn.isEmpty()) {errorMessage="Empty variable name."; return false;}

    //
    // TODO: if vn contains weird characters, begins with a number, etc.
    //

    return true;
}

//-----------------------------------------------------------------------------
bool TpulseProgram::checkReservedWordDeclaration(QString vn){
    if (reservedWords.contains(vn,Qt::CaseInsensitive)) {
      errorMessage = vn +" is a reserved word and cannot be used.";
      return false;
    }
  return true;
}

//-----------------------------------------------------------------------------
bool TpulseProgram::checkMultipleDeclaration(QString vn){

  bool ok=true;

  for(int k=0; k<variables.size(); ++k) {
    int x=QString::compare(vn, variables.at(k)->name(), Qt::CaseInsensitive);  // x == 0 ?
    if (x==0) ok=false;
  }

  if (!ok) errorMessage="Multiple declaration of \"" + vn + "\".";

  return ok;
}
//-----------------------------------------------------------------------------
bool TpulseProgram::processPreamble() {

  while (!variables.isEmpty()) delete variables.takeFirst();
  gates.initialize();
  acqPhaseList.clear();
  preambleInfo.clear();
  setVersion(-1);  // Firstly, let the version undefined.

  NADefined=false;
  NDDefined=false;
  DWDefined=false;
  PDDefined=false;
  ALDefined=false;
  acqPhaseDefined=false;

  bool e; // error detector

  constVar=false;

  // Read the 1st line
  preamble.currentPosition=0;
  if (preamble.currentPosition<preamble.source.size())
      preamble.currentLine=preamble.source.at(preamble.currentPosition).trimmed();

  // Empty check
  if (!preamble.getStr()) {errorMessage="Preamble is empty."; return false;}

  int p;
  QString pCom;
  while (true) {
      if (!preamble.getStr(' ','=',&p)) {errorMessage="Invalid preamble."; return false;}

    int index=-1;
    for(int k=0; k<preambleCommands.size(); ++k) {
        pCom=preamble.currentLine.left(p).trimmed();
        if (0==QString::compare(pCom,preambleCommands.at(k),Qt::CaseInsensitive))
        {index=k; break;}
    }
    if(index<0) {
      error=true;
      errorMessage=QString(Q_FUNC_INFO)+": Unknown command: " + pCom + ".";
      return false;
    }

    // remove command (clk, uses, amp, etc...)
    preamble.currentLine=preamble.currentLine.remove(0,p).trimmed();

    if(!preamble.getStr(';')) {errorMessage="';' expected."; return false;}

    /*    preambleCommands << "uses" << "clk" << "time" << "loop" << "amp"
                   << "int" << "phaselist" << "freq" << "aux" << "phase"
                   << "acqphase" << "const" << "function" << "version"
                   << "transform" << "double";   */

    switch(index) {
      case 0: e=p_uses(); break; // uses
      case 1: e=p_clk(); break; // clk
      case 2: e=p_time(); break; // time
      case 3: e=p_loop(); break; // loop
      case 4: e=p_amp(); break; // amp
      case 5: e=p_int(); break; // int
      case 6: e=p_phaseList(); break; // phaselist
      case 7: e=p_freq(); break; // freq
      case 8: e=p_aux(); break; // aux
      case 9: e=p_phase(); break; // phase
      case 10: e=p_acqPhase(); break; // acqphase
      case 11: e=true; constVar=true; break; // const
      case 12: e=p_function(); break; // function;
      case 13: e=p_version(); break; // version
      case 14: e=p_transform(); break;
      case 15: e=p_double(); break;
      default: e=false; errorMessage=QString(Q_FUNC_INFO)+": Unknown declaration."; break;
    }

    if (!e) return false;

    if (!preamble.getStr()) // end of preamble
    {

        break;
    } // check if aux params have been declared, and if acqPhase has been set.



   // currentPosition++;
  } // while

  if(!NADefined) {errorMessage="Mandatory NA is not declared.";e=false;}
  if(!DWDefined) {errorMessage="Mandatory DW is not declared.";e=false;}
  if(!PDDefined) {errorMessage="Mandatory PD is not declared.";e=false;}
  if(!ALDefined) {errorMessage="Mandatory AL is not declared.";e=false;}
  if(!acqPhaseDefined) {errorMessage="Mandatory acqphase is not declared.";e=false;}

  if(!NDDefined)
  {
      variables.append(new TVariable("ND",TVariable::ndVariable,QVariant(0),TVariable::Unity));
      variables[variables.size()-1]->setCaption("Dummy scans");
      NDDefined=true;
  }


  if(version()<0)
  {
      warning=true;
      warningMessage.append(QString(Q_FUNC_INFO)+": version number is not defined.\n");
  }

  //
  //  ACQTIME
  //
  int dwIndex=0;
  int alIndex=0;
  for(int k=0; k<variables.size(); k++)
  {
      if(0==QString::compare("dw",variables.at(k)->name(),Qt::CaseInsensitive)) dwIndex=k;
      if(0==QString::compare("al",variables.at(k)->name(),Qt::CaseInsensitive)) alIndex=k;
  }
  TVariable::Unit uni=variables.at(dwIndex)->unit();
  double d=variables.at(dwIndex)->numeric().toDouble();
  int a=variables.at(alIndex)->value().toInt();
  variables.append(new TVariable("ACQTIME",TVariable::timeVariable,QVariant(d*a),uni));
  variables[variables.size()-1]->changeUnit(TVariable::Milli);

  if (!e) return false;

  return true;
}
//-----------------------------------------------------------------------------
bool TpulseProgram::processPhaseCycle()
{
    QStringList qs;
    for(int ch=0; ch<channels(); ch++)
    {
        currentCH=ch;

        // phase cycle generation
        if(!gates.genPhaseCycle(ch,&qs)) {errorMessage=gates.errorMessage; return false;}
        for(int k=0; k<qs.size(); k++)
        {
            compiledPhaseCycle[ch]->append(qs.at(k));
        }

        if(ch==0) // acq phase cycle generation (1st channel (ch=0) only)
        {
            if(!genAcqPhaseCycle(&qs)) {return false;}
            for(int k=0; k<qs.size(); k++)
            {
                compiledPhaseCycle[ch]->append(qs.at(k));
            }
        } // if(ch==0)

    }
  return true;
}
//-----------------------------------------------------------------------------
bool TpulseProgram::processAsyncPPG()
{
  // We have nothing to do if asyncPPG is empty
  if(asyncPPG.size()<1) return true;

  TppgLines pl;
  for(int ch=0; ch<channels(); ch++)
  {
      currentCH=ch;
      for(int k=0; k<asyncPPG.size(); k++)
      {
        if((asyncPPG.at(k)->called) && (asyncPPG.at(k)->channel()==ch))
        {
          currentAsyncPPGIndex=k;
        // late address assignment
          if(!errorCheckOnly) asyncPPG[k]->startAddress=compiledPPG.at(ch).size();
          for(int m=0; m<asyncPPG.at(k)->whereIs.size(); m++)
          {
              compiledPPG[ch].replace(asyncPPG.at(k)->whereIs.at(m),
                                       new TcompiledPPG(com_ASYNC,myHex(asyncPPG.at(k)->startAddress,10)));
          }

        // add all but the relevant ch to asyncManager
          asyncManager.clear();
          for(int c=0; c<channels(); c++) if(c!=ch) asyncManager << c;

      //    qDebug() << "async: " << asyncManager;

          loopManager.clear();

          pl=asyncPPG[k]->toPPGLines();
          pl.currentPosition=1; // 2nd line!
          pl.currentLine.clear();

          if (pl.currentPosition<pl.source.size())
              pl.currentLine=pl.source.at(pl.currentPosition);

          // Empty check
          if (!pl.getStr()) {errorMessage=asyncPPG.at(k)->name()+" is empty."; return false;}

          while (true) {

            int index=-1;
            for(int m=0; m<asyncPPGCommands.size(); ++m) {
                if (pl.currentLine.startsWith(asyncPPGCommands.at(m),Qt::CaseInsensitive)) {index=m; break;}
            }
            if(index<0) {
              errorMessage=QString(Q_FUNC_INFO)+": Syntax error (unknown command).";
              return false;
            }

            // remove command string
            pl.currentLine=pl.currentLine.remove(0,asyncPPGCommands.at(index).size()).trimmed();


/*
 *
  ppgCommands << "pulse" << "delay" << "burst" << "ready" << "loop"
              << "endlp" << "async" << "sync" << "wait_h" << "wait_l"
              << "exttrig" << "import" << "init" << "relax";

  asyncPPGCommands << ppgCommands << "continue" << "standby";

 */
            bool ok; // error detector

            switch(index)
            {
              case 0: ok=m_pulse(pl); break; // pulse
              case 1: ok=m_delay(pl); break; // delay
              case 2: ok=m_burst(pl); break; // burst
              case 3: // ready
                ok=false;
                errorMessage="Ready cannot be used inside an asynchronous sequence.";
                break;
              case 4: ok=m_loop(pl); break; // loop
              case 5:
              case 6:
                ok=m_endlp(pl); break; // endlp
              case 7: // async
              case 8: // sync
                ok=false;
                errorMessage="Async or sync cannot be used inside an asynchronous sequence.";
                break;

              case 9: ok=m_wait_hl(pl,1); break; // wait_h
              case 10: ok=m_wait_hl(pl,0); break; // wait_l
              case 11: // exttrig
                ok=false;
                errorMessage="Exttrig cannot be used in an asynchronous sequence. Consider using Wait_H instead.";
                break;

              case 12: ok=m_import(pl); break; // import     TODO: avoiding circular import

              case 13: // init
                ok=false;
                errorMessage="Init cannot be used inside an asynchronous sequence.";
                break;

              case 14: // relax
                ok=false;
                errorMessage="Relax cannot be used inside an asynchronous sequence.";
                break;

            case 15: // sweep
              ok=m_freqSweep(pl); break;
            case 16: // endSweep
              ok=m_endFreqSweep(pl); break;



              case 17: // continue
                if(!errorCheckOnly)
                {
                  compiledPPG[currentCH].append(new TcompiledPPG(com_JUMP,myHex(asyncPPG.at(k)->startAddress,10)));
                }
                ok=true;
                break;
              case 18: // standby
                if(!errorCheckOnly)
                {
                  compiledPPG[currentCH].append(new TcompiledPPG(com_JUMP,myHex(compiledPPG.at(currentCH).size()-1,10)));
                }
                ok=true;
                break;
              default: ok=false; errorMessage=QString(Q_FUNC_INFO)+"A bug occured. Please report to Takeda."; break;
            }

            if (!ok) return false;

            if (!pl.getStr()) // end of async ppg
            {
              break;
            }

          } // while

        }  // if
      } // k

  } // ch
  return true;
}
//-----------------------------------------------------------------------------
bool TpulseProgram::processMainPPG()
{

for(int ch=0; ch<channels(); ch++) {
  currentCH=ch;
  loopManager.clear(); loopCounts.clear();
  asyncManager.clear(); sweepManager.clear();
  elapsedTime.clear();
  elapsedTime.append(0);

  // Read the 1st line
  mainPPG.currentPosition=0; mainPPG.currentLine.clear();
  if (mainPPG.currentPosition<mainPPG.source.size()) mainPPG.currentLine=mainPPG.source.at(mainPPG.currentPosition);

  // Empty check
  if (!mainPPG.getStr()) {errorMessage="Main part of the pulse program is empty."; return false;}

  //
  bool finished=false;
  while (!finished) {


     // qDebug() << mainPPG.currentLine;

    int index=-1;
    for(int k=0; k<ppgCommands.size(); ++k) {
        if (mainPPG.currentLine.startsWith(ppgCommands.at(k),Qt::CaseInsensitive)) {index=k; break;}
    }
    if(index<0) {
      errorMessage=QString(Q_FUNC_INFO)+": Syntax error (unknown command): " + mainPPG.currentLine;
      return false;
    }

    // remove command string
    mainPPG.currentLine=mainPPG.currentLine.remove(0,ppgCommands.at(index).size()).trimmed();

//    ppgCommands << "pulse" << "delay" << "burst" << "ready" << "loop"
//                << "endlp" << "endLoop" << "async" << "sync" << "wait_h" << "wait_l"
//                << "exttrig" << "import" << "init" << "relax" << "sweep" << "endSweep";


    bool ok; // error detector

   // qDebug() << QString(Q_FUNC_INFO)  << "CH" << currentCH << index;


    switch(index) {
      case 0: ok=m_pulse(mainPPG);break; // pulse
      case 1: ok=m_delay(mainPPG); break; // delay
      case 2: ok=m_burst(mainPPG); break; // burst
      case 3: // ready
        if(!asyncManager.isEmpty())
        {
            ok=false;
            errorMessage="Ready cannot be used when an asynchronous operation is active.";
            break;
        }
        else {ok=m_ready(); break;}
      case 4: ok=m_loop(mainPPG); break; // loop

      case 5:
      case 6: ok=m_endlp(mainPPG); break; // endlp
      case 7: ok=m_async(mainPPG); break; // async  TODO: late-address assignment
      case 8: ok=m_sync(mainPPG); break; // sync
      case 9: ok=m_wait_hl(mainPPG,1); break; // wait_h
      case 10: ok=m_wait_hl(mainPPG,0); break; // wait_l
      case 11: if(asyncManager.contains(currentCH)) {ok=true; break;} else {ok=m_exttrig(); break;} // exttrig
      case 12: ok=m_import(mainPPG); break; // import     TODO: avoiding circular import
      case 13: // init
        if(!asyncManager.isEmpty())
        {
            ok=false;
            errorMessage="Init cannot be used when an asynchronous operation is active.";
            break;
        }
        else {ok=m_init(); break;}
      case 14: // relax
        if(!asyncManager.isEmpty())
        {
            ok=false;
            errorMessage="Relax cannot be used when an asynchronous operation is active.";
            break;
        }
        else {ok=m_relax(); break;}
      case 15: // sweep
        ok=m_freqSweep(mainPPG); break;
      case 16: // endSweep
        ok=m_endFreqSweep(mainPPG);
        break;

      default: ok=false; errorMessage=QString(Q_FUNC_INFO)+"A bug occured. Please report to Takeda."; break;
    }

    if (!ok) return false;

    if (!mainPPG.getStr()) finished=true;  // end of ppg

  }

  //qDebug() << loopManager << sweepManager;
  if(!sweepManager.isEmpty()) // We check if the loops are closed.
  {
      errorMessage="Frequency sweep is not closed.";
      error=true;
      return false;
  }
  if(!loopManager.isEmpty()) // We check if the loops are closed.
  {
      errorMessage="Loop is not closed.";
      error=true;
      return false;
  }
  if(!asyncManager.isEmpty()) // We check if the asynchronous operation(s) are synchronized back.
  {
      errorMessage="Asynchronous sequence is not synchronized back.";
      error=true;
      return false;
  }


  for(int ac=0; ac<asyncPPG.size(); ac++)
  {
      if((asyncPPG.at(ac)->called) && (asyncPPG.at(ac)->channel()==currentCH))
      {

          // Read the "2nd" line
          asyncPPG[ac]->currentPosition=1; asyncPPG[ac]->currentLine.clear();
          if (asyncPPG.at(ac)->currentPosition<asyncPPG.at(ac)->source.size())
              asyncPPG[ac]->currentLine=asyncPPG.at(ac)->source.at(asyncPPG.at(ac)->currentPosition);

          // Empty check
          if (!asyncPPG[ac]->getStr())
          {
              errorMessage="Async sequence "+asyncPPG.at(ac)->name() +" is empty.";
              return false;
          }

          //
          if(!errorCheckOnly)
          {
              asyncPPG[ac]->startAddress=compiledPPG.at(currentCH).size();
          }
      } // if
  } // for(ac)

}  // currentCH

  return true;

}
//-----------------------------------------------------------------------------
bool TpulseProgram::getTime(QString q)
{
   // we check if q matches with one of time variables
    int varIndex=-1;
    for(int k=0; k<variables.size(); k++)
    {
        if (0==QString::compare(q,variables.at(k)->name(),Qt::CaseInsensitive)) varIndex=k;
    }

    double d;
    bool ok;
    if (varIndex>-1)
    {
        if((variables.at(varIndex)->type()!=TVariable::timeVariable) &&
                (variables.at(varIndex)->type()!=TVariable::dwVariable))
        {
            errorMessage=QString(Q_FUNC_INFO)+ ": "+ q + " is not a time variable.";
            return false;
        }

        //
        //  Store the address where this time variable is used.
        //  Note that, at this moment, the current address is given by
        //  compiledPPG.at(currentCH)->size()-1. Provided that there is no error,
        //  a new line for the pulse or delay command will be added
        //  at address compiledPPG.at(currentCH)->size().
        //             ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        //
        if(!errorCheckOnly)
        {
          variables[varIndex]->addresses[currentCH].append(compiledPPG.at(currentCH).size());
          variables[varIndex]->gateIndex[currentCH].append(-1);
           // time has nothing to do with the gate   --->   -1
        }

        d=variables.at(varIndex)->value().toDouble(&ok);

    }
    else   // Then, we check if q represents time with a proper unit
    {
        double ex;
       // TUnit uni;
        switch (q.at(q.size()-1).toLatin1()) {
          case 'n':
          case 'N': //uni=Nano;
                    ex=pow(10,-9); break;
          case 'u':
          case 'U': //uni=Micro;
                    ex=pow(10,-6); break;
          case 'm':
          case 'M': //uni=Milli;
                    ex=pow(10,-3); break;
          case 's':
          case 'S': //uni=Unity;
                    ex=1.0;break;
          default:
            error=true; errorMessage=QString(Q_FUNC_INFO)+": Unit of time (n,u,m, or s) is required.";return false;
            break;
        }

        // remove the unit string;
        q.chop(1);
        q=q.trimmed();

        d=q.toDouble(&ok);
        if(!ok) {error=true; errorMessage=QString(Q_FUNC_INFO)+": Invalid number."; return false;}
        d*=ex;

    }

   // negative check;
   if(d<0) {error=true; errorMessage=QString(Q_FUNC_INFO)+": time cannot be negative."; return false;}

   getTimeResult=d*1E6; // in microseconds
   return true;
}

//-----------------------------------------------------------------------------
bool TpulseProgram::m_pulse(TppgLines &ppgLines) {
    bool iteration=false;
    int nOfIteration=1;
    bool ok=true;

    if(ppgLines.currentLine.startsWith('['))
    {
        isShapedPulseUsed = true;  // checked by processGate
        isShapedPulseActive = false;  // will be modified by processGate

        iteration  = true;
        ppgLines.currentLine.remove(0,1).remove(QChar(' '));
        int i;
        if(!ppgLines.getStr(']', &i))
        {
            errorMessage=QString(Q_FUNC_INFO)+": ']' was not found.";
            return false;
        }
        QString ni=ppgLines.currentLine.left(i).trimmed();
        ppgLines.currentLine.remove(0,i+1);
        bool ok;

        ok=evalArg(ni);
        //nOfIteration=ni.toInt(&ok);
        if(!ok) {errorMessage=QString(Q_FUNC_INFO)+": invalid iteration number."; return false;}
        nOfIteration=round(evalArgResult);
        if(nOfIteration<1) {errorMessage=QString(Q_FUNC_INFO)+": iteration number must be larger than 0."; return false;}
    }
    else
    {
        isShapedPulseUsed=false;
    }


    if(!ppgLines.currentLine.startsWith('('))
    {
        if(iteration) errorMessage=QString(Q_FUNC_INFO) + ": \"(\" is expected after \"pulse[int]\".";
            else errorMessage=QString(Q_FUNC_INFO) + ": \"(\" is expected after \"pulse\".";
        return false;
    }

    bool parenthesisClosed=false;
    while (!parenthesisClosed && (ppgLines.currentPosition<ppgLines.source.size()))
    {
        if(ppgLines.currentLine.count('(') <= ppgLines.currentLine.count(')')) parenthesisClosed=true;
        else
        {
            ppgLines.currentPosition++;
            if(ppgLines.currentPosition<ppgLines.source.size()-1)
            {
               ppgLines.currentLine+=ppgLines.source.at(ppgLines.currentPosition).trimmed();
            }
        }
    }

    if(ppgLines.currentPosition>ppgLines.source.size()-1) ppgLines.currentPosition=ppgLines.source.size()-1;

    if(!parenthesisClosed)
    {
          errorMessage=QString(Q_FUNC_INFO) + "Parenthesis is not closed: " + ppgLines.currentLine;
          return false;
    }


    // We remove '(' just after 'pulse', and then all space characters in currentLine
    ppgLines.currentLine.remove(0,1).remove(QChar(' '));

    // We evaluate time expression.
    if (!evalTime(ppgLines.currentLine)) {
      //  qDebug() << getTimeResult << " us";
        errorMessage=QString(Q_FUNC_INFO) + ppgLines.currentLine;
        return false;
    }
    // The result is stored in TpulseProgram::getTimeResult "IN MICROSECONDS".
    // Now, the time expression in currentLine has been eliminated by 'evalTime' function.

    bool equivToDelay;
    if (ppgLines.currentLine.startsWith(')'))
    {
        equivToDelay=true;
        containsAD9858=false;
                    // added 20170116
                    // This is required in case that the previous line happens to contain AD9858 gate,
                    // in which containsAD9858 must be reset to false;
        ppgLines.currentLine.remove(0,1); // We remove ')'
    }
    else if (ppgLines.currentLine.startsWith(';'))
    {
        equivToDelay=false;
        ppgLines.currentLine.remove(0,1); // We remove ';'
    }
    else
    {
        errorMessage=QString(Q_FUNC_INFO) + "Invalid expression.";
        return false;
    }

    lineSet.clear();



    QString currentLine_backup=ppgLines.currentLine;
    int currentPosition_backup=ppgLines.currentPosition;

    //int NOfTables=ppgLines.currentLine.count("table",Qt::CaseInsensitive);
    // qDebug() << "NOfTables: " << NOfTables;

    //qDebug() << nOfIteration;

    int k=0;
    while(k<nOfIteration)
    //for (int k=0; k<nOfIteration; k++)
    {
        sharpK=k; // read by processGate

     //   qDebug() << "iteration "<< k << "ch " << currentCH;
        ppgLines.currentLine=currentLine_backup;
        ppgLines.currentPosition=currentPosition_backup;

       // qDebug() << QString(Q_FUNC_INFO) << "ch " << currentCH << " " <<ppgLines.currentLine;

  //DONE: move to processGate
  // ppgLines.currentLine.replace(QChar('#'),QString::number(k));

        int index0,index1,index2; parenthesisClosed=false;

        while(ppgLines.currentLine.count("table",Qt::CaseInsensitive)>0)
        {
      //      qDebug() << ppgLines.currentLine;

            index0=ppgLines.currentLine.indexOf("table",Qt::CaseInsensitive);
            index1=ppgLines.currentLine.indexOf('(',index0);
            index2=index1;

      //      qDebug() << index0 << index1 << index2;

            QString qs0; qs0.clear(); parenthesisClosed=false;
            while(!parenthesisClosed && index2<ppgLines.currentLine.size())
            {
              qs0.append(ppgLines.currentLine.at(index2));
              if(qs0.count('(')==qs0.count(')')) parenthesisClosed=true;
              else index2++;
            }
            if(!parenthesisClosed) {errorMessage=QString(Q_FUNC_INFO)+": table parenthesis is not closed."; return false;}

            qs0.chop(1); // We remove the last ')'
            qs0.remove(0,1); // We remove the first '('
            qs0=qs0.trimmed();

            QString candidateFileName=qs0;
            int bunch=1;  // number of arguments of the current gate.
                          // Most gates have a single argument, but there are exceptions.
                          // For example, RFIQ gates have 2 arguments.
                          // in-line table is allowed only for the gates with single argument.
                          //
            int lIndex=qs0.lastIndexOf(',');
            if(lIndex>0)
            {
                candidateFileName=qs0.left(lIndex);
                if(QFile::exists(ppgFilePath()+"/"+candidateFileName))
                {
                    bunch=qs0.mid(lIndex+1).toInt(&ok);
                    if(!ok)
                    {  error=true;
                       errorMessage=QString(Q_FUNC_INFO)+": invalid table expression.";
                       return false;
                    }
                }
            }
            else
            {
                candidateFileName=qs0;
                // If no comma is included, qs0 should not be the table value but a filename.
            }

     //   qDebug() << candidateFileName; qDebug()<<bunch;

            QStringList sl0; sl0.clear();

            if(QFile::exists(ppgFilePath()+"/"+candidateFileName))
            {  // read from file
               QFile file(ppgFilePath()+"/"+candidateFileName);
               if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                   error=true;
                   errorMessage=QString(Q_FUNC_INFO)
                           + ": Failed to open file " + candidateFileName +".";
                //   qDebug() << k << c;
                   return false;
               }
               if(!loadedFiles.contains(ppgFilePath()+"/"+candidateFileName))
               {
                 loadedFiles <<  ppgFilePath()+"/"+candidateFileName;
               }
               QTextStream in(&file);
               QString qs1; qs1.clear();
               while(!in.atEnd())
               {
                   qs1=in.readLine().trimmed();
                   if(!qs1.isEmpty())
                   {
                     // We allow either space or comma for the separator for table file.
                     if(qs1.contains(','))
                     {
                       qs1.remove(QRegularExpression("\\s+"));
                       sl0.append(qs1.split(','));
                     }
                     else
                     {
                       sl0.append(qs1.split(QRegularExpression("\\s+")));
                     }
                   }
                   //qDebug() << qs1;

               }
               file.close();

               //qDebug() << sl0;

            } // file exsists
            else
            {
                qs0.remove(QRegularExpression("\\s+"));
                sl0=qs0.split(','); // ',' is the only separator for in-line tables
            }

            if(sl0.size()!=nOfIteration*bunch)
            {
                errorMessage=QString(Q_FUNC_INFO)+ ": Table length does not match with the number of iteration ("+
                        QString::number(nOfIteration)+").";
               //qDebug() << sl0;
                return false;
            }

            QString qs2=ppgLines.currentLine.left(index0);
            qs2 += '!';  // indicator for table element (used in processGate)
            if(bunch==1) qs2 += sl0.at(k);
            else
            {
                for(int c=0; c<bunch; c++)
                { qs2 += sl0.at(k*bunch+c); if(c<bunch-1) qs2+=',';}
            }
            qs2 += ppgLines.currentLine.mid(index2+1);
            ppgLines.currentLine=qs2;

         //   qDebug()<<currentCH<<qs2;

            // if(bunch>1) qDebug() << ppgLines.currentLine;
        }  // while (table)

//        qDebug()<<"B";

      //
      //  Now, we examine gates.
      //
      if(!equivToDelay) if (!processGate(ppgLines)) return false;

//      qDebug()<<"C";


      //
      // If shaped pulse is not used for the current CH, we do not need to iterate.
      //  
      if(!isShapedPulseActive) nOfIteration=1;


      //qDebug()<<"D";

      //
      // If the currentCH is in the asynchronous operation, we do nothing and return,
      //
      if(asyncManager.contains(currentCH)) return true;


      // We get the number of clock counts
      quint64 i64=quint64(round(getTimeResult*CLK/nOfIteration));



      // We check range
      if(i64==0)
      {
          warning=true;
          warningMessage.append(QString(Q_FUNC_INFO) + ": zero time is set.\n");
          return true;
      }
      else if(i64<4)
      {
        errorMessage=QString(Q_FUNC_INFO) + ": time is too short.";
        return false;
      }
      if(i64-4>(quint64(pow(2,40)-1)))
      {
        errorMessage=QString(Q_FUNC_INFO) + ": time is too long.";
        return false;
      }

      QString s1;
 //   quint64 line;
      int nl;

      //  check if containsAD9858 is true or not.   -> set noflinespercommand
      if(containsAD9858)
      {
        // In the header file, we have defined: #define CLK_COUNT_AD9858_COMMAND 6
        int minCount=(CLK_COUNT_AD9858_COMMAND+5)*4;
        //        if(i64<25)
        if(i64<quint64(minCount))
        {
            errorMessage= QString(Q_FUNC_INFO) + ": at least " + QString::number(CLK_COUNT_AD9858_COMMAND)
                                         + " clock counts are required for frequency setting.";
            return false;
        }
        nl=5;

        for(int cl=0; cl<nl; cl++)
        {
  //        line=0;
          if(cl<nl-1) s1=myHex(CLK_COUNT_AD9858_COMMAND,10); // 6+4 -> 10 CLK
          else s1=myHex(i64-minCount,10); // (10*4)+4 -> 44 CLK

          if(!errorCheckOnly)
          {
            if(isInsertActive)
            {
                insertionCompiledPPG.append(new TcompiledPPG(com_OUT,s1,lineSet+AD9858Buffer.at(cl)));
            }
            else
            {
                compiledPPG[currentCH].append(new TcompiledPPG(com_OUT,s1,lineSet+AD9858Buffer.at(cl)));
                if (cl==0) NOfLinesPerCommand[currentCH]->append(5);
                else NOfLinesPerCommand[currentCH]->append(0);
            }
          }
        }

      } // containAD9858
      else
      {
        nl=1;
        s1=myHex(i64-4,10);

        if(!errorCheckOnly)
        {
            if(isInsertActive)
            {
              insertionCompiledPPG.append(new TcompiledPPG(com_OUT,s1,lineSet));
            }
            else
            {
              compiledPPG[currentCH].append(new TcompiledPPG(com_OUT,s1,lineSet));
              NOfLinesPerCommand[currentCH]->append(1);
            }
        }
      }

   k++;
   }// while (k)

    // If we are inside the freq-sweep loop, we update the net elapsed time
    if(sweepManager.contains(currentCH))
    {
      if(!elapsedTime.isEmpty()) elapsedTime.last()+=getTimeResult;
      else
      {
        errorMessage=QString(Q_FUNC_INFO) +
                ": elapsedTime (QList<double>) is empty. Please report to Takeda.";
        return false;
      }
    }

    return true;
}
//-----------------------------------------------------------------------------
bool TpulseProgram::processGate(TppgLines &ppgLines)
// We assume
//   pulse(pw;gate1,gate2(arg),...)
//   wait_H(trig; gate1, gate2(arg),...)
//   burst(gate1,gate2(arg),....)
// have already been modified to
//   gate1,gate2(arg),.....).
//
// We note that user might have used line feed, so that the current line looks like
//   gate1,
// which should be followed by
//   gate2(arg),
//   ...
//   )
{
    int index,gIndex;
    containsAD9858=false;
    bool ok;
    double d;
    int q;
    QString lhs, rhs, opt;
    int profile=0; // profile for AD9858 setup

    usedGateIndex.clear();
    lineSet.clear();

    QString gateString;
    QString arg;

    bool finished=false;

    while(!finished)
    {
      while (   (!ppgLines.currentLine.contains(','))
             && (!ppgLines.currentLine.contains('('))
             && (!ppgLines.currentLine.contains(')'))
             && (ppgLines.currentPosition < ppgLines.source.size()-1)   )
      {
        ppgLines.currentPosition++;
        ppgLines.currentLine += ppgLines.source.at(ppgLines.currentPosition).trimmed();
      }
      if (   (!ppgLines.currentLine.contains(','))
          && (!ppgLines.currentLine.contains('('))
          && (!ppgLines.currentLine.contains(')'))  )  // end of ppgLines without finding the target characters
      {
          errorMessage=QString(Q_FUNC_INFO);
          return false;
      }

      // Now ppgLines.currentLine should contain at least one of the target characters.
      // We look for the first one.

      index=-1;
      for(int k=0; k<ppgLines.currentLine.size(); k++)
          if(   (ppgLines.currentLine.at(k)==',' )
             || (ppgLines.currentLine.at(k)=='(' )
             || (ppgLines.currentLine.at(k)==')' )) {index=k; break;}

      if(index<0) {errorMessage=QString(Q_FUNC_INFO); return false;}

      // We extract the gate name
      gateString=ppgLines.currentLine.left(index).trimmed();
      // We remove the gate name from currentLine.
      ppgLines.currentLine=ppgLines.currentLine.remove(0,index).trimmed();

      if(gateString.isEmpty()) // empty ?
      // If the empty string is followed by ')', we just return without error.
      {
         if(ppgLines.currentLine.at(0)==')')
         {
             ppgLines.currentLine=ppgLines.currentLine.remove(0,1).trimmed();
             return true;
         }
         else
         {
             errorMessage=QString(Q_FUNC_INFO) + ": Invalid character " + ppgLines.currentLine.at(index);
             return false;
         }
      }

      // Gate query
      gIndex=gates.nameIndex(gateString);
      if (gIndex==-1)
      {
          errorMessage=QString(Q_FUNC_INFO) + ": Unknown gate " + gateString;
          return false;
      }

      // Multiple usage check
      if (usedGateIndex.contains(gIndex))
      {
          errorMessage="Multiple usage of " + gates.Items.at(gIndex)->name + ".";
          return false;
      }
      usedGateIndex << gIndex;

      //
      //  check if AD9858 is used.
      //
      if( (gates.Items.at(gIndex)->KindOfGate()==GAD9858) && (gates.Items.at(gIndex)->channel()==currentCH) )
      {
          containsAD9858=true;
      }

      //
      // Async check
      //
      if( asyncManager.contains( gates.Items.at(gIndex)->channel()) )
      {
          errorMessage="Channel number "
                  + QString::number(gates.Items.at(gIndex)->channel()+1)
                  + " has gone into the asynchronous mode, and it is invalid to use "
                  + gates.Items.at(gIndex)->name +" here.";
          return false;
      }



      int nlp; // number of left parenthesis
      int nrp;
      int kk;
      QString argumentString;


         // We obtain argument.
          bool parenthesisClosed=false;
          switch(gates.Items.at(gIndex)->KindOfGate())
          {
            case GAmplitude:
            case GPhase:
            case GLogicVector:
            case GInteger:
            case GAD9858:
            case GRFIQ:
              if(ppgLines.currentLine.at(0)!='(') {errorMessage="An argument is missing."; return false;}
              while (!parenthesisClosed && (ppgLines.currentPosition<ppgLines.source.size()))
              {
                  if(ppgLines.currentLine.count('(') <= ppgLines.currentLine.count(')')) parenthesisClosed=true;
                  else
                  {
                      ppgLines.currentPosition++;
                      if(ppgLines.currentPosition<ppgLines.source.size())
                      {
                         ppgLines.currentLine+=ppgLines.source.at(ppgLines.currentPosition).trimmed();
                      }
                  }
              }

              if(ppgLines.currentPosition>ppgLines.source.size()-1) ppgLines.currentPosition=ppgLines.source.size()-1;

              if(!parenthesisClosed)
              {
                    errorMessage=QString(Q_FUNC_INFO) + "Parenthesis is not closed.";
                    return false;
              }


              //TODO: QString argumentString
                      nlp=1; // number of left parenthesis
                      nrp=0;
                      kk=0;
                      argumentString=ppgLines.currentLine.at(kk);
  ppgLines.currentLine.remove(kk,1);
                      while(nlp>nrp)
                      {
                         // kk++;
                          argumentString+=ppgLines.currentLine.at(kk);
  ppgLines.currentLine.remove(kk,1);

                          nlp=argumentString.count('(');
                          nrp=argumentString.count(')');
                      }

              // We remove the first '('.
   //  ppgLines.currentLine.remove(0,1).remove(QChar(' '));
   argumentString.remove(0,1).remove(QChar(' '));


              if(argumentString.contains('#'))
//              if(ppgLines.currentLine.contains('#'))
              {
                  //qDebug() << QString(Q_FUNC_INFO) << "argumentString:" << argumentString;

                 if(!isShapedPulseUsed)
                 {
                     errorMessage=QString(Q_FUNC_INFO)+": # cannot be used here, because shaped pulse is not active.";
                     return false;
                 }
               //  qDebug() << gates.Items.at(gIndex)->name << gates.Items.at(gIndex)->channel() << currentCH;

                 if(gates.Items.at(gIndex)->channel()==currentCH) {isShapedPulseActive=true;}
           argumentString.replace(QChar('#'),QString::number(sharpK));
                           //  ppgLines.currentLine.replace(
                           //  ppgLines.currentLine.indexOf(QChar('#')),1,QString::number(sharpK));


               //  qDebug() << QString(Q_FUNC_INFO) << "ch:" << currentCH << ppgLines.currentLine;
              }


              while(argumentString.contains('!'))
//              while(ppgLines.currentLine.contains('!'))
              {
                  if(!isShapedPulseUsed)
                  {
                      errorMessage=QString(Q_FUNC_INFO)+": table cannot be used here, because shaped pulse is not active.";
                      return false;
                  }
                 if(gates.Items.at(gIndex)->channel()==currentCH) {isShapedPulseActive=true;}
// ppgLines.currentLine.remove(ppgLines.currentLine.indexOf('!'),1); // We remove '!' added in m_pulse for table operation.
                 argumentString.remove(argumentString.indexOf('!'),1);
                 //     ppgLines.currentLine.remove(0,1); // We remove '!' added in m_pulse for table operation.
                 //qDebug() << ppgLines.currentLine;
              }


              break;
            default: break;
          }

          // We evaluate argument.
          int pos=0;
          double re=0; double im=0; double phaseOffset=0;

          switch(gates.Items.at(gIndex)->KindOfGate())
          {
            case GAmplitude:
            case GPhase:
            case GLogicVector:
            case GInteger:
// TODO (argumentString)
              if(!evalArg(argumentString))
              {
                  errorMessage=QString(Q_FUNC_INFO)+": Invalid expression: " + ppgLines.currentLine;
                  return false;
              }
 //             if (ppgLines.currentLine.startsWith(')'))
 //             {
 //                 ppgLines.currentLine.remove(0,1); // We remove ')'
 //             }
 //             else
 //             {
 //                 errorMessage=QString(Q_FUNC_INFO)+": Parenthesis is not closed?";
 //                 return false;
 //             }
              break;

            case GAD9858:
             // if(gates.Items.at(gIndex)->channel()==currentCH)
             // {
             // check if arg contains ';' (profile; value)

                gates.Items.at(gIndex)->setLogicVector(false);

// TODO (argumentString)

                if(argumentString.contains(';'))
                  //  if(ppgLines.currentLine.contains(';'))
                {
//                    lhs=ppgLines.currentLine.left(ppgLines.currentLine.indexOf(';',0)).trimmed();
                    lhs=argumentString.left(argumentString.indexOf(';',0)).trimmed();

                    if(0==QString::compare(lhs,"setup",Qt::CaseInsensitive))
                    {
                      profile=-1;  // < 0 !, so as not to perform transformation.
                      gates.Items.at(gIndex)->setLogicVector(true);
                      containsAD9858=false;
                    }
                    else
                    {
                      profile=lhs.toInt(&ok);
                      if(!ok)
                      {
                        errorMessage="Invalid profile expression for " + gates.Items.at(gIndex)->name + ".";
                        return false;
                      }
                      if((profile<-1) || (profile>3))
                      {
                        errorMessage="Profile for " +gates.Items.at(gIndex)->name+ " is out of range (0..3 or -1).";
                        return false;
                      }
                    }

   // ppgLines.currentLine.remove(0,ppgLines.currentLine.indexOf(';',0)+1).remove(QChar(' '));
                   argumentString.remove(0,argumentString.indexOf(';',0)+1).remove(QChar(' '));
                }
                else
                {
                    profile=0;
                   // ppgLines.currentLine.remove(QChar(' '));
                    argumentString.remove(QChar(' '));
                }

         //       rhs=ppgLines.currentLine;
                rhs=argumentString;

//                if(!evalArg(ppgLines.currentLine))
                if(!evalArg(argumentString))
                {
                    errorMessage=QString(Q_FUNC_INFO)+": Invalid expression: " + rhs;
                    return false;
                }
//                if (ppgLines.currentLine.startsWith(')'))
//                {
//                    ppgLines.currentLine.remove(0,1); // We remove ')'
//                }
//                else
 //               {
 //                   errorMessage=QString(Q_FUNC_INFO)+": Parenthesis is not closed?";
 //                   return false;
 //               }

            //  }
              break;

            case GRFIQ:
            {
              arg.clear();pos=0;
//              while(arg.count(')')<arg.count('(')+1 && pos<ppgLines.currentLine.size())
              while(arg.count(')')<arg.count('(')+1 && pos<argumentString.size())
              {
             //     arg.append(ppgLines.currentLine.at(pos));
                  arg.append(argumentString.at(pos));
                  pos++;
              }
              if(arg.count(')')!=arg.count('(')+1)
              {
                  errorMessage=QString(Q_FUNC_INFO) // + "In " + ppgLines.currentLine + " "
                          +": Parenthesis is not closed for "
                          + gates.Items.at(gIndex)->name + ".";
                  return false;
              }

              QStringList sl=arg.split(",");
              if(sl.size()<2)
              {
                  errorMessage=QString(Q_FUNC_INFO) // + "In " + ppgLines.currentLine + " "
                          +": insufficient arguments for "
                          + gates.Items.at(gIndex)->name + ".";
                  return false;
              }

              if(sl.size()>3)
              {
                  errorMessage=QString(Q_FUNC_INFO) // + "In " + ppgLines.currentLine + " "
                          +": too many arguments for "
                          + gates.Items.at(gIndex)->name + ".";
                  return false;
              }


//              if (arg.count(',')!=1)
//              {
//                  errorMessage=QString(Q_FUNC_INFO) // + "In " + ppgLines.currentLine + " "
//                          +": 2 parameters are required for "
//                          + gates.Items.at(gIndex)->name + ".";
//                  return false;
//              }

              // We remove the current gate from currentLine, and then,...
//              ppgLines.currentLine.remove(0,arg.size());
              argumentString.remove(0,arg.size());
              // We remove the last ')' in arg.
              arg.chop(1);

              lhs=sl.at(0).trimmed();
              rhs=sl.at(1).trimmed();

//              lhs=arg.left(arg.indexOf(',')).trimmed();
//              rhs=arg.mid(arg.indexOf(',')+1).trimmed();

              if(!evalArg(lhs)) {errorMessage=QString(Q_FUNC_INFO)+": Invalid expression: " + lhs; return false;}
              re=evalArgResult;
              if(!evalArg(rhs)) {errorMessage=QString(Q_FUNC_INFO)+": Invalid expression: " + rhs; return false;}
              im=evalArgResult;

              if(sl.size()==2)
              {
                  ppgLines.currentLine.prepend("," + gates.Items.at(gIndex)->translate(re,im));
              }
              else if(sl.size()==3)
              {
                  opt=sl.at(2).trimmed();
                  if(!evalArg(opt)) {errorMessage=QString(Q_FUNC_INFO)+": Invalid expression: " + opt; return false;}
                  phaseOffset=evalArgResult;
                  ppgLines.currentLine.prepend("," + gates.Items.at(gIndex)->translate(re,im,phaseOffset));
                 // qDebug()<<QString(Q_FUNC_INFO) << re << im << phaseOffset;
              }


              break;
            } // case GRFIQ

            default: break;
          }


          switch(gates.Items.at(gIndex)->KindOfGate())
          {
            case GAmplitude:
            case GPhase:
            case GLogicVector:
            case GInteger:
            case GAD9858:
            // We apply transformation (if defined)
            // By defaut, transformation function is "#" (something like "y=x"),
            // which does nothing.
            // F?Freq(-1;xxx) (profile -1: freq. sweep) is NOT transformed exceptionally.
            if(profile>-1)
            {
              QString trFunction=gates.Items.at(gIndex)->transformFunction;

              //qDebug() << trFunction;

              trFunction.replace("#","1.0*#"); // necessary to avoid error with "--" (20151208 Takeda)
              trFunction.replace("#",QString::number(evalArgResult,'g',12));
              //qDebug() << "before: " << trFunction << evalArgResult;

              if(!evalArg(trFunction))
              {
                errorMessage=QString(Q_FUNC_INFO)+": Invalid transformation expression: " + trFunction;
                return false;
              }
              if(!trFunction.isEmpty())
                  // if ';' is missing, the next preamble command may be ignored, causing malfunction.
                  // Fixed (2017.07.03 Takeda)
              {
                //  qDebug()<<QString(Q_FUNC_INFO)<<trFunction;
                  errorMessage = QString(Q_FUNC_INFO)
                          +": Invalid transformation expression (';' missing at the end of declaration?): "
                          +gates.Items.at(gIndex)->transformFunction;
                  return false;
              }
            //qDebug() << "after: " << trFunction << evalArgResult;
            }
              break;
          default: break;
          }

          // We check range
          switch(gates.Items.at(gIndex)->KindOfGate())
          {
          case GRFIQ: break;
          case GAmplitude:
              //  check if arg is a variable, and it is an amplitude variable.
              if(!TampWord::checkRange(evalArgResult))
              {
                  errorMessage=QString(Q_FUNC_INFO)+": Amplitude ("
                          +gates.Items.at(gIndex)->name + "=" + QString::number(evalArgResult) + ") is out of range.";
                  return false;
              }

              gates.Items[gIndex]->setValue(evalArgResult);
              if(gates.Items.at(gIndex)->channel()==currentCH) lineSet += gates.Items.at(gIndex)->output();
              break;

          case GPhase:
              gates.Items[gIndex]->setValue(evalArgResult);
              if(gates.Items.at(gIndex)->channel()==currentCH) lineSet += gates.Items.at(gIndex)->output();
              break;

          case GLogic:
              if(gates.Items.at(gIndex)->channel()==currentCH)
                  lineSet += gates.Items.at(gIndex)->output();
              break;
          case GPhaselist:
              if(gates.Items.at(gIndex)->channel()==currentCH)
              {

                  int FxPhaseListIndex=-1;
                  for (int k=0; k<gates.Items.size(); k++)
                      if(0==QString::compare("F"+QString::number(currentCH+1)+"PhaseList",
                                             gates.Items.at(k)->name,Qt::CaseInsensitive)
                        ) FxPhaseListIndex=k;
                  if (FxPhaseListIndex<0)
                  {
                     errorMessage="F"+QString::number(currentCH+1)+"PhaseList was not declared in the gates.";
                     return false;
                  }

                  gates.Items[FxPhaseListIndex]->setValue(gates.Items.at(gIndex)->order());
                  lineSet += gates.Items.at(FxPhaseListIndex)->output();

              }
              break;
          case GLogicVector:
          case GInteger:
              q=round(evalArgResult);
              gates.Items[gIndex]->setValue(q);
              if(gates.Items.at(gIndex)->channel()==currentCH) lineSet += gates.Items.at(gIndex)->output();
              break;

          case GAD9858:
              d = evalArgResult;  // in MHz
              //
              //  prepare separate five QSet<int>
              //

              gates.Items[gIndex]->setValue(d);
              if(gates.Items.at(gIndex)->isLogicVector())
              {

                  if(gates.Items.at(gIndex)->channel()==currentCH) lineSet += gates.Items.at(gIndex)->output();

              }
              else
              {
                gates.Items[gIndex]->setProfile(profile);
                for(int ab=0; ab<5; ab++)
                {
                  gates.Items[gIndex]->setOrder(ab+1);   // 1..5
                  if(gates.Items.at(gIndex)->channel()==currentCH) AD9858Buffer[ab] = gates.Items[gIndex]->output();
                }
              }
            break;

          default:
              errorMessage=QString(Q_FUNC_INFO)+": Unknown gate";
              return false;
              break;
          } // switch
//      } // if(gates.Items.at(gIndex)->channel()==currentCH)



      if(!ppgLines.getStr()) {errorMessage=QString(Q_FUNC_INFO)+": Parenthesis is not closed."; return false;}
    //  qDebug()<<ppgLines.currentLine;

      if(ppgLines.currentLine.at(0)==')') // parenthesis closed.
      {
          ppgLines.currentLine=ppgLines.currentLine.remove(0,1).trimmed();
          finished=true;
      }
      else if(ppgLines.currentLine.at(0)==',') // next gate item!
      {
          ppgLines.currentLine=ppgLines.currentLine.remove(0,1).trimmed();
      }
      else
      {
          errorMessage=QString(Q_FUNC_INFO)+": Invalid expression: " + ppgLines.currentLine.at(0);
          return false;
      }
    } // while(!finished)

  return true;
}
//-----------------------------------------------------------------------------
bool TpulseProgram::analyzeGate(TppgLines &ppgLines)
/*
 *
 analyzeGate is to be replaced by processGate, which is capable of handling arithmetic operations
 *
 */
// We assume
//   pulse(pw;gate1,gate2(arg),...)
//   wait_H(trig; gate1, gate2(arg),...)
//   burst(gate1,gate2(arg),....)
// have already been modified to
//   gate1,gate2(arg),.....).
//
// We note that user might have used line feed, so that the current line looks like
//   gate1,
// which should be followed by
//   gate2(arg),
//   ...
//   )
{
    int index,gIndex,vIndex;
    containsAD9858=false;
    bool ok;
    double d;
    int q;
    QString lhs, rhs;
    int profile=0; // profile for AD9858 setup

    usedGateIndex.clear();
    lineSet.clear();

    QString gateString;
    QString arg;

    // We look up the AD9858 gate(s)
    bool finished=false;
    usedGateIndex.clear();

    while(!finished)
    {
      while (   (!ppgLines.currentLine.contains(','))
             && (!ppgLines.currentLine.contains('('))
             && (!ppgLines.currentLine.contains(')'))
             && (ppgLines.currentPosition < ppgLines.source.size()-1)   )
      {
        ppgLines.currentPosition++;
        ppgLines.currentLine += ppgLines.source.at(ppgLines.currentPosition).trimmed();
      }
      if (   (!ppgLines.currentLine.contains(','))
          && (!ppgLines.currentLine.contains('('))
          && (!ppgLines.currentLine.contains(')'))  )  // end of ppgLines without finding the target characters
      {
          errorMessage=QString(Q_FUNC_INFO);
          return false;
      }

      // Now ppgLines.currentLine should contain at least one of the target characters.
      // We look for the first one.

      index=-1;
      for(int k=0; k<ppgLines.currentLine.size(); k++)
          if(   (ppgLines.currentLine.at(k)==',' )
             || (ppgLines.currentLine.at(k)=='(' )
             || (ppgLines.currentLine.at(k)==')' )) {index=k; break;}

      if(index<0) {errorMessage=QString(Q_FUNC_INFO); return false;}

      gateString=ppgLines.currentLine.left(index).trimmed();
      ppgLines.currentLine=ppgLines.currentLine.remove(0,index).trimmed();

      if(gateString.isEmpty()) // empty ?
      // If the empty string is followed by ')', we just return without error.
      {
         if(ppgLines.currentLine.at(0)==')')
         {
             ppgLines.currentLine=ppgLines.currentLine.remove(0,1).trimmed();
             return true;
         }
         else
         {
             errorMessage=QString(Q_FUNC_INFO) + ": Invalid character " + ppgLines.currentLine.at(index);
             return false;
         }
      }


      // Gate query
      gIndex=gates.nameIndex(gateString);
      if (gIndex==-1)
      {
          errorMessage=QString(Q_FUNC_INFO) + ": Unknown gate " + gateString;
          return false;
      }

      // Multiple usage check
      if (usedGateIndex.contains(gIndex))
      {
          errorMessage=QString(Q_FUNC_INFO) +": Multiple usage of " + gates.Items.at(gIndex)->name + ".";
          return false;
      }
      usedGateIndex << gIndex;

      //
      //  check if AD9858 is used.
      //
      if( (gates.Items.at(gIndex)->KindOfGate()==GAD9858) && (gates.Items.at(gIndex)->channel()==currentCH) )
      {
          containsAD9858=true;
      }

      //
      // Async check
      //
      if( asyncManager.contains( gates.Items.at(gIndex)->channel()) )
      {
          errorMessage=QString(Q_FUNC_INFO) +": Channel number "
                  + QString::number(gates.Items.at(gIndex)->channel()+1)
                  + " has gone into the asynchronous mode, and it is invalid to use "
                  + gates.Items.at(gIndex)->name +" here.";
          return false;
      }


    //  if(gates.Items.at(gIndex)->channel()==currentCH)
    //  {
          //enum TKindOfGate {GAmplitude, GPhase, GLogic, GLogicVector, GAD9858, GPhaselist, GInteger};
          switch(gates.Items.at(gIndex)->KindOfGate())
          {
            case GAmplitude:
            case GPhase:
            case GLogicVector:
            case GInteger:
            case GAD9858:
              if(ppgLines.currentLine.at(0)!='(') {errorMessage="An argument is missing."; return false;}
              ppgLines.currentLine=ppgLines.currentLine.remove(0,1).trimmed();
              if(!ppgLines.getStr(')',&index)) {errorMessage="Parenthesis is not closed."; return false;}
              arg=ppgLines.currentLine.left(index);
              ppgLines.currentLine=ppgLines.currentLine.remove(0,index+1).trimmed();

              break;
            default: arg.clear(); break;
          }


          switch(gates.Items.at(gIndex)->KindOfGate())
          {
          case GAmplitude:

              //  check if arg is a variable, and it is an amplitude variable.
              vIndex=-1;
              for(int j=0; j<variables.size(); j++)
                  if(0==QString::compare(arg, variables.at(j)->name(), Qt::CaseInsensitive)) {vIndex=j;}

              if(vIndex>-1)
              {
                  if(variables.at(vIndex)->type()!=TVariable::ampVariable)
                  {
                      errorMessage = variables.at(vIndex)->name() + " is not an amplitude variable.";
                      return false;
                  }
                // add the current address to the variable
                  if(!errorCheckOnly)
                  {
                    variables[vIndex]->addresses[currentCH].append(compiledPPG.at(currentCH).size());
                    variables[vIndex]->gateIndex[currentCH].append(gIndex);
                  }
                  d=variables[vIndex]->value().toDouble();
              } //  check if list1[k] is a number ranging from 0.0 to 100.0
              else
              {
                  d=arg.toDouble(&ok);
                  if(!ok)
                  {
                      errorMessage="Invalid expression for " + gates.Items.at(gIndex)->name + ".";
                      return false;
                  }
                  if(!TampWord::checkRange(d))
                  {
                      errorMessage="Amplitude " + arg + " is out of range.";
                      return false;
                  }
              }

              gates.Items[gIndex]->setValue(d);
              if(gates.Items.at(gIndex)->channel()==currentCH) lineSet += gates.Items.at(gIndex)->output();

              break;
          case GPhase:
              //  check if arg is a variable, and it is a phase variable.
              vIndex=-1;
              for(int j=0; j<variables.size(); j++)
                  if(0==QString::compare(arg, variables.at(j)->name(), Qt::CaseInsensitive)) {vIndex=j;}

              if(vIndex>-1)
              {
                  if(variables.at(vIndex)->type()!=TVariable::phaseVariable)
                  {
                      errorMessage = variables.at(vIndex)->name() + " is not a phase variable.";
                      return false;
                  }
                // add the current address to the variable
                  if(!errorCheckOnly)
                  {
                    variables[vIndex]->addresses[currentCH].append(compiledPPG.at(currentCH).size());
                    variables[vIndex]->gateIndex[currentCH].append(gIndex);
                  }
                  d=variables[vIndex]->value().toDouble();
              }
              else
              {
                  d=arg.toDouble(&ok);
                  if(!ok)
                  {
                      errorMessage="Invalid expression for " + gates.Items.at(gIndex)->name + ".";
                      return false;
                  }
              }

              gates.Items[gIndex]->setValue(d);
              if(gates.Items.at(gIndex)->channel()==currentCH) lineSet += gates.Items.at(gIndex)->output();

              break;
          case GLogic:
              if(gates.Items.at(gIndex)->channel()==currentCH)
                  lineSet += gates.Items.at(gIndex)->output();
              break;
          case GPhaselist:
              if(gates.Items.at(gIndex)->channel()==currentCH)
              {

                  int FxPhaseListIndex=-1;
                  for (int k=0; k<gates.Items.size(); k++)
                      if(0==QString::compare("F"+QString::number(currentCH+1)+"PhaseList",
                                             gates.Items.at(k)->name,Qt::CaseInsensitive)
                        ) FxPhaseListIndex=k;
                  if (FxPhaseListIndex<0)
                  {
                     errorMessage="F"+QString::number(currentCH+1)+"PhaseList was not declared in the gates.";
                     return false;
                  }

                  gates.Items[FxPhaseListIndex]->setValue(gates.Items.at(gIndex)->order());
                  lineSet += gates.Items.at(FxPhaseListIndex)->output();

              }
              break;
          case GLogicVector:
          case GInteger:
              //  check if arg is a variable, and it is an integer variable.
              vIndex=-1;
              for(int j=0; j<variables.size(); j++)
                  if(0==QString::compare(arg, variables.at(j)->name(), Qt::CaseInsensitive)) {vIndex=j;}

              if(vIndex>-1)
              {
                  if(variables.at(vIndex)->type()!=TVariable::intVariable)
                  {
                      errorMessage = variables.at(vIndex)->name() + " is not an integer variable.";
                      return false;
                  }
                // add the current address to the variable
                  if(!errorCheckOnly)
                  {
                    variables[vIndex]->addresses[currentCH].append(compiledPPG.at(currentCH).size());
                    variables[vIndex]->gateIndex[currentCH].append(gIndex);
                  }
                  q=variables[vIndex]->value().toInt();
              }
              else
              {
                  q=arg.toInt(&ok);
                  if(!ok)
                  {
                      errorMessage="Invalid expression for " + gates.Items.at(gIndex)->name + ".";
                      return false;
                  }
              }

              gates.Items[gIndex]->setValue(q);
              if(gates.Items.at(gIndex)->channel()==currentCH) lineSet += gates.Items.at(gIndex)->output();

              break;
          case GAD9858:

            if(gates.Items.at(gIndex)->channel()==currentCH)
            {

              // check if arg contains ';' (profile; value)
              if(arg.contains(';'))
              {
                  lhs=arg.left(arg.indexOf(';',0)).trimmed();
                  profile=lhs.toInt(&ok);
                  if(!ok)
                  {
                      errorMessage="Invalid profile expression for " + gates.Items.at(gIndex)->name + ".";
                      return false;
                  }
                  if((profile<-1) || (profile>3))
                  {
                      errorMessage="Profile for " +gates.Items.at(gIndex)->name+ " is out of range (0..3 or -1).";
                      return false;
                  }

                  rhs=arg.remove(0,arg.indexOf(';',0)+1).trimmed();

              }
              else
              {
                  profile=0;
                  rhs=arg;
              }

              //
              //  rhs should be a frequency variable or a number.
              //
              //  check if rhs is a variable, and it is a frequency variable.


              vIndex=-1;
              for(int j=0; j<variables.size(); j++)
                  if(0==QString::compare(rhs, variables.at(j)->name(), Qt::CaseInsensitive)) {vIndex=j;}

              if(vIndex>-1)
              {
                  if(variables.at(vIndex)->type()!=TVariable::freqVariable)
                  {
                      errorMessage = variables.at(vIndex)->name() + " is not a frequency variable.";
                      return false;
                  }
                // add the current address to the variable
                  if(!errorCheckOnly)
                  {
                    variables[vIndex]->addresses[currentCH].append(compiledPPG.at(currentCH).size());
                    variables[vIndex]->gateIndex[currentCH].append(gIndex);
                    variables[vIndex]->AD9858ProfileIndex[currentCH].append(profile);
                  }
                  d=variables[vIndex]->value().toDouble();
              }
              else
              {
                  d=rhs.toDouble(&ok);
                  if(!ok)
                  {
                      errorMessage="Invalid expression for " + gates.Items.at(gIndex)->name + ".";
                      return false;
                  }
                  d *= 1000000;  // MHz -> Hz conversion

              }


            //  qDebug() << "profile: "<< profile << "    d: " << d;

              //
              //  prepare separate five QSet<int>
              //
              gates.Items[gIndex]->setValue(d);
              gates.Items[gIndex]->setProfile(profile);
              for(int ab=0; ab<5; ab++)
              {
                  gates.Items[gIndex]->setOrder(ab+1);   // 1..5
                  if(gates.Items.at(gIndex)->channel()==currentCH) AD9858Buffer[ab] = gates.Items[gIndex]->output();

//                  qDebug() << "currentCH: " << currentCH << "AD9858Buffer["<<ab<<"]: " << AD9858Buffer[ab];

              }
            } // if(gates.Items.at(gIndex)->channel()==currentCH)

            break;

          default:
              errorMessage=QString(Q_FUNC_INFO)+": Unknown gate";
              return false;
              break;
          } // switch
//      } // if(gates.Items.at(gIndex)->channel()==currentCH)



      if(!ppgLines.getStr()) {errorMessage=QString(Q_FUNC_INFO)+": Parenthesis is not closed."; return false;}
   //   qDebug()<<ppgLines.currentLine;

      if(ppgLines.currentLine.at(0)==')') // parenthesis closed.
      {
          ppgLines.currentLine=ppgLines.currentLine.remove(0,1).trimmed();
          finished=true;
      }
      else if(ppgLines.currentLine.at(0)==',') // next gate item!
      {
          ppgLines.currentLine=ppgLines.currentLine.remove(0,1).trimmed();
      }
      else
      {
          errorMessage=QString(Q_FUNC_INFO)+": Invalid expression: " + ppgLines.currentLine.at(0);
          return false;
      }
    } // while(!finished)

  return true;
}
//-----------------------------------------------------------------------------
bool TpulseProgram::m_delay(TppgLines &ppgLines) {

    if(!ppgLines.currentLine.startsWith('('))
    {
        errorMessage=QString(Q_FUNC_INFO) + ": \"(\" is expected after \"delay\".";
        return false;
    }

// remove '('
//ppgLines.currentLine=ppgLines.currentLine.remove(0,1).trimmed();

    // We remove '(' just after 'delay', and then all space characters in currentLine
    ppgLines.currentLine.remove(0,1).remove(QChar(' '));

    // We evaluate time expression
    if(!evalTime(ppgLines.currentLine)) return false;

    if(!ppgLines.currentLine.startsWith(')'))
    {
        errorMessage=QString(Q_FUNC_INFO)+": \")\" is expected.";
        return false;
    }

    ppgLines.currentLine.remove(0,1); // We remove ')'

//    int index;

//    if(!ppgLines.getStr(')',&index))
//    {
//        errorMessage=QString(Q_FUNC_INFO) + ": \")\" was not found.";
//        return false;
//    }

//    QString q = ppgLines.currentLine.left(index).trimmed();

    // get time from q;
//    if (!getTime(q)) {return false;}
    // Now the result is stored in TpulseProgram::getTimeResult;
    // If a time variable is used, the getTime function should have stored the address.

    // remove ".....)"
//    ppgLines.currentLine.remove(0,index+1);

    //
    // If the currentCH is in the asynchronous operation, we do nothing and return,
    //
    if(asyncManager.contains(currentCH)) return true;

    quint64 i64=quint64(round(getTimeResult*CLK));

    // If we are inside the freq-sweep loop, we update the net elapsed time
    if(sweepManager.contains(currentCH))
    {
      if(!elapsedTime.isEmpty()) elapsedTime.last()+=getTimeResult;
      else
      {
        errorMessage=QString(Q_FUNC_INFO) +
                ": elapsedTime (QList<double>) is empty. Please report to Takeda.";
        return false;
      }
    }


    if(i64==0)
    {
        warning=true;
        warningMessage.append(QString(Q_FUNC_INFO) + ": zero time is set.\n");
        return true;
    }
    else if(i64<4)
    {
        errorMessage=QString(Q_FUNC_INFO) + ": time is too short.";
        return false;
    }
    if(i64-4>(quint64(pow(2,40)-1)))
    {
        errorMessage=QString(Q_FUNC_INFO) + ": time is too long.";
        return false;
    }

    QString s=myHex(i64-4,10);

    if(!errorCheckOnly)
    {
      compiledPPG[currentCH].append(new TcompiledPPG(com_OUT,s));
      NOfLinesPerCommand[currentCH]->append(1);
    }
    return true;
}
//-----------------------------------------------------------------------------
bool TpulseProgram::m_burst(TppgLines &ppgLines) {
    if(!ppgLines.currentLine.startsWith('('))
    {
        errorMessage=QString(Q_FUNC_INFO) + ": \"(\" is expected after \"burst\".";
        return false;
    }

    // remove '('
    ppgLines.currentLine=ppgLines.currentLine.remove(0,1).trimmed();

    //
    //  Now, we examine gates.
    //
//    if (!analyzeGate(ppgLines)) return false;
    if (!processGate(ppgLines)) return false;

    if(containsAD9858)
    {
        errorMessage=QString(Q_FUNC_INFO)+": Frequency control cannot be used in \"burst\".";
        return false;
    }

    //
    // If the currentCH is in the asynchronous operation, we do nothing and return,
    //    after examining the gate.
    //
    if(asyncManager.contains(currentCH)) return true;

    if(!errorCheckOnly)
    {
      compiledPPG[currentCH].append(new TcompiledPPG(com_BURST,"0000000000",lineSet));
      NOfLinesPerCommand[currentCH]->append(1);
    }


    // If we are inside the freq-sweep loop, we update the net elapsed time
    if(sweepManager.contains(currentCH))
    {
      if(CLK==0)
      {
        errorMessage=QString(Q_FUNC_INFO) +
                ": CLK cannot be zero.";
        return false;
      }
      if(!elapsedTime.isEmpty()) elapsedTime.last()+=1.0/CLK;
      else
      {
        errorMessage=QString(Q_FUNC_INFO) +
                ": elapsedTime (QList<double>) is empty. Please report to Takeda.";
        return false;
      }
    }


    return true;
}
//-----------------------------------------------------------------------------
bool TpulseProgram::m_ready() {

    if(!errorCheckOnly)
    {
      compiledPPG[currentCH].append(new TcompiledPPG(com_ALL_SYNC));
      NOfLinesPerCommand[currentCH]->append(1);
    }
    return true;
}
//-----------------------------------------------------------------------------
bool TpulseProgram::m_freqSweep(TppgLines &ppgLines)
{
    //TODO: is sweep-loop closed? Does elapsedTime have only one element? -> Then, reset 0.0;
    // record address for later setup (when endSweep is detected)
    usedGateIndex.clear();
    containsAD9858=false;

    if(!ppgLines.currentLine.startsWith('('))
    {
        errorMessage=QString(Q_FUNC_INFO) + ": \"(\" is expected after \"sweep\".";
        return false;
    }

    bool parenthesisClosed=false;
    while (!parenthesisClosed && (ppgLines.currentPosition<ppgLines.source.size()))
    {
        if(ppgLines.currentLine.count('(') <= ppgLines.currentLine.count(')')) parenthesisClosed=true;
        else
        {
            ppgLines.currentPosition++;
            if(ppgLines.currentPosition<ppgLines.source.size()-1)
            {
               ppgLines.currentLine+=ppgLines.source.at(ppgLines.currentPosition).trimmed();
            }
        }
    }

    if(ppgLines.currentPosition>ppgLines.source.size()-1) ppgLines.currentPosition=ppgLines.source.size()-1;

    if(!parenthesisClosed)
    {
          errorMessage=QString(Q_FUNC_INFO) + "Parenthesis is not closed.";
          return false;
    }

    // We remove '(' just after 'sweep', and then all space characters in currentLine
    ppgLines.currentLine.remove(0,1).remove(QChar(' '));

    bool finished=false;
  while(!finished)
  {
    // Now ppgLines.currentLine should contain at least one of the target characters.
    // We look for the first one.

    int index=-1;
    for(int k=0; k<ppgLines.currentLine.size(); k++)
        if(   (ppgLines.currentLine.at(k)==',' )
           || (ppgLines.currentLine.at(k)=='(' )
           || (ppgLines.currentLine.at(k)==')' )) {index=k; break;}

    if(index<0) {errorMessage=QString(Q_FUNC_INFO); return false;}

    // We extract the gate name
    QString gateString=ppgLines.currentLine.left(index).trimmed();
    // We remove the gate name from currentLine.
    ppgLines.currentLine=ppgLines.currentLine.remove(0,index).trimmed();

    if(gateString.isEmpty()) // empty ?
    // If the empty string is followed by ')', we just return without error.
    {
       if(ppgLines.currentLine.at(0)==')')
       {
           errorMessage=QString(Q_FUNC_INFO) + ": Item is empty";
           return false;
       }
       else
       {
           errorMessage=QString(Q_FUNC_INFO) + ": Invalid character " + ppgLines.currentLine.at(index);
           return false;
       }
    }

    // Gate query
    int gIndex=gates.nameIndex(gateString);
    if (gIndex==-1)
    {
        errorMessage=QString(Q_FUNC_INFO) + ": Unknown gate " + gateString;
        return false;
    }

    if(gates.Items.at(gIndex)->KindOfGate()!=GAD9858)
    {
        errorMessage=QString(Q_FUNC_INFO) + ": " + gateString+ " is not an AD9858 gate.";
        return false;
    }

    // Multiple usage check
    if (usedGateIndex.contains(gIndex))
    {
        errorMessage=QString(Q_FUNC_INFO) + ": Multiple usage of " + gates.Items.at(gIndex)->name + ".";
        return false;
    }

    usedGateIndex << gIndex;

    //
    //  check if AD9858 is used.
    //
    if(gates.Items.at(gIndex)->channel()==currentCH)
    {
        if(sweepManager.contains(currentCH))
        {
            errorMessage=QString(Q_FUNC_INFO) + ": Frequency sweep on channel "
                           + QString::number(currentCH+1) + " is not closed.";
            return false;
        }

        sweepManager << currentCH;
        loopManager.append(1000+currentCH);

       // qDebug()<< QString(Q_FUNC_INFO) << sweepManager << loopManager;
        containsAD9858=true;
    }

    if(elapsedTime.size()!=1)
    {
        errorMessage=QString(Q_FUNC_INFO) + ": There is something wrong with elapsedTime. Please report to Takeda.";
        return false;
    }

    elapsedTime[0]=0.0;

    if(ppgLines.currentLine.at(0)=='(')
    {
        ppgLines.currentLine=ppgLines.currentLine.remove(0,1).trimmed();
    }
    else
    {
        errorMessage=QString(Q_FUNC_INFO)+": '(' is expected after " + gates.Items.at(gIndex)->name+'.';
        return false;
    }

    //TODO: process F?Freq(start,stop), ...
    QString arg; arg.clear();
    int pos=0;

    while(arg.count(')')<arg.count('(')+1 && pos<ppgLines.currentLine.size())
    {
        arg.append(ppgLines.currentLine.at(pos));
        pos++;
    }
    if(arg.count(')')!=arg.count('(')+1)
    {
        errorMessage=QString(Q_FUNC_INFO) // + "In " + ppgLines.currentLine + " "
                +": Parenthesis is not closed for "
                + gates.Items.at(gIndex)->name + ".";
        return false;
    }

    if (arg.count(',')!=1)
    {
        errorMessage=QString(Q_FUNC_INFO) // + "In " + ppgLines.currentLine + " "
                +": 2 parameters are required for "
                + gates.Items.at(gIndex)->name + ".";
        return false;
    }
    // We remove the current gate from currentLine, and then,...
    ppgLines.currentLine.remove(0,arg.size());
    // We remove the last ')' in arg.
    arg.chop(1);

    QString lhs=arg.left(arg.indexOf(',')).trimmed();
    QString rhs=arg.mid(arg.indexOf(',')+1).trimmed();

    if(!evalArg(lhs)) {errorMessage=QString(Q_FUNC_INFO)+": Invalid expression: " + lhs; return false;}
    gates.Items[gIndex]->setSweepStartFreq(evalArgResult);
    if(!evalArg(rhs)) {errorMessage=QString(Q_FUNC_INFO)+": Invalid expression: " + lhs; return false;}
    gates.Items[gIndex]->setSweepStopFreq(evalArgResult);


    if(ppgLines.currentLine.at(0)==')') // parenthesis closed.
    {
        ppgLines.currentLine=ppgLines.currentLine.remove(0,1).trimmed();
        finished=true;
    }
    else if(ppgLines.currentLine.at(0)==',') // next gate item!
    {
        ppgLines.currentLine=ppgLines.currentLine.remove(0,1).trimmed();
    }
    else
    {
        errorMessage=QString(Q_FUNC_INFO)+": Invalid expression: " + ppgLines.currentLine.at(0);
        return false;
    }
  } // while(!finished)


    // If sweep is not used for the current CH, we just add four blank lines
    if(!containsAD9858)
    {  // 100 + 100 + 100 + 500 + 500 + 100 + 100 ns -> 1500 ns (EC)
       //ppgLines.source.insert( ppgLines.currentPosition+1, "delay(1500n)");
       //ppgLines.fileIndex.insert( ppgLines.currentPosition+1, -1); // no line(file) index
       //ppgLines.lineIndex.insert( ppgLines.currentPosition+1, -1); //     ->  -1
   //   qDebug() << "ch" << currentCH << ": 1500 ns delay has been added.";

      QSet<int> lSet;
      lSet.clear();
      compiledPPG[currentCH].append(new TcompiledPPG(com_OUT,"00000000EC", lSet));
      NOfLinesPerCommand[currentCH]->append(1);

      return true;
    }

    if(!errorCheckOnly) freqSweepInsertionPosition=compiledPPG.at(currentCH).size();

    //qDebug() << QString(Q_FUNC_INFO) << "1";

    return true;
}
//-----------------------------------------------------------------------------
bool TpulseProgram::m_endFreqSweep(TppgLines &ppgLines)
{
    usedGateIndex.clear();
    containsAD9858=false;
    int AD9858GateIndex=-1;

    //qDebug() << QString(Q_FUNC_INFO) << "0";

    if(!ppgLines.currentLine.startsWith('('))
    {
        errorMessage=QString(Q_FUNC_INFO) + ": \"(\" is expected after \"endSweep\".";
        return false;
    }

    bool parenthesisClosed=false;
    while (!parenthesisClosed && (ppgLines.currentPosition<ppgLines.source.size()))
    {
        if(ppgLines.currentLine.count('(') <= ppgLines.currentLine.count(')')) parenthesisClosed=true;
        else
        {
            ppgLines.currentPosition++;
            if(ppgLines.currentPosition<ppgLines.source.size()-1)
            {
               ppgLines.currentLine+=ppgLines.source.at(ppgLines.currentPosition).trimmed();
            }
        }
    }

    //qDebug() << QString(Q_FUNC_INFO) << "1";

    if(ppgLines.currentPosition>ppgLines.source.size()-1) ppgLines.currentPosition=ppgLines.source.size()-1;

    if(!parenthesisClosed)
    {
          errorMessage=QString(Q_FUNC_INFO) + "Parenthesis is not closed.";
          return false;
    }

    // We remove '(' just after 'sweep', and then all space characters in currentLine
    ppgLines.currentLine.remove(0,1).remove(QChar(' '));

    bool finished=false;
  while(!finished)
  {
    // Now ppgLines.currentLine should contain at least one of the target characters.
    // We look for the first one.

    int index=-1;
    for(int k=0; k<ppgLines.currentLine.size(); k++)
        if(   (ppgLines.currentLine.at(k)==',' )
           || (ppgLines.currentLine.at(k)=='(' )
           || (ppgLines.currentLine.at(k)==')' )) {index=k; break;}

    if(index<0) {errorMessage=QString(Q_FUNC_INFO); return false;}

    // We extract the gate name
    QString gateString=ppgLines.currentLine.left(index).trimmed();
    // We remove the gate name from currentLine.
    ppgLines.currentLine=ppgLines.currentLine.remove(0,index).trimmed();

    if(gateString.isEmpty()) // empty ?
    // If the empty string is followed by ')', we just return without error.
    {
       if(ppgLines.currentLine.at(0)==')')
       {
           errorMessage=QString(Q_FUNC_INFO) + ": Item is empty";
           return false;
       }
       else
       {
           errorMessage=QString(Q_FUNC_INFO) + ": Invalid character " + ppgLines.currentLine.at(index);
           return false;
       }
    }

    // Gate query
    int gIndex=gates.nameIndex(gateString);
    if (gIndex==-1)
    {
        errorMessage=QString(Q_FUNC_INFO) + ": Unknown gate " + gateString;
        return false;
    }

    if(gates.Items.at(gIndex)->KindOfGate()!=GAD9858)
    {
        errorMessage=QString(Q_FUNC_INFO) + ": " + gateString+ " is not an AD9858 gate.";
        return false;
    }

    // Multiple usage check
    if (usedGateIndex.contains(gIndex))
    {
        errorMessage=QString(Q_FUNC_INFO) + ": Multiple usage of " + gates.Items.at(gIndex)->name + ".";
        return false;
    }

    usedGateIndex << gIndex;

    //
    //  check if AD9858 is used.
    //
    if(gates.Items.at(gIndex)->channel()==currentCH)
    {
       // qDebug()<< QString(Q_FUNC_INFO) << sweepManager << loopManager;

        AD9858GateIndex=gIndex;
        if(!sweepManager.contains(currentCH) || !loopManager.contains(1000+currentCH))
        {
            qDebug() << sweepManager << loopManager;
            errorMessage=QString(Q_FUNC_INFO) + ": Frequency sweep on channel "
                           + QString::number(currentCH+1) + " has not started.";
            return false;
        }

        sweepManager.remove(currentCH);

        int lIndex=loopManager.indexOf(1000+currentCH);
        for(int k=loopManager.size()-1; k>=lIndex; k--)
        {
          if(loopManager.at(k)<1000)
          {
            errorMessage=QString(Q_FUNC_INFO)+ ": Loop and sweep-loop cannot cross.";
            return false;
          }
        }

        loopManager.removeAt(lIndex);

        containsAD9858=true;
    }

    if(ppgLines.currentLine.at(0)==')') // parenthesis closed.
    {
        ppgLines.currentLine=ppgLines.currentLine.remove(0,1).trimmed();
        finished=true;
       // qDebug() << "finished";
    }
    else if(ppgLines.currentLine.at(0)==',') // next gate item!
    {
       // qDebug() << "next";
        ppgLines.currentLine=ppgLines.currentLine.remove(0,1).trimmed();
    }
    else
    {
        errorMessage=QString(Q_FUNC_INFO)+": Invalid expression: " + ppgLines.currentLine.at(0);
        return false;
    }

  } // while (!finished)


  //qDebug() << QString(Q_FUNC_INFO) << "2";

  //QString dt="1u";

  if(!containsAD9858)
  {  // 500 + 500 + 100 + 100 ns -> 1200 ns (BC)
     // ppgLines.source.insert( ppgLines.currentPosition+1, "delay(1200n)");
     // ppgLines.fileIndex.insert( ppgLines.currentPosition+1, -1); // no line(file) index
     // ppgLines.lineIndex.insert( ppgLines.currentPosition+1, -1); //     ->  -1

    QSet<int> lSet;
    lSet.clear();
    compiledPPG[currentCH].append(new TcompiledPPG(com_OUT,"00000000BC", lSet));
    NOfLinesPerCommand[currentCH]->append(1);

    return true;
  }

  // We come here if currentCH is relevant.

//  qDebug() << currentCH << "OK?";

  int FUD = 32768;
  int FREQ_SWEEP_ENABLE = 16768;
  int AUTO_CLR_FREQ_ACCUM = 17024;

  TppgLines tempppgLines;
  tempppgLines.source.clear();
  tempppgLines.source.insert(ppgLines.currentPosition+1,
                         "pulse(500n;"+gates.Items.at(AD9858GateIndex)->name
                         +"("
                         +QString::number(gates.Items.at(AD9858GateIndex)->sweepStopFreq(),'g',12)
                         +"))"
                         );
  tempppgLines.fileIndex.insert( ppgLines.currentPosition+1, -1); // no line(file) index
  tempppgLines.lineIndex.insert( ppgLines.currentPosition+1, -1); //     ->  -1

  tempppgLines.source.insert(ppgLines.currentPosition+2,
                         "pulse(500n;"+gates.Items.at(AD9858GateIndex)->name
                         +"(-1;0.0)"
                         +")"
                         );
  tempppgLines.fileIndex.insert( ppgLines.currentPosition+2, -1); // no line(file) index
  tempppgLines.lineIndex.insert( ppgLines.currentPosition+2, -1); //     ->  -1

  tempppgLines.source.insert(ppgLines.currentPosition+3,
                         "pulse(100n;"+gates.Items.at(AD9858GateIndex)->name
                         +"(setup;"+QString::number(AUTO_CLR_FREQ_ACCUM)
                         +"))"
                         );
  tempppgLines.fileIndex.insert( ppgLines.currentPosition+3, -1); // no line(file) index
  tempppgLines.lineIndex.insert( ppgLines.currentPosition+3, -1); //     ->  -1

  tempppgLines.source.insert(ppgLines.currentPosition+4,
                         "pulse(100n;"+gates.Items.at(AD9858GateIndex)->name
                         +"(setup;"+QString::number(FUD)
                         +"))"
                         );
  tempppgLines.fileIndex.insert( ppgLines.currentPosition+4, -1); // no line(file) index
  tempppgLines.lineIndex.insert( ppgLines.currentPosition+4, -1); //     ->  -1

  // qDebug() << QString(Q_FUNC_INFO) << "3";


  //{--- Frequency sweep start ---
  //pulse(500n; F1Freq(XFR1))  {   -- 1 --
  //pulse(500n; F1Freq(-1;XDF))  { -- 2 --  sets the freq increment (in MHz)
  //pulse(100n; F1FreqSetup(AUTO_CLR_FREQ_ACCUM))   { -- 3 --
  //                        { reset frequency to FR1
  //                        { This line should be followed by "FUD" (see the next line)
  //  pulse(100n; F1FreqSetup(FUD)) {  -- 4 --  Freq. sweep starts.
  //--- Frequency sweep stop ---
  //  pulse(500n; F1Freq(XFR0))
  //  pulse(500n; F1Freq(-1;0.0))
   // pulse(100n; F1Freq(setup;AUTO_CLR_FREQ_ACCUM))
   // pulse(100n; F1Freq(setup;FUD))

  //   int FUD = 32768;
  //   int FREQ_SWEEP_ENABLE = 16768;
  //   int AUTO_CLR_FREQ_ACCUM = 17024;



  // Read the 1st line
  tempppgLines.currentPosition=0; tempppgLines.currentLine.clear();
  if (tempppgLines.currentPosition<tempppgLines.source.size())
      tempppgLines.currentLine=tempppgLines.source.at(tempppgLines.currentPosition);

  // Empty check
  if (!tempppgLines.getStr()) {errorMessage="automatic endSweep assigment is empty."; return false;}

  //
  finished=false;
  while (!finished) {
   //   qDebug() << QString(Q_FUNC_INFO)<< "1";
      int index=-1;
      for(int k=0; k<ppgCommands.size(); ++k) {
          if (tempppgLines.currentLine.startsWith(ppgCommands.at(k),Qt::CaseInsensitive)) {index=k; break;}
      }
      if(index<0) {
        errorMessage=QString(Q_FUNC_INFO)+": Syntax error (this could be a bug): Please report to Takeda" + tempppgLines.currentLine;
        return false;
      }
   //   qDebug() << QString(Q_FUNC_INFO)<< "2";

      // remove command string
      tempppgLines.currentLine=tempppgLines.currentLine.remove(0,ppgCommands.at(index).size()).trimmed();

      bool ok; // error detector
      switch(index) {
        case 0: ok=m_pulse(tempppgLines);  break; // pulse
        default: ok=false; errorMessage=QString(Q_FUNC_INFO)+"A bug occured. Please report to Takeda."; break;
      }
      if(!ok) return false;

      if (!tempppgLines.getStr()) finished=true;  // end of ppg
  }




  if(elapsedTime.size()!=1)
  {
      errorMessage=QString(Q_FUNC_INFO) +": There is something wrong with elapsedTime.";
      return false;
  }
  if(elapsedTime.at(0)<=0)
  {
      errorMessage=QString(Q_FUNC_INFO)+": Sweep time cannot be zero.";
      return false;
  }

  int findex=1;
  double dt=2.048; // we start with 2.048 us (8 ns x 256)
  double df=(gates.Items.at(AD9858GateIndex)->sweepStopFreq()
             -gates.Items.at(AD9858GateIndex)->sweepStartFreq())/(elapsedTime.at(0)/dt);   // elapsedTime [us]
//  while(df<50e-6)
  while(fabs(df)<50e-6)  // 2016.10.6 KT
  {
    dt+=2.048; // we increase dt by 1us
    if(dt>500)
    {
        errorMessage = QString(Q_FUNC_INFO) + ": frequency sweep out of range.";
        return false;
    }
    df=(gates.Items.at(AD9858GateIndex)->sweepStopFreq()
         -gates.Items.at(AD9858GateIndex)->sweepStartFreq())/(elapsedTime.at(0)/dt);   // elapsedTime [us]

   // qDebug() << "elapsed time: " <<elapsedTime.at(0) << "dt:" << dt << "df:" << df;
    findex++;
  }

 // qDebug()<< "elapsedTime:" << elapsedTime << "df: "<< df;

  /*
   //   int FUD = 32768;
   //   int FREQ_SWEEP_ENABLE = 16768;
   //   int AUTO_CLR_FREQ_ACCUM = 17024;
      pulse(1u; F2Freq(FR1))  { start freq
      pulse(1u; F2Freq(-1;DF))  {increment
      pulse(1u; F2FreqSetup(AUTO_CLR_FREQ_ACCUM)) -> pulse(1u; F2Freq(setup;val)
      pulse(1u; F2FreqSetup(FUD))
  */

  //qDebug() << QString(Q_FUNC_INFO) << "4";

   freqSweepPPGLines.source.clear();
   freqSweepPPGLines.lineIndex.clear();
   freqSweepPPGLines.fileIndex.clear();

   // We enable frequency sweep
   freqSweepPPGLines.source.append(
                                  "pulse(100n;"+gates.Items.at(AD9858GateIndex)->name
                                  +"(setup;"+QString::number(FREQ_SWEEP_ENABLE)
                                  +"))"
                                  );
   freqSweepPPGLines.fileIndex.append(-1); // no line(file) index
   freqSweepPPGLines.lineIndex.append(-1); //     ->  -1

   // We set, for the time being (tentatively) the step to be 1 us
   freqSweepPPGLines.source.append(
                                  "pulse(100n;"+gates.Items.at(AD9858GateIndex)->name
                                  +"(setup;18432"  // 100100000000000
                   //               +"(setup;18557"  // 100100001111101
                                  +"))"
                                  );
   freqSweepPPGLines.fileIndex.append(-1); // no line(file) index
   freqSweepPPGLines.lineIndex.append(-1); //     ->  -1
   freqSweepPPGLines.source.append(
                                  "pulse(100n;"+gates.Items.at(AD9858GateIndex)->name
                                  +"(setup;" + QString::number(18688+findex)
                                  +"))"
                                  );
   freqSweepPPGLines.fileIndex.append(-1); // no line(file) index
   freqSweepPPGLines.lineIndex.append(-1); //     ->  -1



   freqSweepPPGLines.source.append(
                          "pulse(500n;"+gates.Items.at(AD9858GateIndex)->name
                          +"("
                          +QString::number(gates.Items.at(AD9858GateIndex)->sweepStartFreq(),'g',12)
                          +"))"
                          );
   freqSweepPPGLines.fileIndex.append(-1); // no line(file) index
   freqSweepPPGLines.lineIndex.append(-1); //     ->  -1

   freqSweepPPGLines.source.append(
                          "pulse(500n;"+gates.Items.at(AD9858GateIndex)->name
                          +"(-1;"+QString::number(df,'g',12)
                          +"))"
                          );
   freqSweepPPGLines.fileIndex.append(-1); // no line(file) index
   freqSweepPPGLines.lineIndex.append(-1); //     ->  -1

   freqSweepPPGLines.source.append(
                          "pulse(100n;"+gates.Items.at(AD9858GateIndex)->name
                          +"(setup;"+QString::number(AUTO_CLR_FREQ_ACCUM)
                          +"))"
                          );
   freqSweepPPGLines.fileIndex.append(-1); // no line(file) index
   freqSweepPPGLines.lineIndex.append(-1); //     ->  -1

   freqSweepPPGLines.source.append(
                          "pulse(100n;"+gates.Items.at(AD9858GateIndex)->name
                          +"(setup;"+QString::number(FUD)
                          +"))"
                          );
   freqSweepPPGLines.fileIndex.append(-1); // no line(file) index
   freqSweepPPGLines.lineIndex.append(-1); //     ->  -1

   // Read the 1st line
   freqSweepPPGLines.currentLine.clear();
   freqSweepPPGLines.currentPosition=0;

   //  if (freqSweepPPGLines.currentPosition<freqSweepPPGLines.source.size())
   //      freqSweepPPGLines.currentLine=freqSweepPPGLines.source.at(freqSweepPPGLines.currentPosition);

 //  qDebug() << QString(Q_FUNC_INFO) << "5";

   if(!m_freqSweepLateAssign(freqSweepPPGLines)) return false;

 //  qDebug() << QString(Q_FUNC_INFO) << "6";

   return true;
}
//-----------------------------------------------------------------------------
bool TpulseProgram::m_freqSweepLateAssign(TppgLines &ppgLines)
{
  while(!insertionCompiledPPG.isEmpty()) delete insertionCompiledPPG.takeLast();
 // insertionCompiledPPG.clear();

  isInsertActive=true;

  // Read the 1st line
  ppgLines.currentPosition=0; ppgLines.currentLine.clear();
  if (ppgLines.currentPosition<ppgLines.source.size()) ppgLines.currentLine=ppgLines.source.at(ppgLines.currentPosition);

  // Empty check
  if (!ppgLines.getStr()) {errorMessage="Sweep late assigment is empty."; return false;}

 // qDebug() << QString(Q_FUNC_INFO)<< "0";

  //
  bool finished=false;
  while (!finished) {
   //   qDebug() << QString(Q_FUNC_INFO)<< "1";
      int index=-1;
      for(int k=0; k<ppgCommands.size(); ++k) {
          if (ppgLines.currentLine.startsWith(ppgCommands.at(k),Qt::CaseInsensitive)) {index=k; break;}
      }
      if(index<0) {
        errorMessage=QString(Q_FUNC_INFO)+": Syntax error (unknown command): " + ppgLines.currentLine;
        return false;
      }
   //   qDebug() << QString(Q_FUNC_INFO)<< "2";

      // remove command string
      ppgLines.currentLine=ppgLines.currentLine.remove(0,ppgCommands.at(index).size()).trimmed();

      bool ok; // error detector
      switch(index) {
        case 0: ok=m_pulse(ppgLines);  break; // pulse
        default: ok=false; errorMessage=QString(Q_FUNC_INFO)+"A bug occured. Please report to Takeda."; break;
      }
      if(!ok) return false;

      if (!ppgLines.getStr()) finished=true;  // end of ppg
  }

 // qDebug() << QString(Q_FUNC_INFO)<< "3";

  while(insertionCompiledPPG.size()>0)
  {
      compiledPPG[currentCH].insert(freqSweepInsertionPosition,
                                    new TcompiledPPG(insertionCompiledPPG.last()->binCommand,
                                                     insertionCompiledPPG.last()->argument,
                                                     insertionCompiledPPG.last()->lineSet
                                                     )
                                    );
      delete insertionCompiledPPG.takeLast();
  }


  isInsertActive=false;
 // qDebug() << QString(Q_FUNC_INFO)<< "4";

  return true;
}
//-----------------------------------------------------------------------------
bool TpulseProgram::m_loop(TppgLines &ppgLines)
{

//    qDebug()<<"Loop starting. loop manager:" <<loopManager;

    if(!ppgLines.currentLine.startsWith('('))
    {
        errorMessage=QString(Q_FUNC_INFO)
                + "Invalid loop expression.";
                // ": loop(c;lp) is expected, where c is 1 or 2, and lp is an integer number or a loop variable.";
        return false;
    }


    /*
     *
    bool parenthesisClosed=false;
    while (!parenthesisClosed && (ppgLines.currentPosition<ppgLines.source.size()))
    {
        if(ppgLines.currentLine.count('(') <= ppgLines.currentLine.count(')')) parenthesisClosed=true;
        else
        {
            ppgLines.currentPosition++;
            if(ppgLines.currentPosition<ppgLines.source.size()-1)
            {
               ppgLines.currentLine+=ppgLines.source.at(ppgLines.currentPosition).trimmed();
            }
        }
    }

     *
     */

    // remove '('
    ppgLines.currentLine=ppgLines.currentLine.remove(0,1).trimmed();




    QString arg; arg.clear();
    int pos=0;

    while(arg.count(')')<arg.count('(')+1 && pos<ppgLines.currentLine.size())
    {
        arg.append(ppgLines.currentLine.at(pos));
        pos++;
    }
    if(arg.count(')')!=arg.count('(')+1)
    {
        errorMessage=QString(Q_FUNC_INFO) // + "In " + ppgLines.currentLine + " "
                +": Loop parenthesis is not closed.";
               // + gates.Items.at(gIndex)->name + ".";
        return false;
    }

    // We remove the current gate from currentLine, and then,...
    ppgLines.currentLine.remove(0,arg.size());
    // We remove the last ')' in arg.
    arg.chop(1);


    int index;
    QString q1=arg;

/*
    int index;
    if(!ppgLines.getStr(')',&index))
    {
        errorMessage=QString(Q_FUNC_INFO) + ": \")\" was not found.";
        return false;
    }
    QString q1 = ppgLines.currentLine.left(index).trimmed();
    ppgLines.currentLine=ppgLines.currentLine.remove(0,index+1).trimmed();
*/

    //
    // If the currentCH is in the asynchronous operation, we do nothing and return,
    //
    if(asyncManager.contains(currentCH)) return true;

    QString ql,qr;
    int l,m;
    bool ok;

    index=q1.indexOf(';');
    if (index<0)
    {
      qr=q1;
      if(loopManager.contains(1) && loopManager.contains(2))
      {
        errorMessage=QString(Q_FUNC_INFO)
                + "loop cannot be nested more than twice.";
        return false;
      }
      if(loopManager.isEmpty()) l=1;
      else if(loopManager.contains(1)) l=2; else l=1;
    }
    else
    {
      ql=q1.left(index).trimmed(); // either 1 or 2
      qr=q1.remove(0,index+1).trimmed();  // either loop var or an integer (< 2^40-1)

      l=ql.toInt(&ok);
      if(!ok)
      {
        errorMessage=QString(Q_FUNC_INFO)+": Either 1 or 2 is allowed for the loop counter.";
        return false;
      }

      if(l<1 || l>MAX_LOOPS)
      {
        errorMessage=QString(Q_FUNC_INFO)+": Either 1 or 2 is allowed for the loop counter.";
        return false;
      }
    }

    ok=evalArg(qr);
    if(!ok) return false;
    m=round(evalArgResult);


    if(m==0)
    {
        loopZero=true;
        zeroLoopCounter=l;
        if(!errorCheckOnly) zeroLoopAddress=compiledPPG.at(currentCH).size();

    }
    else if((m<MIN_LOOP_VALUE) || (m>MAX_LOOP_VALUE))
    {
        errorMessage=QString(Q_FUNC_INFO)+": Out of range.";
        return false;
    }

    //
    //  check if loop usage is valid
    //
    if (loopManager.contains(l))
    {
        errorMessage=QString(Q_FUNC_INFO)+": Loop is not closed.";
        return false;
    }

    loopManager.append(l);

    if(sweepManager.contains(currentCH))
    {
      loopCounts.append(m);
      elapsedTime.append(0.0);
    }

    TbinCommand cm;
    if(l==1) cm=com_LOOP1; else cm=com_LOOP2;

    if(!errorCheckOnly)
    {
      compiledPPG[currentCH].append(new TcompiledPPG(cm,myHex(m,10)));
      NOfLinesPerCommand[currentCH]->append(1);
    }

//    qDebug()<<"Loop started. loop manager:" <<loopManager;


    return true;
}
//-----------------------------------------------------------------------------
bool TpulseProgram::m_endlp(TppgLines &ppgLines) {

//    qDebug()<<"Loop ending. loop manager:" <<loopManager;

    bool ok;
    int l;

    if(!ppgLines.currentLine.startsWith('('))
    {
       l=loopManager.last();
        //errorMessage=QString(Q_FUNC_INFO)
        //        + ": endlp(c) is expected, where c is 1 or 2.";
        //return false;
    }
    else
    {
      // remove '('
      ppgLines.currentLine=ppgLines.currentLine.remove(0,1).trimmed();

      int index;

      if(!ppgLines.getStr(')',&index))
      {
        errorMessage=QString(Q_FUNC_INFO) + ": \")\" was not found.";
        return false;
      }

      QString q1 = ppgLines.currentLine.left(index).trimmed();
      ppgLines.currentLine=ppgLines.currentLine.remove(0,index+1).trimmed();

      l=q1.toInt(&ok);
      if(!ok)
      {
          errorMessage=QString(Q_FUNC_INFO)+": Either 1 or 2 is allowed for the loop counter.";
          return false;
      }
      if(l<1 || l>MAX_LOOPS)
      {
          errorMessage=QString(Q_FUNC_INFO)+": Either 1 or 2 is allowed for the loop counter.";
          return false;
      }


      if (!loopManager.contains(l))
      {
          errorMessage=QString(Q_FUNC_INFO)+": loop counter #" + QString::number(l) +" has not started.";
          return false;
      }

      if(loopManager.last()!=l)
      {
          errorMessage=QString(Q_FUNC_INFO)+": you need to close loop #" +QString::number(loopManager.last())
                  + " before loop #" +QString::number(l) + ".";
          return false;
      }


    }

    //
    // If the currentCH is in the asynchronous operation, we do nothing and return,
    //
    if(asyncManager.contains(currentCH)) return true;    

    if(loopManager.isEmpty())
    {
      errorMessage=QString(Q_FUNC_INFO) + "loop has not started.";
      return false;
    }


    loopManager.removeLast();

    if(sweepManager.contains(currentCH))
    {
      double tTime=elapsedTime.last(); // We get the elapsed time in the loop.
      elapsedTime.removeLast(); // Then, we remove the last item.
      if(elapsedTime.isEmpty()) {
        errorMessage=QString(Q_FUNC_INFO)+": elapsedTime (QList<double>) is empty. Please report to Takeda.";
        return false;
      }
      elapsedTime.last() += tTime * loopCounts.last();
       // And we add (elapsed time) * (loop cout) to the updated last item.
      loopCounts.removeLast(); // Finally we delete the counts of the closed loop.
    }

    TbinCommand cm;
    if (l==1) cm=com_END_LOOP1; else cm=com_END_LOOP2;

    if(!errorCheckOnly)
    {
      compiledPPG[currentCH].append(new TcompiledPPG(cm));
      NOfLinesPerCommand[currentCH]->append(1);

      if(loopZero==true && zeroLoopCounter==l)
      {
          compiledPPG[currentCH].replace(
                      zeroLoopAddress, new TcompiledPPG(com_JUMP,myHex(compiledPPG.at(currentCH).size(),10)));
          loopZero=false;
      }
    }

    if(loopZero==true && zeroLoopCounter==l) // in case errorCheckOnly
    {loopZero=false;}

//    qDebug()<<"Loop ended. loop manager:" <<loopManager;

    return true;
}
//-----------------------------------------------------------------------------
bool TpulseProgram::m_async(TppgLines &ppgLines) {
    if(!ppgLines.currentLine.startsWith('('))
    {
        errorMessage=QString(Q_FUNC_INFO)
                + ": async(aName) is expected, where aName is the name of the asynchronous sequence.";
        return false;
    }

    // remove '('
    ppgLines.currentLine=ppgLines.currentLine.remove(0,1).trimmed();

    int index;

    if(!ppgLines.getStr(')',&index))
    {
        errorMessage=QString(Q_FUNC_INFO) + ": \")\" was not found.";
        return false;
    }

    QString q1 = ppgLines.currentLine.left(index).trimmed();
    ppgLines.currentLine=ppgLines.currentLine.remove(0,index+1).trimmed();

    index=-1;
    for(int k=0; k<asyncPPG.size(); k++)
    {
        if(0==QString::compare(q1,asyncPPG.at(k)->name(),Qt::CaseInsensitive)) index=k;
    }

    if(index<0)
    {
        errorMessage=QString(Q_FUNC_INFO) + ": Asynchronous sequence " + q1 + " was not found.";
        return false;
    }

    //
    //  Check if the relevant channel has already been in the asynchronous mode.
    //
    if (asyncManager.contains(asyncPPG.at(index)->channel()))
    {
        errorMessage = QString(Q_FUNC_INFO) + ": Channel no "
                + QString::number(asyncPPG.at(index)->channel())
                + " is already in the asynchronous mode.";
        return false;
    }

    //
    //  Add the channel number to asyncManager
    //
    asyncManager << asyncPPG.at(index)->channel();
    asyncPPG[index]->called=true;

    // We do nothing if the current channel is irrelevant to the async operation
    if(currentCH!=asyncPPG.at(index)->channel())
      {return true;}

    //
    //  TODO: prepare for the late address assignment
    //

    if(!errorCheckOnly)
    {
      asyncPPG[index]->whereIs.append(compiledPPG.at(currentCH).size());
    }

    if(!errorCheckOnly)
    {
      compiledPPG[currentCH].append(new TcompiledPPG(com_ASYNC));
      NOfLinesPerCommand[currentCH]->append(1);
    }

    return true;
}
//-----------------------------------------------------------------------------
bool TpulseProgram::m_sync(TppgLines &ppgLines) {
    if(!ppgLines.currentLine.startsWith('('))
    {
        errorMessage=QString(Q_FUNC_INFO)
                + ": sync(ch) or sync(ch_a,ch_b) is expected.";
        return false;
    }

    // remove '('
    ppgLines.currentLine=ppgLines.currentLine.remove(0,1).trimmed();

    int index;

    if(!ppgLines.getStr(')',&index))
    {
        errorMessage=QString(Q_FUNC_INFO) + ": \")\" was not found.";
        return false;
    }

    QString q1 = ppgLines.currentLine.left(index).trimmed();
    ppgLines.currentLine=ppgLines.currentLine.remove(0,index+1).trimmed();

    QStringList sl1=q1.split(',');
    QList<int> il;
    bool ok; int in;
    for(int k=0; k<sl1.size(); k++)
    {
        in=sl1.at(k).toInt(&ok)-1;  // 1,2,3 -> 0,1,2
        if(!ok)
        {
            errorMessage=QString(Q_FUNC_INFO)+": Invalid expression: " + sl1.at(k);
            return false;
        }

        if(!asyncManager.contains(in))
        {
            errorMessage=QString(Q_FUNC_INFO)+": Channel " + QString::number(in)
                    +" has not gone into the asyncronous mode.";
            return false;
        }

        il << in;

        asyncManager -= in;
    }


    if(currentCH==0)
    {
      QSet<int> ab; ab.clear();
      if(il.contains(1)) ab << 62;
      if(il.contains(2)) ab << 63;

      if(!errorCheckOnly)
      {
        compiledPPG[currentCH].append(new TcompiledPPG(com_OUT,"0000000000",ab));
        NOfLinesPerCommand[currentCH]->append(1);
      }
    }

    return true;
}
//-----------------------------------------------------------------------------
bool TpulseProgram::m_wait_hl(TppgLines &ppgLines, int hl)
{    
    QString cm;
    if(hl==0) cm="\"wait_l\""; else cm="\"wait_h\"";

    if(!ppgLines.currentLine.startsWith('('))
    {
        errorMessage=QString(Q_FUNC_INFO) + ": \"(\" is expected after " + cm + ".";
        return false;
    }

    // remove '('
    ppgLines.currentLine=ppgLines.currentLine.remove(0,1).trimmed();

    int index=ppgLines.currentLine.indexOf(';',0);
    QString sWait;

    if (index==-1)
    {
        int index2=ppgLines.currentLine.indexOf(')',0);

        if(index2==-1)
        {
          errorMessage=QString(Q_FUNC_INFO) + ": \";\" and \")\" were not found.";
          return false;
        }
        else
        {
            sWait=ppgLines.currentLine.left(index2).trimmed();
            ppgLines.currentLine=ppgLines.currentLine.remove(0,index2+1).trimmed();
            lineSet.clear();
        }
    }
    else
    {
      sWait=ppgLines.currentLine.left(index).trimmed();
      ppgLines.currentLine=ppgLines.currentLine.remove(0,index+1).trimmed();

    //
    //  Now, we examine gates.
    //
      if (!processGate(ppgLines)) return false;

      if(containsAD9858)
      {
        errorMessage="Frequency control cannot be used in " + cm + ".";
        return false;
      }
    }
    //
    // If the currentCH is in the asynchronous operation, we do nothing and return,
    //    after examining the gate.
    //
    if(asyncManager.contains(currentCH)) return true;


    //
    //  Next, we decide which flag we wait for.
    //
    QString w;
    if (0==QString::compare(sWait,"trig_e",Qt::CaseInsensitive))     w="0000000001";
    else if(0==QString::compare(sWait,"trig_r",Qt::CaseInsensitive)) w="0000000002";
    else if(0==QString::compare(sWait,"trig_0",Qt::CaseInsensitive)) w="0000000004";
    else if(0==QString::compare(sWait,"trig_1",Qt::CaseInsensitive)) w="0000000008";
    else if(0==QString::compare(sWait,"trig_2",Qt::CaseInsensitive)) w="0000000010";
    else if(0==QString::compare(sWait,"trig_3",Qt::CaseInsensitive)) w="0000000020";
    else
    {
        errorMessage=QString(Q_FUNC_INFO) + ": Unknown wait flag " + sWait + ".";
        return false;
    }


    TbinCommand bCom;
    if(hl==0) bCom=com_WAIT_LOW; else bCom=com_WAIT_HIGH;
        //cm=" A3 "; // wait_L
//    else cm=" A2 "; // wait_h

    if(!errorCheckOnly)
    {
      compiledPPG[currentCH].append(new TcompiledPPG(bCom,w,lineSet));
      NOfLinesPerCommand[currentCH]->append(1);
    }

    return true;

}
//-----------------------------------------------------------------------------
bool TpulseProgram::m_exttrig()
{
    if(!errorCheckOnly)
    {
      compiledPPG[currentCH].append(new TcompiledPPG(com_WAIT_HIGH,"0000000001"));
                                    //   "0000000001" // the 0-th bit corresponds to the ext. trig
      NOfLinesPerCommand[currentCH]->append(1);
    }
    return true;
}
//-----------------------------------------------------------------------------
bool TpulseProgram::m_import(TppgLines &ppgLines)
{
    if(!ppgLines.currentLine.startsWith('('))
    {
        errorMessage=QString(Q_FUNC_INFO)
                + ": import(filename) is expected.";
        return false;
    }

    // remove '('
    ppgLines.currentLine=ppgLines.currentLine.remove(0,1).trimmed();

    int index;

    if(!ppgLines.getStr(')',&index))
    {
        errorMessage=QString(Q_FUNC_INFO) + ": \")\" was not found.";
        return false;
    }

    QString q1 = ppgLines.currentLine.left(index).trimmed();
    ppgLines.currentLine=ppgLines.currentLine.remove(0,index+1).trimmed();

    QString fname;
    fname=ppgFilePath()+"/"+q1;

    if(ppgLines.fileIndex.at(ppgLines.currentPosition)>0)
    {
        errorMessage=QString(Q_FUNC_INFO) + ": Import from an imported file is not allowed.";
        return false;
    }
//    if(loadedFiles.contains(fname))
//    {
//        errorMessage=QString(Q_FUNC_INFO) + ": Circular file import is not allowed.";
//        return false;
//    }

    QFile file(fname);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        error=true;
        errorMessage=QString(Q_FUNC_INFO) + ": Failed to open file.";
        return false;
    }

    if(!loadedFiles.contains(fname)) loadedFiles << fname;

    //
    //  We remove the "import" line
    //
    ppgLines.source.removeAt(ppgLines.currentPosition);
    ppgLines.fileIndex.removeAt(ppgLines.currentPosition);
    ppgLines.lineIndex.removeAt(ppgLines.currentPosition);

    //
    //  Then we insert the imported lines
    //
    int c=0;

    QTextStream in(&file);
    while (!in.atEnd())
    {
        ppgLines.source.insert( ppgLines.currentPosition+c, in.readLine());
        ppgLines.fileIndex.insert( ppgLines.currentPosition+c, loadedFiles.size()-1);
        ppgLines.lineIndex.insert( ppgLines.currentPosition+c, c );
        c++;
    }
    file.close();

    return true;
}
//-----------------------------------------------------------------------------
bool TpulseProgram::m_init()
{
    int naIndex=-1;
    for(int k=0; k<variables.size(); k++)
    {
        if(QString::compare(variables.at(k)->name(),"NA",Qt::CaseInsensitive)==0) naIndex=k;
    }

    if(naIndex<0)
    {
        errorMessage=QString(Q_FUNC_INFO) + ": Mandatory NA is not declared.";
        return false;
    }
    // phase pointer init
    int FxPhasePointer_Init_Index=gates.nameIndex("F"+QString::number(currentCH+1)+"PhasePointer_Init");
    if(FxPhasePointer_Init_Index<0)
    {
        errorMessage = QString(Q_FUNC_INFO) +
                      "F"+QString::number(currentCH+1)+"PhasePointer_Init was not found in the gates.";
        return false;
    }

    int naValue=variables.at(naIndex)->value().toInt();
    if(naValue<1)
    {
       errorMessage=QString(Q_FUNC_INFO) + ": NA must be larger than 1.";
       return false;
    }

    if(!errorCheckOnly)
    {
      QSet<int> lSet;
      lSet.clear();
      lSet += gates.Items.at(FxPhasePointer_Init_Index)->output();
      compiledPPG[currentCH].append(new TcompiledPPG(com_OUT,"000000009C",lSet));
      NOfLinesPerCommand[currentCH]->append(1);


      variables[naIndex]->addresses[currentCH].append(compiledPPG.at(currentCH).size());
      compiledPPG[currentCH].append(new TcompiledPPG(com_LOOP0,myHex(naValue,10)));
      NOfLinesPerCommand[currentCH]->append(1);

      // ready
      compiledPPG[currentCH].append(new TcompiledPPG(com_ALL_SYNC,"0000000000"));
      NOfLinesPerCommand[currentCH]->append(1);
    }

    return true;

}
//-----------------------------------------------------------------------------
bool TpulseProgram::m_relax()
{
    int pdIndex=-1;
    for(int k=0; k<variables.size(); k++)
    {
        if(QString::compare(variables.at(k)->name(),"PD",Qt::CaseInsensitive)==0) pdIndex=k;
    }

    if(pdIndex<0)
    {
        errorMessage="Mandatory PD is not declared.";
        return false;
    }

    double pdValue=variables.at(pdIndex)->value().toDouble();
    if(pdValue<0)
    {
        errorMessage=QString(Q_FUNC_INFO) + ": PD cannot be negative.";
        return false;
    }
    quint64 i64=quint64(round(1000000*pdValue*CLK));
    // We check range
    if(i64<4)
    {
        errorMessage=QString(Q_FUNC_INFO) + ": PD is too short.";
        return false;
    }
    if(i64-4>(quint64(pow(2,40)-1)))
    {
        errorMessage=QString(Q_FUNC_INFO) + ": PD is too long.";
        return false;
    }


    if(!errorCheckOnly)
    {
      compiledPPG[currentCH].append(new TcompiledPPG(com_OUT_PD,myHex(i64-4,10)));
      NOfLinesPerCommand[currentCH]->append(1);
    }

//    QString com=" = A1 000000009C "; // OUT (1us)
    QSet<int> lSet;
//    quint64 line=0;

    // toggle phase
    int FxTogglePhaseIndex=gates.nameIndex("F"+QString::number(currentCH+1)+"TogglePhase");
    if(FxTogglePhaseIndex<0)
    {
        errorMessage="F"+QString::number(currentCH+1)+"TogglePhase was not found in the gates.";
        return false;
    }
    lSet.clear();
    lSet += gates.Items.at(FxTogglePhaseIndex)->output();

    if(!errorCheckOnly)
    {
      compiledPPG[currentCH].append(new TcompiledPPG(com_OUT,"000000009C",lSet));
      NOfLinesPerCommand[currentCH]->append(1);

    // wait low for RCVR_BUSY
               // when X"A3" => -- WAIT_LOW
               // latchGReg <= '1';
               // trigWReg <= argReg(5 downto 0);
               // stateReg <= WL;
               //    compiledPPG[currentCH]->append(
               //                myHex(compiledPPG.at(currentCH)->size(),3)
               //                + " = A3 0000000002 0000000000000000");
      compiledPPG[currentCH].append(new TcompiledPPG(com_WAIT_LOW,"0000000002"));
      NOfLinesPerCommand[currentCH]->append(1);

    // end loop0
      compiledPPG[currentCH].append(new TcompiledPPG(com_END_LOOP0,"0000000000"));
      NOfLinesPerCommand[currentCH]->append(1);

    // dummy 10^5=$186A0 counts
      compiledPPG[currentCH].append(new TcompiledPPG(com_OUT,"00000186A0"));
      NOfLinesPerCommand[currentCH]->append(1);

    // end ppg
      compiledPPG[currentCH].append(new TcompiledPPG(com_END_PPG,"0000000000"));
      NOfLinesPerCommand[currentCH]->append(1);

    }

    return true;
}
//-----------------------------------------------------------------------------
bool TpulseProgram::genAcqPhaseCycle(QStringList *sl)
{
    sl->clear();
    QString com="A1 000000009C "; // OUT (1us)
    QSet<int> lSet;
    int ch=0;
    int address=0;

    int ACQ_PHASE_CS_Index=gates.nameIndex("ACQ_PHASE_CS");
    if(ACQ_PHASE_CS_Index<0){
          errorMessage="ACQ_PHASE_CS was not found in the gate items."; return false;
    }
    int ACQ_PHASE_Index=gates.nameIndex("ACQ_PHASE");
    if(ACQ_PHASE_Index<0){
        errorMessage="ACQ_PHASE was not found in the gate items."; return false;
    }

    int FxPhaseModeIndex=gates.nameIndex("F"+QString::number(ch+1)+"PhaseMode");
    if(FxPhaseModeIndex<0){
        errorMessage="F"+QString::number(ch+1)+"PhaseMode was not found in the gate items."; return false;
    }

    int FxPhaseListLengthIndex=gates.nameIndex("F"+QString::number(ch+1)+"PhaseListLength");
    if(FxPhaseListLengthIndex<0){
        errorMessage="F"+QString::number(ch+1)+"PhaseListLength was not found in the gate items."; return false;
    }

    int FxPhaseCommandIndex=gates.nameIndex("F"+QString::number(ch+1)+"PhaseCommand");
    if(FxPhaseCommandIndex<0){
        errorMessage="F"+QString::number(ch+1)+"PhaseCommand was not found in the gate items."; return false;
    }

    int FxPhasePointer_InitIndex=gates.nameIndex("F"+QString::number(ch+1)+"PhasePointer_Init");
    if(FxPhasePointer_InitIndex<0){
        errorMessage="F"+QString::number(ch+1)+"PhasePointer_Init was not found in the gate items."; return false;
    }

    int FxTogglePhaseIndex=gates.nameIndex("F"+QString::number(ch+1)+"TogglePhase");
    if(FxTogglePhaseIndex<0){
        errorMessage="F"+QString::number(ch+1)+"TogglePhase was not found in the gate items."; return false;
    }

    quint64 line;
    quint64 one64=1;  // declaration of a 64-bit integer is important.


    lSet.clear();
    lSet += gates.Items.at(ACQ_PHASE_CS_Index)->output();
            gates.Items.at(FxPhaseModeIndex)->setValue(2);   // mode=2
    lSet += gates.Items.at(FxPhaseModeIndex)->output();
            gates.Items.at(FxPhaseListLengthIndex)->setValue(acqPhaseList.size());
    lSet += gates.Items.at(FxPhaseListLengthIndex)->output();
    line=0;
    for(int q=0; q<BITLENGTH; q++) if (lSet.contains(q)) line = (line | (one64 << q));
    sl->append(myHex(address,3)+"="+com+myHex(line,16));
    address++;


    lSet += gates.Items.at(FxPhaseCommandIndex)->output();
    line=0;
    for(int q=0; q<BITLENGTH; q++) if (lSet.contains(q)) line = (line | (one64 << q));
    sl->append(myHex(address,3)+"="+com+myHex(line,16));
    address++;

    lSet.clear();
    lSet += gates.Items.at(ACQ_PHASE_CS_Index)->output();
    lSet += gates.Items.at(FxPhasePointer_InitIndex)->output();
            gates.Items.at(FxPhaseModeIndex)->setValue(1);   // mode=1
    lSet += gates.Items.at(FxPhaseModeIndex)->output();
            gates.Items.at(ACQ_PHASE_Index)->setValue(acqPhaseList.at(0));
    lSet += gates.Items.at(ACQ_PHASE_Index)->output();
    line=0;
    for(int q=0; q<BITLENGTH; q++) if (lSet.contains(q)) line = (line | (one64 << q));
    sl->append(myHex(address,3)+"="+com+myHex(line,16));
    address++;

    lSet.clear();
    lSet += gates.Items.at(ACQ_PHASE_CS_Index)->output();
    lSet += gates.Items.at(FxPhaseCommandIndex)->output();
            gates.Items.at(FxPhaseModeIndex)->setValue(1);   // mode=1
    lSet += gates.Items.at(FxPhaseModeIndex)->output();
            gates.Items.at(ACQ_PHASE_Index)->setValue(acqPhaseList.at(0));
    lSet += gates.Items.at(ACQ_PHASE_Index)->output();
    line=0;
    for(int q=0; q<BITLENGTH; q++) if (lSet.contains(q)) line = (line | (one64 << q));
    sl->append(myHex(address,3)+"="+com+myHex(line,16));
    address++;


    for(int k=1; k<acqPhaseList.size(); k++)  // k starts with 1 (not zero!)
    {
    lSet.clear();
    lSet += gates.Items.at(ACQ_PHASE_CS_Index)->output();
    lSet += gates.Items.at(FxTogglePhaseIndex)->output();
            gates.Items.at(FxPhaseModeIndex)->setValue(1);   // mode=1
    lSet += gates.Items.at(FxPhaseModeIndex)->output();
            gates.Items.at(ACQ_PHASE_Index)->setValue(acqPhaseList.at(k));
    lSet += gates.Items.at(ACQ_PHASE_Index)->output();
    line=0;
    for(int q=0; q<BITLENGTH; q++) if (lSet.contains(q)) line = (line | (one64 << q));
    sl->append(myHex(address,3)+"="+com+myHex(line,16));
    address++;


    lSet.clear();
    lSet += gates.Items.at(ACQ_PHASE_CS_Index)->output();
    lSet += gates.Items.at(FxPhaseCommandIndex)->output();
            gates.Items.at(FxPhaseModeIndex)->setValue(1);   // mode=1
    lSet += gates.Items.at(FxPhaseModeIndex)->output();
            gates.Items.at(ACQ_PHASE_Index)->setValue(acqPhaseList.at(k));
    lSet += gates.Items.at(ACQ_PHASE_Index)->output();
    line=0;
    for(int q=0; q<BITLENGTH; q++) if (lSet.contains(q)) line = (line | (one64 << q));
    sl->append(myHex(address,3)+"="+com+myHex(line,16));
    address++;

    }  // k
    sl->append(myHex(address,3)+"=FF 0000000000 0000000000000000");
    address++;
    sl->append(myHex(address,3)+"=00 0000000000 0000000000000000");
    address++;
    sl->append("g");
    address=0;

    return true;
}

//-----------------------------------------------------------------------------
bool TpulseProgram::compilePPG()
{

    errorCheckOnly=false;
    compiled=false;
    acqPhaseList.clear(); acqPhaseList.append(0);

    userDefinedFunctions.clear();
    loadedFiles.clear();

    for(int ch=0; ch<channels(); ch++)
    {
        compiledPPG_str[ch]->clear();

        while(!compiledPPG.at(ch).isEmpty()) delete compiledPPG[ch].takeLast();
       // compiledPPG[ch].clear();


        compiledPhaseCycle[ch]->clear();
        NOfLinesPerCommand[ch]->clear();
    }
    error=false; errorMessage.clear();
    warning=false; warningMessage.clear();
    loopZero=false;


    if(!analyzePPG()) {error=true; return false;}
    if(!processPreamble()) //{error=true; return false;}
    {
        errorMessage = "In line " + QString::number(preamble.lineIndex.at(preamble.currentPosition) +1 )
                + ": " +errorMessage;
        return false;
    }
    if(!processPhaseCycle()) {error=true; return false;}
    if(!processMainPPG())
    {
        errorMessage = "In line " + QString::number(mainPPG.lineIndex.at(mainPPG.currentPosition) +1 )
                + ": " +errorMessage;
        return false;
    }
    if(!processAsyncPPG())
    {
        errorMessage = "In line "
                + QString::number(
                    asyncPPG.at(currentAsyncPPGIndex)->lineIndex.at(asyncPPG.at(currentAsyncPPGIndex)->currentPosition) +1 )
                +": " + errorMessage;
        return false;
    }

    toFPGA.clear();
    toFPGA.append(TreceiverCommand::updateAR(1));

    for(int ch=0; ch<channels(); ch++)
    {
      if(compiledPhaseCycle.at(ch)->size()>0)
      {
        for(int ch2=0; ch2<channels(); ch2++)
        {
            if(ch2!=ch)
            {
                toFPGA.append("ch"+QString::number(ch2+1));
                toFPGA.append("000=FF 0000000000 0000000000000000");
                toFPGA.append("001=00 0000000000 0000000000000000");
            }
        }
        toFPGA.append("ch"+QString::number(ch+1));
        for(int k=0; k<compiledPhaseCycle.at(ch)->size(); k++)
        {
            toFPGA.append(compiledPhaseCycle.at(ch)->at(k));
        }
      }
    }

    for(int ch=0; ch<channels(); ch++)
    {
      toFPGA.append("ch"+QString::number(ch+1));
      compiledPPG_str[ch]->clear();
      for(int k=0; k<compiledPPG.at(ch).size(); k++)
      {
        compiledPPG_str[ch]->append(myHex(compiledPPG_str[ch]->size(),3)
                                    + " = "
                                    + TcompiledPPG::comArgLineWord(compiledPPG.at(ch).at(k))
                                    );
        toFPGA.append(compiledPPG_str.at(ch)->last());
         // toFPGA.append(compiledPPG_str.at(ch)->last().left(3)+">");
      }
    }
    toFPGA.append("ch1");

    updateReceiverInfo();

    compiled=true;

    //qDebug() << QString(Q_FUNC_INFO) << "n of loaded file:" << loadedFiles.size() <<loadedFiles;


    return true;


}
//-----------------------------------------------------------------------------
bool TpulseProgram::analyzePPG()
{
  //
  // Initialization
  //
    preamble.source.clear();
    preamble.lineIndex.clear();
    preamble.fileIndex.clear();

    mainPPG.source.clear();
    mainPPG.lineIndex.clear();
    mainPPG.fileIndex.clear();

    while (!asyncPPG.isEmpty()) delete asyncPPG.takeFirst();
    while (!variables.isEmpty()) delete variables.takeFirst();

  //
  //  Empty check
  //
    if (sourcePPG.isEmpty())
    {
        errorMessage="Pulse program is empty.";
        return false;
    }

    int currentPosition=0;
    int currentFileIndex=0;
    bool found=false;
    bool error=false;
    warning=false;
    loopZero=false;

    QString s;

  //
  // Set preample
  //
    while((currentPosition<sourcePPG.size()) && (!found)) {
      s=sourcePPG.at(currentPosition).trimmed();
      if (s.startsWith("start", Qt::CaseInsensitive)) {
        found=true;
        break;
      }
      else if ( (!s.startsWith("{")) && (!s.startsWith("//")) && (!s.isEmpty()) ) {
        removeComment(s);
        preamble.source.append(s);
        preamble.lineIndex.append(currentPosition);
        preamble.fileIndex.append(currentFileIndex);
      }
      currentPosition++;
    }

    if (!found) {
        errorMessage= QString(Q_FUNC_INFO) + ": \"start\" is missing.";
        return false;
    }

    // pass over the "start" line
    currentPosition++;

    //
    // Set mainPPG
    //

    mainPPG.source.append("delay(1s)");
    mainPPG.fileIndex.append(-1);  // There is no line(file) index
    mainPPG.lineIndex.append(-1);  //     ->  -1

    mainPPG.source.append("ready");
    mainPPG.fileIndex.append(-1);  // There is no line(file) index
    mainPPG.lineIndex.append(-1);  //     ->  -1


    found=false;
    while(currentPosition<sourcePPG.size() && (!found)) {
      s=sourcePPG.at(currentPosition).trimmed();
      if (s.startsWith("init", Qt::CaseInsensitive)) {
        found=true;
        break;
      }
      else if ((!s.startsWith("{")) && (!s.startsWith("//")) && (!s.isEmpty())) {
        removeComment(s);
        mainPPG.source.append(s);
        mainPPG.fileIndex.append(currentFileIndex);
        mainPPG.lineIndex.append(currentPosition);
      }
      currentPosition++;
    }
    if (!found) {
        errorMessage=QString(Q_FUNC_INFO)+": \"init\" is missing.";
        return false;
    }

    // command insersion by the compiler
    //mainPPG.source.append("pulse(1u; F1PhasePointer_Init, F2PhasePointer_Init, F3PhasePointer_Init)");
    //mainPPG.fileIndex.append(-1);  // There is no line(file) index
    //mainPPG.lineIndex.append(-1);  //     ->  -1
        // Commented out, (2015 Dec 5 by Takeda) because pointer initialization is also set in "m_init")


    found=false;
    while((currentPosition<sourcePPG.size()) && (!found)) {
      s=sourcePPG.at(currentPosition).trimmed();
      if (s.startsWith("relax", Qt::CaseInsensitive)) {
        found=true;
        break;
      }
      else if ((!s.startsWith("{")) && (!s.startsWith("//")) && (!s.isEmpty())) {
          removeComment(s);
          mainPPG.source.append(s);
          mainPPG.fileIndex.append(currentFileIndex);
          mainPPG.lineIndex.append(currentPosition);
      }
      currentPosition++;
    }
    if (!found) {
        errorMessage=QString(Q_FUNC_INFO)+": \"relax\" is missing.";
        return false;
    }

  //
  // Read asyncronous part(s) (optional)
  //
    found=false;
    while((currentPosition<sourcePPG.size()) && (!found)) {
      s=sourcePPG.at(currentPosition).trimmed();
      if (s.startsWith("async", Qt::CaseInsensitive)) {
        found=true;
        break;
      }
      else if ((!s.startsWith('{')) && (!s.startsWith("//"))  && (!s.isEmpty())) {
        removeComment(s);
        mainPPG.source.append(s);
        mainPPG.fileIndex.append(currentFileIndex);
        mainPPG.lineIndex.append(currentPosition);
      }
      currentPosition++;
    }

    if(found)  // asynchronous sequence detected
    {
       TppgLines w;

       while(currentPosition<sourcePPG.size())
       {
         s=sourcePPG.at(currentPosition).trimmed();
         if ((!s.startsWith('{')) && (!s.startsWith("//")) && (!s.isEmpty()))
         {
             removeComment(s);
             w.source.append(s);
             w.fileIndex.append(currentFileIndex);
             w.lineIndex.append(currentPosition);
         }
         currentPosition++;
       } // while

       w.currentPosition=0; w.currentLine=w.source.at(0);
       int wi,wf;

       while(w.getStr()) // Do the following lines as long as w is not empty
       {
         s=w.currentLine;
         removeComment(s);
         if(s.isEmpty()) // The currentLine contains only a comment.
         {  w.currentLine.clear(); }
         else
         {
           if(!s.startsWith("async",Qt::CaseInsensitive)) // it must start with "async"
           {
              error=true;
              errorMessage=QString(Q_FUNC_INFO)+": \"async\" is expected.";
              break;
           }
           else
           {
             wi=w.currentPosition;
             if(!w.getStr("continue", "standby")) // Either "continue" or "standby" has to be detected.
             {
                error=true;
                errorMessage=QString(Q_FUNC_INFO)
                  +": asynchronous sequence must end with either \"continue\" or \"standby\"";
                break;
             }
             wf=w.currentPosition;

             asyncPPG.append(new TasyncPPG);
             for(int k=wi; k<=wf; k++)
             {
                 asyncPPG[asyncPPG.size()-1]->source.append(w.source.at(k));
                 asyncPPG[asyncPPG.size()-1]->fileIndex.append(w.fileIndex.at(k));
                 asyncPPG[asyncPPG.size()-1]->lineIndex.append(w.lineIndex.at(k));
             }

             if (w.source.at(wf).trimmed().startsWith("continue",Qt::CaseInsensitive))
                 asyncPPG[asyncPPG.size()-1]->asyncMode=asyncContinue;
             else if (w.source.at(wf).trimmed().startsWith("standby",Qt::CaseInsensitive))
                 asyncPPG[asyncPPG.size()-1]->asyncMode=asyncStandby;
             w.currentLine.clear();
           }

         } // isEmpty
       } // while

       // qDebug() << "# of asyncPPG: " << asyncPPG.size();

    } // found


    for(int k=0; k<asyncPPG.size(); k++)
    {
        // Read the 1st line
        asyncPPG[k]->currentPosition=0; asyncPPG[k]->currentLine.clear();
        if (asyncPPG[k]->currentPosition<asyncPPG[k]->source.size())
            asyncPPG[k]->currentLine=asyncPPG[k]->source.at(asyncPPG[k]->currentPosition);


        if(!asyncPPG[k]->currentLine.startsWith("async(",Qt::CaseInsensitive))
        {
            error=true;
            errorMessage=QString(Q_FUNC_INFO)+": async is expected.";
            break;
        }

        asyncPPG[k]->currentLine=asyncPPG[k]->currentLine.remove(0,QString("async(").size()).trimmed();
        // remove command string

        QString s1=asyncPPG[k]->currentLine;
        int index=s1.indexOf(';',0);
        if(index<0)
        {
            error=true;
            errorMessage=QString(Q_FUNC_INFO)+": async(ch;aName)";
            break;
        }

        QString s2=s1.left(index).trimmed();
        QString s3=s1.remove(0,index+1).trimmed();
        s3=s3.left(s3.indexOf(')',0)); // remove ')'

        //  qDebug() << s2 << " " << s3;
        bool ok;
        int ch=s2.toInt(&ok)-1;   // 1,2,3  --> 0,1,2
        if(!ok)
        {
            error=true;
            errorMessage=QString(Q_FUNC_INFO)+ ": channel number is expected.";
            break;
        }

        if(ch>channels())
        {
            error=true;
            errorMessage=QString(Q_FUNC_INFO)+ ": Invalid channel number.";
            break;
        }

        if(ch==0)
        {
            error=true;
            errorMessage=QString(Q_FUNC_INFO)+ ": The 1st channel cannot go into the asynchronous mode.";
            break;
        }

        asyncPPG[k]->setChannel(ch);
        asyncPPG[k]->setName(s3);

    } // k


    if (error==true) {
        while (!asyncPPG.isEmpty()) delete asyncPPG.takeFirst();
        return false;
    }


    return true;

}
//-----------------------------------------------------------------------------
bool TpulseProgram::loadPPG()
{
    QFile file(FppgFileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        error=true;
        errorMessage=QString(Q_FUNC_INFO) + ": Failed to open file.";
        return false;
    }

    //
    //  Read the source code and store in "sourcePPG" (QStringList)
    //
    sourcePPG.clear();
    QTextStream in(&file);
    while (!in.atEnd()) sourcePPG.append(in.readLine());
    file.close();

    loadedFiles.clear();
    //loadedFiles << FppgFileName;

   // qDebug() << QString(Q_FUNC_INFO) << "loadedFiles cleared: " << loadedFiles.size();

    compiled=false;

    return true;

}

//-----------------------------------------------------------------------------
bool TpulseProgram::savePPG()
{
    QFile file(FppgFileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        error=true;
        errorMessage=QString(Q_FUNC_INFO) + ": Failed to open file.";
        return false;
    }

    QTextStream out(&file);
    for(int k=0; k<sourcePPG.size(); k++) out << sourcePPG.at(k) << "\n";

    compiled=false;
    return true;

}
//-----------------------------------------------------------------------------
bool TpulseProgram::updateACQTIME()
{
    int dwIndex=0;
    int alIndex=0;
    int acqTimeIndex=0;
    for(int k=0; k<variables.size(); k++)
    {
        if(0==QString::compare("dw",variables.at(k)->name(),Qt::CaseInsensitive)) dwIndex=k;
        if(0==QString::compare("al",variables.at(k)->name(),Qt::CaseInsensitive)) alIndex=k;
        if(0==QString::compare("ACQTIME",variables.at(k)->name(),Qt::CaseInsensitive)) acqTimeIndex=k;
    }

    if(dwIndex<0) {errorMessage=QString(Q_FUNC_INFO) + ": dw was not found."; return false;}
    if(alIndex<0) {errorMessage=QString(Q_FUNC_INFO) + ": al was not found."; return false;}
    if(acqTimeIndex<0) {errorMessage=QString(Q_FUNC_INFO) + ": acqtime was not found."; return false;}

    TVariable::Unit uni=variables.at(dwIndex)->unit();
    double d=variables.at(dwIndex)->numeric().toDouble();
    int a=variables.at(alIndex)->value().toInt();
    variables[acqTimeIndex]->setNumeric(QVariant(d*a));
    variables[acqTimeIndex]->setUnit(uni);
    variables[acqTimeIndex]->changeUnit(TVariable::Milli);

    return true;
}
//-----------------------------------------------------------------------------
void TpulseProgram::updateSourcePPG()
{
    int lineNumber;
    for(int k=0; k<variables.size(); k++)
    {
        if(0!=QString::compare(variables.at(k)->name(),"ACQTIME",Qt::CaseInsensitive))
        {
          lineNumber=preamble.lineIndex.at(variables.at(k)->declarationPosition());
          sourcePPG[lineNumber].clear();
        }
    }// k

    for(int k=0; k<variables.size(); k++)
    {
      if(0!=QString::compare(variables.at(k)->name(),"ACQTIME",Qt::CaseInsensitive))
      {
        QString newLine="";
        lineNumber=preamble.lineIndex.at(variables.at(k)->declarationPosition());

        if(variables.at(k)->isConst()) newLine += "const ";

        newLine += variables.at(k)->typeString() + ' '
                  + variables.at(k)->name() + '='
                    + variables.at(k)->numericString()
                    + variables.at(k)->unitString();

        if(!variables.at(k)->caption().isEmpty())
            newLine += '(' + variables.at(k)->caption() + ')';

        newLine += "; ";

        sourcePPG[lineNumber].append(newLine);

       // qDebug() << lineNumber << sourcePPG.at(lineNumber);
      }
    }// k

    return;
}
//-----------------------------------------------------------------------------
bool TpulseProgram::updateVariable(QString vName, QVariant qv, TVariable::Unit uni)
{
    if(!compiled)
      {errorMessage="Pulse program has not been compiled yet."; return false;}
    if(0==QString::compare(vName, "ACQTIME", Qt::CaseInsensitive))
      {errorMessage="ACQTIME (=DW*AL) cannot be modified directly. Change AL or DW instead."; return false;}

    updatedPPG.clear();

    if(!compiled) {errorMessage="Pulse program has not been compiled yet."; return false;}
    int vIndex=variableIndex(vName);
    if(vIndex<0) {errorMessage= vName + " is not used."; return false;}

    if(variables.at(vIndex)->isConst())
    {errorMessage="Const parameter cannot be changed."; return false;}

    if(variables.at(vIndex)->type()==TVariable::dwVariable)
    {
        double dw=variables.at(vIndex)->value().toDouble();
        if(dw<MIN_DW_VALUE || dw>MAX_DW_VALUE) {errorMessage=QString(Q_FUNC_INFO)+": dw ("
                     + QString::number(dw) + ") is out of range."; return false;}

//       double dw=variables.at(vIndex)->value().toDouble()*1e6;
//       if(dw*halfSamplingRate!=1.0*round(dw*halfSamplingRate))
//       {
//           qDebug()  << dw << " " << a/halfSamplingRate;
//           errorMessage = "Invalid dw."; return false;
//       }
    }

//    if(variables.at(vIndex)->type()==TVariable::alVariable ||
//            variables.at(vIndex)->type()==TVariable::dwVariable)
//    {
        if(!updateACQTIME()) return false;
//    }



    errorCheckOnly=true;
    if(!processMainPPG())  return false;

    if(!processAsyncPPG()) return false;


    for(int ch=0; ch<channels(); ch++)
    {
        compiledPPG[ch].clear();
        NOfLinesPerCommand[ch]->clear();
    }
    errorCheckOnly=false;

    for(int k=0; k<asyncPPG.size(); k++) asyncPPG[k]->whereIs.clear();

    processMainPPG();
    processAsyncPPG();
    updateReceiverInfo();

    for(int ch=0; ch<channels(); ch++)
    {
        int count=0;
        updatedPPG.append("ch"+QString::number(ch+1));

        // if the size of the pulse program has been changed, we replace all lines
        if(compiledPPG_str.at(ch)->size()!=compiledPPG.at(ch).size())
        {
            compiledPPG_str[ch]->clear();
            for(int k=0; k<compiledPPG.at(ch).size(); k++)
            {
                compiledPPG_str[ch]->append(myHex(compiledPPG_str[ch]->size(),3)
                                            + " = "
                                            + TcompiledPPG::comArgLineWord(compiledPPG.at(ch).at(k)));
                updatedPPG.append(compiledPPG_str.at(ch)->last());
                count++;  // Added (2016.10.3)
            }
        }
        else  //  we replace only the lines that have changed
        {
          for(int k=0; k<compiledPPG_str[ch]->size(); k++)
          {
                                // "XXX = "   6 characters including the spaces
            if(0!=QString::compare(compiledPPG_str[ch]->at(k).mid(6),
                                   TcompiledPPG::comArgLineWord(compiledPPG[ch].at(k)),Qt::CaseInsensitive))
            {
                compiledPPG_str[ch]->replace(k, compiledPPG_str[ch]->at(k).left(6)
                                                 + TcompiledPPG::comArgLineWord(compiledPPG[ch].at(k)));
                updatedPPG.append(compiledPPG_str[ch]->at(k));
                count++;
            }
          }
        } // else
      if(count==0) updatedPPG.removeLast(); // remove "ch?" if there is nothing to be updated in the current channel

    }

    if(variables.at(vIndex)->type()==TVariable::naVariable)
        updatedPPG.append(TreceiverCommand::updateNA(variables.at(vIndex)->value().toInt()));
    if(variables.at(vIndex)->type()==TVariable::ndVariable)
        updatedPPG.append(TreceiverCommand::updateND(variables.at(vIndex)->value().toInt()));
    if(variables.at(vIndex)->type()==TVariable::alVariable)
    {
        updatedPPG.append(TreceiverCommand::updateAL(variables.at(vIndex)->value().toInt()));
        if(!updateACQTIME()) return false;
    }
    if(variables.at(vIndex)->type()==TVariable::dwVariable)
    {
        updatedPPG.append(TreceiverCommand::updateDW(variables.at(vIndex)->value().toDouble()*1e6));
        updatedPPG.append(TreceiverCommand::updateCO(variables.at(vIndex)->value().toDouble()*1e6));
        if(!updateACQTIME()) return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
bool TpulseProgram::updateVariable()
{
    if(!compiled)
      {errorMessage="Pulse program has not been compiled yet."; return false;}

    bool ok;
    errorCheckOnly=true;
    ok=processMainPPG();  if(!ok) return false;
    ok=processAsyncPPG();  if(!ok) return false;
    ok=updateACQTIME(); if(!ok) return false;
    errorCheckOnly=false;
    updatedPPG.clear();
    for(int ch=0; ch<channels(); ch++) compiledPPG[ch].clear();
    for(int k=0; k<asyncPPG.size(); k++) asyncPPG[k]->whereIs.clear();
    ok=processMainPPG();
    ok=processAsyncPPG();
    updateReceiverInfo();

    for(int ch=0; ch<channels(); ch++)
    {
        int count=0;
        updatedPPG.append("ch"+QString::number(ch+1));

        // if the size of the pulse program has been changed, we replace all lines
        if(compiledPPG_str.at(ch)->size()!=compiledPPG.at(ch).size())
        {
           // qDebug()<<compiledPPG_str.at(ch)->size() << " !=  " << compiledPPG.at(ch).size();
            compiledPPG_str[ch]->clear();
            for(int k=0; k<compiledPPG.at(ch).size(); k++)
            {
                compiledPPG_str[ch]->append(myHex(compiledPPG_str[ch]->size(),3)
                                            + " = "
                                            + TcompiledPPG::comArgLineWord(compiledPPG.at(ch).at(k)));
                updatedPPG.append(compiledPPG_str.at(ch)->last());
                count++; // Added. This line is absolutely necessary! (20160308 Takeda)
            }
        }
        else  //  we replace only the lines that changed
        {
          for(int k=0; k<compiledPPG_str[ch]->size(); k++)
          {
                                // "XXX = "   6 characters including the spaces
            if(0!=QString::compare(compiledPPG_str[ch]->at(k).mid(6),
                                   TcompiledPPG::comArgLineWord(compiledPPG[ch].at(k)),Qt::CaseInsensitive))
            {
                compiledPPG_str[ch]->replace(k, compiledPPG_str[ch]->at(k).left(6)
                                                 + TcompiledPPG::comArgLineWord(compiledPPG[ch].at(k)));
                updatedPPG.append(compiledPPG_str[ch]->at(k));
                count++;
            }
          }
        } // else
      if(count==0) updatedPPG.removeLast(); // remove "ch?" if there is nothing to be updated in the current channel

    } // ch

    return ok; // ok should be true

}
//-----------------------------------------------------------------------------
QVariant TpulseProgram::evalExpression(const QString &str, int &pos) const
{
    QVariant result=evalTerm(str,pos);
    while(pos<str.size())
    {
        QChar op=str[pos];
        if(op != '+' && op != '-') return result;
        ++pos;

        QVariant term=evalTerm(str,pos);
        if(result.type()==QVariant::Double && term.type()==QVariant::Double)
        {
            if(op=='+') result=result.toDouble()+term.toDouble();
            else result=result.toDouble()-term.toDouble();
        }
        else
        {
            result=QVariant::Invalid;
        }
    } // while
    return result;
}
//-----------------------------------------------------------------------------
QVariant TpulseProgram::evalTerm(const QString &str, int &pos) const
{
    QVariant result=evalFactor(str,pos);
    while(pos<str.size())
    {
        QChar op=str[pos];
        if(op!='*' && op!='/') return result;
        ++pos;

        QVariant factor=evalFactor(str,pos);
        if(result.type()==QVariant::Double && factor.type()==QVariant::Double)
        {
            if(op=='*') result=result.toDouble() * factor.toDouble();
            else
            {
                if(factor.toDouble()==0.0) result=QVariant::Invalid;
                else result =result.toDouble() / factor.toDouble();
            }
        }
        else
         {
            result = QVariant::Invalid;
        }

    }
    return result;
}
//-----------------------------------------------------------------------------
QVariant TpulseProgram::evalFactor(const QString &str, int &pos) const
{
    QVariant result;
    bool negative=false;

    if(str[pos]=='-') {negative=true; ++pos;}
    if(str[pos]=='(')
    {
        ++pos;
        result=evalExpression(str,pos);
        if(str[pos]!=')') result=QVariant::Invalid;
        ++pos;
    }
    else
    {
        QString token;
        while (str[pos].isLetterOrNumber() || str[pos]=='.') {token+=str[pos]; ++pos;}
        int vIndex=-1;
        for(int k=0; k< variables.size(); k++)
          if(0==QString::compare(token,variables.at(k)->name(),Qt::CaseInsensitive)) vIndex=k;
        if(vIndex>-1)  // TODO: variable search
        {
            // TODO variable to number
            switch(variables.at(vIndex)->type()){
              case TVariable::timeVariable:
              case TVariable::ampVariable:
              case TVariable::phaseVariable:
              case TVariable::freqVariable:
              case TVariable::dwVariable:
              case TVariable::doubleVariable:
                result = variables.at(vIndex)->value().toDouble();

              case TVariable::intVariable:
              case TVariable::loopVariable:
              case TVariable::naVariable:
              case TVariable::alVariable:
                result = (double) variables.at(vIndex)->value().toInt();
              default:
                result = QVariant::Invalid;
            }
        }
        else if(token.at(token.size()-1).isLetter()) // number+unit of time
        {
             double ex=1.0;
             switch(token.at(token.size()-1).toUpper().toLatin1()){
               case 'N': ex=pow(10,-9); break;
               case 'U': ex=pow(10,-6); break;
               case 'M': ex=pow(10,-3); break;
               case 'S': ex=1.0; break;
               default: result=QVariant::Invalid; break;
             }

             bool ok;
             token.chop(1);
             result = token.toDouble(&ok)*ex;
             if(!ok) result=QVariant::Invalid;

        }
        else  //  token should be a number
        {
            bool ok;
            result = token.toDouble(&ok);
            if(!ok) result=QVariant::Invalid;
        }

    } // else

    if(negative)
    {
        if(result.type()==QVariant::Double) result=-result.toDouble();
        else result=QVariant::Invalid;
    }
    return result;
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool TpulseProgram::evalArg(QString &str)
{
    bool ok=true;
    int pos=0;
    QString s=str;
    s.remove(QRegExp("\\s+"));
    s.append(QChar::Null);
    double d=evalArgExpression(s,pos,ok);
    if(!ok) return false;

    str.remove(0,pos);

    evalArgResult=d;

    return true;
}
//-----------------------------------------------------------------------------
double TpulseProgram::evalArgExpression(const QString &str, int &pos, bool &ok)
{

    double result=evalArgTerm(str,pos,ok);
    if(!ok) {return 0;}

    while(str[pos]!=QChar::Null)
    {
        QChar op=str[pos];
        if(op != '+' && op != '-') return result;
        ++pos;

        double term=evalArgTerm(str,pos,ok);
        if(!ok) return 0;
            if(op=='+') result=result+term;
            else result=result-term;
    } // while
    return result;
}
//-----------------------------------------------------------------------------
double TpulseProgram::evalArgTerm(const QString &str, int &pos, bool &ok)
{
    double result=evalArgFactor(str,pos,ok);
    if(!ok) {return 0;}

    while(str[pos]!=QChar::Null)
    {
        QChar op=str[pos];
        if(op!='*' && op!='/') return result;
        ++pos;

        double factor=evalArgFactor(str,pos,ok);
        if(!ok) return 0;

        if(op=='*') result=result * factor;
        else
        {
            if(factor==0.0) {ok=false;
                errorMessage=QString(Q_FUNC_INFO)+": Division by zero is not allowed."; return 0;}
            else result = result / factor;
        }
    }
    return result;
}
//-----------------------------------------------------------------------------
double TpulseProgram::evalArgFactor(const QString &str, int &pos, bool &ok)
{
    double result=0;
    ok=true;
    bool negative=false;

    if(str[pos]=='-') {negative=true; ++pos;}
    if(str[pos]=='(')
    {
        ++pos;
        result=evalArgExpression(str,pos,ok);
        if(str[pos]!=')') {errorMessage=QString(Q_FUNC_INFO)+": Parenthesis is not closed"; ok=false; return 0;}
        ++pos;
    }
    else
    {
        QString token;
        while (str[pos].isLetterOrNumber() || str[pos]=='.' || str[pos]=='_') {token+=str[pos]; ++pos;}

       // qDebug()<<str<<token<<token.size();

        // We check if token might be in the form of, e.g., 1.234e-5, etc.
        if (str[pos]=='-' || str[pos]=='+')
        {
            if(token.endsWith('e',Qt::CaseInsensitive))
            {
                QString choppedToken=token; choppedToken.chop(1);
                //double tempDouble=
                bool ok3;
                choppedToken.toDouble(&ok3);
                if(ok3)
                {
                    token+=str[pos]; // Add '-' or '+' to token
                    ++pos;
                    while (str[pos].isNumber()) {token+=str[pos]; ++pos;}
                }
            }
        }

        int uIndex=-1;
        for(int k=0; k<userDefinedFunctions.size(); k++)
            if(0==QString::compare(token,userDefinedFunctions.at(k).split("=").at(0).trimmed(),Qt::CaseInsensitive)) uIndex=k;

        int fIndex=-1;
        QStringList functions=QStringList()<<"sin"<<"cos"<<"sqrt"<<"sinc"<<"abs"<<"exp"<<"tanh"<<"cosh"<<"sinh"
                                          <<"log"<<"log10"<<"sech";
        for(int k=0; k<functions.size(); k++)
            if(0==QString::compare(token,functions.at(k),Qt::CaseInsensitive)) fIndex=k;

        int f2Index=-1;
        QStringList functions2=QStringList() << "atan2" << "pow";
        for(int k=0; k<functions2.size(); k++)
            if(0==QString::compare(token,functions2.at(k),Qt::CaseInsensitive)) f2Index=k;


        int vIndex=-1;
        for(int k=0; k< variables.size(); k++)
          if(0==QString::compare(token,variables.at(k)->name(),Qt::CaseInsensitive)) vIndex=k;
        double a=0; double b=0;


        if(uIndex>-1)
        {
            if(str[pos]!='(') {errorMessage=QString(Q_FUNC_INFO)+": '(' is expected after" + functions.at(fIndex); ok=false; return 0;}
            else
            {
                ++pos;
                a=evalArgExpression(str,pos,ok);
                if(str[pos] != ')') {
                    errorMessage=QString(Q_FUNC_INFO) + ": Parenthesis is not closed"; ok=false; return 0;
                }
                ++pos;

                QString qs=userDefinedFunctions.at(uIndex).split("=").at(1).trimmed();
               // qDebug() <<"user defined function: " << qs;
                qs.replace("#",QString::number(a,'g',13));
               // qDebug() <<"user defined function: " << qs;

                int pos1=0;
                result=evalArgExpression(qs,pos1,ok);
                if(!ok)
                { errorMessage=QString(Q_FUNC_INFO)
                            + ": invalid expression for user defined function."; ok=false; return 0;}


                //qDebug() <<"user defined function: " << qs << a << result;
            }

        }
        else if(f2Index>-1)
        {
            if(str[pos]!='(') {errorMessage=QString(Q_FUNC_INFO)+": '(' is expected after" + functions.at(fIndex); ok=false; return 0;}
            else
            {
                ++pos;
                a=evalArgExpression(str,pos,ok);
                if(str[pos]!=',') {errorMessage=QString(Q_FUNC_INFO)+": ',' is expected"; ok=false; return 0;}
                ++pos;
                b=evalArgExpression(str,pos,ok);
                if(str[pos] != ')') {
                    errorMessage=QString(Q_FUNC_INFO) + ": Parenthesis is not closed"; ok=false; return 0;
                }
                ++pos;
                switch(f2Index) {
                  case 0: result = atan2(a,b); break;
                  case 1: result = pow(a,b); break;
                default:
                    errorMessage=QString(Q_FUNC_INFO) + ": unknown function"; ok=false; return 0;

                }

            }


        }
        else if(fIndex>-1)
        {
           if(str[pos]!='(') {errorMessage=QString(Q_FUNC_INFO)+": '(' is expected after " + functions.at(fIndex); ok=false; return 0;}
           else
           {
               ++pos;
               a = evalArgExpression(str,pos,ok);
               if (str[pos] != ')' ) {
                       errorMessage=QString(Q_FUNC_INFO) + ": Parenthesis is not closed"; ok=false; return 0;
               }
               ++pos;

              }
              switch(fIndex){
              case 0: result=sin(a); break;
              case 1: result=cos(a); break;
              case 2: result=sqrt(a); break;
              case 3: if(a==0) result=1.0; else result=sin(a)/a; break;
              case 4: result=fabs(a); break;
              case 5: result=exp(a); break;
              case 6: result=tanh(a); break;
              case 7: result=cosh(a); break;
              case 8: result=sinh(a); break;
              case 9: result=log(a); break;
              case 10: result=log10(a); break;
              case 11: result=1/cosh(a); break;
              case 12: //;
              default:
                errorMessage=QString(Q_FUNC_INFO)+": unknown function"; ok=false; return 0;
          }
        }
        else if(vIndex>-1)  //
        {
            switch(variables.at(vIndex)->type()){
              case TVariable::timeVariable:
              case TVariable::ampVariable:
              case TVariable::phaseVariable:
              case TVariable::freqVariable:
              case TVariable::dwVariable:
              case TVariable::doubleVariable:
                result = variables.at(vIndex)->value().toDouble(); break;

              case TVariable::intVariable:
              case TVariable::loopVariable:
              case TVariable::naVariable:
              case TVariable::alVariable:
                result = (double) variables.at(vIndex)->value().toInt(); break;
              default:
                errorMessage=QString(Q_FUNC_INFO)+": unknown variable"; ok=false; break;
            }
        }
        else if(token.at(token.size()-1).isLetter()) // number+unit of time
        {
             double ex=1.0;
             switch(token.at(token.size()-1).toUpper().toLatin1()){
               case 'N': ex=pow(10,-9); break;
               case 'U': ex=pow(10,-6); break;
               case 'M': ex=pow(10,-3); break;
               case 'S': ex=1.0; break;
               default: errorMessage=QString(Q_FUNC_INFO)+": Invalid unit of time."; ok=false; break;
             }

            // qDebug()<<token;

             bool ok2;
             token.chop(1);
             result = token.toDouble(&ok2)*ex;
             if(!ok2) {errorMessage=errorMessage=QString(Q_FUNC_INFO)+": Invalid number: " + token; ok=false;}

        }
        else  //  token should be a number
        {
            bool ok2;
            result = token.toDouble(&ok2);
            if(!ok2) {errorMessage=errorMessage=QString(Q_FUNC_INFO)+": Invalid number."; ok=false;}

//            qDebug() << token << result;

        }

    } // else

    if(negative) result=-result;

    return result;

}
//-----------------------------------------------------------------------------
bool TpulseProgram::evalTime(QString &str)
{
  bool ok=true;
  int pos=0;
  QString s=str;
  s.append(QChar::Null);


  double d=evalArgExpression(s,pos,ok);
 // double d=evalTimeExpression(s,pos,ok);
  if(!ok) {return false;}

  str.remove(0,pos);

  // negative check;
  if(d<0) {error=true; errorMessage=QString(Q_FUNC_INFO)+": time cannot be negative."; return false;}

  getTimeResult=d*1E6; // in microseconds

  return true;

}
//-----------------------------------------------------------------------------
double TpulseProgram::evalTimeExpression(const QString &str, int &pos, bool &ok)
{
    double result=evalTimeTerm(str,pos,ok);
    if(!ok) return 0;

    while(str[pos]!=QChar::Null)
    {
        QChar op=str[pos];
        if(op != '+' && op != '-') return result;
        ++pos;

        double term=evalTimeTerm(str,pos,ok);
        if(!ok) return 0;
            if(op=='+') result=result+term;
            else result=result-term;
    } // while
    return result;
}
//-----------------------------------------------------------------------------
double TpulseProgram::evalTimeTerm(const QString &str, int &pos, bool &ok)
{
    double result=evalTimeFactor(str,pos,ok);
    if(!ok) return 0;

    while(str[pos]!=QChar::Null)
    {
        QChar op=str[pos];
        if(op!='*' && op!='/') return result;
        ++pos;

        double factor=evalTimeFactor(str,pos,ok);
        if(!ok) return 0;

        if(op=='*') result=result * factor;
        else
        {
            if(factor==0.0) {ok=false;
                errorMessage=QString(Q_FUNC_INFO)+": Division by zero is not allowed."; return 0;}
            else result = result / factor;
        }
    }
    return result;
}
//-----------------------------------------------------------------------------
double TpulseProgram::evalTimeFactor(const QString &str, int &pos, bool &ok)
{
    double result=0;
    ok=true;
    bool negative=false;

    if(str[pos]=='-') {negative=true; ++pos;}
    if(str[pos]=='(')
    {
        ++pos;
        result=evalTimeExpression(str,pos,ok);
        if(str[pos]!=')') {errorMessage=QString(Q_FUNC_INFO)+": Parenthesis is not closed"; ok=false; return 0;}
        ++pos;
    }
    else
    {
        QString token;
        while (str[pos].isLetterOrNumber() || str[pos]=='.' || str[pos]=='_') {token+=str[pos]; ++pos;}

        // We check if token might be in the form of, e.g., 1.234e-5, etc.
        if (str[pos]=='-' || str[pos]=='+')
        {
            if(token.endsWith('e',Qt::CaseInsensitive))
            {
                QString choppedToken=token; choppedToken.chop(1);
                //double tempDouble=
                choppedToken.toDouble(&ok);
                if(ok)
                {
                    token+=str[pos]; // Add '-' or '+' to token
                    ++pos;
                    while (str[pos].isNumber()) {token+=str[pos]; ++pos;}
                }
            }
        }


        int vIndex=-1;
        for(int k=0; k< variables.size(); k++)
          if(0==QString::compare(token,variables.at(k)->name(),Qt::CaseInsensitive)) vIndex=k;

     //   qDebug() << token << ": " <<vIndex;

        if(vIndex>-1)  // TODO: variable search
        {
//            variables[vIndex]->addresses[currentCH].append(compiledPPG.at(currentCH)->size());
//            variables[vIndex]->gateIndex[currentCH].append(-1);
               // time has nothing to do with the gate   --->   -1
//            variables[vIndex]->expressions[currentCH].append(str);

            // TODO variable to number
            switch(variables.at(vIndex)->type()){
              case TVariable::timeVariable:
              case TVariable::ampVariable:
              case TVariable::phaseVariable:
              case TVariable::freqVariable:
              case TVariable::dwVariable:
              case TVariable::doubleVariable:
                result = variables.at(vIndex)->value().toDouble(); break;

              case TVariable::intVariable:
              case TVariable::loopVariable:
              case TVariable::naVariable:
              case TVariable::alVariable:
                result = (double) variables.at(vIndex)->value().toInt(); break;
              default:
                errorMessage=QString(Q_FUNC_INFO)+": unknown variable"; ok=false; break;
            }
        }
        else if(token.at(token.size()-1).isLetter()) // number+unit of time
        {
             double ex=1.0;
             switch(token.at(token.size()-1).toUpper().toLatin1()){
               case 'N': ex=pow(10,-9); break;
               case 'U': ex=pow(10,-6); break;
               case 'M': ex=pow(10,-3); break;
               case 'S': ex=1.0; break;
               default: errorMessage=QString(Q_FUNC_INFO)+": Invalid unit of time."; ok=false; break;
             }

            // qDebug()<<token;

             bool ok2;
             token.chop(1);
             result = token.toDouble(&ok2)*ex;
             if(!ok2) {errorMessage=errorMessage=QString(Q_FUNC_INFO)+": Invalid number: " + token; ok=false;}

        }
        else  //  token should be a number
        {
            bool ok2;
            result = token.toDouble(&ok2);
            if(!ok2) {errorMessage=errorMessage=QString(Q_FUNC_INFO)+": Invalid number."; ok=false;}
        }

    } // else

    if(negative) result=-result;

    return result;

}
//-----------------------------------------------------------------------------
