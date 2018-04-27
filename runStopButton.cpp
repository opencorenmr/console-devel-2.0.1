#include "runStopButton.h"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TRunStopButton::onScanModeChanged(int mode)
{
    scanMode=mode;
    if(state()==TRunStopButton::Standby)
    {
        if (scanMode==0) // accum
        {
            setIcon(QIcon(":/images/run.png"));
        }
        else
        {
            setIcon(QIcon(":/images/run_rs.png"));
        }
    }

}
//----------------------------------------------------------------------------
TRunStopButton::TRunStopButton(QWidget *parent) :QToolButton(parent)
{
   setState(TRunStopButton::Standby);
   scanMode=0;
   connect(this,SIGNAL(clicked()),this,SLOT(onClicked()));
}
//----------------------------------------------------------------------------
void TRunStopButton::onClicked()
{
   if(state()==TRunStopButton::Standby) emit commandRequest("g");
   else if(state()==TRunStopButton::Running) emit commandRequest("i");
}
//----------------------------------------------------------------------------
void TRunStopButton::setState(TRunStopButton::State state)
{
  FState=state;

  if(state==TRunStopButton::Running)
  {
      if(scanMode==0) setIcon(QIcon(":/images/stop.png"));
      else setIcon(QIcon(":/images/stop_rs.png"));
  }
  else
  {
      if(scanMode==0) setIcon(QIcon(":/images/run.png"));
      else setIcon(QIcon(":/images/run_rs.png"));
  }
}
//----------------------------------------------------------------------------
void TRunStopButton::toggleState()
{
   if(state()==TRunStopButton::Standby) setState(TRunStopButton::Running);
   else if(state()==TRunStopButton::Running) setState(TRunStopButton::Standby);
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
