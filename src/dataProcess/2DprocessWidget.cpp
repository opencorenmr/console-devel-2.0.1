#include "2DprocessWidget.h"
#include "processPanelWidget.h"
#include "transpose.h"
#include "hyperComplex.h"
#include "slow2dft.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>

T2DProcessWidget::T2DProcessWidget()
{
   createWidgets();
   createPanel();
   createConnections();
}



void T2DProcessWidget::createWidgets()
{
    transposeButton = new QPushButton(tr("Apply"));

    hyperComplexPhaseComboBox = new QComboBox();
    hyperComplexPhaseComboBox->addItems(QStringList() << "As it is" << "Reverse");
    hyperComplexButton = new QPushButton(tr("Apply"));
}

void T2DProcessWidget::createPanel()
{
    QVBoxLayout *vLayout= new QVBoxLayout(this);
      QGroupBox *transposeGroupBox = new QGroupBox(tr("Transpose"));
      QHBoxLayout *transposeLayout = new QHBoxLayout;
      transposeLayout->addWidget(transposeButton);
      transposeGroupBox->setLayout(transposeLayout);
    vLayout->addWidget(transposeGroupBox);

      QGroupBox *hyperComplexGroupBox = new QGroupBox(tr("Hypercomplex compression"));
      QHBoxLayout *hyperComplexLayout = new QHBoxLayout;
      hyperComplexLayout->addWidget(new QLabel(tr("Phase")));
      hyperComplexLayout->addWidget(hyperComplexPhaseComboBox);
      hyperComplexLayout->addWidget(hyperComplexButton);
      hyperComplexGroupBox->setLayout(hyperComplexLayout);
    vLayout->addWidget(hyperComplexGroupBox);
    vLayout->addStretch();
    //Kobayashi
    //vLayout->addWidget(new QLabel(tr("slow2dft")));
    //vLayout->addWidget(indirectSlowFTButton);
    //vLayout->addStretch();
    //
}

void T2DProcessWidget::createConnections()
{

    connect(transposeButton,SIGNAL(clicked()),this,SLOT(performTranspose()));
    connect(hyperComplexButton,SIGNAL(clicked()),this,SLOT(performHyperComplexCompression()));

}

void T2DProcessWidget::performHyperComplexCompression()
{
    if(!isAncestorDefined()) return;
    if(ancestor()->FID_2D->FID.isEmpty()) return;

    bool ok;
    THyperComplex *hyperComplex = new THyperComplex;
    if(hyperComplexPhaseComboBox->currentIndex()==0) hyperComplex->setPhaseReverse(false);
    else hyperComplex->setPhaseReverse(true);

    ok=hyperComplex->process(ancestor()->FID_2D);
    if(!ok)
    {
      delete hyperComplex;
      return;
    }

    ancestor()->processOperations->processElements.append(hyperComplex);
    ancestor()->updateProcessSettings();


    for(int k=0; k<ancestor()->plotters->FIDPlotters.size(); k++)
    {
      ancestor()->plotters->FIDPlotters[k]->plotter->xini=0;
      ancestor()->plotters->FIDPlotters[k]->plotter->xfin=ancestor()->FID_2D->FID.at(0)->al()-1;
    }

    ancestor()->plotters->update();

}

void T2DProcessWidget::performTranspose()
{
    if(!isAncestorDefined()) return;
    if(ancestor()->FID_2D->FID.isEmpty()) return;

    bool ok;
    TTranspose *transpose = new TTranspose;
    ok=transpose->process(ancestor()->FID_2D);
    if(!ok)
    {
      delete transpose;
      return;
    }

    ancestor()->processOperations->processElements.append(transpose);
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
