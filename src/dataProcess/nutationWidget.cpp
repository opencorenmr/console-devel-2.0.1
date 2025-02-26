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
    fromAtLabel = new QLabel("At");
    toLabel = new QLabel("To"); toLabel->setVisible(false);
    arrayInitLineEdit = new QLineEdit();
    arrayDeltaLineEdit = new QLineEdit();

    styleComboBox = new QComboBox();
    styleComboBox->addItems(QStringList() << "Point" << "Area");

    startPointSpinBox = new QSpinBox();
    startPointSpinBox->setMaximum(16384);
    endPointSpinBox = new QSpinBox();
    endPointSpinBox->setMaximum(16384);
    endPointSpinBox->setVisible(false);
    startClickSetCheckBox = new QCheckBox(tr("Click-set"));
    endClickSetCheckBox = new QCheckBox(tr("Click-set"));
    endClickSetCheckBox->setVisible(false);

    processArrayButton = new QPushButton(tr("Process"));
    processArrayButton->setFixedSize(QSize(80,80));
    resultTextEdit = new QPlainTextEdit;
    saveButton = new QPushButton(tr("Save"));
    saveButton->setFixedSize(QSize(80,80));

    // 20250226
    selectComponentCombobox = new QComboBox();
    selectComponentCombobox->addItems(QStringList() << "In-phase" << "Quadrature" << "Absolute");
    selectComponentCombobox->setCurrentIndex(0);

}

void KNutationWidget::createPanel()
{

    QVBoxLayout *mainLayout = new QVBoxLayout;

   // QGroupBox *groupBox0 = new QGroupBox(tr("Analyze"));

      QGridLayout *gridLayout0 = new QGridLayout;

      gridLayout0->addWidget(new QLabel(tr("Component")),0,0,1,1);
      gridLayout0->addWidget(selectComponentCombobox,0,1,1,1);

      gridLayout0->addWidget(new QLabel(tr("Point/Area")),1,0,1,1);
      gridLayout0->addWidget(styleComboBox,1,1,1,1);

      gridLayout0->addWidget(fromAtLabel,2,0,1,1);
      gridLayout0->addWidget(startPointSpinBox,2,1,1,1);
      gridLayout0->addWidget(startClickSetCheckBox,2,2,1,1);
      gridLayout0->addWidget(toLabel,3,0,1,1);
      gridLayout0->addWidget(endPointSpinBox,3,1,1,1);
      gridLayout0->addWidget(endClickSetCheckBox,3,2,1,1);

      gridLayout0->addWidget(new QLabel(tr("initial")),4,0,1,1);
      gridLayout0->addWidget(arrayInitLineEdit,4,1,1,1);
      gridLayout0->addWidget(new QLabel(tr("delta")),5,0,1,1);
      gridLayout0->addWidget(arrayDeltaLineEdit,5,1,1,1);




      gridLayout0->addWidget(processArrayButton,4,2,2,1);
      gridLayout0->addWidget(resultTextEdit,6,0,3,2);
      gridLayout0->addWidget(saveButton,7,2,2,1);

   // groupBox0->setLayout(gridLayout0);

      mainLayout->addLayout(gridLayout0);

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

    connect(styleComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(onStyleComboBoxChanged(int)));

    connect(saveButton,SIGNAL(clicked()),this,SLOT(onSaveButtonClicked()));
}

void KNutationWidget::onSaveButtonClicked()
{
    //--------------export *.dat-------------
    QString path = ancestor()->processFileWidget->dataFilePath()+'/';
    QString fileName = QFileDialog::getSaveFileName(this, "Export Ascii Data", path, "dat(*.dat)");

    if(fileName.isEmpty()) return;
    //qDebug() << fileName << endl;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;

    QTextStream out(&file);
    out << resultTextEdit->toPlainText().toUtf8();
    file.close();


}

void KNutationWidget::processArray()
{
    double height;

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

    if(styleComboBox->currentIndex()!=0){
        if(start>end)
        {
            int help=end;
            end=start;
            start=help;
        }
    }

    //qDebug() << init << " " << delta << " " << start << " " << end << endl;

    //--------------export *.dat-------------
//    QString path = ancestor()->processFileWidget->dataFilePath()+'/';
//    QString fileName = QFileDialog::getSaveFileName(this, "Export Ascii Data", path, "dat(*.dat)");

//    if(fileName.isEmpty()) return;
    //qDebug() << fileName << endl;

    resultTextEdit->clear();

    //-----output point-------
    if(styleComboBox->currentIndex()==0)
    {
        for(int k=0;k<fidsize;k++){

            switch(selectComponentCombobox->currentIndex())
            {
              case 0:
                 height=ancestor()->FID_2D->FID.at(k)->real->sig.at(start);
                 break;
              case 1:
                height=ancestor()->FID_2D->FID.at(k)->imag->sig.at(start);
                break;
              case 2:
                height=ancestor()->FID_2D->FID.at(k)->abs->sig.at(start);
                break;
              default:
                 height=0;
              break;
            } // switch

            resultTextEdit->appendPlainText(
             QString::number(init+delta*k) + " "
                  + QString::number(height)
              // + QString::number(ancestor()->FID_2D->FID.at(k)->real->sig.at(start))

                        );
//            out << QString::number(init+delta*k) << " "
//                << QString::number(ancestor()->FID_2D->FID.at(k)->real->sig.at(start))
//                << "\n";
        };
    }
    //-----output area------
    else{
        double calc=0;
        for(int k=0;k<fidsize;k++){
            calc=0;
            for(int j=start;j<end+1;j++){
                switch(selectComponentCombobox->currentIndex())
                {
                  case 0:
                     height=ancestor()->FID_2D->FID.at(k)->real->sig.at(j);
                     break;
                  case 1:
                    height=ancestor()->FID_2D->FID.at(k)->imag->sig.at(j);
                    break;
                  case 2:
                    height=ancestor()->FID_2D->FID.at(k)->abs->sig.at(j);
                    break;
                  default:
                     height=0;
                  break;
                } // switch
//                calc += ancestor()->FID_2D->FID.at(k)->real->sig.at(j);
                calc += height;
            }
            resultTextEdit->appendPlainText(
               QString::number(init+delta*k) + " "
                + QString::number(calc)
               );
        }
    }

/*
    for(int k=0; k<ancestor()->plotters->FIDPlotters.size(); k++)
    {
      ancestor()->plotters->FIDPlotters[k]->plotter->xini=0;
      ancestor()->plotters->FIDPlotters[k]->plotter->xfin=ancestor()->FID_2D->FID.at(0)->al()-1;
    }

    ancestor()->plotters->update();
*/
    //qDebug() << "end";
    return;
}

void KNutationWidget::onStyleComboBoxChanged(int index){
    if(index==0){
        //qDebug() << "point";
        fromAtLabel->setText(tr("At"));
        toLabel->setVisible(false);
        endPointSpinBox->setVisible(false);
        endClickSetCheckBox->setVisible(false);
    } else {
        //qDebug() << "Area";
        fromAtLabel->setText(tr("From"));
        toLabel->setVisible(true);
        endPointSpinBox->setVisible(true);
        endClickSetCheckBox->setVisible(true);
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
