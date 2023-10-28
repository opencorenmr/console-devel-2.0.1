#include "peakPickWidget.h"
#include "processPanelWidget.h"
#include "peakPick.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>
#include <QComboBox>
#include <QDebug>

KPeakPickWidget::KPeakPickWidget()
{
   createWidgets();
   createPanel();
   createConnections();
}

void KPeakPickWidget::createWidgets()
{
    whichFidSpinBox = new QSpinBox;
    whichFidSpinBox->setRange(1,16383);
    intensitySpinBox = new QDoubleSpinBox;
    intensitySpinBox->setSuffix(" %");
    intensitySpinBox->setRange(0,100);
    peakPickButton = new QPushButton(tr("output 1D peak"));

    intensity2DSpinBox = new QDoubleSpinBox;
    intensity2DSpinBox->setSuffix(" %");
    intensity2DSpinBox->setRange(0,100);
    setAreaCheckBox = new QCheckBox(tr("set target point"));
    targetAreaStartSpinBox = new QSpinBox;
    targetAreaStartSpinBox->setRange(1,16383);
    targetAreaEndSpinBox = new QSpinBox;
    targetAreaEndSpinBox->setRange(1,16383);
    peakPick2DButton = new QPushButton(tr("output 2D peak"));

    setAreaCheckBox->setChecked(false);
    targetAreaStartSpinBox->setEnabled(false);
    targetAreaEndSpinBox->setEnabled(false);
}

void KPeakPickWidget::createPanel()
{

    QVBoxLayout *mainLayout = new QVBoxLayout;

    QGroupBox *groupBox0 = new QGroupBox(tr("1D peak pick"));
      QGridLayout *gridLayout0 = new QGridLayout;
      gridLayout0->addWidget(new QLabel(tr("Target Array")),0,0,1,1);
      gridLayout0->addWidget(whichFidSpinBox,0,1,1,1);
      gridLayout0->addWidget(new QLabel(tr("Target intensity")),1,0,1,1);
      gridLayout0->addWidget(intensitySpinBox,1,1,1,1);
      gridLayout0->addWidget(peakPickButton,2,1,1,1);
    groupBox0->setLayout(gridLayout0);

    QGroupBox *groupBox1 = new QGroupBox(tr("2D peak pick"));
      QGridLayout *gridLayout1 = new QGridLayout;
      gridLayout1->addWidget(new QLabel(tr("Target intensity")),0,0,1,1);
      gridLayout1->addWidget(intensity2DSpinBox,0,2,1,1);
      gridLayout1->addWidget(setAreaCheckBox,1,0,1,2);
      gridLayout1->addWidget(targetAreaStartSpinBox,2,0,1,1);
      gridLayout1->addWidget(new QLabel(tr("to")),2,1,1,1);
      gridLayout1->addWidget(targetAreaEndSpinBox,2,2,1,1);
      gridLayout1->addWidget(peakPick2DButton,3,1,1,2);
    groupBox1->setLayout(gridLayout1);

      mainLayout->addWidget(groupBox0);
      mainLayout->addWidget(groupBox1);

      mainLayout->addStretch();
      setLayout(mainLayout);
}

void KPeakPickWidget::createConnections()
{
    connect(peakPickButton,SIGNAL(clicked()),this,SLOT(performPeakPick()));
    connect(setAreaCheckBox,SIGNAL(toggled(bool)),targetAreaStartSpinBox,SLOT(setEnabled(bool)));
    connect(setAreaCheckBox,SIGNAL(toggled(bool)),targetAreaEndSpinBox,SLOT(setEnabled(bool)));
    connect(peakPick2DButton,SIGNAL(clicked()),this,SLOT(performPeakPick2D()));
}

void KPeakPickWidget::performPeakPick()
{
    double tgPeak = intensitySpinBox->value();
    int tgFID = whichFidSpinBox->value();
    qDebug() << "clicked" << tgPeak;

    if(!isAncestorDefined()) {return;}
    if(ancestor()->FID_2D->FID.isEmpty()) {return;}

    if(tgFID < 1 || tgFID > ancestor()->FID_2D->FID.size()) {return;}

    bool ok;
    KPeakPick *peakPick = new KPeakPick;

    ok=peakPick->process(ancestor()->FID_2D, tgPeak, tgFID);
    if(!ok) {delete peakPick; return;}

    ancestor()->processOperations->processElements.append(peakPick);
    ancestor()->updateProcessSettings();

    //    ancestor()->axisFormatWidget->init();
    //    ancestor()->axisFormatWidget->refresh();
    //    ancestor()->phaseWidget->phasePivotSpinBox->setValue(
    //                    ancestor()->FID_2D->FID.at(0)->abs->maxAt()
    //                    );

    for(int k=0; k<ancestor()->plotters->FIDPlotters.size(); k++)
    {
      ancestor()->plotters->FIDPlotters[k]->plotter->xini=0;
      ancestor()->plotters->FIDPlotters[k]->plotter->xfin=ancestor()->FID_2D->FID.at(0)->al()-1;
    }

    ancestor()->plotters->update();

}

void KPeakPickWidget::performPeakPick2D()
{
    double tgPeak = intensity2DSpinBox->value();
    //qDebug() << "clicked 2D" << tgPeak;

    if(!isAncestorDefined()) {return;}
    if(ancestor()->FID_2D->FID.isEmpty()) {return;}
    if(ancestor()->FID_2D->FID.size()!=ancestor()->FID_2D->defaultAL()){return;}

    int tgSt = 0;
    int tgEn = ancestor()->FID_2D->defaultAL();

    bool ok;
    KPeakPick *peakPick = new KPeakPick;

    if(setAreaCheckBox->isChecked())
    {
        tgSt = targetAreaStartSpinBox->value();
        tgEn = targetAreaEndSpinBox->value();
    }

    if(tgEn - tgSt < 3){return;}

    ok=peakPick->process2D(ancestor()->FID_2D, tgPeak, tgSt, tgEn);
    if(!ok) {delete peakPick; return;}

    ancestor()->processOperations->processElements.append(peakPick);
    ancestor()->updateProcessSettings();

    for(int k=0; k<ancestor()->plotters->FIDPlotters.size(); k++)
    {
      ancestor()->plotters->FIDPlotters[k]->plotter->xini=0;
      ancestor()->plotters->FIDPlotters[k]->plotter->xfin=ancestor()->FID_2D->FID.at(0)->al()-1;
    }

    ancestor()->plotters->update();

}
