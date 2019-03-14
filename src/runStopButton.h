#ifndef RUNSTOPBUTTON_H
#define RUNSTOPBUTTON_H

#include <QToolButton>

class TRunStopButton : public QToolButton
{
    Q_OBJECT
public:
  explicit TRunStopButton(QWidget *parent = 0);
  enum State {Running, Standby};

  TRunStopButton::State state() {return FState;}
  void setState(TRunStopButton::State state);

signals:
  void commandRequest(QString s);

public slots:
  void onClicked();
  void onStateChanged(TRunStopButton::State state) {setState(state);}
  void onScanModeChanged(int mode);
  void toggleState();

protected:
  TRunStopButton::State FState;
  int scanMode;



};

#endif // RUNSTOPBUTTON_H
