#include "nutationWidget.h"
#include "processPanelWidget.h"
#include "math.h"
//#include <random>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>
#include <QComboBox>

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
    endPointSpinBox = new QSpinBox();
    startClickSetCheckBox = new QCheckBox(tr("Click-set"));
    endClickSetCheckBox = new QCheckBox(tr("Click-set"));

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

    /*
    s = arrayLineEdit->text();
    int array = s.toInt();
    s = alLineEdit->text();
    int newal = s.toInt();
    s = dwLineEdit->text();
    double newdw = s.toDouble();
    */

    //--------------calc-------------
/*
    if(noiseCheckBox->isChecked())
    {
        s = noiseLineEdit->text();
        ampNoise = s.toDouble();

        for(int k=0;k<fidsize;k++)
        {
            for(int i=0;i<fidal;i++)
            {
                a = rand();
                b = rand();
                c = ampNoise*((2*a-RAND_MAX)/RAND_MAX);
                d = ampNoise*((2*b-RAND_MAX)/RAND_MAX);
                ancestor()->FID_2D->FID[k]->real->sig[i] += c;
                ancestor()->FID_2D->FID[k]->imag->sig[i] += d;
            }
        }
    }
*/


    for(int k=0; k<ancestor()->plotters->FIDPlotters.size(); k++)
    {
      ancestor()->plotters->FIDPlotters[k]->plotter->xini=0;
      ancestor()->plotters->FIDPlotters[k]->plotter->xfin=ancestor()->FID_2D->FID.at(0)->al()-1;
    }

    ancestor()->plotters->update();

    //qDebug() << "end";
    return;
}
