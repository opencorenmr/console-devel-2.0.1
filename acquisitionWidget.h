#ifndef TACQUISITIONWIDGET_H
#define TACQUISITIONWIDGET_H

#include <QWidget>
#include <QCheckBox>
#include <QSettings>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QPushButton>

#include "pulseProgram.h"

class TlogicSwitch : public QObject
{
    Q_OBJECT
public:
    signals:
        void stateSignal(bool state); // reverse the state
    public slots:
        void stateSlot(bool state)
        {
            if(state==true) emit stateSignal(false);
        }
 };

//class TAcquisition
//{
//public:
//    enum MultipleAcquisitionMode {JoinData,SeparateData,AddData};
//};

class TAcquisitionWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TAcquisitionWidget(QWidget *parent = 0);
    enum MultipleAcquisitionMode {SeparateData,AddData,JoinData,JoinAverageData};
    MultipleAcquisitionMode multipleAcquisitionMode;
    TpulseProgram *ppg;
    void setPPG(TpulseProgram *p)
    {
        ppg=p; ppgSetted=true;
        //setOrigNA(ppg->variables.at(ppg->variableIndex("NA"))->numeric().toInt());
        //setOrigAL(ppg->variables.at(ppg->variableIndex("AL"))->numeric().toInt());
    }
    int origNA() {return ForigNA;}

    void setOrigAL(int a) {ForigAL=a;}
    int origAL() {return ForigAL;}

    int multiplicity;  // used for JoinData


    QCheckBox *separateDataStorageCheckBox;
    QSpinBox *separateDataStorageSpinBox;
    //QComboBox *separateDataStorageComboBox;

    QCheckBox *multipleAcquisitionsCheckBox;
    QSpinBox *multipleAcquisitionSpinBox;
    QComboBox *multipleAcquisitionComboBox;

    QCheckBox *inFPGAAccumCheckBox;
    QSpinBox *inFPGAAccumSpinBox;

//    QCheckBox *spectralDensityAccumCheckBox;

    QCheckBox *phaseReverseCheckBox;
    QCheckBox *phaseRotationCheckBox;
    QDoubleSpinBox *phaseRotationSpinBox;
    QCheckBox *FFTCheckBox;
    QCheckBox *replaceRealWithAbsCheckBox;

    QCheckBox *QDCheckBox;
    QCheckBox *offsetCorrectionCheckBox;
    QSpinBox *offsetCorrectionSpinBox;
    QCheckBox *digitalFilterCheckBox;

    QPushButton *defaultPushButton;


signals:
    void commandRequest(QStringList st);
    void separateDataStorageChanged(int i);
    void modified();

public slots:
    void setOrigNA(int i) {ForigNA=i;}
    void restoreDefault();
    void onMultipleAcquisitionOptionChanged();
    void onSeparateDataStorageOptionChanged();
    void onSeparateDataStorageSpinBoxChanged(int i);
    void onOffsetChanged();

private slots:

    void onPhaseReverseCheckBoxChanged(bool b) {if(ppgSetted) {ppg->receiverInfo.setPhaseReverseEnabled(b);emit modified();}}
    void onPhaseRotationCheckBoxChanged(bool b) {if(ppgSetted) {ppg->receiverInfo.setPhaseRotationEnabled(b);emit modified();}}
    void onPhaseRotationSpinBoxChanged(double d) {if(ppgSetted) {ppg->receiverInfo.setPhaseRotationAngle(d);emit modified();}}
    void onFFTCheckBoxChanged(bool b) {if(ppgSetted) {ppg->receiverInfo.setFFTEnabled(b);emit modified();}}
    void onReplaceRealWithAbsCheckBoxChanged(bool){;}
 //   void onSpectralDensityAccumCheckBoxChanged(bool b)
 //          {if(ppgSetted) {ppg->receiverInfo.setSpectralDensityAccumEnabled(b);emit modified();}}

    void onQDCheckBoxChanged(bool b) {
        if(ppgSetted) {
          if(b) ppg->receiverInfo.qdOption=TDSPOption::QD_On;
          else ppg->receiverInfo.qdOption=TDSPOption::QD_Off;
          emit modified();
        }
    }

    void onDigitalFilterCheckBoxChanged(bool b) {
        if(ppgSetted) {
          if(b) ppg->receiverInfo.digitalFilterOption=TDSPOption::FIR_On;
          else ppg->receiverInfo.digitalFilterOption=TDSPOption::FIR_Off;
          emit modified();
        }
    }

    void onInFPGAAccumCheckBokChanged(bool b) {
       if(!ppgSetted) return;
       if(b) ppg->receiverInfo.setStep(inFPGAAccumSpinBox->value());
       else ppg->receiverInfo.setStep(1);
       emit modified();
    }
    void onInFPGAAccumSpinBoxChanged(int i) {
        if(!ppgSetted) return;
        if(inFPGAAccumCheckBox->isChecked()) ppg->receiverInfo.setStep(i);
            else ppg->receiverInfo.setStep(1);
        emit modified();
    }

    // QCheckBox *offsetCorrectionCheckBox;
    // QSpinBox *offsetCorrectionSpinBox;

private:
    bool ppgSetted;
    int ForigNA;
    int ForigAL;
    void createWidgets();
    void createPanel();
    



};

#endif // TACQUISITIONWIDGET_H
