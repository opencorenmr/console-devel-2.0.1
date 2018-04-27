//#include <QtGui>
#include <QProgressDialog>
#include <QDebug>

#include <QStatusBar>
#include <QDateTime>

#include "mainwindow.h"
#include "codeeditor.h"
#include "highlighter.h"
#include "inifile.h"
#include "gates.h"
#include "pulseProgram.h"
#include "plotter.h"


TwelcomeWidget::TwelcomeWidget(QWidget *parent) :
    QWidget(parent)
{
    path0=QDir::homePath()+"/.opencorenmr";

    builtInJobList.clear(); recentJobList.clear(); favoriteJobList.clear();

    QHBoxLayout *layout0 = new QHBoxLayout; setLayout(layout0);
      QVBoxLayout *layout1 = new QVBoxLayout;
        topButton = new QPushButton(tr("About"));
        pulseProgramButton = new QPushButton(tr("Pulse Programs"));
        jobButton = new QPushButton(tr("Jobs"));
        layout1->addWidget(jobButton);
        layout1->addWidget(pulseProgramButton);
        layout1->addStretch();
        layout1->addWidget(topButton);

      layout0->addLayout(layout1);
      stackedLayout = new QStackedLayout(layout0);

    QWidget *topWidget = new QWidget;
    QVBoxLayout *topLayout1 = new QVBoxLayout(topWidget);
    QHBoxLayout *topLayout2 = new QHBoxLayout;
    topLayout1->addLayout(topLayout2);
    topLayout2->addStretch();
    topLayout2->addWidget(new QLabel(tr("Opencore NMR Ver ")
                                     + QString::number(VERSION_NUMBER)
                                     + tr(", Build #")+QString::number(BUILD_NUMBER)
                                     + ", "
                                     + "written by Kazuyuki Takeda ("
                                     + QString::number(RELEASE_YEAR)
                                     + ")"
                                     )
                          );

    //topLayout2->addWidget(new QLabel(tr("http://kuchem.kyoto-u.ac.jp/bun/indiv/takezo/opencorenmr2/index.html")
    //                                 )
    //                      );

    topLayout2->addStretch();

    QWidget *ppgWidget = new QWidget;
    QHBoxLayout *ppgLayout0 = new QHBoxLayout(ppgWidget);
    QVBoxLayout *ppgLayout1 = new QVBoxLayout;
      ppgListWidget = new QListWidget;
      ppgListWidget->setMaximumWidth(200);
      ppgLayout1->addWidget(new QLabel(tr("Built-in pulse programs")));
      ppgLayout1->addWidget(ppgListWidget);
      openWithPPGEditButton = new QPushButton(tr("Open"));
      ppgLayout1->addWidget(openWithPPGEditButton);


    //  QMessageBox::about(this,"",path0 + "/examples/builtInPulsePrograms.ini");


      QSettings settings(path0 + "/examples/builtInPulsePrograms.ini", QSettings::IniFormat);
      settings.beginGroup("main");
        int nOfPPG = settings.value("ppgs",0).toInt();
        ppgListWidget->clear();
        ppgList.clear();
        ppgPathList.clear();
        ppgCaptionList.clear();
      for (int k=0; k<nOfPPG; k++)
      {
          ppgList.append(settings.value("ppg_"+QString::number(k),"").toString());
      }
      settings.endGroup();
      ppgListWidget->addItems(ppgList);

      for(int k=0; k<nOfPPG; k++)
      {
        settings.beginGroup(ppgList.at(k));
          ppgPathList.append(path0 + "/examples/" + settings.value("path","").toString());
          ppgCaptionList.append(settings.value("caption","").toString());
        settings.endGroup();
      }

      QSettings jobSettings(path0 + "/examples/builtInJobs.ini", QSettings::IniFormat);
      jobSettings.beginGroup("main");
        int nOfJobs = jobSettings.value("jobs",0).toInt();
        //qDebug() << nOfJobs;
        builtInJobList.clear();
      for (int k=0; k<nOfJobs; k++)
      {
          builtInJobList.append(path0 +
                                 + "/examples/nmrJobs/" + jobSettings.value("job_"+QString::number(k),"").toString()
                                );
      }
      jobSettings.endGroup();

     // qDebug() << builtInJobList;


    QVBoxLayout *ppgLayout2 = new QVBoxLayout;
      ppgPlainTextEdit = new CodeEditor;
      ppgCaptionPlainTextEdit = new QPlainTextEdit; ppgCaptionPlainTextEdit->setMaximumHeight(50);
      ppgPathPlainTextEdit = new QPlainTextEdit; ppgPathPlainTextEdit->setMaximumHeight(50);
      ppgPlainTextEdit->clear();
      ppgPlainTextEdit->setReadOnly(true);
        highlighter = new Highlighter(ppgPlainTextEdit->document());

      ppgCaptionPlainTextEdit->clear();
      ppgCaptionPlainTextEdit->setReadOnly(true);
      ppgPathPlainTextEdit->clear();
      ppgPathPlainTextEdit->setReadOnly(true);
      ppgLayout2->addWidget(new QLabel(tr("Path")));
      ppgLayout2->addWidget(ppgPathPlainTextEdit);
      ppgLayout2->addWidget(new QLabel(tr("Description")));
      ppgLayout2->addWidget(ppgCaptionPlainTextEdit);
      ppgLayout2->addWidget(new QLabel(tr("Source Code")));
      ppgLayout2->addWidget(ppgPlainTextEdit);
    ppgLayout0->addLayout(ppgLayout1);
    ppgLayout0->addLayout(ppgLayout2);


      //pulseProgramTabWidget = new QTabWidget;
      //pulseProgramTabWidget->addTab(builtInPPGListWidget,tr("Built-in pulse programs"));
      //pulseProgramTabWidget->addTab(new QWidget, tr("Add-on pulse programs (under dev.)"));
    //ppgLayout->addWidget(pulseProgramTabWidget);
 //   ppgLayout->addWidget(builtInPPGListWidget);


    QWidget *jobWidget = new QWidget;
    QHBoxLayout *jobLayout0 = new QHBoxLayout(jobWidget);
    QVBoxLayout *jobLayout1 = new QVBoxLayout;

      jobCategoryCombobox = new QComboBox;
        jobCategoryCombobox->addItems(QStringList()<<"Built-in Jobs"<<"Recent Jobs"<<"Favorite Jobs");
      welcomeJobListWidget = new QListWidget; welcomeJobListWidget->setMaximumWidth(200);
      jobLayout1->addWidget(jobCategoryCombobox);
//      jobLayout1->addWidget(new QLabel(tr("Recent Jobs")));
      jobLayout1->addWidget(welcomeJobListWidget);
      editJobButton = new QPushButton(tr("Edit Job"));
      setupJobButton = new QPushButton(tr("Setup Job"));
      clearJobButton = new QPushButton(tr("Clear list"));
      showFavoriteJobWidgetButton = new QPushButton(tr("Edit Favorites"));
      jobLayout1->addWidget(editJobButton);
      jobLayout1->addWidget(setupJobButton);
//      jobLayout1->addWidget(showFavoriteJobWidgetButton);
//      jobLayout1->addWidget(clearJobButton);


    QVBoxLayout *jobLayout2 = new QVBoxLayout;
      jobPathPlainTextEdit = new QPlainTextEdit; jobPathPlainTextEdit->setMaximumHeight(50);
      jobPathPlainTextEdit->setReadOnly(true);
      jobPlainTextEdit = new QPlainTextEdit;
      jobPlainTextEdit->setReadOnly(true);
      jobLayout2->addWidget(new QLabel(tr("Path")));
      jobLayout2->addWidget(jobPathPlainTextEdit);
      jobLayout2->addWidget(new QLabel(tr("Description")));
      jobLayout2->addWidget(jobPlainTextEdit);

    jobLayout0->addLayout(jobLayout1);
    jobLayout0->addLayout(jobLayout2);

    stackedLayout->addWidget(topWidget);
    stackedLayout->addWidget(ppgWidget);
    stackedLayout->addWidget(jobWidget);

    setLayout(layout0);

    connect(topButton,SIGNAL(clicked()),this,SLOT(setAbout()));
    connect(pulseProgramButton,SIGNAL(clicked()),this,SLOT(setPulseProgram()));
    connect(jobButton,SIGNAL(clicked()),this,SLOT(setJob()));
    connect(jobCategoryCombobox,SIGNAL(currentIndexChanged(int)),this,SLOT(setJobCategoty()));
    connect(welcomeJobListWidget,SIGNAL(currentRowChanged(int)),this,SLOT(displayJob(int)));

    connect(ppgListWidget,SIGNAL(currentRowChanged(int)),this,SLOT(displayPulseProgram(int)));

    connect(ppgListWidget,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(onOpenWithPPGEditButtonClicked()));
    connect(openWithPPGEditButton,SIGNAL(clicked()),this,SLOT(onOpenWithPPGEditButtonClicked()));
                //void onOpenWithPPGEditButtonClicked();

    setAbout();
    jobCategoryCombobox->setCurrentIndex(0);
    setJobCategoty();

}

void TwelcomeWidget::onOpenWithPPGEditButtonClicked()
{
    if(ppgListWidget->currentRow()<0) return;
    emit openPPGRequest(ppgPathList.at(ppgListWidget->currentRow()));
}


TwelcomeWidget::~TwelcomeWidget()
{

}


void TwelcomeWidget::setJobCategoty()
{
  int i = jobCategoryCombobox->currentIndex();
  if(i<0) return;
  welcomeJobListWidget->clear();
  jobPathPlainTextEdit->clear();
  jobPlainTextEdit->clear();
  if(i==0) // i=0: built-in
  {
      for(int k=0; k<builtInJobList.size(); k++)
      {
          QFileInfo fileInfo(builtInJobList.at(k));
          welcomeJobListWidget->addItem(fileInfo.baseName());
      }

  }
  else if(i==1)   // i=1: recent
  {
      for(int k=0; k<recentJobList.size(); k++)
      {
          QFileInfo fileInfo(recentJobList.at(k));
          welcomeJobListWidget->addItem(fileInfo.baseName());
      }

  }
  else if(i==2)  // i=2: favorite
  {
      for(int k=0; k<favoriteJobList.size(); k++)
      {
          QFileInfo fileInfo(favoriteJobList.at(k));
          welcomeJobListWidget->addItem(fileInfo.baseName());
      }

  }
}

void TwelcomeWidget::displayJob(int index)
{
    if(index<0) return;

    QString jobFilename;
    if(jobCategoryCombobox->currentIndex()==0) jobFilename=builtInJobList.at(index);
    else if(jobCategoryCombobox->currentIndex()==1) jobFilename=recentJobList.at(index);
    else if (jobCategoryCombobox->currentIndex()==2) jobFilename=favoriteJobList.at(index);
    else return;

    jobPathPlainTextEdit->setPlainText(jobFilename);
    QFile file(jobFilename);

    if (!file.exists())
    {
       // qDebug()<<jobFilename;
        jobPlainTextEdit->setPlainText(tr("File was not found."));
        return;
    }

    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream ReadFile(&file);
    jobPlainTextEdit->setPlainText(ReadFile.readAll());

}

void TwelcomeWidget::displayPulseProgram(int index)
{
    if(index<0) return;
    if(index>ppgPathList.size()-1) return;
    ppgPathPlainTextEdit->setPlainText(ppgPathList.at(index));

    if(index>ppgCaptionList.size()-1) return;
    ppgCaptionPlainTextEdit->setPlainText(ppgCaptionList.at(index));

    QFile file(ppgPathList.at(index));
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream ReadFile(&file);
    ppgPlainTextEdit->setPlainText(ReadFile.readAll());

}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
//MainWindow::MainWindow()users/takeda
{
    hasCloseEventCalled=false;
    // For Retina display
    devicePixelRatio=qApp->devicePixelRatio();

    path0=QDir::homePath()+"/.opencorenmr";

//    qDebug() << "1";

    #if defined(__APPLE__)
      defaultFont.setFamily("Monaco");
      defaultFont.setPointSize(11);
    #elif defined(_WIN32)
//      defaultFont.setFamily("Consolas");
      defaultFont.setFamily("Verdana");
      defaultFont.setPointSize(9);
    #endif
    defaultFont.setFixedPitch(true);


    createActionsAndButtons();
    createLeftToolBar();
    // A layout cannot be set on the MainWindow.
    // Accordingly a central widget is set.  (2014.1.17 Takeda)
    centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);

//qDebug() << "2";

    usbStatusLabel = new QLabel; usbStatusLabel->setFont(defaultFont); usbStatusLabel->setFixedWidth(200);
  //  expStatusLabel = new QLabel; expStatusLabel->setFont(defaultFont);
  //  receiverStatusLabel = new QLabel;  receiverStatusLabel->setFont(defaultFont);
    acqCounterLabel = new QLabel;  acqCounterLabel->setFont(defaultFont);
       acqCounterLabel->setFixedWidth(200);
    arrayCounterLabel = new QLabel; arrayCounterLabel->setFont(defaultFont);
      arrayCounterLabel->setFixedWidth(200);
    remainingTimeLabel = new QLabel; remainingTimeLabel->setFont(defaultFont);
      remainingTimeLabel->setFixedWidth(200);
    finishTimeLabel = new QLabel; finishTimeLabel->setFont(defaultFont);
      finishTimeLabel->setFixedWidth(200);

    statusBar()->addWidget(acqCounterLabel);
    statusBar()->addWidget(arrayCounterLabel);
    statusBar()->addWidget(remainingTimeLabel);
    statusBar()->addWidget(finishTimeLabel);
    statusBar()->addWidget(usbStatusLabel);

    QWidget *leftMenuWidget = new QWidget;
      QLinearGradient lg(QPointF(0,0),QPointF(800,800));
      lg.setColorAt(1,Qt::lightGray);
      lg.setColorAt(0,Qt::white);
      QBrush brush(lg);
      leftMenuWidget->setAutoFillBackground(true);
      QPalette pal(palette());
      pal.setBrush(QPalette::Window,brush);
      leftMenuWidget->setPalette(pal);

    QVBoxLayout *leftMenuWidgetLayout = new QVBoxLayout(leftMenuWidget);
    leftMenuWidgetLayout->setMargin(0);

    leftMenuWidgetLayout->addWidget(leftToolBar);
    leftMenuWidgetLayout->addStretch(1);
    leftMenuWidgetLayout->addWidget(leftToolBar2);
//    leftMenuWidgetLayout->addWidget(acqCounterLabel);
//    leftMenuWidgetLayout->addWidget(arrayCounterLabel);
//    leftMenuWidgetLayout->addWidget(remainingTimeLabel);
//    leftMenuWidgetLayout->addWidget(finishTimeLabel);
//    leftMenuWidgetLayout->addWidget(usbStatusLabel);

    welcomeWidget = new TwelcomeWidget;
      welcomeWidget->path0=path0;
      welcomeWidget->setAutoFillBackground(true);
      welcomeWidget->setPalette(pal);

      connect(welcomeWidget->editJobButton,SIGNAL(clicked()),this,SLOT(onEditJobButtonClicked()));
      connect(welcomeWidget->setupJobButton,SIGNAL(clicked()),this,SLOT(onSetupJobButtonClicked()));
      connect(welcomeWidget->clearJobButton,SIGNAL(clicked()),this,SLOT(onClearJobButtonClicked()));
     // connect(welcomeWidget->welcomeJobListWidget,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(onSetupJobButtonClicked()));

    ppgEdit = new TppgEdit;
      #if defined(_WIN32)
        ppgEdit->setFont(defaultFont);
      #endif

    fpgaTerminal = new TfpgaTerminal;
    fpgaTerminal->setDevicePixelRatio(devicePixelRatio);
      #if defined(_WIN32)
        fpgaTerminal->setFont(defaultFont);
      #endif


 //   processPanel = new TProcessPanelWidget;
 //   processPanel->setWindowTitle(tr("Data Process Panel"));
 //   processPanel->setDevicePixelRatio(devicePixelRatio);
 //   processPanel->setSettingDirPath(path0);

    stackedLayout = new QStackedLayout();
      stackedLayout->addWidget(welcomeWidget);
      stackedLayout->addWidget(ppgEdit);
      stackedLayout->addWidget(fpgaTerminal);
      //stackedLayout->addWidget(processPanel->plotters);


    QHBoxLayout *layout = new QHBoxLayout(centralWidget);
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->setContentsMargins(2,2,2,2);
    layout->addWidget(leftMenuWidget,1);
    layout->addLayout(stackedLayout,20);
    centralWidget->setLayout(layout);

    QSettings settings(path0 + "/nmr.ini", QSettings::IniFormat);
    settings.beginGroup("MainWindow");
      resize(settings.value("size", QSize(1000, 600)).toSize());
      move(settings.value("position", QPoint(0, 0)).toPoint());
    settings.endGroup();

  //  settings.beginGroup("ProcessPanel");
  //    processPanel->move(settings.value("position",QPoint(100,100)).toPoint());
  //  settings.endGroup();

    settings.beginGroup("ppgEdit");
      ppgEdit->ppgCompilerRightSplitter->restoreState(settings.value("rightSplitterSizes").toByteArray());
    settings.endGroup();

    settings.beginGroup("ExpSettingWindow");
      //ppgEdit->expSettings->move(settings.value("position",QPoint(0,0)).toPoint());
      fpgaTerminal->expSettings->move(settings.value("position",QPoint(0,0)).toPoint());
    settings.endGroup();

    settings.beginGroup("JobQueueWindow");
      fpgaTerminal->jobQueueWidget->move(settings.value("position",QPoint(0,0)).toPoint());
    settings.endGroup();

    settings.beginGroup("FavoriteJobWindow");
      fpgaTerminal->favoriteJobWidget->move(settings.value("position",QPoint(0,0)).toPoint());
    settings.endGroup();

 //   settings.beginGroup("Console");
 //     fpgaTerminal->splitter->restoreState(settings.value("splitter").toByteArray());
 //     fpgaTerminal->openOnStartupCheckBox->setChecked(settings.value("openOnStartup").toBool());

 //   settings.endGroup();

    settings.beginGroup("Path");
    ppgEdit->ppg->setppgFilePath(settings.value("ppgSourceFilePath").toString());

    QString defaultPath1;
    #if defined(__APPLE__)
      defaultPath1=QDir::homePath();
    #elif defined(_WIN32)
      defaultPath1=tr("C:/temp");
    #endif

    fpgaTerminal->expSettings->pathLineEdit->setText(settings.value("acqDataFilePath",QVariant(defaultPath1)).toString());
    dataFilePath=settings.value("dataFilePath").toString();
//    processPanel->processFileWidget->setDataFilePath(dataFilePath);
//    processFilePath=settings.value("processFilePath").toString();
//    processPanel->setProcessFilePath(processFilePath);
    settings.endGroup();

    settings.beginGroup("File");
      welcomeWidget->recentJobList=settings.value("recentJobs").toStringList();

//      for(int k=0; k<welcomeWidget->recentJobList.size(); k++)
//      {
//          QFileInfo fileInfo(welcomeWidget->recentJobList.at(k));
//          welcomeWidget->welcomeJobListWidget->addItem(fileInfo.baseName());
//      }

      //QStringList favoriteJobList;
      welcomeWidget->favoriteJobList=settings.value("favoriteJobs").toStringList();
      fpgaTerminal->favoriteJobWidget->jobListWidget->addItems(welcomeWidget->favoriteJobList);
    settings.endGroup();

    connect(fpgaTerminal->expSettings,SIGNAL(addJobListRequest(QString)),this,SLOT(addJobList(QString)));
    connect(ppgEdit->expSettings,SIGNAL(addJobListRequest(QString)),this,SLOT(addJobList(QString)));
    connect(fpgaTerminal->expSettings,SIGNAL(addJobListRequest(QString)),
            ppgEdit->expSettings,SLOT(readJob(QString)));

    connect(runStopButton,SIGNAL(clicked(bool)),this,SLOT(onRunStopButtonClicked()));

//    connect(runStopButton,SIGNAL(commandRequest(QString)),fpgaTerminal,SLOT(transferPPG(QString)));
    connect(fpgaTerminal, SIGNAL(runStateChanged(TRunStopButton::State)),
            runStopButton, SLOT(onStateChanged(TRunStopButton::State)));

    connect(fpgaTerminal, SIGNAL(scanModeChanged(int)), scanModeComboBox, SLOT(setCurrentIndex(int)));
   // connect(pageListWidget, SIGNAL(currentRowChanged(int)), stackedLayout, SLOT(setCurrentIndex(int)));


    connect(this,SIGNAL(init()),this,SLOT(showUSBNotOpend()));
    connect(fpgaTerminal,SIGNAL(usbClosed()),this,SLOT(showUSBNotOpend()));
    connect(fpgaTerminal,SIGNAL(transferStarted()), this, SLOT(showUSBBusy()));
    connect(fpgaTerminal,SIGNAL(transferStarted()), ppgEdit, SLOT(disableTransferButton()));
    connect(fpgaTerminal,SIGNAL(disableTransferButton()), ppgEdit, SLOT(disableTransferButton()));
    connect(fpgaTerminal,SIGNAL(enableTransferButton()), ppgEdit, SLOT(enableTransferButton()));
    connect(fpgaTerminal,SIGNAL(disableTransferButton()), this, SLOT(onExpBusy()));
    connect(fpgaTerminal,SIGNAL(enableTransferButton()), this, SLOT(onExpReady()));

    connect(&fpgaTerminal->tThread,SIGNAL(transferComplete()),this,SLOT(showUSBReady()));
//    connect(&fpgaTerminal->tThread,SIGNAL(transferComplete()),ppgEdit,SLOT(enableTransferButton()));
 //   connect(fpgaTerminal,SIGNAL(usbStatusChanged(bool)),ppgEdit,SLOT(onUSBStatusChanged(bool)));
    connect(&fpgaTerminal->fidThread,SIGNAL(receiverStatusChanged(bool)),this,SLOT(updateReceiverStatus(bool)));
    connect(fpgaTerminal,SIGNAL(copyComplete()),this,SLOT(updateReceiverCounter()));
    //connect(fpgaTerminal,SIGNAL(copyComplete()),this,SLOT(transferToProcess()));

    connect(&fpgaTerminal->expSettings->arrayWidget->arrayCounter,SIGNAL(updateCurrentCountRequest(QString)),
            arrayCounterLabel,SLOT(setText(QString)));
    connect(fpgaTerminal,SIGNAL(hideArrayCounterRequest()),this,SLOT(hideArrayCounter()));

    connect(fpgaTerminal,SIGNAL(jobComplete()),this,SLOT(saveSettings()));
    connect(ppgEdit,SIGNAL(toTerminal(QString)),fpgaTerminal,SLOT(fromEditor(QString)));
    connect(ppgEdit,SIGNAL(viewRequest()), this, SLOT(setTerminal()));
//    connect(ppgEdit,SIGNAL(viewRequest()), this, SLOT(ppgTransfer()));
    connect(ppgEdit->expSettings,SIGNAL(queueJobRequest(QString)),fpgaTerminal->jobQueueWidget,SLOT(addJob(QString)));


    connect(welcomeWidget,SIGNAL(openPPGRequest(QString)),ppgEdit,SLOT(ppgReadFromFile(QString)));
    connect(welcomeWidget,SIGNAL(openPPGRequest(QString)),this,SLOT(setCompiler()));
    connect(welcomeWidget->showFavoriteJobWidgetButton,SIGNAL(clicked()),fpgaTerminal,SLOT(showFavoriteJobWidget()));

    emit init();


    //fpgaTerminal->showFavoriteJobButton->click();
   // if(fpgaTerminal->openOnStartupCheckBox->isChecked())
   // {
   //     fpgaTerminal->FTDIOpen();
   // }
}

//----------------------------------------------------------------------------
void MainWindow::transferToProcess()
{
    // Operation too slow! 20170411 KT

    //processPanel->currentNMRData=fpgaTerminal->nmrData;
    //processPanel->FID_2D=fpgaTerminal->nmrData;
    //processPanel->plotters->setFID2D(fpgaTerminal->nmrData);
    //processPanel->initialize();
}
//----------------------------------------------------------------------------
void MainWindow::saveSettings()
{
    QSettings settings(path0 + "/nmr.ini", QSettings::IniFormat);

    settings.beginGroup("MainWindow");
    settings.setValue("size", size());
    settings.setValue("position", pos());
    settings.endGroup();

//    settings.beginGroup("ProcessPanel");
//    settings.setValue("position",processPanel->pos());
//    settings.endGroup();

    settings.beginGroup("ppgEdit");
    settings.setValue("rightSplitterSizes",ppgEdit->ppgCompilerRightSplitter->saveState());
    settings.endGroup();

    settings.beginGroup("ExpSettingWindow");
      settings.setValue("position",fpgaTerminal->expSettings->pos());
    //  settings.setValue("isHidden",fpgaTerminal->expSettings->isHidden());
    settings.endGroup();

    settings.beginGroup("JobQueueWindow");
      settings.setValue("position",fpgaTerminal->jobQueueWidget->pos());
    //  settings.setValue("isHidden",fpgaTerminal->jobQueueWidget->isHidden());
    settings.endGroup();

    settings.beginGroup("FavoriteJobWindow");
      settings.setValue("position",fpgaTerminal->favoriteJobWidget->pos());
      settings.setValue("isHidden",fpgaTerminal->favoriteJobWidget->isHidden());
    settings.endGroup();


 //   settings.beginGroup("Console");
 //     settings.setValue("splitter",fpgaTerminal->splitter->saveState());
 //   settings.setValue("openOnStartup",fpgaTerminal->openOnStartupCheckBox->isChecked());
 //   settings.endGroup();

    settings.beginGroup("Path");
      settings.setValue("ppgSourceFilePath", ppgEdit->ppg->ppgFilePath());
      settings.setValue("acqDataFilePath", fpgaTerminal->expSettings->pathLineEdit->text());
      //settings.setValue("dataFilePath", processPanel->processFileWidget->dataFilePath());
      //settings.setValue("processFilePath",processPanel->processFilePath());
    settings.endGroup();


    QStringList favoriteJobList;
    for(int k=0; k<fpgaTerminal->favoriteJobWidget->jobListWidget->count(); k++)
    {
        favoriteJobList.append(fpgaTerminal->favoriteJobWidget->jobListWidget->item(k)->text());
      //  qDebug() << favoriteJobList;
      // qDebug() << fpgaTerminal->favoriteJobWidget->jobListWidget->item(k)->text();
    }

    settings.beginGroup("File");
      settings.setValue("recentJobs", welcomeWidget->recentJobList);
      settings.setValue("favoriteJobs", favoriteJobList);
//      settings.setValue("favoriteJobs", welcomeWidget->favoriteJobList);
    settings.endGroup();

    settings.beginGroup("fidPlotters");
      settings.setValue("count",fpgaTerminal->fidPlotters.count());
    settings.endGroup();

    settings.beginGroup("plotSplitters");
      settings.setValue("count",fpgaTerminal->plotSplitters.count());
    settings.endGroup();



    for(int k=0; k<fpgaTerminal->plotSplitters.count(); k++)
    {
        settings.beginGroup("plotSplitter_"+QString::number(k));
          settings.setValue("orientation",fpgaTerminal->plotSplitters[k]->orientation());
          int nc=fpgaTerminal->plotSplitters[k]->count();
          settings.setValue("count",nc);
          if(nc>1)
          {
              settings.setValue("sizes",QVariant::fromValue(fpgaTerminal->plotSplitters[k]->sizes()) );
//              settings.setValue("foo", QVariant::fromValue(myList));
//              QList<int> myList2 = settings.value("foo").value<QList<int> >();

          }
          else
          {
              settings.setValue("fidPlotterIndex",fpgaTerminal->fidPlotters.indexOf(
                                     (FIDPlotter*)fpgaTerminal->plotSplitters[k]->widget(0))
                                );
          }
        settings.endGroup();
    }

    settings.beginGroup("plotWindows");
      settings.setValue("count",fpgaTerminal->plotWindows.count());
    settings.endGroup();


    for(int k=0; k<fpgaTerminal->plotWindows.count(); k++)
    {
        settings.beginGroup("plotWindow_"+QString::number(k));
          settings.setValue("size",fpgaTerminal->plotWindows[k]->size());
          settings.setValue("position",fpgaTerminal->plotWindows[k]->pos());
          settings.setValue("plotSplitterIndex",fpgaTerminal->plotSplitters.indexOf(
                                (QSplitter *)fpgaTerminal->plotWindows[k]->layout()->widget()));
        settings.endGroup();

    }

   settings.sync();

}
//----------------------------------------------------------------------------
void MainWindow::createActionsAndButtons()
{
    welcomeAction = new QAction(QIcon(":/images/welcomeDango.png"),tr("welcome"),this);
    welcomeAction->setCheckable(true);
    connect(welcomeAction,SIGNAL(triggered(bool)),this,SLOT(setWelcome()));
    ppgCompilerAction = new QAction(QIcon(":/images/pulseProgram.png"),tr("pulse program"),this);
    ppgCompilerAction->setCheckable(true);
    connect(ppgCompilerAction,SIGNAL(triggered(bool)),this,SLOT(setCompiler()));
    terminalAction = new QAction(QIcon(":/images/nmrExp.png"),tr("terminal"),this);
    terminalAction->setCheckable(true);
    connect(terminalAction,SIGNAL(triggered(bool)),this,SLOT(setTerminal()));
//    processAction = new QAction(QIcon(":/images/process.png"),tr("process"),this);
//    processAction->setCheckable(true);
//    connect(processAction,SIGNAL(triggered(bool)),this,SLOT(setProcess()));

    runStopButton = new TRunStopButton;
    runStopButton->setIcon(QIcon(":/images/run.png"));

    scanModeComboBox = new QComboBox;
    scanModeComboBox->addItems(QStringList()<<"Accum"<<"Repeat");
    connect(scanModeComboBox,SIGNAL(currentIndexChanged(int)),runStopButton,SLOT(onScanModeChanged(int)));

    lightningBoltButton = new QToolButton;
    lightningBoltButton->setIcon(QIcon());
}
//----------------------------------------------------------------------------
void MainWindow::createLeftToolBar()
{
    leftToolBar = new QToolBar(this);
    leftToolBar->setOrientation(Qt::Vertical);

    leftToolBar->setIconSize(QSize(80,60));
    leftToolBar->setFixedWidth(80);
  //  leftToolBar->addWidget(welcomeToolButton);

    leftToolBar->addAction(welcomeAction); welcomeAction->setChecked(true);
    leftToolBar->addAction(ppgCompilerAction);
    leftToolBar->addAction(terminalAction);
//    leftToolBar->addAction(processAction);
//    leftToolBar->addSeparator();


    leftToolBar2 = new QToolBar(this);
    leftToolBar2->setOrientation(Qt::Vertical);

    leftToolBar2->setIconSize(QSize(80,60));
    leftToolBar2->setFixedWidth(80);
    leftToolBar2->addWidget(runStopButton);

    leftToolBar2->addWidget(scanModeComboBox);
    leftToolBar2->addWidget(lightningBoltButton);

}
//----------------------------------------------------------------------------
void MainWindow::addJobList(QString jobFilename)
{

    if(welcomeWidget->recentJobList.contains(jobFilename))
    {
        int i=welcomeWidget->recentJobList.indexOf(jobFilename);
        if(i>0) {welcomeWidget->recentJobList.removeAt(i); welcomeWidget->recentJobList.prepend(jobFilename);}
    }
    else
    {
        welcomeWidget->recentJobList.prepend(jobFilename);
    }

    while(welcomeWidget->recentJobList.size()>100) welcomeWidget->recentJobList.removeLast();

    welcomeWidget->welcomeJobListWidget->clear();

    if(welcomeWidget->jobCategoryCombobox->currentIndex()==1) // recent
    welcomeWidget->setJobCategoty();

  //  for(int k=0; k<welcomeWidget->recentJobList.size(); k++)
  //  {
  //      QFileInfo fileInfo(welcomeWidget->recentJobList.at(k));
  //      welcomeWidget->welcomeJobListWidget->addItem(fileInfo.baseName());
  //  }

}
//----------------------------------------------------------------------------
void MainWindow::onRunStopButtonClicked()
{
    if(runStopButton->state()==TRunStopButton::Running)
    {
        fpgaTerminal->transferPPG("i");
        return;
    }

    if(!fpgaTerminal->ppgTransferred) return;
    //fpgaTerminal->transferPPG(fpgaTerminal->ppg->receiverInfo.receiverCommands());

    //TODO:  RS or G
    if(scanModeComboBox->currentIndex()==0) fpgaTerminal->transferPPG("g");
    else fpgaTerminal->transferPPG("rs");
}
//----------------------------------------------------------------------------
void MainWindow::setWelcome()
{
    welcomeAction->setChecked(true);
    ppgCompilerAction->setChecked(false);
    terminalAction->setChecked(false);
//    processAction->setChecked(false);

    stackedLayout->setCurrentIndex(0);
    welcomeWidget->setJobCategoty();
}
//----------------------------------------------------------------------------
void MainWindow::setCompiler()
{
    welcomeAction->setChecked(false);
    ppgCompilerAction->setChecked(true);
    terminalAction->setChecked(false);
//    processAction->setChecked(false);

    stackedLayout->setCurrentIndex(1);
}
//----------------------------------------------------------------------------
void MainWindow::setTerminal()
{
    welcomeAction->setChecked(false);
    ppgCompilerAction->setChecked(false);
    terminalAction->setChecked(true);
//    processAction->setChecked(false);

    stackedLayout->setCurrentIndex(2);
}
//----------------------------------------------------------------------------
//void MainWindow::setProcess()
//{
//    welcomeAction->setChecked(false);
//    ppgCompilerAction->setChecked(false);
//    terminalAction->setChecked(false);
//    processAction->setChecked(true);

//    stackedLayout->setCurrentIndex(3);


//    processPanel->setWindowFlags(Qt::Window);
//    processPanel->show();


//}
//----------------------------------------------------------------------------
void MainWindow::updateReceiverCounter()
{
    QString s;
    QMutex mutex;
    QMutexLocker locker(&mutex);

    if(fpgaTerminal->repeatScanQ)
    {
        s="Repeat Scan";
        acqCounterLabel->setText(s);
        remainingTimeLabel->setText("");
        finishTimeLabel->setText("");
        return;
    }

    if(fpgaTerminal->nmrData->FID[fpgaTerminal->nmrData->currentFID()]->dummyCount>0)
    {
        s=QString::number(fpgaTerminal->nmrData->FID[fpgaTerminal->nmrData->currentFID()]->dummyCount)+" dummys";
    }
    else
    {
        s=QString::number(fpgaTerminal->nmrData->FID[fpgaTerminal->nmrData->currentFID()]->actualNA);

        if(fpgaTerminal->nmrData->FID.size()>1)
        {
            s=s+"(" + QString::number(fpgaTerminal->nmrData->currentFID()+1) +")";
        }

        s=s+" / "
           +QString::number(fpgaTerminal->ppg->receiverInfo.na());
    }


    acqCounterLabel->setText(s);

    QDateTime currentTime=QDateTime::currentDateTime();
    int pdIndex=fpgaTerminal->ppg->variableIndex("PD");
    if(pdIndex<0) return;

    double secs=fpgaTerminal->ppg->variables.at(pdIndex)->value().toDouble()
                      *(fpgaTerminal->ppg->receiverInfo.na()
                        -fpgaTerminal->nmrData->FID[fpgaTerminal->nmrData->currentFID()]->actualNA)
                      *fpgaTerminal->ppg->receiverInfo.nc();

    if(fpgaTerminal->arrayQ)
        secs += fpgaTerminal->ppg->variables.at(pdIndex)->value().toDouble()
               *fpgaTerminal->ppg->receiverInfo.na()*fpgaTerminal->ppg->receiverInfo.nc()
               *(fpgaTerminal->expSettings->arrayWidget->arrayCounter.remainingArrayCount()-1);

    remainingTimeLabel->setText(QString::number(round(secs)) + " s left");

    QDateTime finishTime=currentTime.addSecs(secs);
    finishTimeLabel->setText(finishTime.toString("hh:mm"));

    return;
}
//----------------------------------------------------------------------------
/*void MainWindow::ppgTransfer()
{
    if(!fpgaTerminal->device->ppgConnected) return;
    setTerminal();
}*/
//----------------------------------------------------------------------------
void MainWindow::closeEvent(QCloseEvent *event)
{
    if(hasCloseEventCalled) {event->accept(); return;}
    // On Mac OSX, the closeEvent seems to be called twice,
    // causing crash when the application tries to delete the object which has already been deleted.
    // Accordingly, we check is the closeEvent has ever been called, and if so,
    // we skip the following confirmation and destructor calls.

    event->ignore();
    if (QMessageBox::Yes == QMessageBox::question(this, "Close Confirmation?",
                                 "Are you sure you want to exit?",
                                 QMessageBox::Yes|QMessageBox::No))
    {
        //qDebug()<<"1";

        saveSettings();
        //qDebug()<<"2";
        delete fpgaTerminal;
        //qDebug()<<"3";
        //delete processPanel;
        //qDebug()<<"4";
        delete ppgEdit;
        //qDebug()<<"5";

        hasCloseEventCalled=true;
        event->accept();
    }

}
//----------------------------------------------------------------------------
void MainWindow::onClearJobButtonClicked()
{

  //  welcomeWidget->recentJobList.clear();
  //  welcomeWidget->welcomeJobListWidget->clear();

}
//----------------------------------------------------------------------------
void MainWindow::onEditJobButtonClicked()
{
    int index=welcomeWidget->welcomeJobListWidget->currentRow();
    if(index<0) return;

    QString jobFilename;
    if(welcomeWidget->jobCategoryCombobox->currentIndex()==0) jobFilename=welcomeWidget->builtInJobList.at(index);
    else if(welcomeWidget->jobCategoryCombobox->currentIndex()==1) jobFilename=welcomeWidget->recentJobList.at(index);
    else if (welcomeWidget->jobCategoryCombobox->currentIndex()==2) jobFilename=welcomeWidget->favoriteJobList.at(index);
    else return;

    QFileInfo info;
    info.setFile(jobFilename);

    if(!info.exists())
    {
        QMessageBox::warning(this,tr("error"),tr("Job file was not found."));
        return;
    }

    setCompiler();
    ppgEdit->expSettings->readJob(jobFilename);

}
//----------------------------------------------------------------------------
void MainWindow::onSetupJobButtonClicked()
{
    int index=welcomeWidget->welcomeJobListWidget->currentRow();
    if(index<0) return;

    if(!fpgaTerminal->device->ppgConnected)
    {
        QMessageBox::warning(this,tr("Device not opened"),tr("Device has not been opened."));
        return;
    }
    QString jobFilename;
    if(welcomeWidget->jobCategoryCombobox->currentIndex()==0) jobFilename=welcomeWidget->builtInJobList.at(index);
    else if(welcomeWidget->jobCategoryCombobox->currentIndex()==1) jobFilename=welcomeWidget->recentJobList.at(index);
    else if (welcomeWidget->jobCategoryCombobox->currentIndex()==2) jobFilename=welcomeWidget->favoriteJobList.at(index);
    else return;

    QFileInfo info;
    info.setFile(jobFilename);

    if(!info.exists())
    {
        QMessageBox::warning(this,tr("error"),tr("Job file was not found."));
        return;
    }

    fpgaTerminal->expSettings->pathLineEdit->setText(info.absolutePath());
    fpgaTerminal->expSettings->nameLineEdit->setText(info.baseName());


    setTerminal();
    fpgaTerminal->expSettings->readJob(jobFilename);
    addJobList(jobFilename);

}
