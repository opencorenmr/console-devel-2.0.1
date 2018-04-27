#include "arrayCounter.h"
//--------------------------------------------------------------------------------
TArrayCounter::TArrayCounter()
{
    NOfCounts.clear();
    currentCount.clear();
    updated.clear();

}
//--------------------------------------------------------------------------------
void TArrayCounter::append(int k)
{
    NOfCounts.append(k);
    currentCount.append(0);
    updated.append(false);
}
//--------------------------------------------------------------------------------
void TArrayCounter::removeAt(int k)
{
    NOfCounts.removeAt(k);
    currentCount.removeAt(k);
    updated.removeAt(k);
}
//--------------------------------------------------------------------------------
void TArrayCounter::clear()
{
    while (NOfCounts.size()>0) NOfCounts.removeLast();
    while (currentCount.size()>0) currentCount.removeLast();
    while (updated.size()>0) updated.removeLast();
}
//--------------------------------------------------------------------------------
void TArrayCounter::init(TpulseProgram *ppg)
{
    FRemainingArrayCount=arrayCount();  // total array count

    for(int k=0; k<NOfCounts.size(); k++)
    {
        currentCount[k]=0;
        updated[k]=false;
    }

    for(int a=0; a<ppg->variables.size();a++)
    {
        if(ppg->variables.at(a)->arrayDimension()>0)  // array variable?
        {
            ppg->variables[a]->setArray(0);
        }
    } // for(a)

    hasFinished=false;
    emit updateCurrentCountRequest(currentCountString());

}
//--------------------------------------------------------------------------------
void TArrayCounter::increment(TpulseProgram *ppg)
{
    bool carry=true;
    for(int k=0; k<NOfCounts.size(); k++)
    {
      if(carry)
      {
        if(currentCount.at(k)==NOfCounts.at(k)-1)
        {
            carry=true;
            currentCount[k]=0;
        }
        else
        {
            carry=false;
            currentCount[k]++;
        }
        updated[k]=true;
        emit incremented(k);
        for(int a=0; a<ppg->variables.size();a++)
        {
            if(ppg->variables.at(a)->arrayDimension()==k+1)  // update
            {
                ppg->variables[a]->setArray(currentCount[k]);
                //qDebug() << "!";
            }
        } // for(a)

      } // if(carry)
    } // for(k)

    FRemainingArrayCount--;

    if(carry) {init(ppg); hasFinished=true; emit finished();} // array finished!
    else emit updateCurrentCountRequest(currentCountString());
}
//--------------------------------------------------------------------------------
int TArrayCounter::arrayCount()
{
    int i=1;
    for(int k=0; k<NOfCounts.size(); k++)
    {
        i *= NOfCounts.at(k);
    }
    return i;
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
QString TArrayCounter::currentCountString()
{
    QString qs="(";

    for(int k=NOfCounts.size()-1; k>=0; k--)
    {
       qs.append(QString::number(currentCount.at(k)+1) + "/" + QString::number(NOfCounts.at(k)));
       if(k>0) qs.append(",");
    }
    qs.append(")");

    return qs;
}
//--------------------------------------------------------------------------------
/*
QStringList TArrayCounter::arrayScript(TpulseProgram *ppg)
{

    QStringList sl;
    sl.clear();
    if(!ppg->compiled) return sl;

    if(NOfCounts.size()<1) return sl;

    sl.append("[Array]");
    sl.append("dimensions=" + QString::number(NOfCounts.size()));
    for(int k=0; k<NOfCounts.size(); k++)
    {
      sl.append("arrayCount"+QString::number(k+1)+ "=" + QString::number(NOfCounts.at(k)));
    }

    for(int a=0; a<ppg->variables.size();a++)
    {
        if(ppg->variables.at(a)->arrayDimension()>0)  // update
        {
          sl.append("[" + ppg->variables.at(a)->name() + "]");
          sl.append("dimension=" + QString::number(ppg->variables.at(a)->arrayDimension()));
          sl.append("mode="+ppg->variables.at(a)->arrayModeString());
          if(ppg->variables.at(a)->arrayMode()==TVariable::LinearArray)
          {
            sl.append("initial=" + ppg->variables.at(a)->iniNumericString());
            sl.append("increment=" + ppg->variables.at(a)->incNumericString());
          }
        }
    } // for(a)

    return sl;
}

*/

