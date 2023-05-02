#include "processPanelWidget.h"


#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include "../fid.h"


TProcessFileWidget::TProcessFileWidget(QWidget *parent) : QWidget(parent)
{
    createWidgets();
    createPanel();
    createConnections();
    fidSetted=false;
    processSetted=false;

}

void TProcessFileWidget::createWidgets()
{
    openDataButton = new QPushButton(tr("Open"));
    openAndProcessCheckBox = new QCheckBox(tr("Apply process"));
//    openAndProcessButton = new QPushButton(tr("Open and Process"));
    saveDataButton = new QPushButton(tr("Save"));

    currentFileLineEdit = new QLineEdit;
    currentFileLineEdit->setReadOnly(true);

    infoTabWidget = new QTabWidget;
    infoTabWidget->setTabPosition(QTabWidget::North);
      parameterPlainTextEdit = new QPlainTextEdit;
      parameterPlainTextEdit->setReadOnly(true);
      ppgPlainTextEdit = new QPlainTextEdit;
      ppgPlainTextEdit->setReadOnly(true);
      nmrjobPlainTextEdit = new QPlainTextEdit;
      nmrjobPlainTextEdit->setReadOnly(true);
    infoTabWidget->addTab(parameterPlainTextEdit,tr("Params"));
    infoTabWidget->addTab(ppgPlainTextEdit,tr("ppg"));
    infoTabWidget->addTab(nmrjobPlainTextEdit,tr("nmrjob"));

    exportDataButton = new QPushButton(tr("Export ASCII Data"));
    plotterIDSpinBox = new QSpinBox;
    plotterIDSpinBox->setMinimum(0);
    plotterIDSpinBox->setMaximum(0);
    openProcessButton = new QPushButton(tr("Open Process"));
    openProcessAndApplyCheckBox = new QCheckBox(tr("Apply to data"));
    saveProcessButton = new QPushButton(tr("Save Process"));
}

void TProcessFileWidget::createPanel()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    QGroupBox *dataGroupBox = new QGroupBox(tr("Data"));
    QGroupBox *processGroupBox = new QGroupBox(tr("Process"));

    QGridLayout *gLayout1 = new QGridLayout;
    gLayout1->addWidget(openDataButton,0,0,1,1);
    gLayout1->addWidget(openAndProcessCheckBox,0,1,1,1);
    gLayout1->addWidget(saveDataButton,0,2,1,1);
    gLayout1->addWidget(currentFileLineEdit,1,0,1,3);
    gLayout1->addWidget(infoTabWidget,2,0,1,3);
//    gLayout1->addWidget(parameterPlainTextEdit,2,0,1,3);
    QGridLayout *gLayout2 = new QGridLayout;
    gLayout2->addWidget(openProcessButton,0,0,1,1);
    gLayout2->addWidget(openProcessAndApplyCheckBox,0,1,1,1);
    gLayout2->addWidget(saveProcessButton,1,0,1,1);

    dataGroupBox->setLayout(gLayout1);
    processGroupBox->setLayout(gLayout2);

    mainLayout->addWidget(dataGroupBox);
    mainLayout->addWidget(processGroupBox);
    mainLayout->addStretch();
    setLayout(mainLayout);
}

void TProcessFileWidget::createConnections()
{
    connect(openDataButton,SIGNAL(clicked()),this,SLOT(openFile()));
    connect(saveDataButton,SIGNAL(clicked()),this,SLOT(saveFile()));
}

void TProcessFileWidget::setNOfPlotters(int n)
{

    plotterIDSpinBox->setMaximum(n);
}

void TProcessPanelWidget::importProcess()
{
    QString path="~/";

    if(QDir(processFilePath()).exists()) path=processFilePath()+'/';
    QString fileName = QFileDialog::getOpenFileName(this, tr("Import process settings"),
                                                    path,
                                                    tr("Process setting file (*.process)"),
                                                    nullptr

                                                    );
    if(fileName.isNull()) return;
    if (fileName.isEmpty()) {return;}
    setProcessFilePath(QFileInfo(fileName).absolutePath());

//    QFileInfo fi;
//    fi.setFile(fileName);
//    QString base = fi.baseName();
//    path = fi.absolutePath()+'/';
//    QString newFileName = path+base+".process";

    if(!processOperations->loadFromFile(fileName))
    {
        QMessageBox::warning(this, tr("Import error"),
                             tr("Failed to import process."
                              ));
        return;
    }

    updateProcessSettings();
    if(processFileWidget->openProcessAndApplyCheckBox->isChecked())
    {
        applyProcess();
    }
}

void TProcessPanelWidget::exportProcess()
{
//    if(!QFile::exists(settingDirPath()+"/process.ini"))
    if(processOperations->processElements.isEmpty())
    {
        QMessageBox::warning(this, tr("Export error"),
                             tr("Process settings are empty."
                              ));
       return;
    }

    QString path="~/";

    if(QDir(processFilePath()).exists()) path=processFilePath()+'/';
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export process settings"),
                                                    path,
                                                    tr("Process setting file (*.process)"),
                                                    nullptr
                                                   //, QFileDialog::DontConfirmOverwrite
                                                    );
    if(fileName.isNull()) return;
    if (fileName.isEmpty()) {return;}
    setProcessFilePath(QFileInfo(fileName).absolutePath());

    QFileInfo fi;
    fi.setFile(fileName);
    QString base = fi.completeBaseName();
    path = fi.absolutePath()+'/';
    QString newFileName = path+base+".process";
    if(QFile::exists(newFileName))
    {
       //if(QMessageBox::No == QMessageBox::question(this, "Override Confirmation?",
       //                                 "File already exists. <p>Do you want to override?",
       //                                  QMessageBox::No|QMessageBox::Yes)) return;
       //else
       QFile::remove(newFileName);

    }

    if(!processOperations->saveToFile(newFileName))
   // if(!QFile::copy(settingDirPath()+"/process.ini",newFileName))
    {
        QMessageBox::warning(this, tr("Export error"),
                             tr("Failed to export process settings."
                              ));

       return;
    }
    return;
}

void TProcessFileWidget::saveFile()
{
    if(FID_2D->FID.isEmpty()) return;

    QString path="~/";
    if(QDir(dataFilePath()).exists()) path=dataFilePath()+'/';
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save data"),
                                                    path,
                                                    tr("Opencore files (*.opp *.opd *.sm2p *.sm2d)"));
    if (fileName.isEmpty()) {return;}

    bool ok;

    QFileInfo fi(fileName);
    QString ext = fi.suffix();

    if(0==QString::compare(ext,"opp") || 0==QString::compare(ext,"opd"))
    {
      ok=FID_2D->WriteopFiles(fileName);
    }
    else if(0==QString::compare(ext,"sm2p") || 0==QString::compare(ext,"sm2d"))
    {
      ok=FID_2D->Writesm2Files(fileName);
    }
    else if(0==QString::compare(ext,"smd"))
    {
      ok=FID_2D->FID.at(FID_2D->currentFID())->WritesmdFile(fileName);
    }
    else
    {
      ok=FID_2D->WriteopFiles(fileName);
    }

    if(!ok)
    {
        QMessageBox::warning(this,tr(""),
                           tr("<p>Failed to save data. "
                              ));
        return;
    }


    return;
}


void TProcessFileWidget::openFileAndProcess()
{    
    QString path="~/";
    if(QDir(dataFilePath()).exists()) path=dataFilePath()+'/';
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open data"),
                                                    path,
                                                    tr("Opencore files (*.opp *.opd *.sm2p *.sm2d)"));
    if (fileName.isEmpty()) {return;}

    setDataFilePath(QFileInfo(fileName).absolutePath());
    QString fileExt=QFileInfo(fileName).suffix();

    if(0==QString::compare(fileExt,"sm2p") || 0==QString::compare(fileExt,"sm2d"))
    {
      if(!FID_2D->Readsm2Files(fileName))
      {
        //qDebug()<<FID_2D->errorMessage;
        QMessageBox::warning(this,QString(Q_FUNC_INFO)+tr(""), FID_2D->errorMessage);
        return;
      }
    }
    else if(0==QString::compare(fileExt,"opp") || 0==QString::compare(fileExt,"opd"))
    {
      if(!FID_2D->ReadopFiles(fileName))
      {
                    //qDebug()<<FID_2D->errorMessage;
         QMessageBox::warning(this,QString(Q_FUNC_INFO)+tr(""), FID_2D->errorMessage);
         return;
      }
    }
    else
    {
        QMessageBox::warning(this,tr(""), "." + fileExt + " is not supported.");
        return;
    }


    currentFileLineEdit->setText(fileName);
    parameterPlainTextEdit->clear();
    parameterPlainTextEdit->setPlainText(FID_2D->parameters.join("\n")
                                         +"\n"+
                                         FID_2D->comments.join("\n"));

    fidSetted=true;

    emit initializeRequest();
    emit applyProcessRequest();
}

void TProcessFileWidget::openFile()
{

    QString path="~/";
    if(QDir(dataFilePath()).exists()) path=dataFilePath()+'/';
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open data"),
                                                    path,
                                                    tr("Opencore files (*.opp *.opd *.sm2p *.sm2d *.smd *.jdf)"));
    if (fileName.isEmpty()) {return;}

    setDataFilePath(QFileInfo(fileName).absolutePath());
    QString fileExt=QFileInfo(fileName).suffix();


    if(0==QString::compare(fileExt,"sm2p") || 0==QString::compare(fileExt,"sm2d"))
    {
      if(!FID_2D->Readsm2Files(fileName))
      {
        //qDebug()<<FID_2D->errorMessage;
        QMessageBox::warning(this,QString(Q_FUNC_INFO)+tr(""), FID_2D->errorMessage);
        return;
      }
    }
    else if(0==QString::compare(fileExt,"opp") || 0==QString::compare(fileExt,"opd"))
    {
      if(!FID_2D->ReadopFiles(fileName))
      {
                    //qDebug()<<FID_2D->errorMessage;
         QMessageBox::warning(this,QString(Q_FUNC_INFO)+tr(""), FID_2D->errorMessage);
         return;
      }
    }
    else if(0==QString::compare(fileExt,"smd"))
    {
        if(!FID_2D->ReadsmdFile(fileName))
        {
                      //qDebug()<<FID_2D->errorMessage;
           QMessageBox::warning(this,QString(Q_FUNC_INFO)+tr(""), FID_2D->errorMessage);
           return;
        }
    }
    else if(0==QString::compare(fileExt,"jdf"))
    {
        if(!FID_2D->ReadjdfFile(fileName))
        {
                      //qDebug()<<FID_2D->errorMessage;
           QMessageBox::warning(this,QString(Q_FUNC_INFO)+tr(""), FID_2D->errorMessage);
           return;
        }
    }
    else
    {
        QMessageBox::warning(this,tr(""), "." + fileExt + " is not supported.");
        return;
    }


    currentFileLineEdit->setText(fileName);
    parameterPlainTextEdit->clear();
    parameterPlainTextEdit->setPlainText(FID_2D->parameters.join("\n")
                                         +"\n"+
                                         FID_2D->comments.join("\n"));

    nmrjobPlainTextEdit->clear();
    ppgPlainTextEdit->clear();

    QString nmrjobFilePath = QFileInfo(fileName).absolutePath()
                           + "/"
                           + QFileInfo(fileName).completeBaseName()+".nmrjob";

    if(QFileInfo::exists(nmrjobFilePath))
    {
        QFile nj(nmrjobFilePath);
        if(nj.open(QFile::ReadOnly | QFile::Text))
        {
          nmrjobPlainTextEdit->setPlainText(nj.readAll());
          nj.close();
        }

        QSettings settings(nmrjobFilePath, QSettings::IniFormat);
        settings.beginGroup("File");
          QString ppgFileName = QFileInfo(fileName).absolutePath()
                  + "/"
                  + settings.value("ppgFilename").toString();
          QFile pg(ppgFileName);
          if(pg.open(QFile::ReadOnly | QFile::Text))
          {
            ppgPlainTextEdit->setPlainText(pg.readAll());
            pg.close();
          }


        settings.endGroup();

    }


    fidSetted=true;

 //   emit initializeRequest();

    if(openAndProcessCheckBox->isChecked())
    {
       emit applyProcessRequest();
    }
    else
    {
       emit initializeRequest();
       emit clearProcessRequest();
    }


}

void TProcessFileWidget::exportASCIIData()
{
    if(!fidSetted) return;
    QString path="~/";
    if(QDir(dataFilePath()).exists()) path=dataFilePath()+'/';

    QString fileName = QFileDialog::getSaveFileName(this, tr("Export ascii data"),
                                                    path
                                                    );
    if(fileName.isNull()) return;
    if (fileName.isEmpty()) {return;}



}

TProcessPanelWidget::TProcessPanelWidget(QWidget *parent) : QWidget(parent)
{
    setSettingDirPath(QDir::homePath()+"/.opencorenmr");

    FID_2D = new TFID_2D;
    FID_2D->setDefaultAl(2);
    currentNMRData = new TFID_2D;
    processOperations=new TProcessOperations;
    processSettings=new QSettings;

    setIsFirstTime(true);

    createWidgets();
    createPanel();
    createConnections();

    stackedWidget->setCurrentIndex(0);


}

TProcessPanelWidget::~TProcessPanelWidget()
{
    QString qs=settingDirPath()+"/process.ini";
    if(QFile::exists(qs)){QFile::remove(qs);}
    delete processSettings;
    delete processOperations;
    delete plotters;

}

void TProcessPanelWidget::setSettingDirPath(QString qs)
{
    FSettingDirPath=qs;
}

void TProcessPanelWidget::resetProcessSettings()
{
    //qDebug() << QString(Q_FUNC_INFO);
    QString qs=settingDirPath()+"/process.ini";
    if(QFile::exists(qs)){QFile::remove(qs);}
    delete processSettings;
    processSettings = new QSettings(settingDirPath() + "/process.ini", QSettings::IniFormat);
    processSettings->sync();
}

void TProcessPanelWidget::setDevicePixelRatio(int r)
{
    FDevicePixelRatio=r;
    plotters->setDevicePixelRatio(r);
}

void TProcessPanelWidget::createWidgets()
{
    splitter0=new QSplitter(Qt::Vertical);
    splitter0->setHandleWidth(1);
    splitter1 = new QSplitter(Qt::Horizontal);
    splitter1->setHandleWidth(1);

    plotters = new TFIDPlotters;
    plotters->setFFTEnabled(false);
    plotters->setDevicePixelRatio(devicePixelRatio());
    plotters->setBackgroundColor0(QColor("white"));
    plotters->setBackgroundColor1(QColor("skyblue"));
    plotters->setRealColor(QColor("deeppink"));
    plotters->setImagColor(QColor("darkgreen"));
    plotters->setAbsColor(QColor("blue"));
    plotters->setPolarColor(QColor("darkorange"));


    plotters->show();
    plotters->setFID2D(FID_2D);

    operationListWidget = new QListWidget;
    operationListWidget->setMinimumWidth(100);
      //operationListWidget->setFixedWidth(100);
      operationListWidget->addItems(QStringList()
                                    << "File"
                                    << "Image (eps)"
                                    << "Cut/Add"
                                    << "Apodization"
                                    << "Transform"
                                    << "Phase"
                                    << "Axis Format"
                                    << "Array Analysis"
                                    << "Array"
                                    << "Covariance"
                                    << "exportData"
                                    << "exportAbsData"
                                    << "Create FID"
                                    << "Math"
                                    << "Peak"
                                    << "Cartesian Map (experimental)"
                                    << "3Drearrangement"
                                    );

      // Return Values (Array included)
      // View (plotter manipulation)

    commandTextEdit = new QPlainTextEdit;
      //commandTextEdit->setFixedHeight(100);

   // console = new Console;
    commandHistoryListWidget = new QListWidget;
    stackedWidget = new QStackedWidget;
    processFileWidget=new TProcessFileWidget;
      processFileWidget->setFID2D(FID_2D);

    apodizationWidget = new TApodizationWidget;
      apodizationWidget->setAncestor(this);
      //apodizationWidget->setApplyToSelectable(true);

    transformWidget=new TTransformWidget;
      transformWidget->setAncestor(this);

      //transformWidget->setApplyToSelectable(true);
    axisFormatWidget=new TAxisFormatWidget;
      axisFormatWidget->setAncestor(this);

      //axisFormatWidget->setApplyToSelectable(true);
    phaseWidget = new TPhaseWidget;
      phaseWidget->setAncestor(this);
      //phaseWidget->setApplyToSelectable(true);
    addCutPointsWidget = new TAddCutPointsWidget;
      //addCutPointsWidget->setFID2D(FID_2D);
      addCutPointsWidget->setAncestor(this);
      //addCutPointsWidget->setApplyToSelectable(false);
    twoDProcessWidget = new T2DProcessWidget;
      twoDProcessWidget->setAncestor(this);
      //twoDProcessWidget->setApplyToSelectable(false);

#if defined(__APPLE__) || defined(__linux__) || defined(_WIN32)
   CovarianceWidget = new KCovarianceWidget;
   CovarianceWidget->setAncestor(this);
#endif
      //CovarianceWidget->setApplyToSelectable(false);
    exportWidget = new KExportWidget;
      exportWidget->setAncestor(this);

    exportAbsWidget = new SExportAbsWidget;
      exportAbsWidget->setAncestor(this);

    NutationWidget = new KNutationWidget;
      NutationWidget->setAncestor(this);

    createFIDWidget = new TCreateFIDWidget;
      createFIDWidget->setFID2D(FID_2D);

    FIDMathWidget = new TFIDMathWidget;
      FIDMathWidget->setAncestor(this);

    peakPickWidget = new KPeakPickWidget;
      peakPickWidget->setAncestor(this);

    NutationWidget = new KNutationWidget;
      NutationWidget->setAncestor(this);

//    interpolateWidget = new KInterpolateWidget;
//      interpolateWidget->setAncestor(this);

    imageGenWidget = new TImageGenWidget;
      imageGenWidget->setAncestor(this);

    cartesianMapWidget = new TCartesianMapWidget;
      cartesianMapWidget->setAncestor(this);

    threeDrearrangementWidget = new S3DrearrangementWidget;
        threeDrearrangementWidget->setAncestor(this);
}

void TProcessPanelWidget::createPanel()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
//    setFixedWidth(400); setFixedHeight(400);

    stackedWidget->addWidget(processFileWidget);
    stackedWidget->addWidget(imageGenWidget);
    stackedWidget->addWidget(addCutPointsWidget);
    stackedWidget->addWidget(apodizationWidget);
    stackedWidget->addWidget(transformWidget);
    stackedWidget->addWidget(phaseWidget);
    stackedWidget->addWidget(axisFormatWidget);
    stackedWidget->addWidget(NutationWidget);
    stackedWidget->addWidget(twoDProcessWidget);
    stackedWidget->addWidget(CovarianceWidget);
    stackedWidget->addWidget(exportWidget);
    stackedWidget->addWidget(exportAbsWidget);
    stackedWidget->addWidget(createFIDWidget);
    stackedWidget->addWidget(FIDMathWidget);
    stackedWidget->addWidget(peakPickWidget);
//    stackedWidget->addWidget(interpolateWidget);
    stackedWidget->addWidget(cartesianMapWidget);
    stackedWidget->addWidget(threeDrearrangementWidget);


//    QHBoxLayout *layout1 = new QHBoxLayout;
    //layout1->addWidget(new QLabel(tr("Operation")));
    splitter1->addWidget(operationListWidget);
    splitter1->addWidget(stackedWidget);



    splitter0->addWidget(splitter1);
    splitter0->addWidget(commandHistoryListWidget);
    mainLayout->addWidget(splitter0);
   // mainLayout->addWidget(commandTextEdit);
   // mainLayout->addWidget(console);

    setLayout(mainLayout);

}

void TProcessPanelWidget::createConnections()
{

    connect(cartesianMapWidget,SIGNAL(isCartesianMapIdle(bool)),stackedWidget,SLOT(setEnabled(bool)));
    connect(operationListWidget,SIGNAL(currentRowChanged(int)),stackedWidget,SLOT(setCurrentIndex(int)));
      operationListWidget->setCurrentRow(0);

    connect(createFIDWidget,SIGNAL(clearProcessRequest()),this,SLOT(clearProcess()));
    connect(processFileWidget,SIGNAL(clearProcessRequest()),this,SLOT(clearProcess()));
    connect(processFileWidget,SIGNAL(initializeRequest()),this,SLOT(initialize()));
    connect(processFileWidget->saveProcessButton,SIGNAL(clicked(bool)),this,SLOT(exportProcess()));
    connect(processFileWidget,SIGNAL(applyProcessRequest()),this,SLOT(applyProcess()));
    connect(processFileWidget->openProcessButton,SIGNAL(clicked(bool)),this,SLOT(importProcess()));
    connect(plotters,SIGNAL(numberOfPlottersUpdated(int)),processFileWidget,SLOT(setNOfPlotters(int)));
    connect(plotters,SIGNAL(numberOfPlottersUpdated(int)),this,SLOT(updateNumberOfPlotters(int)));

    connect(createFIDWidget,SIGNAL(updateRequest()),this,SLOT(onFIDCreated()));

    connect(transformWidget,SIGNAL(vOffsetRequest(double)),this,SLOT(onVOffsetRequestReceived(double)));
}
void TProcessPanelWidget::applyProcess()
{
    if(FID_2D->FID.isEmpty())
    {
        QMessageBox::warning(this,tr("process error"), "Data is empty.");
        return;
    }
    if(processOperations->processElements.isEmpty())
    {
        QMessageBox::warning(this,tr("process error"), "Process is empty.");
        return;
    }

    if(isFirstTime())
    {
        initializePlotter();
        setIsFirstTime(false);
        updatePlotter();
    }

    for(int k=0; k<processOperations->processElements.size(); k++)
    {
        bool ok=processOperations->processElements[k]->process(FID_2D);
        if(!ok)
        {
            QMessageBox::warning(this,tr("process error"), processOperations->processElements[k]->errorMessage());
            return;
        }


        //TODO: widget update

        switch(processOperations->processElements.at(k)->processType())
        {
        case TProcessElement::CutAdd:
          break;
        case TProcessElement::Apodization:
          break;
        case TProcessElement::Phase:
          phaseWidget->breakConnections();
          phaseWidget->phase0ValueDoubleSpinBox->setValue(processOperations->processElements.at(k)->accumPhase0());
          phaseWidget->phase1ValueDoubleSpinBox->setValue(processOperations->processElements.at(k)->accumPhase1());
          phaseWidget->phasePivotSpinBox->setValue(processOperations->processElements.at(k)->pivot());
          phaseWidget->createConnections();

          // We set the values of the accumulated phase
          // At this moment, the initial phase is set to the identical value.
          phaseWidget->phaseRotation->setAccumPhase0(
                      processOperations->processElements.at(k)->accumPhase0());
          phaseWidget->phaseRotation->setInitialPhase0(
                      phaseWidget->phaseRotation->accumPhase0());
          phaseWidget->phaseRotation->setAccumPhase1(
                      processOperations->processElements.at(k)->accumPhase1());
          phaseWidget->phaseRotation->setInitialPhase1(
                      phaseWidget->phaseRotation->accumPhase1());

          break;
        case TProcessElement::FFT:
          transformWidget-> emit vOffsetRequest(0.1);
          break;
        case TProcessElement::IFFT:
          transformWidget->emit vOffsetRequest(0.5);
          break;
        case TProcessElement::AxisStyle:
          if(processOperations->processElements.at(k)->domain()==TAxisStyle::TimeDomain)
            axisFormatWidget->domainComboBox->setCurrentIndex(0);
          else if (processOperations->processElements.at(k)->domain()==TAxisStyle::FrequencyDomain)
            axisFormatWidget->domainComboBox->setCurrentIndex(1);
          else axisFormatWidget->domainComboBox->setCurrentIndex(2);
          axisFormatWidget->setDomain();

          axisFormatWidget->setUnitComboBox(processOperations->processElements.at(k)->unit());

          axisFormatWidget->axisLabelLineEdit->setText(
                      processOperations->processElements.at(k)->label()
                      );

          axisFormatWidget->setReferenceSpinBox->setValue(
                      processOperations->processElements.at(k)->referencePoint()
                      );

          axisFormatWidget->referenceValueLineEdit->setText(
                      QString::number(processOperations->processElements.at(k)->referenceValue())
                      );
          axisFormatWidget->setUnit();


          break;
        case TProcessElement::Transpose: break;
        case TProcessElement::ArraySum: break;
        case TProcessElement::Flatten: break;
        case TProcessElement::Reshape: break;

        case TProcessElement::CartesianMap3D:

          break;

        case TProcessElement::FFT3D:

          break;

        default:
          break;
        }

    }

//    refresh();
    updatePlotter();
}

void TProcessPanelWidget::onVOffsetRequestReceived(double vo)
{
    for(int k=0; k<plotters->FIDPlotters.size(); k++)
    {
        plotters->FIDPlotters[k]->plotterDetails->vOffsetSpinBox->setValue(vo);

    }


}
void TProcessPanelWidget::updateNumberOfPlotters(int i)
{
    apodizationWidget->applyModeWidget->currentPlotterSpinBox->setMaximum(i-1);
    transformWidget->applyModeWidget->currentPlotterSpinBox->setMaximum(i-1);

    imageGenWidget->plotterIDSpinBox->setMaximum(i-1);
}

void TProcessPanelWidget::clearProcess()
{
    //processOperations->clear();
    commandHistoryListWidget->clear();
    while(!processOperations->processElements.isEmpty()) processOperations->processElements.removeLast();
    resetProcessSettings();

}


void TProcessPanelWidget::initializePlotter()
{
// qDebug() << QString(Q_FUNC_INFO) << plotters->FIDPlotters.size();

    for(int k=0; k<plotters->FIDPlotters.size(); k++)
    {
        plotters->FIDPlotters[k]->plotter->xini=0;
        plotters->FIDPlotters[k]->plotter->xfin=FID_2D->defaultAl()-1;
        plotters->FIDPlotters[k]->plotter->setScale(1/FID_2D->FID[0]->abs->absMax());
        plotters->FIDPlotters[k]->FIDSelectSpinBox->setMinimum(1);
        plotters->FIDPlotters[k]->FIDSelectSpinBox->setMaximum(FID_2D->FID.size());

        plotters->FIDPlotters[k]->plotterDetails->vOffsetSpinBox->setValue(0.5);

    }


// plotters->update();  // commented out 12 Mar 2019 (KT)

}

void TProcessPanelWidget::updatePlotter()
{
//    qDebug() << QString(Q_FUNC_INFO) << FID_2D->al();
    for(int k=0; k<plotters->FIDPlotters.size(); k++)
    {
        if(plotters->FIDPlotters[k]->plotter->xini>FID_2D->defaultAl()-1) plotters->FIDPlotters[k]->plotter->xini=0;
        if(plotters->FIDPlotters[k]->plotter->xfin>FID_2D->defaultAl()-1) plotters->FIDPlotters[k]->plotter->xfin=FID_2D->defaultAl()-1;
        //plotters->FIDPlotters[k]->plotter->setScale(1/FID_2D->FID[0]->abs->absMax());

        plotters->FIDPlotters[k]->FIDSelectSpinBox->setMinimum(1);
        plotters->FIDPlotters[k]->FIDSelectSpinBox->setMaximum(FID_2D->FID.size());

        plotters->FIDPlotters[k]->setPath(processFileWidget->dataFilePath());
    }
    plotters->update();

}

void TProcessPanelWidget::onFIDCreated()
{
    initialize();
    processFileWidget->currentFileLineEdit->clear();
}


void TProcessPanelWidget::initialize()
{
    exportWidget->setDataFilePath(processFileWidget->dataFilePath());
    initializePlotter();

/*
    if(isFirstTime())
    {
        initializePlotter();
        setIsFirstTime(false);
        updatePlotter();
    }
    else
    {
        updatePlotter();
    }
*/

    axisFormatWidget->domainComboBox->setCurrentIndex(0);
    axisFormatWidget->axisStyle->setDomain("time");
    axisFormatWidget->init();
    axisFormatWidget->axisLabelLineEdit->clear();
    axisFormatWidget->refresh();
    // Before resetting the phase widget we break connections tentatively.
    // Otherwise, the unintended phasing is performed to the data.
    // (17 June 2020 K. Takeda)
    phaseWidget->breakConnections();
      phaseWidget->reset();
      phaseWidget->setFID2D(FID_2D);
      phaseWidget->phase0ValueDoubleSpinBox->setValue(0);
      phaseWidget->phase1ValueDoubleSpinBox->setValue(0);
      phaseWidget->phasePivotCheckBox->setEnabled(true);
      phaseWidget->phasePivotCheckBox->setChecked(false);
    phaseWidget->createConnections(); // We resume the connection

    phaseWidget->phaseRotation->resetInitialPhase();

}


/*

void TProcessPanelWidget::refresh()
{
  //  updatePlotter();
    if(!FID_2D->FID.isEmpty())
    {
      phaseWidget->phasePivotSpinBox->setMaximum(FID_2D->defaultAl()-1);
      // mathwidget spinedit update?
      FIDMathWidget->xIniSpinBox->setMaximum(FID_2D->defaultAl()-1);
      FIDMathWidget->xFinSpinBox->setMaximum(FID_2D->defaultAl()-1);
    }

}
*/



//
//-----------------------------------------------------------------------------
//

//
//-----------------------------------------------------------------------------
//
void TProcessPanelWidget::updateProcessSettings()
{
    commandHistoryListWidget->clear();
    for(int k=0; k<processOperations->processElements.size(); k++)
    {
        commandHistoryListWidget->addItem(
                    QString::number(k+1) + ": " +
                    processOperations->processElements.at(k)->command()
                    );
    }
    processSettings->beginGroup("main");
      processSettings->setValue("numberOfOperations",processOperations->processElements.size());
    processSettings->endGroup();
    processSettings->sync();
    processSettings->beginGroup(QString::number(processOperations->processElements.size()-1));
      processSettings->setValue("operation",processOperations->processElements.last()->command());
    processSettings->endGroup();
    processSettings->sync();
}
