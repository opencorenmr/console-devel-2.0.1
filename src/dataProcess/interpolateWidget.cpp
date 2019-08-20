#include "interpolateWidget.h"
#include "processPanelWidget.h"
#include "interpolate.h"
#include "math.h"
#include "../plotter.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>
#include <QComboBox>

#include <QFileDialog>

#include <QDebug>

KInterpolateWidget::KInterpolateWidget()
{
   createWidgets();
   createPanel();
   createConnections();
}

void KInterpolateWidget::createWidgets()
{
    initialAngleLineEdit = new QLineEdit();
    deltaAngleLineEdit = new QLineEdit();

    processButton = new QPushButton(tr("Process"));
}

void KInterpolateWidget::createPanel()
{

    QVBoxLayout *mainLayout = new QVBoxLayout;

    QGroupBox *groupBox0 = new QGroupBox(tr("Interpolate from Radial data"));

      QGridLayout *gridLayout0 = new QGridLayout;
      //gridLayout0->addWidget(new QLabel(tr("initial")),0,0,1,1);
      gridLayout0->addWidget(new QLabel(tr("delta")),1,0,1,1);
      //gridLayout0->addWidget(initialAngleLineEdit,0,1,1,1);
      gridLayout0->addWidget(deltaAngleLineEdit,1,1,1,1);

      gridLayout0->addWidget(processButton,2,0,1,1);

    groupBox0->setLayout(gridLayout0);

      mainLayout->addWidget(groupBox0);

      mainLayout->addStretch();
      setLayout(mainLayout);

}

void KInterpolateWidget::createConnections()
{
    connect(processButton,SIGNAL(clicked()),this,SLOT(processInterpolate()));
}

void KInterpolateWidget::processInterpolate()
{
    if(!isAncestorDefined()) return;
    if(ancestor()->FID_2D->FID.isEmpty()) return;

    QString s = deltaAngleLineEdit->text();
    double delta = s.toDouble();

    bool ok;
    KInterpolate *interpolate = new KInterpolate;
    ok=interpolate->processInterpolate(ancestor()->FID_2D, delta);
    if(!ok)
    {
      delete interpolate;
      return;
    }
    ancestor()->processOperations->processElements.append(interpolate);
    ancestor()->updateProcessSettings();

    ancestor()->axisFormatWidget->init();
    ancestor()->axisFormatWidget->refresh();
//    ancestor()->phaseWidget->phasePivotSpinBox->setValue(
//                    ancestor()->FID_2D->FID.at(0)->abs->maxAt()
//                    );


    for(int k=0; k<ancestor()->plotters->FIDPlotters.size(); k++)
    {

      ancestor()->plotters->FIDPlotters[k]->plotter->xini=0;
      ancestor()->plotters->FIDPlotters[k]->plotter->xfin=ancestor()->FID_2D->FID.at(0)->al()-1;

    }

    ancestor()->plotters->update();
    return;
}
