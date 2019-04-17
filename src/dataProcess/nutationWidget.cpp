#include "nutationWidget.h"
#include "processPanelWidget.h"
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

KNutationWidget::KNutationWidget()
{
   createWidgets();
   createPanel();
   createConnections();
}

void KNutationWidget::createWidgets()
{
    arrayInitLineEdit = new QLineEdit();
    arrayDeltaLineEdit = new QLineEdit();

    processStyleComboBox = new QComboBox();
    processStyleComboBox->addItems(QStringList() << "Point" << "Area");

    startPointSpinBox = new QSpinBox();
    startPointSpinBox->setMaximum(16384);
    endPointSpinBox = new QSpinBox();
    endPointSpinBox->setMaximum(16384);
    endPointSpinBox->setDisabled(true);
    startClickSetCheckBox = new QCheckBox(tr("Click-set"));
    endClickSetCheckBox = new QCheckBox(tr("Click-set"));
    endClickSetCheckBox->setDisabled(true);

    processArrayButton = new QPushButton(tr("Process"));
}

void KNutationWidget::createPanel()
{

    QVBoxLayout *mainLayout = new QVBoxLayout;

    QGroupBox *groupBox0 = new QGroupBox(tr("Process Array"));

      QGridLayout *gridLayout0 = new QGridLayout;
      gridLayout0->addWidget(new QLabel(tr("initial")),0,0,1,1);
      gridLayout0->addWidget(new QLabel(tr("delta")),1,0,1,1);
      gridLayout0->addWidget(arrayInitLineEdit,0,1,1,1);
      gridLayout0->addWidget(arrayDeltaLineEdit,1,1,1,1);

      gridLayout0->addWidget(processStyleComboBox,2,1,1,3);

      gridLayout0->addWidget(new QLabel(tr("start")),3,0,1,1);
      gridLayout0->addWidget(new QLabel(tr("end")),4,0,1,1);
      gridLayout0->addWidget(startPointSpinBox,3,1,1,1);
      gridLayout0->addWidget(endPointSpinBox,4,1,1,1);
      gridLayout0->addWidget(startClickSetCheckBox,3,2,1,1);
      gridLayout0->addWidget(endClickSetCheckBox,4,2,1,1);

      gridLayout0->addWidget(processArrayButton,5,0,1,1);

    groupBox0->setLayout(gridLayout0);

      mainLayout->addWidget(groupBox0);

      mainLayout->addStretch();
      setLayout(mainLayout);

}

void KNutationWidget::createConnections()
{
    connect(processArrayButton,SIGNAL(clicked()),this,SLOT(processArray()));
    connect(startPointSpinBox,SIGNAL(valueChanged(int)),this,SLOT(setStartPoints(int)));
    connect(endPointSpinBox,SIGNAL(valueChanged(int)),this,SLOT(setEndPoints(int)));
    connect(startClickSetCheckBox,SIGNAL(toggled(bool)),this,SLOT(clickSetStartPoints()));
    connect(endClickSetCheckBox,SIGNAL(toggled(bool)),this,SLOT(clickSetEndPoints()));

    connect(processStyleComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(enableArea()));
}

void KNutationWidget::processArray()
{
    if(!isAncestorDefined()) return;
    if(ancestor()->FID_2D->FID.isEmpty()) return;

    int fidsize = ancestor()->FID_2D->FID.size();

    //--------------------------------------------
    QString s = arrayInitLineEdit->text();
    double init = s.toDouble();
    s = arrayDeltaLineEdit->text();
    double delta = s.toDouble();

    int start = startPointSpinBox->value();
    int end = endPointSpinBox->value();

    if(processStyleComboBox->currentIndex()!=0){
        if(start>=end) return;
    }

    //qDebug() << init << " " << delta << " " << start << " " << end << endl;

    //--------------export *.dat-------------
    QString path = ancestor()->processFileWidget->dataFilePath()+'/';
    QString fileName = QFileDialog::getSaveFileName(this, "Export Ascii Data", path, "dat(*.dat)");

    if(fileName.isEmpty()) return;
    //qDebug() << fileName << endl;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;

    QTextStream out(&file);

    //-----output point-------
    if(processStyleComboBox->currentIndex()==0)
    {
        for(int k=0;k<fidsize;k++){
            out << QString::number(init+delta*k) << " "
                << QString::number(ancestor()->FID_2D->FID.at(k)->real->sig.at(start))
                << "\r\n";
        };
    }
    //-----output area------
    else{
        double calc=0;
        for(int k=0;k<fidsize;k++){
            calc=0;
            for(int j=start;j<end+1;j++){
                calc += ancestor()->FID_2D->FID.at(k)->real->sig.at(j);
            }
            out << QString::number(init+delta*k) << " "
                << QString::number(calc)
                << "\r\n";
        }
    }

    file.close();

    for(int k=0; k<ancestor()->plotters->FIDPlotters.size(); k++)
    {
      ancestor()->plotters->FIDPlotters[k]->plotter->xini=0;
      ancestor()->plotters->FIDPlotters[k]->plotter->xfin=ancestor()->FID_2D->FID.at(0)->al()-1;
    }

    ancestor()->plotters->update();

    //qDebug() << "end";
    return;
}

void KNutationWidget::enableArea(){
    if(processStyleComboBox->currentIndex()==0){
        //qDebug() << "point";
        endPointSpinBox->setDisabled(true);
        endClickSetCheckBox->setDisabled(true);
    } else {
        //qDebug() << "Area";
        endPointSpinBox->setEnabled(true);
        endClickSetCheckBox->setEnabled(true);
    }
}

void KNutationWidget::setStartPoints(int p){
    startPointSpinBox->setValue(p);
}

void KNutationWidget::setEndPoints(int p){
    endPointSpinBox->setValue(p);
}

void KNutationWidget::clickSetStartPoints()
{
    if(startClickSetCheckBox->isChecked())
    {
      if(endClickSetCheckBox->isChecked()) endClickSetCheckBox->setChecked(false);
    }

    if(!isAncestorDefined()) return;
    if(ancestor()->FID_2D->FID.isEmpty()) return;

    if(startClickSetCheckBox->isChecked())
    {
      for(int k=0; k<ancestor()->plotters->FIDPlotters.size(); k++)
      {
        connect(ancestor()->plotters->FIDPlotters[k]->plotter,SIGNAL(clickedXPosition(int)),
                this,SLOT(setStartPoints(int)));
      }
    }
    else
    {
      for(int k=0; k<ancestor()->plotters->FIDPlotters.size(); k++)
      {
         disconnect(ancestor()->plotters->FIDPlotters[k]->plotter,SIGNAL(clickedXPosition(int)),
                  this,SLOT(setStartPoints(int)));
      }
    }

}

void KNutationWidget::clickSetEndPoints()
{
    if(endClickSetCheckBox->isChecked())
    {
      if(startClickSetCheckBox->isChecked()) startClickSetCheckBox->setChecked(false);
    }

    if(!isAncestorDefined()) return;
    if(ancestor()->FID_2D->FID.isEmpty()) return;

    if(endClickSetCheckBox->isChecked())
    {
      for(int k=0; k<ancestor()->plotters->FIDPlotters.size(); k++)
      {
        connect(ancestor()->plotters->FIDPlotters[k]->plotter,SIGNAL(clickedXPosition(int)),
                this,SLOT(setEndPoints(int)));
      }
    }
    else
    {
      for(int k=0; k<ancestor()->plotters->FIDPlotters.size(); k++)
      {
         disconnect(ancestor()->plotters->FIDPlotters[k]->plotter,SIGNAL(clickedXPosition(int)),
                  this,SLOT(setEndPoints(int)));
      }
    }

}
