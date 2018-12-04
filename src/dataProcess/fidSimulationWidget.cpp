#include "fidSimulationWidget.h"
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

KFidSimulationWidget::KFidSimulationWidget()
{
   createWidgets();
   createPanel();
   createConnections();
}

void KFidSimulationWidget::createWidgets()
{
    fid1_AmpLineEdit = new QLineEdit();
    fid1_FreqLineEdit = new QLineEdit();
    fid1_T2LineEdit = new QLineEdit();

    arrayLineEdit = new QLineEdit();
    alLineEdit = new QLineEdit();
    dwLineEdit = new QLineEdit();

    noiseCheckBox = new QCheckBox();
    noiseLineEdit = new QLineEdit();
    addCheckBox = new QCheckBox();
    addFidButton = new QPushButton(tr("Add"));
}

void KFidSimulationWidget::createPanel()
{

    QVBoxLayout *mainLayout = new QVBoxLayout;

    QGroupBox *groupBox0 = new QGroupBox(tr("Peak Setting"));
      QGridLayout *gridLayout0 = new QGridLayout;
      gridLayout0->addWidget(new QLabel(tr("Amp.")),0,0,1,1);
      gridLayout0->addWidget(new QLabel(tr("Freq. [kHz]")),0,1,1,1);
      gridLayout0->addWidget(new QLabel(tr("T2 [ms]")),0,2,1,1);
      gridLayout0->addWidget(fid1_AmpLineEdit,1,0,1,1);
      gridLayout0->addWidget(fid1_FreqLineEdit,1,1,1,1);
      gridLayout0->addWidget(fid1_T2LineEdit,1,2,1,1);
    groupBox0->setLayout(gridLayout0);



    QGroupBox *groupBox1 =new QGroupBox(tr("FID Setting"));
      QGridLayout *gridLayout1 = new QGridLayout;
      gridLayout1->addWidget(new QLabel(tr("add to current Data")),0,0,1,1);
      gridLayout1->addWidget(addCheckBox,0,1,1,1);
      /*
      gridLayout1->addWidget(new QLabel(tr("Array (eq)")),0,0,1,1);
      gridLayout1->addWidget(new QLabel(tr("al")),0,1,1,1);
      gridLayout1->addWidget(new QLabel(tr("dw (us)")),0,2,1,1);
      gridLayout1->addWidget(arrayLineEdit,1,0,1,1);
      gridLayout1->addWidget(alLineEdit,1,1,1,1);
      gridLayout1->addWidget(dwLineEdit,1,2,1,1);
      */
    groupBox1->setLayout(gridLayout1);

    QGroupBox *groupBox2 =new QGroupBox(tr("Noise Setting"));
      QGridLayout *gridLayout2 = new QGridLayout;
      gridLayout2->addWidget(new QLabel(tr("Noise")),0,0,1,1);
      gridLayout2->addWidget(noiseCheckBox,0,1,1,1);
      gridLayout2->addWidget(new QLabel(tr("Amp.")),0,2,1,1);
      gridLayout2->addWidget(noiseLineEdit,0,3,1,1);
    groupBox2->setLayout(gridLayout2);



      mainLayout->addWidget(groupBox0);
      mainLayout->addWidget(groupBox2);
      mainLayout->addWidget(groupBox1);

      mainLayout->addWidget(addFidButton);

      mainLayout->addStretch();
      setLayout(mainLayout);

}

void KFidSimulationWidget::createConnections()
{
    connect(addFidButton,SIGNAL(clicked()),this,SLOT(performFidCalculation()));
}

void KFidSimulationWidget::performFidCalculation()
{
    if(!isAncestorDefined()) return;
    if(ancestor()->FID_2D->FID.isEmpty()) return;

    //srand((unsigned int) time(NULL));

    int fidsize = ancestor()->FID_2D->FID.size();
    int fidal = ancestor()->FID_2D->al();
    //------------remove original fid-------------
    if(!addCheckBox->isChecked()){
        for(int k=0;k<fidsize;k++){
            for(int i=0;i<fidal;i++){
                ancestor()->FID_2D->FID[k]->real->sig[i] = 0.0;
                ancestor()->FID_2D->FID[k]->imag->sig[i] = 0.0;
            }
        }
    }

    //--------------------------------------------

    QString s = fid1_AmpLineEdit->text();
    double amp = s.toDouble();
    s = fid1_FreqLineEdit->text();
    double freq = s.toDouble();
    s = fid1_T2LineEdit->text();
    double rt2 = s.toDouble();
    double ampNoise;
    /*
    s = arrayLineEdit->text();
    int array = s.toInt();
    s = alLineEdit->text();
    int newal = s.toInt();
    s = dwLineEdit->text();
    double newdw = s.toDouble();
    */

    //--------------calc-------------
    double dw = ancestor()->FID_2D->dw();
    double t, a, b, c, d;

    for(int k=0;k<fidsize;k++){
        for(int i=0;i<fidal;i++){
            t = (i+k)*dw/1000;
            ancestor()->FID_2D->FID[k]->real->sig[i] += amp*cos(freq*2*M_PI*t)*exp(-t/rt2);
            ancestor()->FID_2D->FID[k]->imag->sig[i] += amp*sin(freq*2*M_PI*t)*exp(-t/rt2);
        }
        ancestor()->FID_2D->FID[k]->updateAbs();
    }

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



    for(int k=0; k<ancestor()->plotters->FIDPlotters.size(); k++)
    {
      ancestor()->plotters->FIDPlotters[k]->plotter->xini=0;
      ancestor()->plotters->FIDPlotters[k]->plotter->xfin=ancestor()->FID_2D->FID.at(0)->al()-1;
    }

    ancestor()->plotters->update();

    //qDebug() << "end";
    return;
}
