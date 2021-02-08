#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>

//#include "phaseWidget.h"
//#include "phase.h"
#include "processPanelWidget.h"

//TPhaseWidget::TPhaseWidget(QWidget *parent) : QWidget(parent)
TPhaseWidget::TPhaseWidget() : TProcessBase()
{
    phaseRotation = new TPhaseRotation;
    //FFIDSetted=false;

    createWidgets();
    createPanel();
    createConnections();
    initialize();

}

TPhaseWidget::~TPhaseWidget()
{
    delete phaseRotation;
}

void TPhaseWidget::setFID2D(TFID_2D *f)
{
//    FID_2D=f;
//    FFIDSetted=true;
    phasePivotSpinBox->setMaximum(f->al()-1);

}

void TPhaseWidget::createWidgets()
{
    phasePivotSpinBox=new QSpinBox;
    phasePivotCheckBox=new QCheckBox("Click-set"); phasePivotCheckBox->setEnabled(false);
    phaseOrderSpinBox=new QSpinBox;
    phaseOrderSpinBox->setMaximum(1);
    phase0ValueDoubleSpinBox=new QDoubleSpinBox;
    phase0ValueDoubleSpinBox->setSuffix(" deg.");
    phase0ValueDoubleSpinBox->setRange(-360,360);
    phase0ResolutionComboBox=new QComboBox;
    phase0ResolutionComboBox->addItems(QStringList()<<"0.01"<<"0.05"<<"0.1"<<"1"<<"5"<<"10"<<"25");
    phase0ResolutionComboBox->setCurrentIndex(5);

    phase1ValueDoubleSpinBox=new QDoubleSpinBox;
    phase1ValueDoubleSpinBox->setSuffix(" deg./point");
    phase1ValueDoubleSpinBox->setDecimals(4);
    phase1ValueDoubleSpinBox->setRange(-36000,36000);
    phase1ResolutionComboBox=new QComboBox;
    phase1ResolutionComboBox->addItems(QStringList()<<"0.0005"<<"0.001"<<"0.005"<<"0.01"<<"0.05"<<"0.1"<<"1"<<"5");
    phase1ResolutionComboBox->setCurrentIndex(1);

    applyPushButton=new QPushButton(tr("Save change"));
    applyPushButton->setEnabled(false);
    restorePushButton=new QPushButton(tr("Restore change"));
    restorePushButton->setEnabled(false);
}

void TPhaseWidget::createPanel()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
   // mainLayout->setMargin(0);

    QGroupBox *groupBox0=new QGroupBox(tr("0th Order"));


      QGridLayout *gridLayout0 = new QGridLayout;
      gridLayout0->addWidget(new QLabel(tr("Value")),0,0,1,1);
      gridLayout0->addWidget(phase0ValueDoubleSpinBox,0,1,1,1);
      gridLayout0->addWidget(new QLabel(tr("Resolution")),1,0,1,1);
      gridLayout0->addWidget(phase0ResolutionComboBox,1,1,1,1);

    groupBox0->setLayout(gridLayout0);

    QGroupBox *groupBox1=new QGroupBox(tr("1st Order"));


      QGridLayout *gridLayout1 = new QGridLayout;
        gridLayout1->addWidget(new QLabel(tr("Pivot")),0,0,1,1);
        gridLayout1->addWidget(phasePivotSpinBox,0,1,1,1);
        gridLayout1->addWidget(phasePivotCheckBox,0,2,1,1);
        gridLayout1->addWidget(new QLabel(tr("Value")),1,0,1,1);
        gridLayout1->addWidget(phase1ValueDoubleSpinBox,1,1,1,2);
        gridLayout1->addWidget(new QLabel(tr("Resolution")),2,0,1,1);
        gridLayout1->addWidget(phase1ResolutionComboBox,2,1,1,1);
      groupBox1->setLayout(gridLayout1);

      QHBoxLayout *hLayout1 = new QHBoxLayout;
      hLayout1->addWidget(applyPushButton);
    //  hLayout1->addWidget(restorePushButton);

      mainLayout->addWidget(groupBox0);
      mainLayout->addWidget(groupBox1);
   //   mainLayout->addWidget(applyModeWidget);
      mainLayout->addStretch();
      mainLayout->addLayout(hLayout1);
      setLayout(mainLayout);

}

void TPhaseWidget::createConnections()
{
    connect(phase0ResolutionComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(setResolution0()));
    connect(phase1ResolutionComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(setResolution1()));
    connect(phase0ValueDoubleSpinBox,SIGNAL(valueChanged(double)),this,SLOT(changePhase0To()));
    connect(phase1ValueDoubleSpinBox,SIGNAL(valueChanged(double)),this,SLOT(changePhase1To()));
    connect(phasePivotSpinBox,SIGNAL(valueChanged(int)),this,SLOT(setPivot(int)));
    connect(applyPushButton,SIGNAL(clicked(bool)),this,SLOT(addOperation()));
    connect(phasePivotCheckBox,SIGNAL(toggled(bool)),this,SLOT(clickSetPhasePivot()));
}

void TPhaseWidget::breakConnections()
{
    disconnect(phase0ResolutionComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(setResolution0()));
    disconnect(phase1ResolutionComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(setResolution1()));
    disconnect(phase0ValueDoubleSpinBox,SIGNAL(valueChanged(double)),this,SLOT(changePhase0To()));
    disconnect(phase1ValueDoubleSpinBox,SIGNAL(valueChanged(double)),this,SLOT(changePhase1To()));
    disconnect(phasePivotSpinBox,SIGNAL(valueChanged(int)),this,SLOT(setPivot(int)));
    disconnect(applyPushButton,SIGNAL(clicked(bool)),this,SLOT(addOperation()));
    disconnect(phasePivotCheckBox,SIGNAL(toggled(bool)),this,SLOT(clickSetPhasePivot()));
}


void TPhaseWidget::setPivot(int p)
{
    phaseRotation->setPivot(p);
    phasePivotSpinBox->setValue(p);
    phasePivotCheckBox->setChecked(false);
}

void TPhaseWidget::initialize()
{
    phaseRotation->resetPhase();
    phaseRotation->resetAccumPhase();
    phase0ValueDoubleSpinBox->setValue(0);
    phase0ResolutionComboBox->setCurrentIndex(5);
    phase1ValueDoubleSpinBox->setValue(0);
    phase1ResolutionComboBox->setCurrentIndex(1);
    setResolution0();
    setResolution1();
//    previousPhase0Value=0;
//    previousPhase1Value=0;
    applyPushButton->setEnabled(false);
}

void TPhaseWidget::reset()
{
    phaseRotation->resetPhase();
    phaseRotation->resetAccumPhase();
//    previousPhase0Value=0;
//    previousPhase1Value=0;
}

void TPhaseWidget::setZero()
{
    phase0ValueDoubleSpinBox->setValue(0);
    phase1ValueDoubleSpinBox->setValue(0);

}

void TPhaseWidget::createSettings(QSettings *settings, QString section)
{
    settings->beginGroup(section);
      settings->setValue("phase0",phase0ValueDoubleSpinBox->value());
      settings->setValue("phase1",phase1ValueDoubleSpinBox->value());
      settings->setValue("pivot",phasePivotSpinBox->value());
    settings->endGroup();
    settings->sync();
}

void TPhaseWidget::readSettings(QSettings *settings, QString section)
{
    settings->beginGroup(section);
      phase0ValueDoubleSpinBox->setValue(settings->value("phase0",0).toDouble());
      phase1ValueDoubleSpinBox->setValue(settings->value("phase1",0).toDouble());
      phasePivotSpinBox->setValue(settings->value("pivot",0).toInt());
    settings->endGroup();
    refresh();
}

void TPhaseWidget::refresh()
{

    changePhaseTo();

}

void TPhaseWidget::performOperation()
{
    if(!isAncestorDefined()) return;
    if(ancestor()->FID_2D->FID.isEmpty()) return;
    phaseRotation->process(ancestor()->FID_2D);
}


void TPhaseWidget::addOperation()
{
    if(!isAncestorDefined()) return;
    if(ancestor()->FID_2D->FID.isEmpty()) return;
    TPhaseRotation *phRot = new TPhaseRotation; //qDebug() << phRot;
      phRot->setInitialPhase0(phaseRotation->initialPhase0());
      phRot->setInitialPhase1(phaseRotation->initialPhase1());
      phRot->setAccumPhase0(phaseRotation->accumPhase0());
      phRot->setAccumPhase1(phaseRotation->accumPhase1());
      phRot->setPivot(phaseRotation->pivot());
      phRot->setPhase0(phRot->accumPhase0() - phRot->initialPhase0());
      phRot->setPhase1(phRot->accumPhase1() - phRot->initialPhase1());

    phaseRotation->setInitialPhase0(phaseRotation->accumPhase0());
    phaseRotation->setInitialPhase1(phaseRotation->accumPhase1());


    //if the type of the last operation is also phase, we overwrite it.
    //if(!ancestor()->processOperations->processElements.isEmpty())
    //{
    //   if(ancestor()->processOperations->processElements.last()->processType()==TProcessElement::Phase)
    //   {
    //     ancestor()->processOperations->processElements.removeLast();
    //   }
    //}


    ancestor()->processOperations->processElements.append(phRot);
    // common settings
    ancestor()->updateProcessSettings();
    // settings specific to phase rotation
    createSettings(ancestor()->processSettings,QString::number(ancestor()->processOperations->processElements.size()-1));

    applyPushButton->setEnabled(false);
//    restorePushButton->setEnabled(false);
    phasePivotCheckBox->setEnabled(true);
    phasePivotSpinBox->setEnabled(true);

}

void TPhaseWidget::clickSetPhasePivot()
{
    if(!isAncestorDefined()) return;
    if(ancestor()->FID_2D->FID.isEmpty()) return;
    if(phasePivotCheckBox->isChecked())
    {
      for(int k=0; k<ancestor()->plotters->FIDPlotters.size(); k++)
      {
        connect(ancestor()->plotters->FIDPlotters[k]->plotter,SIGNAL(clickedXPosition(int)),
                this,SLOT(setPivot(int)));
      }
    }
    else
    {
      for(int k=0; k<ancestor()->plotters->FIDPlotters.size(); k++)
      {
         disconnect(ancestor()->plotters->FIDPlotters[k]->plotter,SIGNAL(clickedXPosition(int)),
                  this,SLOT(setPivot(int)));
      }
    }
}

void TPhaseWidget::changePhase0To()
{
//    double diff=phase0ValueDoubleSpinBox->value()-previousPhase0Value;

   // qDebug() << QString(Q_FUNC_INFO) << "prev. phase0: " << previousPhase0Value
   //          << "diff: " << diff;

//    previousPhase0Value=phase0ValueDoubleSpinBox->value();
    phaseRotation->changePhase0To(phase0ValueDoubleSpinBox->value());
    phaseRotation->setPhase1(0);

    //qDebug() << phaseRotation->phase0();

    performOperation();
    if(isAncestorDefined()) ancestor()->plotters->update();

    if(phaseRotation->initialPhase0()==phaseRotation->accumPhase0() &&
       phaseRotation->initialPhase1()==phaseRotation->accumPhase1() )
      applyPushButton->setEnabled(false);
    else
      applyPushButton->setEnabled(true);
//    emit updatePlottersRequest();
}

void TPhaseWidget::changePhase1To()
{
    phaseRotation->setPhase0(0);
    phaseRotation->changePhase1To(phase1ValueDoubleSpinBox->value());

    performOperation();
    if(isAncestorDefined()) ancestor()->plotters->update();


    if(phaseRotation->initialPhase1()==phaseRotation->accumPhase1())
    {
        phasePivotCheckBox->setEnabled(true);
        phasePivotSpinBox->setEnabled(true);
    }
    else
    {
        phasePivotCheckBox->setEnabled(false);
        phasePivotSpinBox->setEnabled(false);
    }

    if(phaseRotation->initialPhase0()==phaseRotation->accumPhase0() &&
       phaseRotation->initialPhase1()==phaseRotation->accumPhase1() )
      applyPushButton->setEnabled(false);
    else
      applyPushButton->setEnabled(true);
}

void TPhaseWidget::changePhaseTo()
{

    phaseRotation->changePhase0To(phase0ValueDoubleSpinBox->value());
    phaseRotation->changePhase1To(phase1ValueDoubleSpinBox->value());

    performOperation();

    if(isAncestorDefined()) ancestor()->plotters->update();
//    emit updatePlottersRequest();
}

void TPhaseWidget::setResolution0()
{
    phase0ValueDoubleSpinBox->setSingleStep(phase0ResolutionComboBox->currentText().toDouble());
}

void TPhaseWidget::setResolution1()
{
    phase1ValueDoubleSpinBox->setSingleStep(phase1ResolutionComboBox->currentText().toDouble());
}
