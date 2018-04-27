#include "inifile.h"
#include "gates.h"
#include <math.h>
#include <QSettings>
#include <QTemporaryFile>
#include "rcvrcom.h"

TGates::TGates() {

  Items.clear();
  info.clear();

}

TGates::~TGates() {

  while (!Items.isEmpty()) delete Items.takeFirst();

}

void TGates::initialize() {

  while (!Items.isEmpty()) delete Items.takeFirst();

  FNOfPhaseListGates.fill(0);

  error=false;
  errorMessage.clear();
  info.clear();
}

int TGates::nameIndex(QString gateName){
  int pos=-1;
  for (int j=0; j<Items.size(); ++j) {
      if(QString::compare(Items.at(j)->name,gateName,Qt::CaseInsensitive)==0) {pos=j; break;}
  }
  return pos;
}



bool TGates::readFromFile(QString fn)
{
    QFile file(fn);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
         errorMessage=QString(Q_FUNC_INFO)+ ": Failed to open " + fn;
         return false;
    }

//    QString tmpfn=fn+".tmp";
//    QFile tempFile(tmpfn);
    QTemporaryFile tempFile;
    if(tempFile.exists()) tempFile.remove();

    if(!tempFile.open())
    // if(!tempFile.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        errorMessage=QString(Q_FUNC_INFO)+": Failed to open a temporary gate file.";
        return false;
    }

    QTextStream out(&tempFile);
    QTextStream in(&file);

    QStringList sections;
    QString qs;
    while (!in.atEnd())
    {
        qs=in.readLine().toLower().trimmed();
        out << qs << "\n";

        if (qs.startsWith("[") && qs.endsWith("]")) {
          qs.chop(1);
          qs.remove(0,1);
          qs=qs.trimmed();
          if (qs.length()>0) sections.append(qs);
        }
    }

    file.close();
    tempFile.close();

  QString s, t;
  QSettings settings(tempFile.fileName(), QSettings::IniFormat);

  for(int i=0; i<sections.size(); ++i) {

    if(nameIndex(sections.at(i))!=-1)
      {error=true; errorMessage="Multiple declaration of \"" + sections.at(i) + "\"" ; return false;}

    s = settings.value(sections.at(i)+"/kind", "notfound").toString();
    int len = settings.value(sections.at(i)+"/bitlength", 1).toInt();

    if (s=="notfound") {
        error=true;
        errorMessage="Key \"kind\" is missing in section \"" + sections.at(i) + "\"";
        return false;
    } else if (s=="amplitude") {
        Items.append(new TampWord(len));
        //s="Amplitude";

    } else if (s=="phase") {
        Items.append(new TphaseWord(len));
        //s="Phase";

    } else if (s=="logic") {
        if (len!=1) {error=true; errorMessage="Logic gate \"" + sections.at(i) + "\" must have BitLength=1";return false;}
        Items.append(new TlogicWord);
        //s="Logic";

    } else if (s=="logic_vector" || s=="logic vector" || s=="logicvector") {
        Items.append(new TlogicVectorWord(len));
        //s="Logic_Vector";

    } else if (s=="integer") {
        Items.append(new TintWord(len));
        //s="Integer";

    } else if (s=="ad9858") {
        if (len!=16) {error=true; errorMessage="AD9858 gate \"" + sections.at(i) + "\" must have BitLength=16"; return false;}
        double oset = settings.value(sections.at(i)+"/offset",0).toDouble();

       // qDebug() << "offset: " << oset;

        if(oset!=0.0) {error=true; errorMessage="In the current version, \"offset\" is no longer available for AD9858 gate \"" + sections.at(i)
                    +"\". Use the transform command instead."; return false;}
        Items.append(new TAD9858Word);
        Items[Items.size()-1]->setOffset(oset);

        //s="AD9858";

    } else if(s=="rfiq") {
        Items.append(new TRFIQWord());
    }
    else {
        error=true;
        errorMessage="Invalid \"kind\" key \"" + s + "\" in section " + sections.at(i) + "\"";
        return false;

    }

    Items[Items.size()-1]->name=sections.at(i);
    int ch = settings.value(sections.at(i)+"/channel", 1).toInt();
    ch--; //  ch=1,2,3  --->  0,1,2
    Items[Items.size()-1]->setChannel(ch);


    if(Items.last()->KindOfGate()==GRFIQ)
    {
        QString s1=settings.value(sections.at(i)+"/amp","").toString();
        int gIndex1=nameIndex(s1);
        if(gIndex1<0) {error=true; errorMessage=Items.last()->name+": amplitude gate "+ s1 + " is not declared"; return false;}
        if(Items.last()->channel()!=Items.at(gIndex1)->channel())
        {
           error=true;
           errorMessage=Items.last()->name+": channel number ("+ QString::number(Items.last()->channel())
                          + ") is different from that of " +Items.at(gIndex1)->name
                          + " (" + QString::number(Items.at(gIndex1)->channel())+ ").";
           return false;
        }
        else Items.last()->setAmpGate(Items.at(gIndex1)->name);

        QString s2=settings.value(sections.at(i)+"/phase","").toString();
        int gIndex2=nameIndex(s2);
        if(gIndex2<0) {error=true; errorMessage=Items.last()->name+": phase gate "+ s2 + " is not declared"; return false;}
        if(Items.last()->channel()!=Items.at(gIndex2)->channel())
        {
           error=true;
           errorMessage=Items.last()->name+": channel number ("+ QString::number(Items.last()->channel())
                          + ") is different from that of " +Items.at(gIndex2)->name
                          + " (" + QString::number(Items.at(gIndex2)->channel())+ ").";
           return false;
        }
        else Items.last()->setPhaseGate(Items.at(gIndex2)->name);

    }
    else
    {
      for(int c=0; c<len; ++c) {
        t = sections.at(i)+"/"+sections.at(i)+"_"+QString::number(c);
        int gn = settings.value(t,-1).toInt();
        if (gn==-1)
        {   error=true;
          QString tt; if (c==1) {tt="-st";} else if (c==2) {tt="-nd";} else {tt="-th";}
          errorMessage="Failed to assign "+QString::number(c)+tt+"bit of the gate \""+ sections.at(i) +"\"";
          return false;
        }

        // Bit assignment
        Items[Items.size()-1]->assign(c, gn);

      }  // for (c)
    } // if

    Items[Items.size()-1]->caption=settings.value(sections.at(i)+"/caption","no caption").toString();

    s=Items[Items.size()-1]->KindOfGate_str();
    info.append( s + " " + Items[Items.size()-1]->name);
//    if (Items[Items.size()-1]->KindOfGate()==GAD9858) {
//        t=QString::number(Items[Items.size()-1]->offset());
//        info.append("(Offset: " + t + " MHz)");
//    }

  } // i = 0..sections.size()-1

  if(tempFile.exists()) tempFile.remove();

  error=false;
  return true;
}

QString TGateControlWord::KindOfGate_str() {
  QString s;
  switch(FKindOfGate) { // one of (GPhase, GLogic, GLogicVector, GAD9858, GPhaselist, GInteger)
    case GAmplitude: s="amplitude"; break;
    case GPhase: s="phase"; break;
    case GLogic: s="logic"; break;
    case GLogicVector: s="logic_vector"; break;
    case GAD9858: s="AD9858"; break;
    case GPhaselist: s="phaelist"; break;
    case GInteger: s="integer"; break;
    case GRFIQ: s="rfiq"; break;
    default: s=""; break;
  }

  return s;
}

// number ^ index: power function for integers
int power (int number, int index) {
    if (index == 0) {
        return 1;
    }
    int num = number;
    for (int i = 1; i < index; i++) {
        number = number * num;
    }
    return number;
}



void TampWord::setValue(double d)
{
  if(d<MIN_AMP_VALUE)
  {
    Fvalue=MIN_AMP_VALUE;
    error=true;
    errorMessage="Out of range (cannot be smaller than)"+QString::number(MIN_AMP_VALUE);
  }
  else if(d>MAX_AMP_VALUE)
  {
    Fvalue=MAX_AMP_VALUE;
    error=true;
    errorMessage="Out of range (cannot be larger than)"+QString::number(MAX_AMP_VALUE);
  }
  else
  {
    Fvalue=d;
    error=false;
  }
}

QSet<int> TampWord::output()
{
  QSet<int> res;
  int a,j,k,m;

  m=power(2,bitLength())-1;
  a=(int) round(value()*(m/100.0));

  res.clear(); // First, empty QSet<int>
  for (j=0; j<bitLength(); ++j) {
    k = (a >> j) & 1;
    if (k==1) res << getAssignment(j);
  }

  return res;

}

QSet<int> TphaseWord::output()
{
    QSet<int> res;
    int a,j,k,m;

    m=power(2,bitLength())-1;

    double q=value();
    while (q>=360) {q-=360;}  //
    while (q<0)    {q+=360;}  //

    a=(int) round(q*(m/360.0));

    res.clear(); // First, empty QSet<int>
    for (j=0; j<bitLength(); ++j) {
      k = (a >> j) & 1;
      if (k==1) res << getAssignment(j);
    }

    return res;
}

QSet<int> TphaseListWord::output()
{
    QSet<int> res;
    int j,k;

    res.clear();
    for (j=0; j<bitLength(); ++j) {
      k=(order() >> j) &1;
      if (k==1) res << getAssignment(j);

    }

    return res;
}


//-----------------------------------------------------------------------------
bool TGates::genPhaseCycle(int ch, QStringList *sl)
{
    sl->clear();
    QString com="A1 000000009C "; // OUT (1us)
    QSet<int> lSet;

    int FxPhaseListIndex=nameIndex("F"+QString::number(ch+1)+"PhaseList");
    if(FxPhaseListIndex<0){
        errorMessage="F"+QString::number(ch+1)+"PhaseList was not found in the gate items."; return false;
    }

    int FxPhaseModeIndex=nameIndex("F"+QString::number(ch+1)+"PhaseMode");
    if(FxPhaseModeIndex<0){
        errorMessage="F"+QString::number(ch+1)+"PhaseMode was not found in the gate items."; return false;
    }

    int FxPhaseListLengthIndex=nameIndex("F"+QString::number(ch+1)+"PhaseListLength");
    if(FxPhaseListLengthIndex<0){
        errorMessage="F"+QString::number(ch+1)+"PhaseListLength was not found in the gate items."; return false;
    }

    int FxPhaseCommandIndex=nameIndex("F"+QString::number(ch+1)+"PhaseCommand");
    if(FxPhaseCommandIndex<0){
        errorMessage="F"+QString::number(ch+1)+"PhaseCommand was not found in the gate items."; return false;
    }

    int FxPhasePointer_InitIndex=nameIndex("F"+QString::number(ch+1)+"PhasePointer_Init");
    if(FxPhasePointer_InitIndex<0){
        errorMessage="F"+QString::number(ch+1)+"PhasePointer_Init was not found in the gate items."; return false;
    }

    int FxPhaseIndex=nameIndex("F"+QString::number(ch+1)+"Phase");
    if(FxPhaseIndex<0){
        errorMessage="F"+QString::number(ch+1)+"Phase was not found in the gate items."; return false;
    }

    int FxTogglePhaseIndex=nameIndex("F"+QString::number(ch+1)+"TogglePhase");
    if(FxTogglePhaseIndex<0){
        errorMessage="F"+QString::number(ch+1)+"TogglePhase was not found in the gate items."; return false;
    }

    int FxPhaseCycle_InitIndex=nameIndex("F"+QString::number(ch+1)+"PhaseCycle_Init");
    if(FxPhaseCycle_InitIndex<0){
        errorMessage="F"+QString::number(ch+1)+"PhaseCycle_Init was not found in the gate items."; return false;
    }

    quint64 line;
    quint64 one64=1;

    bool used=false;
    for(int i=0; i<Items.size(); i++)
    {
        if((Items.at(i)->KindOfGate()==GPhaselist) && (Items.at(i)->channel()==ch))
        {
            used=true;
            break;
        }
    }

    int address=0;

    if(used)  // phase cycle is used in channel ch -> We apply initialization.
    {
        lSet=Items.at(FxPhaseCycle_InitIndex)->output();
        line=0;
        for(int q=0; q<BITLENGTH; q++) if (lSet.contains(q)) line = (line | (one64 << q));
        sl->append(myHex(address,3)+"="+com+myHex(line,16));
        address++;

    }

    for(int i=0; i<Items.size(); i++)
    {
        if((Items.at(i)->KindOfGate()==GPhaselist) && (Items.at(i)->channel()==ch))
        {

            lSet.clear();
            Items.at(FxPhaseListIndex) -> setValue(Items.at(i)->order());
            lSet+=Items.at(FxPhaseListIndex)->output();
            Items.at(FxPhaseModeIndex)->setValue(2);   // mode=2
            lSet+=Items.at(FxPhaseModeIndex)->output();
            Items.at(FxPhaseListLengthIndex)->setValue(Items.at(i)->phaseListLength());
            lSet+=Items.at(FxPhaseListLengthIndex)->output();

            line=0;
            for(int q=0; q<BITLENGTH; q++) if (lSet.contains(q)) line = (line | (one64 << q));
            sl->append(myHex(address,3)+"="+com+myHex(line,16));
            address++;

            lSet+=Items.at(FxPhaseCommandIndex)->output();
            line=0;
            for(int q=0; q<BITLENGTH; q++) if (lSet.contains(q)) line = (line | (one64 << q));
            sl->append(myHex(address,3)+"="+com+myHex(line,16));
            address++;

            lSet.clear();
            lSet+=Items.at(FxPhasePointer_InitIndex)->output();
            Items.at(FxPhaseListIndex) -> setValue(Items.at(i)->order());
            lSet+=Items.at(FxPhaseListIndex)->output();
            Items.at(FxPhaseModeIndex)->setValue(1);   // mode=1
            lSet+=Items.at(FxPhaseModeIndex)->output();
            Items.at(FxPhaseIndex)->setValue(Items.at(i)->phaseList(0));
            lSet+=Items.at(FxPhaseIndex)->output();
            line=0;
            for(int q=0; q<BITLENGTH; q++) if (lSet.contains(q)) line = (line | (one64 << q));
            sl->append(myHex(address,3)+"="+com+myHex(line,16));
            address++;


            lSet.clear();
            lSet+=Items.at(FxPhaseCommandIndex)->output();
            Items.at(FxPhaseListIndex) -> setValue(Items.at(i)->order());
            lSet+=Items.at(FxPhaseListIndex)->output();
            Items.at(FxPhaseModeIndex)->setValue(1);   // mode=1
            lSet+=Items.at(FxPhaseModeIndex)->output();
            Items.at(FxPhaseIndex)->setValue(Items.at(i)->phaseList(0));
            lSet+=Items.at(FxPhaseIndex)->output();
            line=0;
            for(int q=0; q<BITLENGTH; q++) if (lSet.contains(q)) line = (line | (one64 << q));
            sl->append(myHex(address,3)+"="+com+myHex(line,16));
            address++;

            for(int k=1; k<Items.at(i)->phaseListLength(); k++) // NOTE: k starts with 1
            {
                lSet.clear();
                lSet+=Items.at(FxTogglePhaseIndex)->output();
                Items.at(FxPhaseListIndex) -> setValue(Items.at(i)->order());
                lSet+=Items.at(FxPhaseListIndex)->output();
                Items.at(FxPhaseModeIndex)->setValue(1);   // mode=1
                lSet+=Items.at(FxPhaseModeIndex)->output();
                Items.at(FxPhaseIndex)->setValue(Items.at(i)->phaseList(k));
                lSet+=Items.at(FxPhaseIndex)->output();
                line=0;

             //   qDebug()<<lSet;

                for(int q=0; q<BITLENGTH; q++) if (lSet.contains(q)) line = (line | (one64 << q));

             //   qDebug()<<line;

                sl->append(myHex(address,3)+"="+com+myHex(line,16));
                address++;

                lSet.clear();
                lSet+=Items.at(FxPhaseCommandIndex)->output();
                Items.at(FxPhaseListIndex) -> setValue(Items.at(i)->order());
                lSet+=Items.at(FxPhaseListIndex)->output();
                Items.at(FxPhaseModeIndex)->setValue(1);   // mode=1
                lSet+=Items.at(FxPhaseModeIndex)->output();
                Items.at(FxPhaseIndex)->setValue(Items.at(i)->phaseList(k));
                lSet+=Items.at(FxPhaseIndex)->output();
                line=0;
                for(int q=0; q<BITLENGTH; q++) if (lSet.contains(q)) line = (line | (one64 << q));
                sl->append(myHex(address,3)+"="+com+myHex(line,16));
                address++;

            } //k

          sl->append(myHex(address,3)+"=FF 0000000000 0000000000000000");
          address++;
          sl->append(myHex(address,3)+"=00 0000000000 0000000000000000");
          address++;
          sl->append("g");
          address=0;
      } // if
    } // i

    return true;
}
//-----------------------------------------------------------------------------
QStringList TphaseListWord::genInitialSettingSource()
{
    QStringList sl; sl.clear();
    QString dt="1u";
    QString chs=QString::number(channel());
    QString ord=QString::number(order());

    sl.append("pulse("+dt+"; "+"F"+chs+"PhaseList("+ord+"), "
                                 + "F"+chs+"PhaseMode(2), "
                                 + "F"+chs+"PhaseListLength("+QString::number(phaseListLength())+"))" );

    sl.append("pulse("+dt+"; " + "F"+chs+"PhaseCommand, "
                                 +"F"+chs+"PhaseList("+ord+"), "
                                 + "F"+chs+"PhaseMode(2), "
                                 + "F"+chs+"PhaseListLength("+QString::number(phaseListLength())+"))" );

    sl.append("pulse("+dt+"; " + "F"+chs+"PhasePointer_Init, "
                                 +"F"+chs+"PhaseList("+ord+"), "
                                 + "F"+chs+"PhaseMode(1), "
                                 + "F"+chs+"Phase("+QString::number(phaseList(0))+"))" );

    sl.append("pulse("+dt+"; " + "F"+chs+"PhaseCommand, "
                                 +"F"+chs+"PhaseList("+ord+"), "
                                 + "F"+chs+"PhaseMode(1), "
                                 + "F"+chs+"Phase("+QString::number(phaseList(0))+"))" );

    for (int q=1; q<phaseListLength(); ++q) {

        sl.append("pulse("+dt+"; " + "F"+chs+"TogglePhase, "
                                     +"F"+chs+"PhaseList("+ord+"), "
                                     + "F"+chs+"PhaseMode(1), "
                                     + "F"+chs+"Phase("+QString::number(phaseList(q))+"))" );

        sl.append("pulse("+dt+"; " + "F"+chs+"PhaseCommand, "
                                     +"F"+chs+"PhaseList("+ord+"), "
                                     + "F"+chs+"PhaseMode(1), "
                                     + "F"+chs+"Phase("+QString::number(phaseList(q))+"))" );
    } // q

    return sl;
}

QSet<int> TlogicVectorWord::output()
{
    QSet<int> res;
    int j,k;

    res.clear(); // First, empty QSet<int>
    for (j=0; j<bitLength(); ++j) {
      k = (value() >> j) & 1;
      if (k==1) res << getAssignment(j);
    }

    return res;

}

QSet<int> TintWord::output()
{
    QSet<int> res;
    int j,k;

    res.clear(); // First, empty QSet<int>
    for (j=0; j<bitLength(); ++j) {
      k = (value() >> j) & 1;
      if (k==1) res << getAssignment(j);
    }

    return res;

}


QSet<int> TAD9858Word::output()
{
    QSet<int> res,dat,add,wr,fud;
    int datOffset,addOffset;
    int a;
    int j,m;
    unsigned FTW0;   // Unsigned !
    int DFTW;        // Signed ! for freq-sweep mode operation

    res.clear(); dat.clear(); add.clear(); wr.clear(); fud.clear();
    addOffset=8;

    if(isLogicVector())
    {
        QSet<int> res;
        int j,k;

        res.clear(); // First, empty QSet<int>
        for (j=0; j<bitLength(); ++j) {
          k = ((int)value() >> j) & 1;
          if (k==1) res << getAssignment(j);
        }

        return res;

    }

    if ((FOrder==1) || (FOrder==2) || (FOrder==3) || (FOrder==4)) wr << getAssignment(14);
    if (FOrder==5) fud << getAssignment(15);

    if ((FOrder>0) && (FOrder<5))
    {
      switch(FOrder) {  // AD9858 address
        case 1: a = 10 + (profile()*6); break;  // 10 <- 0x0A
        case 2: a = 10 + (profile()*6) + 1; break;
        case 3: a = 10 + (profile()*6) + 2; break;
        case 4: a = 10 + (profile()*6) + 3; break;
        default: a = 0; break;
        } // switch(order)

      for(j=0; j<=5; ++j) {  //  The address of AD9858 ranges from 0 to 5
        m = (a >> j) & 1;
        if (m==1) add << getAssignment(j+addOffset);
      } // j

      // Now the address (for the order-th step) has been set. Next, data!

      double f;
      if (profile()==-1) f=value(); else f=value()+offset();
      f*=1.0e6; // MHz->Hz


     // qDebug()<< "value: " << value() <<"  offset: "<< offset() << "   f: " << f;

        // freq. offset is NOT applied for freq. sweep (profile==-1).
      FTW0 = (unsigned) round(f*pow(2,32)/pow(10,9)); // IMPORTANT: unsigned!
      DFTW = (int) round(f*pow(2,32)/pow(10,9)); // IMPORTANT: signed (int)!
         // needs check! (2014.1.28 KT)

      switch(FOrder){
        case 1: datOffset=0; break;
        case 2: datOffset=8; break;
        case 3: datOffset=16; break;
        case 4: datOffset=24; break;
        default: datOffset=0; break;
      }

      switch(profile()) {
        case -1:   // sweep mode
                   // signed -> data is prepared from int type.
          for (j=datOffset; j<=datOffset+7; ++j) {
            if (((DFTW >> j) & 1) == 1) dat << getAssignment(j-datOffset);
          }
          break;
        case 0:
        case 1:
        case 2:  // A common process is performed for 0..3
        case 3:
          for (j=datOffset; j<=datOffset+7; ++j) {
              if (((FTW0 >> j) & 1) == 1) dat << getAssignment(j-datOffset);
          }
          break;
        default: break;
      } // switch(profile())

    } // ((order>0) && (order<5))

    return dat+add+wr+fud;

}

QStringList TGates::AD9858GatesInfo()
{
    QStringList sl; sl.clear();
    for(int k=0; k<Items.size(); k++)
    {
      if(Items.at(k)->KindOfGate()==GAD9858)
      {
         sl.append(Items.at(k)->name+ ": offset = " + QString::number(Items.at(k)->offset(),'g',12) + " Hz");
      }
    }
  return sl;
}
