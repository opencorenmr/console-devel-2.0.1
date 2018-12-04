#include "slowftWidget.h"
#include "processPanelWidget.h"
#include "timeAcquire.h"
#include "slowft.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>
#include <QComboBox>
#include <QPlainTextEdit>

KSlowFTWidget::KSlowFTWidget()
{
   createWidgets();
   createPanel();
   createConnections();
}



void KSlowFTWidget::createWidgets()
{
    timeAcquireTextEdit = new QPlainTextEdit();
    timeTypeComboBox = new QComboBox();
    timeTypeComboBox->addItems(QStringList()<<"Double"/*<<"Double"*/);
    timeAcquireButton = new QPushButton(tr("Acquire"));
    slowFTButton = new QPushButton(tr("perform slowFT"));
}

void KSlowFTWidget::createPanel()
{
  QVBoxLayout *mainLayout= new QVBoxLayout(this);

  QGroupBox *groupBox0 = new QGroupBox(tr("Sampling Time Setting"));
    QGridLayout *gridLayout0 = new QGridLayout;
    gridLayout0->addWidget(timeAcquireTextEdit,0,0,2,1);
    gridLayout0->addWidget(timeTypeComboBox,0,1,1,1);
    gridLayout0->addWidget(timeAcquireButton,1,1,1,1);
  groupBox0->setLayout(gridLayout0);

  mainLayout->addWidget(groupBox0);
  mainLayout->addWidget(slowFTButton);
  mainLayout->addStretch();
  setLayout(mainLayout);
}

void KSlowFTWidget::createConnections()
{
    connect(timeAcquireButton,SIGNAL(clicked()),this,SLOT(timeAcquisition()));
    //connect(indirectTimeTypeComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT());
    connect(slowFTButton,SIGNAL(clicked()),this,SLOT(performSlowFT()));
}

void KSlowFTWidget::timeAcquisition()
{
  if(!isAncestorDefined()) return;
  if(ancestor()->FID_2D->FID.isEmpty()) return;

  //-----

  /*
  int timeType;
  if(indirectTimeTypeComboBox->currentIndex()==0){timeType = 0;}
  else{timeType = 1;}
  */

  QString s=timeAcquireTextEdit->toPlainText().trimmed();
  QStringList sl=s.split(QRegExp("\\s+"));
  //-----

  bool ok;
  KTimeAcquire *timeAcquire = new KTimeAcquire;
  ok=timeAcquire->process(ancestor()->FID_2D, &sl);
  if(!ok)
  {
    delete timeAcquire;
    return;
  }

  ancestor()->processOperations->processElements.append(timeAcquire);
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

void KSlowFTWidget::performSlowFT()
{
    if(!isAncestorDefined()) return;
    if(ancestor()->FID_2D->FID.isEmpty()) return;

    bool ok;
    KSlowFT *slowft = new KSlowFT;
    ok=slowft->process(ancestor()->FID_2D);
    if(!ok)
    {
      delete slowft;
      return;
    }

    ancestor()->processOperations->processElements.append(slowft);
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
