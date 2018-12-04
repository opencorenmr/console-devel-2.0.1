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

      mainLayout->addWidget(groupBox0);
      mainLayout->addWidget(groupBox1);
      mainLayout->addStretch();
      mainLayout->addWidget(applyPushButton);
      setLayout(mainLayout);

}

void TPhaseWidget::createConnections()
{
    connect(phase0ResolutionComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(setResolution0()));
    connect(phase1ResolutionComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(setResolution1()));
    connect(phase0ValueDoubleSpinBox,SIGNAL(valueChanged(double)),this,SLOT(setPhase0()));
    connect(phase1ValueDoubleSpinBox,SIGNAL(valueChanged(double)),this,SLOT(setPhase1()));
    connect(phasePivotSpinBox,SIGNAL(valueChanged(int)),this,SLOT(setPivot(int)));

    connect(applyPushButton,SIGNAL(clicked(bool)),this,SLOT(addOperation()));
    connect(phasePivotCheckBox,SIGNAL(toggled(bool)),this,SLOT(clickSetPhasePivot()));
}

void TPhaseWidget::setPivot(int p)
{
    phaseRotation->setPivot(p);
    phasePivotSpinBox->setValue(p);
    phasePivotCheckBox->setChecked(false);
}

void TPhaseWidget::initialize()
{
    phase0ValueDoubleSpinBox->setValue(0);
    phase0ResolutionComboBox->setCurrentIndex(5);
    phase1ValueDoubleSpinBox->setValue(0);
    phase1ResolutionComboBox->setCurrentIndex(1);
    setResolution0();
    setResolution1();
    previousPhase0Value=0;
    previousPhase1Value=0;
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

    setPhase();

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
      phRot->setPhase0(phaseRotation->phase0());
      phRot->setPhase1(phaseRotation->phase1());
      phRot->setPivot(phaseRotation->pivot());
    ancestor()->processOperations->processElements.append(phRot);
    // common settings
    ancestor()->updateProcessSettings();
    // settings specific to phase rotation
    createSettings(ancestor()->processSettings,QString::number(ancestor()->processOperations->processElements.size()-1));
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

void TPhaseWidget::setPhase0()
{
    double diff=phase0ValueDoubleSpinBox->value()-previousPhase0Value;

   // qDebug() << QString(Q_FUNC_INFO) << "prev. phase0: " << previousPhase0Value
   //          << "diff: " << diff;

    previousPhase0Value=phase0ValueDoubleSpinBox->value();
    phaseRotation->setPhase0(diff);
    phaseRotation->setPhase1(0);

    //qDebug() << phaseRotation->phase0();

    performOperation();
    if(isAncestorDefined()) ancestor()->plotters->update();
//    emit updatePlottersRequest();
}
void TPhaseWidget::setPhase1()
{
    double diff=phase1ValueDoubleSpinBox->value()-previousPhase1Value;
    previousPhase1Value=phase1ValueDoubleSpinBox->value();
    phaseRotation->setPhase0(0);
    phaseRotation->setPhase1(diff);

    performOperation();
    if(isAncestorDefined()) ancestor()->plotters->update();
//    emit updatePlottersRequest();
}

void TPhaseWidget::setPhase()
{

    double diff0=phase0ValueDoubleSpinBox->value()-previousPhase0Value;
    phaseRotation->setPhase0(diff0);
    double diff1=phase1ValueDoubleSpinBox->value()-previousPhase1Value;
    phaseRotation->setPhase1(diff1);

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
