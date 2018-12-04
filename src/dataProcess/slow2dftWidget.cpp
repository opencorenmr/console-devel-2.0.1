#include "slow2dftWidget.h"
#include "processPanelWidget.h"
#include "indirectTimeAcquire.h"
#include "slow2dft.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>
#include <QLineEdit>
#include <QComboBox>
#include <QPlainTextEdit>

Kslow2DFTWidget::Kslow2DFTWidget()
{
   createWidgets();
   createPanel();
   createConnections();
}



void Kslow2DFTWidget::createWidgets()
{
    indirectTimeAcquireTextEdit = new QPlainTextEdit();
    indirectTimeTypeComboBox = new QComboBox();
    indirectTimeTypeComboBox->addItems(QStringList()<<"Double"/*<<"Double"*/);
    indirectTimeAcquireButton = new QPushButton(tr("Acquire"));
    arrayAxisInitialLineEdit = new QLineEdit();
    arrayAxisIntervalLineEdit = new QLineEdit();
    arrayAxisPointsLineEdit = new QLineEdit();
    indirectSlowFTButton = new QPushButton(tr("perform slow2DFT"));
    //indirectSlowFTButton = new QPushButton(tr("process slow2DFT"));
}

void Kslow2DFTWidget::createPanel()
{
  QVBoxLayout *mainLayout= new QVBoxLayout(this);

  QGroupBox *groupBox0 = new QGroupBox(tr("indirect Time Setting"));
    QGridLayout *gridLayout0 = new QGridLayout;
    gridLayout0->addWidget(indirectTimeAcquireTextEdit,0,0,2,1);
    gridLayout0->addWidget(indirectTimeTypeComboBox,0,1,1,1);
    gridLayout0->addWidget(indirectTimeAcquireButton,1,1,1,1);
  groupBox0->setLayout(gridLayout0);

  QGroupBox *groupBox1 = new QGroupBox(tr("Array Axis Setting"));
    QGridLayout *gridLayout1 = new QGridLayout;
    gridLayout1->addWidget(new QLabel(tr("Initial")),0,0,1,1);
    gridLayout1->addWidget(arrayAxisInitialLineEdit,0,1,1,1);
    gridLayout1->addWidget(new QLabel(tr("Interval")),1,0,1,1);
    gridLayout1->addWidget(arrayAxisIntervalLineEdit,1,1,1,1);
    gridLayout1->addWidget(new QLabel(tr("Points")),2,0,1,1);
    gridLayout1->addWidget(arrayAxisPointsLineEdit,2,1,1,1);
    gridLayout1->addWidget(indirectSlowFTButton,3,1,1,1);
  groupBox1->setLayout(gridLayout1);

  mainLayout->addWidget(groupBox0);
  mainLayout->addWidget(groupBox1);
  mainLayout->addStretch();
  setLayout(mainLayout);
}

void Kslow2DFTWidget::createConnections()
{
    connect(indirectTimeAcquireButton,SIGNAL(clicked()),this,SLOT(indirectTimeAcquisition()));
    //connect(indirectTimeTypeComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT());
    connect(indirectSlowFTButton,SIGNAL(clicked()),this,SLOT(performIndirectSlowFT()));
}

void Kslow2DFTWidget::indirectTimeAcquisition()
{
  if(!isAncestorDefined()) return;
  if(ancestor()->FID_2D->FID.isEmpty()) return;

  //-----
  int timeType;
  if(indirectTimeTypeComboBox->currentIndex()==0){timeType = 0;}
  else{timeType = 1;}

  QString s=indirectTimeAcquireTextEdit->toPlainText().trimmed();
  QStringList sl=s.split(QRegExp("\\s+"));
  //-----

  bool ok;
  KIndirectTimeAcquire *indirectTimeAcquire = new KIndirectTimeAcquire;
  ok=indirectTimeAcquire->process(ancestor()->FID_2D, &sl, &timeType);
  if(!ok)
  {
    delete indirectTimeAcquire;
    return;
  }

  ancestor()->processOperations->processElements.append(indirectTimeAcquire);
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

void Kslow2DFTWidget::performIndirectSlowFT()
{
    if(!isAncestorDefined()) return;
    if(ancestor()->FID_2D->FID.isEmpty()) return;

    QString s = arrayAxisInitialLineEdit->text();
    double init = s.toDouble();
    s = arrayAxisIntervalLineEdit->text();
    double inter = s.toDouble();
    s = arrayAxisPointsLineEdit->text();
    int pts = s.toInt();

    bool ok;
    KSlow2DFT *slow2dft = new KSlow2DFT;
    ok=slow2dft->ftprocess(ancestor()->FID_2D, &init, &inter, &pts);
    if(!ok)
    {
      delete slow2dft;
      return;
    }

    ancestor()->processOperations->processElements.append(slow2dft);
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
