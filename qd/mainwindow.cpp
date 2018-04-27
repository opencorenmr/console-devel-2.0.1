#include "mainwindow.h"
#include "../digitalfilter.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    devicePixelRatio=qApp->devicePixelRatio();


  #if defined(__APPLE__)
    defaultFont.setFamily("Monaco");
  #elif defined(_WIN32)
    defaultFont.setFamily("Consolas");
  #endif
    defaultFont.setFixedPitch(true);
    defaultFont.setPointSize(11);


  #if defined(__APPLE__)
    editorFont.setFamily("Monaco");
    editorFont.setPointSize(12);
  #elif defined(_WIN32)
    editorFont.setFamily("Consolas");
    editorFont.setPointSize(11);
  #endif
    editorFont.setFixedPitch(true);

    QSettings settings(QApplication::applicationDirPath() + "/nmr.ini", QSettings::IniFormat);
    settings.beginGroup("MainWindow");
      resize(settings.value("size", QSize(600, 400)).toSize());
      move(settings.value("pos", QPoint(200, 200)).toPoint());
    settings.endGroup();
    settings.beginGroup("Path");
//    ppg->setppgFilePath(settings.value("ppgSourceFilePath","").toString());
    dataFilePath=settings.value("dataFilePath").toString();
    exportFilePath=settings.value("exportFilePath").toString();
    settings.endGroup();


    // A layout cannot be set on the MainWindow.
    // Accordingly, a central widget is set.
    centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);

    openDataButton = new QPushButton(tr("Open"));
    cosSinMultiplyButtion = new QPushButton(tr("Multiply"));
    cosSinLineEdit = new QLineEdit;
    applyDigitalFilterButtion = new QPushButton(tr("Apply"));
    digitalFilterLineEdit = new QLineEdit;

    QGridLayout *leftLayout = new QGridLayout;
    leftLayout->addWidget(new QLabel(tr("Current data: ")), 0, 0);
      currentFileLabel = new QLabel;
    leftLayout->addWidget(currentFileLabel, 0,1);
    leftLayout->addWidget(openDataButton,0,2);
      plotter=new Plotter;
      plotter->setDevicePixedRatio(devicePixelRatio);
    leftLayout->addWidget(plotter,1,0,1,3);
    leftLayout->addWidget(new QLabel(tr("cos/sin (MHz)")),2,0,1,1);
    leftLayout->addWidget(cosSinLineEdit, 2,1,1,1);
    leftLayout->addWidget(cosSinMultiplyButtion,2,2,1,1);
    leftLayout->addWidget(new QLabel(tr("Cutoff (0-0.5)")),3,0,1,1);
    leftLayout->addWidget(digitalFilterLineEdit, 3,1,1,1);
    leftLayout->addWidget(applyDigitalFilterButtion,3,2,1,1);

    exportDataButton = new QPushButton(tr("Export"));
    leftLayout->addWidget(exportDataButton,4,2,1,1);
//    QVBoxLayout *rightLayout = new QVBoxLayout;
//    rightLayout->addStretch();
//    rightLayout->addWidget(exportDataButton);

    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setSpacing(2);
    mainLayout->setMargin(2);
    mainLayout->setContentsMargins(2,2,2,2);
    mainLayout->addLayout(leftLayout,1);
//    mainLayout->addLayout(rightLayout,0);


    inData.clear();

    cosSinMultiplyButtion->setEnabled(false);
    applyDigitalFilterButtion->setEnabled(false);
    exportDataButton->setEnabled(false);


    connect(openDataButton,SIGNAL(clicked()),this,SLOT(openData()));
    connect(cosSinMultiplyButtion,SIGNAL(clicked()),this,SLOT(cosSinMultiply()));
    connect(applyDigitalFilterButtion,SIGNAL(clicked()),this,SLOT(applyDigitalFilter()));
    connect(exportDataButton,SIGNAL(clicked()),this,SLOT(exportData()));

}

MainWindow::~MainWindow()
{
    QSettings settings(QApplication::applicationDirPath() + "/nmr.ini", QSettings::IniFormat);


    settings.beginGroup("MainWindow");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.endGroup();

    settings.beginGroup("Path");
//    settings.setValue("ppgSourceFilePath", ppg->ppgFilePath());
    settings.setValue("dataFilePath",dataFilePath);
    settings.setValue("exportFilePath",exportFilePath);
    settings.endGroup();

    inData.clear();
}


void MainWindow::openData()
{


    errorMessage.clear();

    QString path="~/";
    if(QDir(dataFilePath).exists()) {path=dataFilePath+'/';}
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Data"),
                                                    path,
                                                    tr("*.dat"));

    if (fileName.isEmpty()) {return;}
    dataFilePath=QFileInfo(fileName).absolutePath();

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
         errorMessage=QString(Q_FUNC_INFO)+ ": Failed to open " + fileName;
         return;
    }

    currentFileLabel->setText(fileName);
    QString source; source.clear();
    QTextStream in(&file);

    //bool ok;

    QString line;
    int al=1;
    double dw=1;

    inData.clear();

    if(!in.atEnd())
    {
      line=in.readLine().trimmed();
      al=line.toInt();
      // qDebug() << "al=" << al;
    }

    if(!in.atEnd())
    {
        line=in.readLine().trimmed();
        dw=line.toDouble()*1.0e6;
    }
    if(!in.atEnd()) {line=in.readLine();}
    if(!in.atEnd()) {line=in.readLine();}

    FID_2D.FID.clear();
    FID_2D.setAl(al);
    FID_2D.FID.append(new TFID(al));
    FID_2D.setDW(dw);
//    FID_2D.FID[0]->setDW(dw);

    for(int k=0; k<al; k++)
    {
        line = in.readLine().trimmed();
        if(line.contains(','))
        {
            int p=line.indexOf(',');
            line=line.mid(p+1);
        }
        FID_2D.FID[0]->real->sig[k]=line.toFloat();
        FID_2D.FID[0]->imag->sig[k]=0.0;
    }
    FID_2D.FID[0]->updateAbs();
    FID_2D.FID[0]->setEmpty(false);

    file.close();


    plotter->xini=0;
    plotter->xfin=FID_2D.al()-1;
    plotter->setScale(1/(FID_2D.FID[0]->abs->absMax()));

    plotter->setFID(FID_2D.FID[0]);
    plotter->refresh();

    cosSinMultiplyButtion->setEnabled(true);
    applyDigitalFilterButtion->setEnabled(false);
    exportDataButton->setEnabled(false);


    return;

}

void MainWindow::cosSinMultiply()
{
    bool ok;
    double freq=cosSinLineEdit->text().toDouble(&ok);

    if(!ok) return;

    int al=FID_2D.al();
    double dw=FID_2D.dw();
    FID_2D.setSF1(freq);

    FID_2D.FID.append(new TFID(al));
    for(int k=0; k<al;k++)
    {
        double twoPi=2.0*3.14159265358979;
        double c=cos(k*twoPi*dw*freq);
        double s=sin(k*twoPi*dw*freq);

        FID_2D.FID[1]->real->sig[k]=c*FID_2D.FID[0]->real->sig[k];
        FID_2D.FID[1]->imag->sig[k]=s*FID_2D.FID[0]->real->sig[k];

    }

    FID_2D.FID[1]->updateAbs();
    FID_2D.FID[1]->setEmpty(false);

    plotter->xini=0;
    plotter->xfin=FID_2D.al()-1;
    plotter->setScale(1/(FID_2D.FID[1]->abs->absMax()));

    plotter->setFID(FID_2D.FID[1]);
    plotter->refresh();

    cosSinMultiplyButtion->setEnabled(false);
    applyDigitalFilterButtion->setEnabled(true);
    exportDataButton->setEnabled(false);

}

void MainWindow::applyDigitalFilter()
{

    bool ok;
    double d=digitalFilterLineEdit->text().toDouble(&ok);

    if(!ok) return;
    if(d<=0) return;
    if(d>0.5) return;

    if(FID_2D.FID.size()<2) return;

    int al=FID_2D.FID.at(1)->al();
    //double dw=FID_2D.FID.at(1)->dw();

    TdigitalFilter *df = new TdigitalFilter(513);
    df->setCutOff(d);
    QVector<double> q;
    q.clear();

    for(int k=0; k<df->taps(); k++) q << df->coeff(k);


    FID_2D.FID.append(new TFID(al));
    for(int k=0; k<al;k++)
    {
        double re=0,im=0,fr=0,fi=0;
        for(int j=0; j<df->taps(); j++)
        {
            if((k-j)<0) {fr=0.0; fi=0.0;} else {fr=FID_2D.FID[1]->real->sig[k-j]; fi=FID_2D.FID[1]->imag->sig[k-j];}

            re += q.at(j)*fr; im += q.at(j)*fi;

        }

        FID_2D.FID[2]->real->sig[k]=re;
        FID_2D.FID[2]->imag->sig[k]=im;

    }

    FID_2D.FID[2]->updateAbs();
    FID_2D.FID[2]->setEmpty(false);

    plotter->xini=0;
    plotter->xfin=FID_2D.al()-1;
    plotter->setScale(1/(FID_2D.FID[2]->abs->absMax()));

    plotter->setFID(FID_2D.FID[2]);
    plotter->refresh();

    cosSinMultiplyButtion->setEnabled(false);
    applyDigitalFilterButtion->setEnabled(false);
    exportDataButton->setEnabled(true);
}

void MainWindow::exportData()
{

    errorMessage.clear();

    QString path="~/";
    if(QDir(exportFilePath).exists()) {path=exportFilePath+'/';}
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export Data"),
                                                    path,
                                                    tr("*.sm2d"));

    if(!FID_2D.Writesm2Files(fileName))
    {
        qDebug()<<FID_2D.errorMessage;
    }
    else
    { exportFilePath=path;}

}
