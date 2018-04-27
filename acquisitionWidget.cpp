#include "acquisitionWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

TAcquisitionWidget::TAcquisitionWidget(QWidget *parent) : QWidget(parent)
{
    ppgSetted=false;
    createWidgets();
    createPanel();
}

void TAcquisitionWidget::createWidgets()
{
    separateDataStorageCheckBox = new QCheckBox(tr("Separate data storage over "));
    separateDataStorageSpinBox = new QSpinBox;
    separateDataStorageSpinBox->setMinimum(2);
    separateDataStorageSpinBox->setMaximum(10000);
    separateDataStorageSpinBox->setValue(2);
    separateDataStorageSpinBox->setEnabled(false);
  //  separateDataStorageComboBox = new QComboBox;
  //  separateDataStorageComboBox->addItems(QStringList()<<tr("NA")<<tr("2*NA"));
  //  separateDataStorageComboBox->setEnabled(false);

    multipleAcquisitionsCheckBox = new QCheckBox(tr("Multiple acquisions: "));
    multipleAcquisitionSpinBox = new QSpinBox;
    multipleAcquisitionSpinBox->setMinimum(1);
    multipleAcquisitionSpinBox->setMaximum(10000);
    multipleAcquisitionSpinBox->setEnabled(false);
    multipleAcquisitionComboBox = new QComboBox;
    multipleAcquisitionComboBox->addItems(QStringList()<<tr("Separate data") << tr("Add data")
                                                       << tr("Join data")
                                                       << tr("Average and Join")
                                          );
    multipleAcquisitionComboBox->setEnabled(false);
    multiplicity=1;


    inFPGAAccumCheckBox = new QCheckBox(tr("In-FPGA accumulations"));
    inFPGAAccumSpinBox = new QSpinBox;
    inFPGAAccumSpinBox->setEnabled(false);
    inFPGAAccumSpinBox->setMinimum(1);
    inFPGAAccumSpinBox->setMaximum(1024);

  //  spectralDensityAccumCheckBox = new QCheckBox(tr("Spectral density accumulations"));
  //  spectralDensityAccumCheckBox->setEnabled(true);


    phaseReverseCheckBox = new QCheckBox(tr("Phase reversal"));
    phaseRotationCheckBox = new QCheckBox(tr("Phase rotation"));
    phaseRotationSpinBox = new QDoubleSpinBox;
    phaseRotationSpinBox->setMinimum(-360);
    phaseRotationSpinBox->setMaximum(360);
    phaseRotationSpinBox->setDecimals(2);
    phaseRotationSpinBox->setEnabled(false);

    FFTCheckBox = new QCheckBox(tr("FFT"));
    replaceRealWithAbsCheckBox = new QCheckBox(tr("Replace real with abs (experimental)"));

    QDCheckBox = new QCheckBox(tr("Digital quadrature demodulation"));
    QDCheckBox->setChecked(true);
    offsetCorrectionCheckBox = new QCheckBox(tr("Offset correction"));
    offsetCorrectionSpinBox = new QSpinBox;
    offsetCorrectionSpinBox->setMinimum(-255);
    offsetCorrectionSpinBox->setMaximum(255);
    offsetCorrectionSpinBox->setEnabled(false);

    digitalFilterCheckBox = new QCheckBox(tr("Digital filter"));
    digitalFilterCheckBox->setChecked(true);

    defaultPushButton = new QPushButton(tr("Restore default"));
}

void TAcquisitionWidget::restoreDefault()
{
    separateDataStorageCheckBox->setChecked(false);
    separateDataStorageSpinBox->setValue(2);
    //separateDataStorageComboBox->setCurrentIndex(0);
    multipleAcquisitionsCheckBox->setChecked(false);
    multipleAcquisitionSpinBox->setValue(1);
    multipleAcquisitionComboBox->setCurrentIndex(0);
    inFPGAAccumCheckBox->setChecked(false);
    inFPGAAccumSpinBox->setValue(1);
  //  spectralDensityAccumCheckBox->setChecked(false);

    phaseReverseCheckBox->setChecked(false);
    phaseRotationCheckBox->setChecked(false);
    phaseRotationSpinBox->setValue(0.0);
    FFTCheckBox->setChecked(false);
    replaceRealWithAbsCheckBox->setChecked(false);

    QDCheckBox->setChecked(true);
    digitalFilterCheckBox->setChecked(true);
    offsetCorrectionCheckBox->setChecked(false);
    offsetCorrectionSpinBox->setValue(0);
}

void TAcquisitionWidget::createPanel()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;

      QHBoxLayout *hLayout1 = new QHBoxLayout;
      hLayout1->addWidget(separateDataStorageCheckBox);
      hLayout1->addWidget(separateDataStorageSpinBox);
      hLayout1->addWidget(new QLabel(tr("shots")));
      hLayout1->addStretch();
    //  hLayout1->addWidget(separateDataStorageComboBox);
      connect(separateDataStorageCheckBox,SIGNAL(toggled(bool)),separateDataStorageSpinBox,SLOT(setEnabled(bool)));
    //  connect(separateDataStorageCheckBox,SIGNAL(toggled(bool)),separateDataStorageComboBox,SLOT(setEnabled(bool)));

      TlogicSwitch *sw1 = new TlogicSwitch;
      connect(separateDataStorageCheckBox,SIGNAL(toggled(bool)),sw1,SLOT(stateSlot(bool)));
      // When sw1 receives "true", it emits "false", which uncheck the following checkboxes.
      connect(sw1,SIGNAL(stateSignal(bool)),multipleAcquisitionsCheckBox,SLOT(setChecked(bool)));
      connect(sw1,SIGNAL(stateSignal(bool)),inFPGAAccumCheckBox,SLOT(setChecked(bool)));
      connect(separateDataStorageCheckBox,SIGNAL(toggled(bool)),this,SLOT(onSeparateDataStorageOptionChanged()));
      connect(separateDataStorageSpinBox,SIGNAL(valueChanged(int)),this,SLOT(onSeparateDataStorageOptionChanged()));
    //  connect(separateDataStorageComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(onSeparateDataStorageOptionChanged()));

      // We call "onSeparateDataStorageSpinBoxChanged" to change the text in the spin box (?*NA)
      connect(separateDataStorageSpinBox,SIGNAL(valueChanged(int)),this,SLOT(onSeparateDataStorageSpinBoxChanged(int)));

      QHBoxLayout *hLayout2 = new QHBoxLayout;
      hLayout2->addWidget(multipleAcquisitionsCheckBox);
      hLayout2->addWidget(multipleAcquisitionSpinBox);
      hLayout2->addWidget(new QLabel(tr("per shot")));
      hLayout2->addStretch();
      hLayout2->addWidget(multipleAcquisitionComboBox);
      connect(multipleAcquisitionsCheckBox,SIGNAL(toggled(bool)),multipleAcquisitionSpinBox,SLOT(setEnabled(bool)));
      connect(multipleAcquisitionsCheckBox,SIGNAL(toggled(bool)),multipleAcquisitionComboBox,SLOT(setEnabled(bool)));
      TlogicSwitch *sw2 = new TlogicSwitch;
      connect(multipleAcquisitionsCheckBox,SIGNAL(toggled(bool)),sw2,SLOT(stateSlot(bool)));
      connect(sw2,SIGNAL(stateSignal(bool)),separateDataStorageCheckBox,SLOT(setChecked(bool)));
      connect(sw2,SIGNAL(stateSignal(bool)),inFPGAAccumCheckBox,SLOT(setChecked(bool)));

      // We call "onMultipleAcquisitionOptionChanged" when the relevant Widgets have been accessed.
      connect(multipleAcquisitionsCheckBox,SIGNAL(toggled(bool)),this,SLOT(onMultipleAcquisitionOptionChanged()));
      connect(multipleAcquisitionSpinBox,SIGNAL(valueChanged(int)),this,SLOT(onMultipleAcquisitionOptionChanged()));
      connect(multipleAcquisitionComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(onMultipleAcquisitionOptionChanged()));



      QHBoxLayout *hLayout3 = new QHBoxLayout;
      hLayout3->addWidget(inFPGAAccumCheckBox);
      hLayout3->addWidget(inFPGAAccumSpinBox);
      hLayout3->addStretch();
      TlogicSwitch *sw3 = new TlogicSwitch;
      connect(inFPGAAccumCheckBox,SIGNAL(toggled(bool)),inFPGAAccumSpinBox,SLOT(setEnabled(bool)));
      connect(inFPGAAccumCheckBox,SIGNAL(toggled(bool)),sw3,SLOT(stateSlot(bool)));
      connect(sw3,SIGNAL(stateSignal(bool)),separateDataStorageCheckBox,SLOT(setChecked(bool)));
      connect(sw3,SIGNAL(stateSignal(bool)),multipleAcquisitionsCheckBox,SLOT(setChecked(bool)));

   //   QHBoxLayout *hLayout4 = new QHBoxLayout;
   //   hLayout4->addWidget(spectralDensityAccumCheckBox);
   //   hLayout4->addStretch();
   //   connect(spectralDensityAccumCheckBox,SIGNAL(toggled(bool)),this,SLOT(onSpectralDensityAccumCheckBoxChanged(bool)));

      connect(inFPGAAccumCheckBox,SIGNAL(toggled(bool)),this,SLOT(onInFPGAAccumCheckBokChanged(bool)));
      connect(inFPGAAccumSpinBox,SIGNAL(valueChanged(int)),this,SLOT(onInFPGAAccumSpinBoxChanged(int)));


      QFrame *line1 = new QFrame(this);
      line1->setFrameShape(QFrame::HLine); // Horizontal line
      line1->setFrameShadow(QFrame::Sunken);
      line1->setLineWidth(1);


      QHBoxLayout *hLayout5 = new QHBoxLayout;
      hLayout5->addWidget(phaseReverseCheckBox);
      hLayout5->addSpacing(50);
      hLayout5->addWidget(phaseRotationCheckBox);
      hLayout5->addWidget(phaseRotationSpinBox);
      hLayout5->addStretch();

      connect(phaseRotationCheckBox,SIGNAL(toggled(bool)),phaseRotationSpinBox,SLOT(setEnabled(bool)));
      connect(phaseReverseCheckBox,SIGNAL(toggled(bool)),this,SLOT(onPhaseReverseCheckBoxChanged(bool)));
      connect(phaseRotationCheckBox,SIGNAL(toggled(bool)),this,SLOT(onPhaseRotationCheckBoxChanged(bool)));
      connect(phaseRotationSpinBox,SIGNAL(valueChanged(double)),this,SLOT(onPhaseRotationSpinBoxChanged(double)));

      QHBoxLayout *hLayout7 = new QHBoxLayout;
      hLayout7->addWidget(FFTCheckBox);
      hLayout7->addStretch();
      connect(FFTCheckBox,SIGNAL(toggled(bool)),this,SLOT(onFFTCheckBoxChanged(bool)));

      QHBoxLayout *hLayout11 = new QHBoxLayout;
      hLayout11->addWidget(replaceRealWithAbsCheckBox);
      hLayout11->addStretch();
      connect(replaceRealWithAbsCheckBox,SIGNAL(toggled(bool)),this,SLOT(onReplaceRealWithAbsCheckBoxChanged(bool)));


    mainLayout->addLayout(hLayout3);  // inFPGA accum
      QFrame *line3 = new QFrame(this);
      line3->setFrameShape(QFrame::HLine); // Horizontal line
      line3->setFrameShadow(QFrame::Sunken);
      line3->setLineWidth(1);
    mainLayout->addWidget(line3);

    mainLayout->addLayout(hLayout5); // phase
    mainLayout->addLayout(hLayout7); // FFT
    mainLayout->addLayout(hLayout11); // real<-abs

      QFrame *line2 = new QFrame(this);
      line2->setFrameShape(QFrame::HLine); // Horizontal line
      line2->setFrameShadow(QFrame::Sunken);
      line2->setLineWidth(1);

    mainLayout->addWidget(line2);



    mainLayout->addLayout(hLayout1);  // separate data storage
    mainLayout->addLayout(hLayout2);  // multipule acquisition
    //mainLayout->addLayout(hLayout4);  // spectralDensity accum
    mainLayout->addWidget(line1);







      QHBoxLayout *hLayout8 = new QHBoxLayout;
      hLayout8->addWidget(digitalFilterCheckBox);
      hLayout8->addStretch();
      connect(digitalFilterCheckBox,SIGNAL(toggled(bool)),this,SLOT(onDigitalFilterCheckBoxChanged(bool)));

      QHBoxLayout *hLayout9 = new QHBoxLayout;
      hLayout9->addWidget(QDCheckBox);
      connect(QDCheckBox,SIGNAL(toggled(bool)),this,SLOT(onQDCheckBoxChanged(bool)));
      hLayout9->addStretch();
      hLayout9->addWidget(offsetCorrectionCheckBox);
      hLayout9->addWidget(offsetCorrectionSpinBox);
      connect(offsetCorrectionCheckBox,SIGNAL(toggled(bool)),offsetCorrectionSpinBox,SLOT(setEnabled(bool)));
      connect(offsetCorrectionSpinBox,SIGNAL(valueChanged(int)),this,SLOT(onOffsetChanged()));
      connect(offsetCorrectionCheckBox,SIGNAL(toggled(bool)),this,SLOT(onOffsetChanged()));


      QHBoxLayout *hLayout10 = new QHBoxLayout;
      hLayout10->addStretch();
      hLayout10->addWidget(defaultPushButton);
      connect(defaultPushButton,SIGNAL(clicked(bool)),this,SLOT(restoreDefault()));

    mainLayout->addLayout(hLayout8);
    mainLayout->addLayout(hLayout9);
    mainLayout->addLayout(hLayout10);
    mainLayout->addStretch();
    setLayout(mainLayout);
}


void TAcquisitionWidget::onSeparateDataStorageSpinBoxChanged(int i)
{
  //  separateDataStorageComboBox->setItemText(1,QString::number(i)+"*NA");
}

void TAcquisitionWidget::onSeparateDataStorageOptionChanged()
{
    if(!ppgSetted) return;
   // if(!ppg->compiled) return;
    int naIndex=ppg->variableIndex("NA");
    int m=separateDataStorageSpinBox->value();
    int newNA;

    if(separateDataStorageCheckBox->isChecked())
    {

            ppg->receiverInfo.setNC(m);
            newNA=m*origNA();
    }
    else
    {
        ppg->receiverInfo.setNC(1);
        newNA=origNA();
    }

    if(naIndex>-1 && naIndex<ppg->variables.size()) ppg->variables[naIndex]->setNumeric(QVariant(newNA));
    ppg->updateVariable();

    emit commandRequest(ppg->updatedPPG);
    emit modified();
}

void TAcquisitionWidget::onMultipleAcquisitionOptionChanged()
{
    if(!ppgSetted) return;
   // int alIndex=ppg->variableIndex("AL");
    multiplicity=multipleAcquisitionSpinBox->value();
   // int newAL;
    int newNA=multiplicity*origNA();

    if(multipleAcquisitionsCheckBox->isChecked())
    {
        if(multipleAcquisitionComboBox->currentIndex()==0) // Separate
        {
            //qDebug()<<"separate data";
            multipleAcquisitionMode=SeparateData;
            ppg->receiverInfo.setNC(multiplicity);
            ppg->receiverInfo.setNA(multiplicity*origNA());

        }
        else if(multipleAcquisitionComboBox->currentIndex()==1)  // Add
        {
            //qDebug() <<"add data";
            multipleAcquisitionMode=AddData;
            ppg->receiverInfo.setNC(1);
            ppg->receiverInfo.setNA(multiplicity*origNA());

        }
        else if(multipleAcquisitionComboBox->currentIndex()==2) // Join
        {
           multipleAcquisitionMode=JoinData;
           ppg->receiverInfo.setNC(1);
           //multiplicity=m;
            //newAL=m*origAL;
            //if(newAL>16384) newAL=16384;

        }
        else if(multipleAcquisitionComboBox->currentIndex()==3) // Join Average
        {
           multipleAcquisitionMode=JoinAverageData;
           ppg->receiverInfo.setNC(1);
        }
        //else {return;}
    }
    else
    {
    //    newAL=origAL();
        newNA=origNA();
        ppg->receiverInfo.setNC(1);
        multiplicity=1;
    }
    //ppg->receiverInfo.setAL(newAL);


   // qDebug() << QString(Q_FUNC_INFO) << ": multiplicity=" << multiplicity;

  emit modified();
}


void TAcquisitionWidget::onOffsetChanged()
{
    if(!ppgSetted) return;
    if(offsetCorrectionCheckBox->isChecked())
    {
        ppg->receiverInfo.setDCOffset(offsetCorrectionSpinBox->value());
    }
    else
    {
        ppg->receiverInfo.setDCOffset(0);
    }
    emit modified();
}
