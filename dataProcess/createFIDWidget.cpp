#include <QLayout>
#include <QWidget>
#include <QGroupBox>
#include <QLabel>
#include <QFileDialog>
#include <QFileInfo>
#include <QDebug>

#include "createFIDWidget.h"

TCreateFIDWidget::TCreateFIDWidget(QWidget *parent) : QWidget(parent)
{


    createWidgets();
    createPanel();
    createConnections();

}

void TCreateFIDWidget::createWidgets()
{
    plainTextEdit = new QPlainTextEdit;
    loadFromFileButton = new QPushButton("Load from file");
    dwDoubleSpinBox = new QDoubleSpinBox;
      dwDoubleSpinBox->setSuffix(" us");
      dwDoubleSpinBox->setMinimum(0);
      dwDoubleSpinBox->setMaximum(1e100);
      dwDoubleSpinBox->setValue(1.0);
    sf1DoubleSpinBox = new QDoubleSpinBox;
    freqDoubleSpinBox = new QDoubleSpinBox;
    createModeComboBox = new QComboBox;
      createModeComboBox->addItems(QStringList() << "ascii data");
                                                // << "fid properties");
    decayFunctionComboBox = new QComboBox;
    alSpinBox = new QSpinBox;
      alSpinBox->setMinimum(2);
      alSpinBox->setMaximum(32768);
    createFIDPushButton = new QPushButton(tr("Create FID"));

    stackedWidget = new QStackedWidget;

    groupBox1 = new QGroupBox;
    groupBox2 = new QGroupBox;

}



void TCreateFIDWidget::createPanel()
{
    QGridLayout *gLayout0 = new QGridLayout(this); // main layout
    QGridLayout *gLayout1 = new QGridLayout;
    QGridLayout *gLayout2 = new QGridLayout;

    gLayout1->addWidget(loadFromFileButton,0,0,1,2);
    gLayout1->addWidget(plainTextEdit,1,0,1,2);

//    gLayout2->addWidget()

    groupBox1->setLayout(gLayout1);
    groupBox2->setLayout(gLayout2);
    stackedWidget->addWidget(groupBox1);
    stackedWidget->addWidget(groupBox2);
    createModeComboBox->setCurrentIndex(0);
    stackedWidget->setCurrentIndex(0);

    gLayout0->addWidget(new QLabel(tr("Mode")),0,0,1,1);
    gLayout0->addWidget(createModeComboBox,0,1,1,1);
    gLayout0->addWidget(new QLabel(tr("dwell time")),1,0,1,1);
    gLayout0->addWidget(dwDoubleSpinBox,1,1,1,1);
    gLayout0->addWidget(stackedWidget,2,0,1,2);
    gLayout0->addWidget(createFIDPushButton,3,0,1,2);
}

void TCreateFIDWidget::createConnections()
{

    connect(createModeComboBox,SIGNAL(currentIndexChanged(int)),stackedWidget,SLOT(setCurrentIndex(int)));
    connect(createFIDPushButton,SIGNAL(clicked()),this,SLOT(onCreateFIDButtonClicked()));
    connect(loadFromFileButton,SIGNAL(clicked()),this,SLOT(onLoadFromFileButtonClicked()));

}


void TCreateFIDWidget::onCreateFIDButtonClicked()
{
    if(createModeComboBox->currentIndex()==0) createFIDFromAsci();
    else createFIDFromPropeties();
}

void TCreateFIDWidget::createFIDFromAsci()
{
    QStringList sl1,sl2;
    double re,im;
    QVector<double> inPhase,quadrature;
    inPhase.clear();quadrature.clear();

    bool ok;

    sl1=plainTextEdit->toPlainText().split('\n');
    for(int k=0; k<sl1.size(); k++)
    {
       sl2=sl1.at(k).trimmed().split(QRegExp("\\s+"));
       if(sl2.size()==2)
       {
           re=sl2.at(0).toDouble(&ok);
           im=sl2.at(1).toDouble(&ok);
           if(ok)
           {
               inPhase.append(re);
               quadrature.append(im);
           }
       }
       else if(sl2.size()==3) // we assume the first column represents time, which is ignored.
       {
           re=sl2.at(1).toDouble(&ok);
           im=sl2.at(2).toDouble(&ok);
           if(ok)
           {
               inPhase.append(re);
               quadrature.append(im);
           }

       }

    } // k



    // We clear the content of FID, and immediately append one.
    FID_2D->FID.clear();
    FID_2D->FID.append(new TFID(inPhase.size()));
    // Since we have only a single item, we set the currentFID to be the first one.
    FID_2D->setCurrentFID(0);

    // Then we set dw according to the dwDoubleSpinBox, and al.
    FID_2D->setDW(dwDoubleSpinBox->value());
    FID_2D->setAl(inPhase.size());
    for(int k=0; k< FID_2D->al(); k++)
    {
        FID_2D->FID[0]->real->sig[k]=inPhase.at(k);
        FID_2D->FID[0]->imag->sig[k]=quadrature.at(k);
        // qDebug() << QString(Q_FUNC_INFO) << FID_2D->FID[0]->real->sig[k]
        // << FID_2D->FID[0]->imag->sig[k];
    }
    FID_2D->FID[0]->updateAbs();
    FID_2D->FID[0]->domain=TFID::TimeDomain;
    FID_2D->FID[0]->setEmpty(false);

   // qDebug() << QString(Q_FUNC_INFO) << "update request";
    emit updateRequest();


}

void TCreateFIDWidget::createFIDFromPropeties()
{
    ;
}

void TCreateFIDWidget::onLoadFromFileButtonClicked()
{
    QString path="~/";
    if(QDir(dataFilePath()).exists()) path=dataFilePath()+'/';
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open data"),
                                                    path
                                                    );
    if (fileName.isEmpty()) {return;}
    setDataFilePath(QFileInfo(fileName).absolutePath());

    QFile file(fileName);
    file.open(QFile::ReadOnly | QFile::Text);
    plainTextEdit->setPlainText(file.readAll());

    return;
}
