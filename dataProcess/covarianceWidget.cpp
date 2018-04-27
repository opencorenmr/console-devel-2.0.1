#include "covarianceWidget.h"
#include "processPanelWidget.h"
#include "svd.h"
#include "covariance.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>
#include <QComboBox>

KCovarianceWidget::KCovarianceWidget()
{
   createWidgets();
   createPanel();
   createConnections();
}

void KCovarianceWidget::createWidgets()
{
  cutArraySpinBox = new QSpinBox();
  cutArrayLineEdit = new QLineEdit();
  cutArrayButton = new QPushButton(tr("Cut"));

  svdSettingComboBox = new QComboBox();
  svdSettingComboBox->addItems(QStringList() << "Covariance" << "Inner-Product");
  imagKeepCheckBox = new QCheckBox(tr("Keep imaginary"));
  svdButton = new QPushButton(tr("Apply"));

  covarianceComboBox = new QComboBox();
  covarianceComboBox->addItems(QStringList() << "Covariance" << "Inner-Product");
  imagKeepCovCheckBox = new QCheckBox(tr("Keep imaginary"));
  covarianceButton = new QPushButton(tr("Apply"));

  rootMatrixButton = new QPushButton(tr("Root"));
}

void KCovarianceWidget::createPanel()
{

    QVBoxLayout *mainLayout = new QVBoxLayout;

    QGroupBox *groupBox0 = new QGroupBox(tr("Cut Array Data"));
      QGridLayout *gridLayout0 = new QGridLayout;
      gridLayout0->addWidget(new QLabel(tr("remain head array")),0,0,1,1);
      gridLayout0->addWidget(cutArrayLineEdit,0,1,1,1);
      gridLayout0->addWidget(cutArrayButton,0,2,1,1);
    groupBox0->setLayout(gridLayout0);

    QGroupBox *groupBox1 =new QGroupBox(tr("Singular Value Decomposition"));
      QGridLayout *gridLayout1 = new QGridLayout;
      gridLayout1->addWidget(new QLabel(tr("calculation")),0,0,1,1);
      gridLayout1->addWidget(svdSettingComboBox,0,1,1,2);
      gridLayout1->addWidget(imagKeepCheckBox,1,0,1,1);
      gridLayout1->addWidget(svdButton,1,2,1,1);
    groupBox1->setLayout(gridLayout1);

    QGroupBox *groupBox2 =new QGroupBox(tr("straight forward calculation"));
      QGridLayout *gridLayout2 = new QGridLayout;
      gridLayout2->addWidget(new QLabel(tr("calculation")),0,0,1,1);
      gridLayout2->addWidget(covarianceComboBox,0,1,1,2);
      gridLayout2->addWidget(imagKeepCovCheckBox,1,0,1,1);
      gridLayout2->addWidget(covarianceButton,1,2,1,1);
      //gridLayout2->addWidget(rootMatrixButton,2,2,1,1);
    groupBox2->setLayout(gridLayout2);

      mainLayout->addWidget(groupBox0);
      mainLayout->addWidget(groupBox1);
      //mainLayout->addWidget(groupBox2);

      mainLayout->addStretch();
      setLayout(mainLayout);

}

void KCovarianceWidget::createConnections()
{
    connect(cutArrayButton,SIGNAL(clicked()),this,SLOT(performCutArray()));
    connect(svdButton,SIGNAL(clicked()),this,SLOT(performSVD()));
    connect(covarianceButton,SIGNAL(clicked()),this,SLOT(performCovariance()));
    //connect(rootMatrixButton,SIGNAL(clicked()),this,SLOT(performRootMatrix()));
    //connect(cutArraySpinBox,SIGNAL(valueChanged(int)),this,SLOT(setCutArray()));
}

void KCovarianceWidget::performCutArray()
{
    if(!isAncestorDefined()) return;
    if(ancestor()->FID_2D->FID.isEmpty()) return;

    int fidsize = ancestor()->FID_2D->FID.size();

    QString s = cutArrayLineEdit->text();
    int remain = s.toInt();

    if(fidsize<=remain){return;}

    while(ancestor()->FID_2D->FID.size()>remain){ancestor()->FID_2D->FID.removeLast();}

    for(int k=0; k<ancestor()->plotters->FIDPlotters.size(); k++)
    {
      ancestor()->plotters->FIDPlotters[k]->plotter->xini=0;
      ancestor()->plotters->FIDPlotters[k]->plotter->xfin=ancestor()->FID_2D->FID.at(0)->al()-1;
    }

    ancestor()->plotters->update();


}

void KCovarianceWidget::performSVD()
{

    if(!isAncestorDefined()) return;
    if(ancestor()->FID_2D->FID.isEmpty()) return;

    bool ok;
    KSVD *svd = new KSVD;

    if(svdSettingComboBox->currentIndex()==0)
    {
        svd->setInnerProduct(false);
    } else {
        svd->setInnerProduct(true);
    }

    if(imagKeepCheckBox->isChecked())
    {
        svd->setImagKeep(true);
    } else {
        svd->setImagKeep(false);
    }

    ok=svd->process(ancestor()->FID_2D);
    if(!ok)
    {
      delete svd;
      return;
    }

    ancestor()->processOperations->processElements.append(svd);
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

void KCovarianceWidget::performCovariance()
{
    if(!isAncestorDefined()) return;
    if(ancestor()->FID_2D->FID.isEmpty()) return;

    bool ok;
    KCovariance *covariance = new KCovariance;

    if(covarianceComboBox->currentIndex()==0) covariance->setInnerProduct(false);
    else covariance->setInnerProduct(true);

    if(imagKeepCovCheckBox->isChecked()) covariance->setImagKeep(true);
    else covariance->setImagKeep(false);

    ok=covariance->process(ancestor()->FID_2D);
    if(!ok) {delete covariance; return;}

    ancestor()->processOperations->processElements.append(covariance);
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


/*
void KCovarianceWidget::performRootMatrix()
{
    if(!isAncestorDefined()) return;
    if(ancestor()->FID_2D->FID.isEmpty()) return;

    if(ancestor()->FID_2D->al()!=ancestor()->FID_2D->FID.size()) return;

    bool ok;
    KRootMatrix *rootMatrix = new KRootMatrix;

    if(imagKeepCovCheckBox->isChecked()) rootMatrix->setImagKeep(true);
    else rootMatrix->setImagKeep(false);

    ok=rootMatrix->process(ancestor()->FID_2D);
    if(!ok) {delete rootMatrix; return;}

    ancestor()->processOperations->processElements.append(rootMatrix);
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
*/
