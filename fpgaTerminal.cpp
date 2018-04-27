#include "fpgaTerminal.h"
#include "dataProcess/fft.h"
#include "dataProcess/phase.h"
#include "dataProcess/replaceRealWithAbsolute.h"

#include <QEventLoop>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QFont>
#include <QFileInfo>
#include <QMessageBox>
#include <QDebug>
#include <QThread>
#include <QTimer>
#include <QFileDialog>
#include <QPalette>
#include <QDir>
#include <QGroupBox>
#include <QTimer>
#include <QDateTime>
#include <QProcess>


Tdevice::Tdevice()
{
  ppgConnected=false;
  rcvrConnected=false;
  stopped=false;
}

Tdevice::~Tdevice()
{
    if(ppgConnected) FT_Close(pulserHandle);
    if(rcvrConnected) FT_Close(receiverHandle);
}
void Tdevice::run()
{
    if(stopped)return;

    int delay=100;
    error=false;
    message.clear();

    QMutexLocker locker(&mutex);

    // create the device information list
    emit sendMessage("Getting number of devices...\n");

    ftStatus = createDeviceInfoList(numDevs);

    if (ftStatus == FT_OK) {
        emit sendMessage("Number of devices is " + QString::number(numDevs) + "\n");
        QThread::msleep(delay);
    } else {
        emit sendMessage("Failed to get the number of devices.\n");
        QThread::msleep(delay);
        error=true;
        emit finished();
        return;
    }

    if (numDevs > 0) // allocate storage for list based on numDevs devInfo =
    {
      // get the device information list
      devInfo=(FT_DEVICE_LIST_INFO_NODE*)malloc(sizeof(FT_DEVICE_LIST_INFO_NODE)*numDevs);
      ftStatus = FT_GetDeviceInfoList(devInfo,&numDevs);
      if (ftStatus == FT_OK)
      {
        for (int i = 0; i < (int) numDevs; i++)
        {
          emit sendMessage("Dev " + QString::number(i)
           + " Flags=0x" + QString::number(devInfo[i].Flags)
           + " Type=0x" + QString::number(devInfo[i].Type)
           + " ID=0x" + QString::number(devInfo[i].ID)
           + " LocID=0x" + QString::number(devInfo[i].LocId) + "\n");
            QThread::msleep(delay);

          s=QString(devInfo[i].SerialNumber);
          emit sendMessage(" SerialNumber=" + s + "\n");
          QThread::msleep(delay);

          strcpy(deviceDescription[i],devInfo[i].Description);
          s=QString(deviceDescription[i]);
          emit sendMessage(" Description=" + s + "\n");
          QThread::msleep(delay);

        } // for
      } // if (ftStatus == FT_OK)
    }  // if (numDevs > 0)

    if((ftStatus = FT_OpenEx(deviceDescription[0], FT_OPEN_BY_DESCRIPTION, &pulserHandle)) != FT_OK)
    {
        ppgConnected=false;
        emit sendMessage("Failed to connect to channel A of FT2232.\n");
        QThread::msleep(delay);
        error=true;
        emit finished();
        return;
    }
    else // connection to the pulse programmer is successful!
    {
        ppgConnected=true;
        emit sendMessage("Connection to channel A of FT2232 has been successful.\n");
        QThread::msleep(delay);

        if((ftStatus = FT_SetBaudRate(pulserHandle, FT_BAUD_38400)) != FT_OK) {
           emit sendMessage("Failed to set the baud rate to 38400 bps.\n");
            QThread::msleep(delay);
           error=true;
           emit finished();
           return;
       } else {
           emit sendMessage("Baud rate has been set to 38400 bps.\n");
            QThread::msleep(delay);
       }
    }

    if((ftStatus = FT_OpenEx(deviceDescription[1], FT_OPEN_BY_DESCRIPTION, &receiverHandle)) != FT_OK)
    {
        rcvrConnected=false;
        emit sendMessage("Failed to connect to channel B of FT2232.\n");
        QThread::msleep(delay);
        error=true;
        emit finished();
        return;
    } else {
        rcvrConnected=true;
        emit sendMessage("Connection to channel B of FT2232 has been successful.\n");
        QThread::msleep(delay);
    }

    emit finished();

}

void Tdevice::open()
{
    //FT_STATUS ftStatus;
    //FT_DEVICE_LIST_INFO_NODE *devInfo;
    //DWORD numDevs;
    //QString s;
    //char deviceDescription[2][64];

    error=false;
    message.clear();

    QMutexLocker locker(&mutex);

    // create the device information list
    ftStatus = createDeviceInfoList(numDevs);

    if (ftStatus == FT_OK) {
        message.append("Number of devices is " + QString::number(numDevs));
    } else {
        message.append("Failed to get the number of devices.");
        error=true;
        return;
    }

    if (numDevs > 0) // allocate storage for list based on numDevs devInfo =
    {
      // get the device information list
      devInfo=(FT_DEVICE_LIST_INFO_NODE*)malloc(sizeof(FT_DEVICE_LIST_INFO_NODE)*numDevs);
      ftStatus = FT_GetDeviceInfoList(devInfo,&numDevs);
      if (ftStatus == FT_OK)
      {
        for (int i = 0; i < (int) numDevs; i++)
        {
          message.append("Dev " + QString::number(i)
           + " Flags=0x" + QString::number(devInfo[i].Flags)
           + " Type=0x" + QString::number(devInfo[i].Type)
           + " ID=0x" + QString::number(devInfo[i].ID)
           + " LocID=0x" + QString::number(devInfo[i].LocId));

          s=QString(devInfo[i].SerialNumber);
          message.append(" SerialNumber=" + s);

          strcpy(deviceDescription[i],devInfo[i].Description);
          s=QString(deviceDescription[i]);
          message.append(" Description=" + s);

          //printf(" ftHandle=0x%x\n",devInfo[i].ftHandle);
        } // for
      } // if (ftStatus == FT_OK)
    }  // if (numDevs > 0)

    if((ftStatus = FT_OpenEx(deviceDescription[0], FT_OPEN_BY_DESCRIPTION, &pulserHandle)) != FT_OK)
    {
        ppgConnected=false;
        message.append("Failed to connect to channel A of FT2232.");
        error=true;
        return;
    }
    else // connection to the pulse programmer is successful!
    {
        ppgConnected=true;
        message.append("Connection to channel A of FT2232 has been successful.");

        if((ftStatus = FT_SetBaudRate(pulserHandle, FT_BAUD_38400)) != FT_OK) {
           message.append("Failed to set the baud rate to 38400 bps.");
           error=true;
           return;
       } else {
           message.append("Baud rate has been set to 38400 bps.");
       }
    }

    if((ftStatus = FT_OpenEx(deviceDescription[1], FT_OPEN_BY_DESCRIPTION, &receiverHandle)) != FT_OK)
    {
        rcvrConnected=false;
        message.append("Failed to connect to channel B of FT2232.");
        error=true;
        return;
    } else {
        rcvrConnected=true;
        message.append("Connection to channel B of FT2232 has been successful.");
    }
}



TfpgaTerminal::TfpgaTerminal(QWidget *parent) :
    QWidget(parent)
{
    // TProcessOperations processOperations;

    device = new Tdevice;
    isConnectionSuccessful=false;
    runQ=false;
    arrayQ=false;
    ppgTransferred=false;

    nmrData = new TFID_2D;
    ppg = new TpulseProgram(3);

    plotSplitters.append(new QSplitter);
    plotSplitters[0]->setHandleWidth(0);

    fidPlotters.append(new FIDPlotter);
    fidPlotters[0]->setFID2D(nmrData);
    fidPlotters[0]->plotter->xini=0;
    fidPlotters[0]->plotter->xfin=32768;

    plotSplitters[0]->addWidget(fidPlotters[0]);
    connect(fidPlotters[0],SIGNAL(splitRequest(FIDPlotter*,TFIDPlotters::PlotSplitMode)),
            this,SLOT(splitPlot(FIDPlotter*,TFIDPlotters::PlotSplitMode)));




    QGroupBox *spectrometerGroupBox = new QGroupBox(tr("Spectrometer"));
    terminalOpenCloseButtion = new QPushButton(tr("Open"));
      QHBoxLayout *layout10 = new QHBoxLayout;
      layout10->addWidget(terminalOpenCloseButtion);
      layout10->addStretch();
      buildNumberLabel = new QLabel(tr(""));
  //  openOnStartupCheckBox = new QCheckBox(tr("Open on startup"));
       QVBoxLayout *openCloseLayout = new QVBoxLayout;
       //openCloseLayout->setMargin(0);
       //openCloseLayout->addStretch();
       openCloseLayout->addLayout(layout10);
       openCloseLayout->addWidget(buildNumberLabel);
  //     openCloseLayout->addWidget(openOnStartupCheckBox);
    spectrometerGroupBox->setLayout(openCloseLayout);

    FPGAStatusTextEdit = new QPlainTextEdit;
    FPGAStatusTextEdit->document()->setMaximumBlockCount(1000);
    pulserConsole = new Console;

    QGroupBox *expGroupBox = new QGroupBox(tr("Experiment"));
    loadJobButton = new QPushButton(tr("Load job"));
    loadJobButton->setEnabled(false); // We set it false until USB connection has been established.
    showExpSettingsButton = new QPushButton(tr("Exp. Settings"));
    showJobButton = new QPushButton(tr("Job Queue"));
    showFavoriteJobButton = new QPushButton(tr("Favorite Jobs"));
//    showExpSettingsButton = new QPushButton(QIcon(":/images/hammer1.png"),tr("Settings"));
//    showJobButton = new QPushButton(QIcon(":/images/hammer1.png"),tr("Job"));
      QVBoxLayout *layout0 = new QVBoxLayout;
      //QWidget *widget0 = new QWidget;
      QHBoxLayout *layout1 = new QHBoxLayout;
      layout1->addWidget(loadJobButton);
      layout1->addWidget(showFavoriteJobButton);
      layout1->addStretch();
      QHBoxLayout *layout2 = new QHBoxLayout;
      layout2->addWidget(showExpSettingsButton);
      layout2->addWidget(showJobButton);
      layout2->addStretch();
      layout0->addLayout(layout1);
      layout0->addLayout(layout2);
    expGroupBox->setLayout(layout0);

  //  qDebug()<<"TpgaTerminal::TfpgaTerminal 2";

  #if defined(__APPLE__)
      pulserConsole->setFont(QFont("Monaco",9));
      FPGAStatusTextEdit->setFont(QFont("Monaco",9));
  #elif defined(_WIN32)
      pulserConsole->setFont(QFont("Consolas",8));
      FPGAStatusTextEdit->setFont(QFont("Consolas",8));

  #elif defined(__linux__)
    GPIB488Console=new TGPIB488Console;
    GPIB488Console->show();
  #endif

    //commentTextEdit = new QPlainTextEdit;
    //commentTextEdit->document()->setMaximumBlockCount(1000);

    QLinearGradient lg(QPointF(0,0),QPointF(400,400));
    lg.setColorAt(0,Qt::white);
    lg.setColorAt(1,Qt::lightGray);
    QBrush brush(lg);
    QPalette pal(palette());
    pal.setBrush(QPalette::Window,brush);

    QWidget *widget2 = new QWidget;
    QWidget *consoleWidget = new QWidget;
    QWidget *FPGAStatusWidget = new QWidget;


 //   qDebug()<<"TpgaTerminal::TfpgaTerminal 3";


        QVBoxLayout *FPGAStatusLayout = new QVBoxLayout(FPGAStatusWidget);
        FPGAStatusLayout->setMargin(0);
        FPGAStatusLayout->setSpacing(0);
        FPGAStatusLayout->addWidget(FPGAStatusTextEdit);
//        FPGAStatusLayout->addLayout(openCloseLayout);
        FPGAStatusLayout->addWidget(spectrometerGroupBox);

          QVBoxLayout *consoleLayout = new QVBoxLayout(consoleWidget);
          consoleLayout->setMargin(0);
          consoleLayout->setSpacing(0);


          consoleLayout->addWidget(expGroupBox);
//          consoleLayout->addWidget(showExpSettingsButton);
          consoleLayout->addWidget(new QLabel(tr("FPGA Terminal")));
          consoleLayout->addWidget(pulserConsole);
          consoleLayout->addWidget(new QLabel(tr("Messages")));
          consoleLayout->addWidget(FPGAStatusWidget);

          QWidget *pWidget = new QWidget;
          QHBoxLayout *pLayout = new QHBoxLayout(pWidget);
          pLayout->setMargin(0);
          pLayout->addWidget(plotSplitters[0]);

          QSplitter *pSplitter = new QSplitter(Qt::Horizontal);
          pSplitter->setHandleWidth(3);
//          pSplitter->addWidget(plotSplitters[0]);
          pSplitter->addWidget(pWidget);
          pSplitter->addWidget(consoleWidget);
          pSplitter->setStretchFactor(0,1);
          pSplitter->setStretchFactor(1,0);

          pSplitter->setCollapsible(0,false);
          pSplitter->setCollapsible(1,false);


      widget2->setAutoFillBackground(true);
      widget2->setPalette(pal);

      QVBoxLayout *mainLayout = new QVBoxLayout;
      mainLayout->setMargin(0);
      mainLayout->addWidget(pSplitter,0);
      setLayout(mainLayout);

      expSettings=new TExpSettings; //expSettings->hide();
      expSettings->setPPG(ppg);

      //QLinearGradient lg2(QPointF(0,0),QPointF(300,300));
      //lg2.setColorAt(1,QColor("silver"));
      //lg2.setColorAt(0,QColor("lightblue"));
      //QBrush brush2(lg2);
      //expSettings->setAutoFillBackground(true);
      //QPalette pal2(palette());
      //pal2.setBrush(QPalette::Window,brush2);
      //expSettings->setPalette(pal2);


      jobQueueWidget = new TJobQueueWidget;
      favoriteJobWidget = new TFavoriteJobWidget;
      favoriteJobWidget->loadJobPushButton->setEnabled(false);

      connect(loadJobButton,SIGNAL(clicked()),expSettings,SLOT(onJobLoadButtonClicked()));
      connect(this,SIGNAL(jobComplete()),jobQueueWidget,SLOT(onJobFinished()));
      connect(jobQueueWidget,SIGNAL(runJobRequest(QString)),this,SLOT(setAndRunJob(QString)));

      connect(expSettings,SIGNAL(queueJobRequest(QString)),jobQueueWidget,SLOT(addJob(QString)));
      connect(expSettings,SIGNAL(setJobRequest()),this,SLOT(setJob()));
      connect(expSettings,SIGNAL(setCarrierFreqRequest(double)),this,SLOT(setCarrierFreq(double)));
      connect(expSettings,SIGNAL(transferRequest(QStringList)),expSettings,SLOT(showWindow()));
      connect(expSettings,SIGNAL(transferRequest(QStringList)),this,SLOT(transferPPG(QStringList)));

      connect(expSettings->arrayWidget,SIGNAL(setAR(int)),this,SLOT(setAR(int)));
      connect(expSettings->variableTable,SIGNAL(transferRequest(QStringList)),this,SLOT(transferPPG(QStringList)));
      connect(expSettings->variableTable,SIGNAL(updateAuxParamsRequest()),this,SLOT(updateAuxParams()));
//      connect(expSettings->variableTable,SIGNAL(updateOrigNARequest(int)),expSettings->acquisitionWidget,SLOT(setOrigNA(int)));
      connect(&tThread,SIGNAL(transferComplete()),this,SLOT(onTransferComplete()));
      connect(expSettings->dataSaveButton,SIGNAL(clicked()),this,SLOT(onSaveButtonClicked()));
      connect(this,SIGNAL(dataSaveRequest()),this,SLOT(saveFID()));
      connect(expSettings->acquisitionWidget,SIGNAL(separateDataStorageChanged(int)),this,SLOT(FIDSelectSpinBoxUpdate(int)));

      connect(expSettings->variableTable,SIGNAL(message(QString)),this,SLOT(displayLogMessage(QString)));
      connect(expSettings,SIGNAL(message(QString)),this,SLOT(displayLogMessage(QString)));

      connect(terminalOpenCloseButtion,SIGNAL(clicked()),this,SLOT(FTDIOpenClose()));
      connect(pulserConsole,SIGNAL(sendCommand(QString)),this,SLOT(transferPPG(QString)));
      connect(&rThread,SIGNAL(gotChar(QChar)),this,SLOT(displayPPGMessage(QChar)));
      connect(&rThread,SIGNAL(gotSentence(QString)),this,SLOT(getReplyFromFPGA(QString)));

      connect(&rThread,SIGNAL(runPrompt()),this,SLOT(onRunPromptReceived()));
      connect(&rThread,SIGNAL(readyPrompt()),this,SLOT(onReadyPromptReceived()));
      connect(&rThread,SIGNAL(arrayPrompt()),this,SLOT(onArrayPromptReceived()));

      connect(&fidThread,SIGNAL(gotFID(TFID *)),this,SLOT(copyFID(TFID *)));


      connect(this,SIGNAL(copyComplete()),this,SLOT(displayData()));
      connect(this,SIGNAL(copyComplete()),this,SLOT(enableSaveButton()));
      connect(this,SIGNAL(copyComplete()),&fidThread,SIGNAL(copyCompleteReceived()));


      connect(showExpSettingsButton,SIGNAL(clicked()),expSettings,SLOT(showWindow()));
      connect(showJobButton,SIGNAL(clicked()),this,SLOT(showJobWidget()));
      connect(showFavoriteJobButton,SIGNAL(clicked()),this,SLOT(showFavoriteJobWidget()));
      connect(this,SIGNAL(usbStatusChanged(bool)),expSettings->jobLoadButton,SLOT(setEnabled(bool)));
      connect(this,SIGNAL(usbStatusChanged(bool)),expSettings->jobQueueButton,SLOT(setEnabled(bool)));
      connect(this,SIGNAL(usbStatusChanged(bool)),loadJobButton,SLOT(setEnabled(bool)));
      connect(this,SIGNAL(usbStatusChanged(bool)),favoriteJobWidget->loadJobPushButton,SLOT(setEnabled(bool)));

      connect(favoriteJobWidget,SIGNAL(loadJobRequest(QString)),expSettings,SLOT(readJob(QString)));

}
//----------------------------------------------------------------------------
TfpgaTerminal::~TfpgaTerminal()
{
   // qDebug()<<"deleting fpgaterminal...";
    FTDIClose();

    if(!jobQueueWidget->isHidden()) jobQueueWidget->close();
    if(!favoriteJobWidget->isHidden()) favoriteJobWidget->close();
    if(!expSettings->isHidden()) expSettings->close();
    while(!plotWindows.isEmpty()) plotWindows.last()->close();

#if defined(__linux__)
    if(!GPIB488Console->isHidden()) GPIB488Console->close();
#endif
}
//----------------------------------------------------------------------------
void TfpgaTerminal::addAndCopyPlotter(FIDPlotter *fp)
{
    fidPlotters.append(new FIDPlotter);
    int k=fidPlotters.size()-1;
 //fidPlotters[k]->testLabel->setText(QString::number(k));
    fidPlotters[k]->setFID2D(nmrData);
    fidPlotters[k]->setDevicePixelRatio(devicePixelRatio());
    fidPlotters[k]->FIDSelectSpinBox
            ->setMaximum(fp->FIDSelectSpinBox->maximum());
    fidPlotters[k]->FIDSelectSpinBox->setValue(fp->FIDSelectSpinBox->value());
    fidPlotters[k]->plotter->xini=fp->plotter->xini;
    fidPlotters[k]->plotter->xfin=fp->plotter->xfin;

    if(fp->fid2d->FID.size()>0)
    {
      fidPlotters[k]->plotter->setFID(nmrData->FID[fidPlotters[k]->FIDSelectSpinBox->value()-1]);
      fidPlotters[k]->plotter->adjustScale();
      fidPlotters[k]->plotter->refresh();
    }

    connect(fidPlotters[k],SIGNAL(splitRequest(FIDPlotter*,TFIDPlotters::PlotSplitMode)),
            this,SLOT(splitPlot(FIDPlotter*,TFIDPlotters::PlotSplitMode)));

}

//----------------------------------------------------------------------------
void TfpgaTerminal::deletePlotWindow(TPlotWindow *pw)
{
    for(int fi=fidPlotters.size()-1; fi>=0; fi--)  // Descending order is important! (takeda)
    {
        if(pw->isAncestorOf(fidPlotters[fi]))
        {
            QSplitter *sp=(QSplitter*)fidPlotters.at(fi)->parentWidget();
            int sIndex=plotSplitters.indexOf(sp);
          //  qDebug() << "Trying to delete" << fi << "/"<< (fidPlotters.size()-1) << "th plotter and "
          //           << sIndex << "/" << (plotSplitters.size()-1) << "th splitter.";

            fidPlotters[fi]->setParent(NULL);
            fidPlotters[fi]->deleteLater();
            fidPlotters.removeAt(fi);

            plotSplitters[sIndex]->setParent(NULL);
            plotSplitters[sIndex]->deleteLater();
            plotSplitters.removeAt(sIndex);

         //   qDebug()<<"OK.";
        }
    }

    for(int si=plotSplitters.size()-1; si>=0; si--) // Descending order is important! (takeda)
    {
        if(pw->isAncestorOf(plotSplitters[si]))
        {
         //   qDebug()<<"Trying to delete " << si << "/" << plotSplitters.size()-1 << "th splitter";
            plotSplitters[si]->setParent(NULL);
            plotSplitters[si]->deleteLater();
            plotSplitters.removeAt(si);
         //   qDebug() << "OK.";
        }
    }

    int wIndex=plotWindows.indexOf(pw);
    plotWindows.removeAt(wIndex);

   // pw->setAttribute(Qt::WA_DeleteOnClose);
}
//----------------------------------------------------------------------------
void TfpgaTerminal::splitPlot(FIDPlotter *fp, TFIDPlotters::PlotSplitMode sMode)
{

//    qDebug() << QString(Q_FUNC_INFO);

    QSplitter *parentSplitter = (QSplitter*)fp->parentWidget();
  //  qDebug() <<"split mode: " << sMode;

    //if(parentSplitter->count()==0) return;
    if(sMode==TFIDPlotters::NewWindow)
    {
       plotWindows.append(new TPlotWindow);
       //plotWindows.last()->setAttribute(Qt::WA_DeleteOnClose);
       plotWindows.last()->show();

       connect(plotWindows.last(),SIGNAL(deleteRequest(TPlotWindow*)),
                    this,SLOT(deletePlotWindow(TPlotWindow*)));

       QVBoxLayout *mainLayout = new QVBoxLayout;
       mainLayout->setMargin(0);
       mainLayout->setSpacing(0);

       plotSplitters.append(new QSplitter);
       plotSplitters[plotSplitters.size()-1]->setHandleWidth(0);
       mainLayout->addWidget(plotSplitters[plotSplitters.size()-1]);

       addAndCopyPlotter(fp);
       plotSplitters[plotSplitters.size()-1]->addWidget(fidPlotters[fidPlotters.size()-1]);

       plotWindows.last()->setLayout(mainLayout);
       return;
    }

    if(sMode==TFIDPlotters::RemoveSplit) // remove
    {
        int fIndex=fidPlotters.indexOf(fp);
    //    qDebug() << "parent: " << parentSplitter->metaObject()->className()
     //            << "  grandParent: " << parentSplitter->parentWidget()->metaObject()->className();

        if(0!=strcmp(parentSplitter->parentWidget()->metaObject()->className(),"QSplitter"))
        {
      //      qDebug()<<"!!!";
            return;
        }

      //  qDebug() << "fIndex: " << fIndex;

        if(fIndex>-1 && fidPlotters.size()>1)
        {
           fidPlotters[fIndex]->setParent(NULL);
           fidPlotters[fIndex]->deleteLater();
           fidPlotters.removeAt(fIndex);

      //     qDebug() << "removed view!";

           QSplitter *grandParentSplitter = (QSplitter*)parentSplitter->parentWidget();

           int sIndex=plotSplitters.indexOf(parentSplitter);
      //     qDebug() << "sIndex: " << sIndex;
           plotSplitters[sIndex]->setParent(NULL);
           plotSplitters[sIndex]->deleteLater();
           plotSplitters.removeAt(sIndex);
      //     qDebug() << "splitter removed. Total " << plotSplitters.size();



           if(grandParentSplitter->count()==1)
           {
            //  qDebug() << "child removing child...";
              QSplitter *childSplitter=(QSplitter *)grandParentSplitter->widget(0);
          //    qDebug() << "It has " << childSplitter->count() << " grandchildren.";
              int cIndex=plotSplitters.indexOf(childSplitter);
              for(int k=childSplitter->count()-1;k>=0; k--)
                  // Descending order is important! 2015Sep Takeda
              {
                  childSplitter->widget(k)->setParent(grandParentSplitter);
          //        qDebug() << k;
              }
              plotSplitters[cIndex]->setParent(NULL);
              plotSplitters[cIndex]->deleteLater();
              plotSplitters.removeAt(cIndex);
          //    qDebug() << "splitter removed. Total " << plotSplitters.size();

           }

          //  qDebug() << "splitter removed. Total " << plotSplitters.size();
        }
        return;
    }

    Qt::Orientation ori=Qt::Horizontal;
    if(sMode==TFIDPlotters::Horizontal) ori=Qt::Horizontal;
    else if(sMode==TFIDPlotters::Vertical) ori=Qt::Vertical;

    parentSplitter->setOrientation(ori);

    plotSplitters.append(new QSplitter);
    plotSplitters[plotSplitters.size()-1]->setHandleWidth(0);

    parentSplitter->addWidget(plotSplitters[plotSplitters.size()-1]);
    fp->setParent(plotSplitters[plotSplitters.size()-1]);

    plotSplitters.append(new QSplitter);
    plotSplitters[plotSplitters.size()-1]->setHandleWidth(0);
    parentSplitter->addWidget(plotSplitters[plotSplitters.size()-1]);

    parentSplitter->setCollapsible(0,false);
    parentSplitter->setCollapsible(1,false);

    addAndCopyPlotter(fp);
    plotSplitters[plotSplitters.size()-1]->addWidget(fidPlotters[fidPlotters.size()-1]);

}
//----------------------------------------------------------------------------
void TfpgaTerminal::onArrayPromptReceived()
{

    QMutexLocker locker(&mutex);
    disconnect(&rThread,SIGNAL(arrayPrompt()),this,SLOT(onArrayPromptReceived()));

   // save data
    QString fn=expSettings->pathLineEdit->text().trimmed() + '/'
               + expSettings->nameLineEdit->text() + '/'
               + expSettings->nameLineEdit->text();

    QFileInfo fi;
    fi.setFile(fn);
    QString base = fi.baseName();
    QString path = fi.absolutePath()+'/';

    QString opd = path+base+"_array.opd";
    QString opp = path+base+"_array.opp";

        QString sm2d = path+base+"_array.sm2d";
        QString sm2p = path+base+"_array.sm2p";

    nmrData->comments=expSettings->commentTextEdit->toPlainText().split(QChar::ParagraphSeparator);

        if(QFile::exists(sm2d)) nmrData->Writesm2dFile(sm2d,QIODevice::Append);
        else  nmrData->Writesm2dFile(sm2d);
    if(QFile::exists(opd)) nmrData->WriteopdFile(opd,QIODevice::Append);
    else  nmrData->WriteopdFile(opd);

        nmrData->Writesm2pFile(sm2p);
    nmrData->WriteoppFile(opp);

   // update variable
   expSettings->arrayWidget->arrayCounter.increment(ppg);
   updateVariableTable();

   // qDebug() << arrayWidget->arrayCounter.currentCountString();

#if defined(__linux__)

    if(GPIB488Console->writeOnArrayCheckBox->isChecked())
    {
        QString qs0;
        int ai=expSettings->arrayWidget->arrayCounter.currentCount.at(0);

        QStringList sl0=GPIB488Console->writeOnArrayTextEdit->toPlainText().split('\n');
        if(ai<=sl0.size()-1)
        {
          qs0=sl0.at(ai);
          GPIB488Console->write(qs0);
        }

    }
#endif

   if(expSettings->arrayWidget->arrayCounter.hasFinished)
   {
       runQ=false;
       transferPPG(ppg->updatedPPG);
   }
   else
   {
       for(int k=0; k<ppg->receiverInfo.nc(); k++) // we cover hypercomplex acq cases
       {
         nmrData->FID[k]->initialize();
         nmrData->FID[k]->dummyCount=ppg->receiverInfo.nd();
         nmrData->FID[k]->actualNA=0;
       }
       nmrData->setCurrentFID(0);

     ppg->updatedPPG.clear();
     if(!ppg->updateVariable())
     {
         QMessageBox::warning(this,tr("compile error"),
                              QString(Q_FUNC_INFO)+tr(": Failed to compile pulse program: ")+ppg->errorMessage);
         return;
     }

     runQ=false;

     updateAuxParams();
     if(!ppg->updatedPPG.isEmpty()) transferPPG(ppg->updatedPPG);
     transferPPG(QStringList()<<"g");

     runQ=true;
     ppg->updatedPPG.clear();
   }
   connect(&rThread,SIGNAL(arrayPrompt()),this,SLOT(onArrayPromptReceived()));
}
//----------------------------------------------------------------------------
void TfpgaTerminal::displayData()
{
    mutex.lock();
    for(int k=0; k<fidPlotters.size(); k++)
    {
        fidPlotters[k]->update();
      //fidPlotters[k]->plotter->setFID(nmrData->FID[fidPlotters[k]->FIDSelectSpinBox->value()-1]);
      //fidPlotters[k]->plotter->adjustScale();
      //fidPlotters[k]->plotter->refresh();
    }
    fidThread.wake();
    mutex.unlock();

//    emit plotComplete();
}
//----------------------------------------------------------------------------
void TfpgaTerminal::updateAuxParams()
{
//  setNA(ppg->variables.at(ppg->variableIndex("NA"))->value().toInt());

  setNA(ppg->origNA());
  setND(ppg->variables.at(ppg->variableIndex("ND"))->value().toInt());
  fidThread.localFid->setDW(ppg->variables.at(ppg->variableIndex("DW"))->value().toDouble()*1000000);
  ppg->receiverInfo.setDW(ppg->variables.at(ppg->variableIndex("DW"))->value().toDouble());
  //setAL(ppg->variables.at(ppg->variableIndex("AL"))->value().toInt());
  ppg->receiverInfo.setAL(ppg->variables.at(ppg->variableIndex("AL"))->value().toInt());
  fidThread.localFid->setAL(ppg->variables.at(ppg->variableIndex("AL"))->value().toInt());

 // expSettings->acquisitionWidget->setOrigNA(ppg->variables.at(ppg->variableIndex("NA"))->value().toInt());
  expSettings->acquisitionWidget->setOrigAL(ppg->variables.at(ppg->variableIndex("AL"))->value().toInt());
}
//----------------------------------------------------------------------------
void TfpgaTerminal::onTransferComplete()
{
  if(!runQ)
  {
      jobQueueWidget->runJobPushButton->setEnabled(true);
      emit enableTransferButton();
  }
}
//----------------------------------------------------------------------------
void TfpgaTerminal::copyPPG(TpulseProgram *p)
{
    ppg->compiled = p->compiled;
    ppg->error = p->error;
    ppg->errorMessage = p->errorMessage;

    ppg->digitalFilter = p->digitalFilter;
    ppg->loadedFiles = p->loadedFiles;

    ppg->sourcePPG = p->sourcePPG;
    ppg->preamble = p->preamble;
    ppg->mainPPG = p->mainPPG;

    ppg->CLK = p->CLK;
    ppg->gates = p->gates;
    ppg->gateFilePaths = p->gateFilePaths;
    ppg->preambleInfo = p->preambleInfo;
    ppg->toFPGA = p->toFPGA;
    ppg->updatedPPG = p->updatedPPG;
    ppg->errorCheckOnly = p->errorCheckOnly;
    ppg->receiverInfo = p->receiverInfo;



}

//----------------------------------------------------------------------------
/*
void TfpgaTerminal::setVariableTableState(TRunStopButton::State state)
{

   QTableWidgetItem *it = new QTableWidgetItem;

      disconnect(realTimeVariableTable,SIGNAL(itemChanged(QTableWidgetItem *)),
                 realTimeVariableTable,SLOT(tryUpdateVariable(QTableWidgetItem *)));

   for(int k=0; k<realTimeVariableTable->rowCount(); k++)
   {
     it = realTimeVariableTable->item(k,2);
     if(state==TRunStopButton::Running)
       {
           it->setFlags(it->flags() & ~Qt::ItemIsEditable);
       }
      else
      {
          if(0==QString::compare("ACQTIME",realTimeVariableTable->item(k,0)->text(),Qt::CaseInsensitive))
                  it->setFlags(it->flags() & ~Qt::ItemIsEditable);
          else  it->setFlags(it->flags() & Qt::ItemIsEditable);
      }
    }

   connect(realTimeVariableTable,SIGNAL(itemChanged(QTableWidgetItem *)),
              realTimeVariableTable,SLOT(tryUpdateVariable(QTableWidgetItem *)));

}
*/
//----------------------------------------------------------------------------
/*
void TfpgaTerminal::setDirectory()
{
   QFileDialog *dialog = new QFileDialog;

   dialog->setFileMode(QFileDialog::Directory);
   dialog->setOption(QFileDialog::ShowDirsOnly);
   dialog->setViewMode(QFileDialog::Detail);
   dialog->setLabelText(QFileDialog::Accept, tr("Choose directory"));

   if(dialog->exec())
   {
       pathLineEdit->setText(dialog->selectedFiles().at(0));
   }

}
*/
//----------------------------------------------------------------------------
void TfpgaTerminal::enableSaveButton()
{
    expSettings->dataSaveButton->setEnabled(true);
}
//----------------------------------------------------------------------------
void TfpgaTerminal::disableSaveButton()
{
    expSettings->dataSaveButton->setEnabled(false);
}
//----------------------------------------------------------------------------
//void TfpgaTerminal::setFont(QFont font)
//{
//     FPGAStatusTextEdit->setFont(font);
//     pulserConsole->setFont(font);
//     commentTextEdit->setFont(font);
//     pathLineEdit->setFont(font);
//     nameLineEdit->setFont(font);
//     fidplotter->setFont(font);
     //rcvrConsole->setFont(font);
//}

//----------------------------------------------------------------------------
void TfpgaTerminal::FTDIOpen()
{
#if defined(__APPLE__)

    QString cmd = "bash -c \"kextstat | grep com.apple.driver.AppleUSBFTDI\"";
    QProcess *process = new QProcess;
    process->start(cmd);
    process->waitForBytesWritten();
    process->waitForFinished();
    QString pr=QString(process->readAll());
    if(!pr.isEmpty())
    {
        QMessageBox::warning(this,tr(""),
                           tr("<p>Built-in VCP driver detected."
                              "You need to disable it to open USB connection. "
                              "Open a terminal and execute the following commands:"
                              "<p> sudo kextunload -bundle com.apple.driver.AppleUSBFTDI"
                              ));

        return;
    }
/*
    if(QFileInfo::exists("/System/Library/Extensions/IOUSBFamily.kext/Contents/PlugIns/AppleUSBFTDI.kext"))
    {
      QMessageBox::warning(this,tr(""),
                         tr("<p>Built-in VCP driver deteccted. "
                            "You need to disable it to open USB connection. "
                            "Open a terminal and execute the following commands:"
                            "<p> OSX Yosemite:"
                            "<p>cd /System/Library/Extensions/IOUSBFamily.kext/Contents/PlugIns"
                            "<p>sudo mv AppleUSBFTDI.kext AppleUSBFTDI.disabled"
                            "<p>Then, restart the computer."
                            "<p> OSX El Capitan:"
                            "<p> sudo kextunload -bundle com.apple.driver.AppleUSBFTDI"
                            ));
      return;

//      QProcess proc;
//      proc.start("sudo /bin/mv -c",
//                 proc.start("/usr/bin/sudo",
//                 QStringList() << "-c"
//                               << "/bin/mv"
//                              << "/System/Library/Extensions/IOUSBFamily.kext/Contents/PlugIns/AppleUSBFTDI.kext"
//                              << "/System/Library/Extensions/IOUSBFamily.kext/Contents/PlugIns/AppleUSBFTDI.disabled");

 //     proc.waitForFinished();

    }
*/
#endif

    if(device->ppgConnected || device->rcvrConnected) return;  // already connected to FPGA?

    QEventLoop loop;

    FPGAStatusTextEdit->clear();

    connect(device,SIGNAL(sendMessage(QString)),FPGAStatusTextEdit,SLOT(insertPlainText(QString)));
//    device->open();
    loop.connect(device,SIGNAL(finished()), &loop, SLOT(quit()));
    device->start();
    loop.exec();
    disconnect(device,SIGNAL(sendMessage(QString)),FPGAStatusTextEdit,SLOT(insertPlainText(QString)));
    FPGAStatusTextEdit->moveCursor(QTextCursor::End);

    if ((device->ppgConnected) && (device->rcvrConnected)) {

      rThread.setDeviceHandle(device->pulserHandle);
      rThread.standBy();
      tThread.setDeviceHandle(device->pulserHandle);
      connect(&rThread,SIGNAL(gotPrompt(QChar)),&tThread,SLOT(proceed(QChar)));
      connect(&rThread,SIGNAL(gotPrompt(QChar)),this,SLOT(promptReceived(QChar)));

      terminalOpenCloseButtion->setText("Close");

      FPGAStatusTextEdit->insertPlainText("\nEstablishing communication with FPGA...\n");
      FPGAStatusTextEdit->moveCursor(QTextCursor::End);



      isConnectionSuccessful=false;

      disconnect(pulserConsole,SIGNAL(sendCommand(QString)),this,SLOT(transferPPG(QString)));
      connect(pulserConsole,SIGNAL(sendCommand(QString)),this,SLOT(sendToPulser(QString)));


      pulserConsole->sendCommand("i");
      QThread::msleep(400);
      pulserConsole->sendCommand("i");
      QThread::msleep(400);
      pulserConsole->sendCommand("i");
      QThread::msleep(400);

      QTimer timer;
      timer.setSingleShot(true);
      loop.connect(&timer,SIGNAL(timeout()), &loop, SLOT(quit()));
      timer.start(1000);
      loop.exec();

      disconnect(pulserConsole,SIGNAL(sendCommand(QString)),this,SLOT(sendToPulser(QString)));
      connect(pulserConsole,SIGNAL(sendCommand(QString)),this,SLOT(transferPPG(QString)));

      if(!isConnectionSuccessful) {

          FPGAStatusTextEdit->insertPlainText("Error: no reply from FPGA.\n");
          FPGAStatusTextEdit->moveCursor(QTextCursor::End);

          return;}


      tThread.send("v");
      loop.connect(this,SIGNAL(replyFromFPGACopied()), &loop, SLOT(quit()));
      loop.exec();
      QThread::msleep(400);



      loop.connect(this,SIGNAL(replyFromFPGACopied()), &loop, SLOT(quit()));
      tThread.send("b");
      //pulserConsole->sendCommand("b");
      loop.exec();
      QThread::msleep(400);

      buildNumberLabel->setText(tr("Spectrometer build #")+replyFromFPGA);

      FPGAStatusTextEdit->insertPlainText("NMR-core build number: "+replyFromFPGA + '\n');
      FPGAStatusTextEdit->moveCursor(QTextCursor::End);

      if(replyFromFPGA.toInt()<2006)
      {
        FPGAStatusTextEdit->insertPlainText("Error: This software requires build number 2006 or higher.");
        FPGAStatusTextEdit->moveCursor(QTextCursor::End);
      }
      else
      {
          FPGAStatusTextEdit->insertPlainText("Ready.\n");
          FPGAStatusTextEdit->moveCursor(QTextCursor::End);
      }

      QThread::msleep(400);
      pulserConsole->sendCommand("i");
      QThread::msleep(400);


      fidThread.setDeviceHandle(device->receiverHandle);
      fidThread.standBy();


    } else {
      FPGAStatusTextEdit->insertPlainText("Connection to FPGA failed.\n");
    }

    emit usbStatusChanged(device->ppgConnected && device->rcvrConnected);

}
//----------------------------------------------------------------------------
void TfpgaTerminal::FTDIClose()
{
    if(device->ppgConnected) {

        disconnect(&rThread,SIGNAL(gotPrompt(QChar)),&tThread,SLOT(proceed(QChar)));
        disconnect(&rThread,SIGNAL(gotPrompt(QChar)),this,SLOT(promptReceived(QChar)));

        rThread.stop();
        rThread.wait();

        FT_Close(device->pulserHandle);
        FPGAStatusTextEdit->moveCursor(QTextCursor::End);
        FPGAStatusTextEdit->insertPlainText("Connection to pulse programmer closed.\n");
        FPGAStatusTextEdit->moveCursor(QTextCursor::End);
        device->ppgConnected=false;
    }
    if(device->rcvrConnected) {

        fidThread.stop();
        fidThread.wait();

        FT_Close(device->receiverHandle);

        FPGAStatusTextEdit->moveCursor(QTextCursor::End);
        FPGAStatusTextEdit->insertPlainText("Connection to receiver closed.\n");
        FPGAStatusTextEdit->moveCursor(QTextCursor::End);
        device->rcvrConnected=false;

    }

    if ((!device->ppgConnected) && (!device->rcvrConnected))
    {
        emit usbClosed();
        emit usbStatusChanged(device->ppgConnected && device->rcvrConnected);
        terminalOpenCloseButtion->setText("Open");
        buildNumberLabel->clear();
        isConnectionSuccessful=false;
    }

}
//----------------------------------------------------------------------------
void TfpgaTerminal::FTDIOpenClose()
{
    if ((!device->ppgConnected) && (!device->rcvrConnected)) FTDIOpen();
    else if(device->ppgConnected && device->rcvrConnected)
    {
        if (QMessageBox::Yes == QMessageBox::question(this, "Close?",
                                     "Are you sure you want to close USB connection?",
                                     QMessageBox::Yes|QMessageBox::No))
        {
          FTDIClose();
        }
    }
}
//----------------------------------------------------------------------------
void TfpgaTerminal::getReplyFromFPGA(QString qs)
{
    replyFromFPGA=qs;
    emit replyFromFPGACopied();

    if(0==QString::compare(qs,"Finished.",Qt::CaseInsensitive))
    {
        emit runStateChanged(TRunStopButton::Standby);
       // emit dataSaveRequest();
    }
}
//----------------------------------------------------------------------------
void TfpgaTerminal::displayPPGMessage(QChar c)
{
    pulserConsole->insertPlainText(c);
    pulserConsole->moveCursor(QTextCursor::End);
}
//----------------------------------------------------------------------------
void TfpgaTerminal::onRunPromptReceived()
{
    if(runQ) {return;}
    runQ=true;
}
//----------------------------------------------------------------------------
void TfpgaTerminal::onReadyPromptReceived()
{
    // Just return if we either interrupt experiment or type "I".
    if(!runQ) {return;}

//    QMutexLocker locker(&mutex);
    mutex.lock();
    // We do something if the sequence is finished.
    runQ=false;

    expSettings->onExpFinished();
    loadJobButton->setEnabled(true);
    favoriteJobWidget->loadJobPushButton->setEnabled(true);

    // save data
    QString fn=expSettings->pathLineEdit->text().trimmed() + '/'
               + expSettings->nameLineEdit->text() + '/'
               + expSettings->nameLineEdit->text();

    QFileInfo fi;
    fi.setFile(fn);
    QString base = fi.baseName();
    QString path = fi.absolutePath()+'/';
    QString sm2d = path+base+".sm2d";
    QString sm2p = path+base+".sm2p";
    QString asm2d = path+base+"_array.sm2d";
    QString asm2p = path+base+"_array.sm2p";
    QString opd = path+base+".opd";
    QString opp = path+base+".opp";
    QString aopd = path+base+"_array.opd";
    QString aopp = path+base+"_array.opp";

    if(QFile::exists(opd))
    {
        int k=1;
        while(QFile::exists(path+base+"_"+QString::number(k)+".opd")) k++;
        opd=path+base+"_"+QString::number(k)+".opd";
        opp=path+base+"_"+QString::number(k)+".opp";
    }

    if(QFile::exists(sm2d))
    {
        // QFile::remove(sm2d);
        int k=1;
        while(QFile::exists(path+base+"_"+QString::number(k)+".sm2d")) k++;
        sm2d=path+base+"_"+QString::number(k)+".sm2d";
        sm2p=path+base+"_"+QString::number(k)+".sm2p";
    }

    nmrData->comments=expSettings->commentTextEdit->toPlainText().split(QChar::ParagraphSeparator);

    if(!arrayQ)
    {
        if(QFile::exists(path+base+"_unfinished.sm2p")) QFile::copy(path+base+"_unfinished.sm2p",sm2p);
        else nmrData->Writesm2pFile(sm2p);

        if(QFile::exists(path+base+"_unfinished.opp")) QFile::copy(path+base+"_unfinished.opp",opp);
        else nmrData->WriteoppFile(opp);

        if(!nmrData->Writesm2dFile(sm2d))
        {
            QMessageBox::warning(this,tr(""),tr("<p>Failed to save data: ") + sm2d );
            mutex.unlock();
            return;
        }
        if(!nmrData->WriteopdFile(opd))
        {
            QMessageBox::warning(this,tr(""),tr("<p>Failed to save data: ") + opd);
            mutex.unlock();
            return;
        }

        disableSaveButton();
      //  if(QFile::exists(path+base+"_unfinished.sm2d")) QFile::remove(path+base+"_unfinished.sm2d");
      //  if(QFile::exists(path+base+"_unfinished.sm2p")) QFile::remove(path+base+"_unfinished.sm2p");
      //  if(QFile::exists(path+base+"_unfinished.opd")) QFile::remove(path+base+"_unfinished.opd");
      //  if(QFile::exists(path+base+"_unfinished.opp")) QFile::remove(path+base+"_unfinished.opp");
        mutex.unlock();
        emit jobComplete();
        FPGAStatusTextEdit->insertPlainText("Sequence finished at "
                                            + QDateTime::currentDateTime().toString("hh:mm:ss dd MMM yyyy.\n"));
        FPGAStatusTextEdit->moveCursor(QTextCursor::End);

        return;
    }

    if(QFile::exists(asm2d)) nmrData->Writesm2dFile(asm2d,QIODevice::Append);
    else  nmrData->Writesm2dFile(asm2d);
    QFile::rename(asm2d,sm2d);

    if(QFile::exists(asm2p)) QFile::rename(asm2p,sm2p); // path+base+"_array.sm2p"
    else nmrData->Writesm2pFile(sm2p);

    if(QFile::exists(aopd)) nmrData->WriteopdFile(aopd,QIODevice::Append);
    else  nmrData->WriteopdFile(aopd);
    QFile::rename(aopd,opd);

    if(QFile::exists(aopp)) QFile::rename(aopp,opp); // path+base+"_array.opp"
    else nmrData->WriteoppFile(opp);

    disableSaveButton();

    // We do the followings when an array experiment has been finished.
    arrayQ=false;
    expSettings->arrayWidget->arrayCounter.init(ppg);
    ppg->updatedPPG.clear(); updateVariableTable();
    if(!ppg->updateVariable())
    {
        QMessageBox::warning(this,tr("compile error"),tr("Failed to compile pulse program: ")+ppg->errorMessage);
        mutex.unlock();
        return;
    }
    transferPPG(ppg->updatedPPG);
    transferPPG(ppg->receiverInfo.receiverCommands());

    updateAuxParams();

    ppg->updatedPPG.clear();
    mutex.unlock();
    emit jobComplete();

    FPGAStatusTextEdit->insertPlainText("Sequence finished at "
                                        + QDateTime::currentDateTime().toString("hh:mm:ss dd MMM yyyy.\n"));
    FPGAStatusTextEdit->moveCursor(QTextCursor::End);

    return;
}
//----------------------------------------------------------------------------
void TfpgaTerminal::transferPPG(const QStringList &sl)
{
    if (!device->ppgConnected) return;  // Do nothing if connection to FPGA has not been established.
    if(sl.isEmpty())  return;

    QProgressDialog *progressDialog=new QProgressDialog("Transferring...",QString(), 0 ,sl.size());
    progressDialog->setWindowModality(Qt::WindowModal);
    progressDialog->setMinimumDuration(100);

    connect(&tThread,SIGNAL(sent(QString)),this,SLOT(displaySentString(QString)));    
      disconnect(this,SIGNAL(dataSaveRequest()),this,SLOT(saveFID()));
        emit transferStarted();
        loadJobButton->setEnabled(false);
        expSettings->jobLoadButton->setEnabled(false);
        favoriteJobWidget->loadJobPushButton->setEnabled(false);
        expSettings->acquisitionWidget->setEnabled(false);

        connect(&tThread,SIGNAL(currentLine(int)), progressDialog, SLOT(setValue(int)));
        tThread.send(sl);

        QEventLoop loop;
        loop.connect(&tThread, SIGNAL(transferComplete()), &loop, SLOT(quit()));
        loop.exec();
        expSettings->jobLoadButton->setEnabled(true);
        loadJobButton->setEnabled(true);
        favoriteJobWidget->loadJobPushButton->setEnabled(true);
        expSettings->acquisitionWidget->setEnabled(true);
        disconnect(&tThread,SIGNAL(currentLine(int)), progressDialog, SLOT(setValue(int)));

      connect(this,SIGNAL(dataSaveRequest()),this,SLOT(saveFID()));
    disconnect(&tThread,SIGNAL(sent(QString)),this,SLOT(displaySentString(QString)));

    delete progressDialog;

}
//----------------------------------------------------------------------------
void TfpgaTerminal::transferPPG(const QString &s)
{
    if (!device->ppgConnected) return;  // Do nothing if connection to FPGA has not been established.
    if(s.isEmpty()) return;

    if(   (0==QString::compare(s,"G",Qt::CaseInsensitive))
       || (0==QString::compare(s,"RS",Qt::CaseInsensitive)))
    {
        if(!ppgTransferred)
        {
            QMessageBox::warning(this,tr(""),
                                  "Command '"+ s + "' was interrupted, "
                                  + "because a pulse program has not been transferred.\n"
                                  );
            clearConsoleLine();
            return;
        }
    }

    if(s=="g" || s=="G")
    {
      if(!accumulation()) return;
    }


    if(0==QString::compare(s,"RS",Qt::CaseInsensitive))
    {
      if(!repeatScan()) return;
    }

    // We set runQ=false before "I" is sent.
    if(0==QString::compare(s,"I",Qt::CaseInsensitive)) runQ=false;


    emit transferStarted();
    loadJobButton->setEnabled(false);
    expSettings->jobLoadButton->setEnabled(false);
    favoriteJobWidget->loadJobPushButton->setEnabled(false);
    tThread.send(s);
    QEventLoop loop;
    loop.connect(&tThread, SIGNAL(transferComplete()), &loop, SLOT(quit()));
    loop.exec();
    expSettings->jobLoadButton->setEnabled(true);
    loadJobButton->setEnabled(true);
    favoriteJobWidget->loadJobPushButton->setEnabled(true);

    if(0==QString::compare(s,"I",Qt::CaseInsensitive))
    {
        interrupt();
    }


}
//----------------------------------------------------------------------------
bool TfpgaTerminal::initData()
{

    int al2=ppg->receiverInfo.al();
    if(al2>16384)
    {
       QMessageBox::warning(this,tr(""),"al is too large.");
       return false;
    }

    if(expSettings->acquisitionWidget->multipleAcquisitionMode==TAcquisitionWidget::JoinData)
    {
        al2=ppg->receiverInfo.al()*expSettings->acquisitionWidget->multiplicity;
    }
    else if (expSettings->acquisitionWidget->multipleAcquisitionMode==TAcquisitionWidget::JoinAverageData)
    {
        al2=expSettings->acquisitionWidget->multiplicity;
    }

    mutex.lock();
      nmrData->FID.clear();
      nmrData->setAl(al2);

      nmrData->setDW(ppg->receiverInfo.dw());
      for(int k=0; k<ppg->receiverInfo.nc(); k++)
      {
          nmrData->FID.append(new TFID(al2));
          nmrData->FID[nmrData->FID.size()-1]->setDW(ppg->receiverInfo.dw());
          nmrData->FID[nmrData->FID.size()-1]->initialize();
          nmrData->FID[nmrData->FID.size()-1]->dummyCount=ppg->receiverInfo.nd();
          nmrData->FID[nmrData->FID.size()-1]->actualNA=0;

          nmrData->FID[nmrData->FID.size()-1]->xunit=TFID::Second;
          nmrData->FID[nmrData->FID.size()-1]->metricPrefix.setPrefix(TMetricPrefix::Micro);
          nmrData->FID[nmrData->FID.size()-1]->plotMetricPrefix.setPrefix(TMetricPrefix::Milli);
          nmrData->FID[nmrData->FID.size()-1]->setXInitialValue(0.0);
          nmrData->FID[nmrData->FID.size()-1]->setDx(nmrData->FID[nmrData->FID.size()-1]->dw()
                  *TMetricPrefix::Decimal(TMetricPrefix::Micro));
          nmrData->FID[nmrData->FID.size()-1]->setXAxisUnitSymbol("sec");

          if(expSettings->xAxisOptionWidget->isCustom())
          {
              nmrData->FID[nmrData->FID.size()-1]->setCustomXAxis(true);
              nmrData->FID[nmrData->FID.size()-1]->setXAxisLabel(expSettings->xAxisOptionWidget->axisLabel());
              nmrData->FID[nmrData->FID.size()-1]->setXInitialValue(expSettings->xAxisOptionWidget->initialValue());
              nmrData->FID[nmrData->FID.size()-1]->setDx(expSettings->xAxisOptionWidget->increment());
              nmrData->FID[nmrData->FID.size()-1]->plotMetricPrefix.setPrefix(expSettings->xAxisOptionWidget->getPrefix());
              nmrData->FID[nmrData->FID.size()-1]->setXAxisUnitSymbol(expSettings->xAxisOptionWidget->unitSymbol());
          }
          else
          {
              nmrData->FID[nmrData->FID.size()-1]->setXAxisLabel("");
          }
      }
      nmrData->setCurrentFID(0);
    mutex.unlock();

     for(int k=0; k<fidPlotters.size();k++)
     {
  //     qDebug() << fidPlotters[k]->plotter->xini;
           if(fidPlotters[k]->plotter->xini > nmrData->al()-2) fidPlotters[k]->plotter->xini=0;
  //     qDebug() << fidPlotters[k]->plotter->xfin;
           if(fidPlotters[k]->plotter->xfin > nmrData->al()-1) fidPlotters[k]->plotter->xfin=nmrData->al()-1;
     }

    return true;
}
//----------------------------------------------------------------------------
bool TfpgaTerminal::accumulation()
{

    if(!jobQueueWidget->isRunning() && jobQueueWidget->jobListWidget->count()>0)
    {
        showJobWidget();
//        QMessageBox::warning(this,tr(""),"Job queue exists.\n");


        if (QMessageBox::No == QMessageBox::question(this, "",
                                        "Job queue exists. <p>Do you want to cut in and run?",
                                        QMessageBox::No|QMessageBox::Yes)) return false;
        }


    if(runQ) return false;



    if(!checkPath())
    {
        clearConsoleLine();
        return false;
    }

    expSettings->onJobSaveButtonClicked();
    if(!expSettings->ok) return false;

    checkOverride();

//    transferPPG(ppg->receiverInfo.receiverCommands());

    QString fn=expSettings->pathLineEdit->text().trimmed() + '/' + expSettings->nameLineEdit->text() + ".nmrjob";
    jobQueueWidget->currentJobLineEdit->setText(fn);




      if(expSettings->arrayWidget->arrayCheckBox->isChecked())
      {
        expSettings->arrayWidget->arrayCounter.init(ppg);
        updateVariableTable();

        ppg->updatedPPG.clear();
        if(!ppg->updateVariable())
        {
            QMessageBox::warning(this,tr("compile error"),tr("Failed to compile pulse program: ")+ppg->errorMessage);
            return false;
        }
        transferPPG(ppg->updatedPPG);
//        transferPPG(ppg->receiverInfo.receiverCommands());
        ppg->updatedPPG.clear();
        updateAuxParams();
        arrayQ=true;

      } // arrayCheckBoxIsChecked
      else
      {
        emit hideArrayCounterRequest();
      }

      //Change NA if necessary (for separate or multiple acq.)
      if(expSettings->acquisitionWidget->separateDataStorageCheckBox->isChecked())
      {
        expSettings->acquisitionWidget->onSeparateDataStorageOptionChanged();
        ppg->updatedPPG.clear();
      }

      if(expSettings->acquisitionWidget->multipleAcquisitionsCheckBox->isChecked())
      {
          expSettings->acquisitionWidget->onMultipleAcquisitionOptionChanged();

      }

      if(expSettings->xAxisOptionWidget->customAxisRadioButton->isChecked())
          expSettings->xAxisOptionWidget->onApplyButtonClicked();


      //qDebug() << QString(Q_FUNC_INFO) << "initData()...";
      if(!initData()) return false;


        fidThread.localFid->setAL(ppg->receiverInfo.al());
        fidThread.localFid->setDW(ppg->receiverInfo.dw());

        for(int k=0; k<fidPlotters.size(); k++)
        {
            fidPlotters[k]->FIDSelectSpinBox->setMaximum(ppg->receiverInfo.nc());
           fidPlotters[k]->plotter->setFID(nmrData->FID[fidPlotters[k]->FIDSelectSpinBox->value()]);
            if(ppg->receiverInfo.nc()>1) fidPlotters[k]->FIDSelectSpinBox->show();
            else fidPlotters[k]->FIDSelectSpinBox->hide();            
        }
        repeatScanQ=false;

      transferPPG(ppg->receiverInfo.receiverCommands());

      nCopyOperations=0;
      emit scanModeChanged(0); // accum
      emit runStateChanged(TRunStopButton::Running);
      emit disableTransferButton();
      disableSaveButton();
      expSettings->onAccum();
      loadJobButton->setEnabled(false);
      favoriteJobWidget->loadJobPushButton->setEnabled(false);

      displayLogMessage("Sequence started at "
                       + QDateTime::currentDateTime().toString("hh:mm:ss dd MMM yyyy.\n"));

     return true;
}
//----------------------------------------------------------------------------
bool TfpgaTerminal::repeatScan()
{
    if(runQ) return false;

    transferPPG(ppg->receiverInfo.receiverCommands());

    if(expSettings->xAxisOptionWidget->customAxisRadioButton->isChecked())
        expSettings->xAxisOptionWidget->onApplyButtonClicked();


    if(!initData()) return false;

        for(int k=0; k<fidPlotters.size(); k++)
        {
          fidPlotters[k]->FIDSelectSpinBox->setMaximum(1);
          fidPlotters[k]->FIDSelectSpinBox->hide();
          fidPlotters[k]->plotter->setFID(nmrData->FID[nmrData->currentFID()]);
//  qDebug()<<QString(Q_FUNC_INFO)<<"nmrData dx: " << nmrData->FID[nmrData->FID.size()-1]->dx();
//  qDebug()<<QString(Q_FUNC_INFO)<<"plotter dx: " << fidPlotters[k]->plotter->fid->dx();
        }

    repeatScanQ=true;
    nCopyOperations=0;
    emit scanModeChanged(1); // repeat scan
    emit runStateChanged(TRunStopButton::Running);
    emit disableTransferButton();
    expSettings->onRepeatScan();
    loadJobButton->setEnabled(false);
    favoriteJobWidget->loadJobPushButton->setEnabled(false);
    return true;
}
//----------------------------------------------------------------------------
void TfpgaTerminal::interrupt()
{
    DWORD dwRxSize, dwBytesRead;
    char pcBufRead[1];
    FT_STATUS	ftStatus;

    ftStatus=FT_GetQueueStatus(device->receiverHandle, &dwRxSize);

    if(dwRxSize>0)
    {
      QString t;
      if(dwRxSize==1) t="byte"; else t="bytes";
      FPGAStatusTextEdit->insertPlainText(QString::number(dwRxSize) +
                                   " garbage " + t + " left on data-transfer interruption. Discarding... ");
      QString s; s.clear();
      while(dwRxSize>0)
      {
        ftStatus=FT_Read(device->receiverHandle, pcBufRead, 1, &dwBytesRead);
        s.append(pcBufRead[0]);
//          qDebug()<<pcBufRead[0];
        ftStatus=FT_GetQueueStatus(device->receiverHandle, &dwRxSize);
      }
      FPGAStatusTextEdit->insertPlainText("Done.\n");
      FPGAStatusTextEdit->moveCursor(QTextCursor::End);
    }

    if(arrayQ)
    //if(arrayWidget->arrayCounter.arrayCount()>1)
    {
      arrayQ=false;
      expSettings->arrayWidget->arrayCounter.init(ppg);
      ppg->updatedPPG.clear(); updateVariableTable();
      if(!ppg->updateVariable())
      {
          QMessageBox::warning(this,tr("compile error"),tr("Failed to compile pulse program: ")+ ppg->errorMessage);
          return;
      }
      transferPPG(ppg->updatedPPG);
      transferPPG(ppg->receiverInfo.receiverCommands());

      updateAuxParams();

      ppg->updatedPPG.clear();
    }


    mutex.lock();
    fidThread.stop();
    fidThread.wait();
    fidThread.standBy();
    mutex.unlock();
    emit runStateChanged(TRunStopButton::Standby);

    expSettings->onExpFinished();
    jobQueueWidget->onJobAborted();
    loadJobButton->setEnabled(true);
    favoriteJobWidget->loadJobPushButton->setEnabled(true);

    FPGAStatusTextEdit->insertPlainText("Sequence has been interrupted at "
                                        + QDateTime::currentDateTime().toString("hh:mm:ss dd MMM yyyy.\n"));
    FPGAStatusTextEdit->moveCursor(QTextCursor::End);

}
//----------------------------------------------------------------------------
void TfpgaTerminal::updateVariableTable()
{
   expSettings->updateVariableTable();
//    while(expSettings->variableTable->rowCount()>0) expSettings->variableTable->removeRow(0);
//    for(int row=0; row<ppg->variables.size(); row++)
//    {
//        expSettings->variableTable->addRow(ppg->variables.at(row));
//    }

}
//----------------------------------------------------------------------------
void TfpgaTerminal::setAndRunJob(QString qs)
{
    expSettings->jobLoadButton->setEnabled(false); loadJobButton->setEnabled(false);
    favoriteJobWidget->loadJobPushButton->setEnabled(false);
    expSettings->readJob(qs);
    transferPPG("g");
}
//----------------------------------------------------------------------------
void TfpgaTerminal::setJob()
{
    if (!device->ppgConnected) return;  // Do nothing if connection to FPGA has not been established.

    updateVariableTable();

    //setAL(ppg->variables.at(ppg->variableIndex("AL"))->value().toInt());
    fidThread.localFid->setAL(ppg->variables.at(ppg->variableIndex("AL"))->value().toInt());

    setNA(ppg->variables.at(ppg->variableIndex("NA"))->value().toInt());
    setND(ppg->variables.at(ppg->variableIndex("ND"))->value().toInt());
    fidThread.localFid->setDW(ppg->variables.at(ppg->variableIndex("DW"))->value().toDouble()*1000000);

    ppgTransferred=true;
//    expSettings->jobSaveButton->setEnabled(true);
}
//----------------------------------------------------------------------------
void TfpgaTerminal::fromEditor(QString fn)
{
    if (!device->ppgConnected) return;  // Do nothing if connection to FPGA has not been established.
    expSettings->showWindow();
    expSettings->readJob(fn);
}
//----------------------------------------------------------------------------
void TfpgaTerminal::clearConsoleLine()
{
    pulserConsole->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);
    pulserConsole->moveCursor(QTextCursor::StartOfLine,QTextCursor::MoveAnchor);
    pulserConsole->moveCursor(QTextCursor::Right,QTextCursor::MoveAnchor);
    pulserConsole->moveCursor(QTextCursor::End,QTextCursor::KeepAnchor);
    pulserConsole->textCursor().removeSelectedText();
}
//----------------------------------------------------------------------------
void TfpgaTerminal::sendToPulser(QString command)
{
    if (!device->ppgConnected) return;  // Do nothing if connection to FPGA has not been established.

    char c[1];
    FT_STATUS ftStatus;
    DWORD dwBytesWritten;

    for(int k=0; k<command.size(); k++)
    {
        c[0]=command.at(k).toLatin1();
        ftStatus = FT_Write(device->pulserHandle, c, 1, &dwBytesWritten);
    }
          c[0]=QChar(0x0d).toLatin1();
          ftStatus = FT_Write(device->pulserHandle, c, 1, &dwBytesWritten);
    // qDebug()<< "ftStatus: " <<ftStatus;

}

//----------------------------------------------------------------------------
void TfpgaTerminal::displaySentString(QString s)
{

    pulserConsole->insertPlainText(s);
    pulserConsole->moveCursor(QTextCursor::End);

}
//----------------------------------------------------------------------------
void TfpgaTerminal::displayLogMessage(QString s)
{
    FPGAStatusTextEdit->insertPlainText(s);
    FPGAStatusTextEdit->moveCursor(QTextCursor::End);
}
//----------------------------------------------------------------------------
void TfpgaTerminal::copyFID(TFID *f)
{
   // TODO: initialize number of copy operations (do it in transferPPG!)
   // nCopyOperations=0;(transferPPG)
   // check multiplicity

   //mutex.lock();

   if(nmrData->al()!=f->al() &&
     (
      expSettings->acquisitionWidget->multipleAcquisitionMode!=TAcquisitionWidget::JoinData
      || expSettings->acquisitionWidget->multipleAcquisitionMode!=TAcquisitionWidget::JoinAverageData)
      )
    {
       //  nmrData->setAl(f->al());
       // We do not correct al if multiple acquisition is active and its mode is either JoinData or JoinAverageData.
    }

   if(nmrData->dw()!=f->dw())
   {
       nmrData->setDW(f->dw());
   }

   if(ppg->receiverInfo.isPhaseReverseEnabled()) f->phaseReverse();
   if(ppg->receiverInfo.isPhaseRotationEnabled()) f->rotate(ppg->receiverInfo.phaseRotationAngle());

   //fidDomain::process(nmrData->FID[nmrData->currentFID()], fidDomain::TimeDomain);

   if(ppg->receiverInfo.isFFTEnabled())
   {
       TFFT fft;
       fft.setAxisDomain(fidDomain::ToggleDomain);
       fft.process(f);
       fidDomain::process(nmrData->FID[nmrData->currentFID()], fidDomain::FrequencyDomain);
   }

   if(expSettings->acquisitionWidget->replaceRealWithAbsCheckBox->isChecked())
   {
       TReplaceRealWithAbsolute rrwa;
       rrwa.process(f);
   }


     int ofs=0; // offset used for multipleAcquisition (JoinData)
     if(expSettings->acquisitionWidget->multipleAcquisitionsCheckBox->isChecked() &&
        expSettings->acquisitionWidget->multipleAcquisitionMode==TAcquisitionWidget::JoinData)
        // && nmrData->FID[nmrData->currentFID()]->actualNA>0)
     {
         ofs=nCopyOperations*f->al();
         //ofs=nmrData->FID[nmrData->currentFID()]->al();

//         if((ofs+f->al()) > nmrData->FID[nmrData->currentFID()]->al()) //=> do the following line.
//         {
//           nmrData->setAl(ofs+f->al());
//         }

//         for(int k=0; k<fidPlotters.size();k++)
//         {
//           fidPlotters[k]->plotter->xini=0;
//           fidPlotters[k]->plotter->xfin=expSettings->acquisitionWidget->multiplicity-1;
//         }

     }

     if(expSettings->acquisitionWidget->multipleAcquisitionsCheckBox->isChecked() &&
        expSettings->acquisitionWidget->multipleAcquisitionMode==TAcquisitionWidget::JoinAverageData)
     {
         ofs=nCopyOperations; // *1
         //if((ofs+1) > nmrData->FID[nmrData->currentFID()]->al()) nmrData->setAl(ofs+1);

//         for(int k=0; k<fidPlotters.size();k++)
//         {
//           fidPlotters[k]->plotter->xini=0;
//           fidPlotters[k]->plotter->xfin=expSettings->acquisitionWidget->multiplicity-1;
//         }
     }

     double scale=ppg->receiverInfo.dfScale();

     if(expSettings->acquisitionWidget->multipleAcquisitionsCheckBox->isChecked() &&
        expSettings->acquisitionWidget->multipleAcquisitionMode==TAcquisitionWidget::JoinAverageData)
     {
         if(repeatScanQ)
         {
             nmrData->FID[nmrData->currentFID()]->real->sig[ofs] = scale*f->real->average();
             nmrData->FID[nmrData->currentFID()]->imag->sig[ofs] = scale*f->imag->average();
//             nmrData->FID[nmrData->currentFID()]->real->sig[ofs] = scale*f->real->sum();
//             nmrData->FID[nmrData->currentFID()]->imag->sig[ofs] = scale*f->imag->sum();
         }
         else // accum
         {
           nmrData->FID[nmrData->currentFID()]->real->sig[ofs] += scale*f->real->average();
           nmrData->FID[nmrData->currentFID()]->imag->sig[ofs] += scale*f->imag->average();
         }
     }
     else
     {
       for(int k=0; k<f->al(); k++)
       {
         if(repeatScanQ)
         {
             nmrData->FID[nmrData->currentFID()]->real->sig[ofs+k] = scale*f->real->sig[k];
             nmrData->FID[nmrData->currentFID()]->imag->sig[ofs+k] = scale*f->imag->sig[k];

         //    qDebug() << QString(Q_FUNC_INFO) << "scale: "<< scale << "ofs:" << ofs << "k: " << k
         //             << nmrData->FID[nmrData->currentFID()]->real->sig[ofs+k] //= scale*f->real->sig[k];
         //             << nmrData->FID[nmrData->currentFID()]->imag->sig[ofs+k]; //= scale*f->imag->sig[k];
         }
         else // accum
         {
           nmrData->FID[nmrData->currentFID()]->real->sig[ofs+k] += scale*f->real->sig[k];
           nmrData->FID[nmrData->currentFID()]->imag->sig[ofs+k] += scale*f->imag->sig[k];
         }
       }
     }

     nmrData->FID[nmrData->currentFID()]->updateAbs();
   //  qDebug() << QString(Q_FUNC_INFO) <<nmrData->currentFID() <<nmrData->FID[nmrData->currentFID()]->xAxisUnitString();

   // TODO: increment number of copy operations
   nCopyOperations++;
   //qDebug() << QString(Q_FUNC_INFO) << "nCopyOperations++";

   //qDebug() << expSettings->acquisitionWidget->multiplicity;

   if(nCopyOperations==expSettings->acquisitionWidget->multiplicity)
   {
       nCopyOperations=0;
       nmrData->FID[nmrData->currentFID()]-> actualNA+=ppg->receiverInfo.step();//actualNA++;

      // qDebug() << "actualNA: "<< nmrData->FID[nmrData->currentFID()]->actualNA;

       if(nmrData->FID[nmrData->currentFID()]->actualNA>ppg->variables.at(ppg->variableIndex("NA"))->numeric().toInt())
          nmrData->FID[nmrData->currentFID()]->actualNA=ppg->variables.at(ppg->variableIndex("NA"))->numeric().toInt();
   }
   // then increment actualNA (just below), and set nCopyOperations=0

/*
   if(expSettings->xAxisOptionWidget->isCustom())
   {
       nmrData->FID[nmrData->currentFID()]->setCustomXAxis(true);
       nmrData->FID[nmrData->currentFID()]->setXAxisLabel(expSettings->xAxisOptionWidget->axisLabel());
       nmrData->FID[nmrData->currentFID()]->setXInitialValue(expSettings->xAxisOptionWidget->initialValue());
       nmrData->FID[nmrData->currentFID()]->setDx(expSettings->xAxisOptionWidget->increment());
       nmrData->FID[nmrData->currentFID()]->plotMetricPrefix.setPrefix(expSettings->xAxisOptionWidget->getPrefix());
       nmrData->FID[nmrData->currentFID()]->setXAxisUnitSymbol(expSettings->xAxisOptionWidget->unitSymbol());

   }
   else
   {
       nmrData->FID[nmrData->currentFID()]->setXAxisLabel("");
   }
*/

//mutex.unlock();

   //qDebug() << QString(Q_FUNC_INFO) << " 2";

   if(nmrData->FID[nmrData->currentFID()]->isEmpty()) nmrData->FID[nmrData->currentFID()]->setEmpty(false);

   emit copyComplete();

   //qDebug() << QString(Q_FUNC_INFO) << "copyComplete emitted.";

   nmrData->toggleCurrentFID(); // for hypercomplex acq.

//   qDebug() << "dummyCount:" << nmrData->FID[nmrData->currentFID()]->dummyCount;

   if(nmrData->FID[nmrData->currentFID()]->dummyCount>0)
   {
     for(int k=0; k<ppg->receiverInfo.nc(); k++) // we cover hypercomplex acq cases
     {
       nmrData->FID[k]->dummyCount-=ppg->receiverInfo.step();
     }
        // qDebug() << QString(Q_FUNC_INFO) << "dummyCount-=step"
        //          << " dummyCount:" << nmrData->FID[nmrData->currentFID()]->dummyCount
        //          << " step:" << ppg->receiverInfo.step()
        //          << " currentFID(): " << nmrData->currentFID()
        //          << " / " << nmrData->FID.size();

     // if(nmrData->FID[nmrData->currentFID()]->dummyCount==0 && nmrData->currentFID()==nmrData->FID.size()-1)
      if(nmrData->FID[nmrData->currentFID()]->dummyCount==0)
      {
         // qDebug() << "dummy scan finished.";
          nmrData->initialize();
          nmrData->setCurrentFID(0);
      }
   }
   else if((!repeatScanQ)
           && nmrData->FID[nmrData->FID.size()-1]->actualNA==ppg->receiverInfo.na())
   {
       emit accumComplete();
   }
}
//----------------------------------------------------------------------------
void TfpgaTerminal::checkOverride()
{
    QString fn=expSettings->pathLineEdit->text().trimmed() + '/'
                   + expSettings->nameLineEdit->text() + '/'
                   + expSettings->nameLineEdit->text();
    QFileInfo fi;
    fi.setFile(fn);
    QString base = fi.baseName();
    QString path = fi.absolutePath()+'/';


//    if(QFile::exists(sm2p) || QFile::exists(sm2d))
//    {
//      if (QMessageBox::No == QMessageBox::question(this, "Override Confirmation?",
//                                 "File already exists. <p>Do you want to override?",
//                                 QMessageBox::No|QMessageBox::Yes)) ok=false;
//    }
//    else
//    {
  //  qDebug()<<"!!"<<path+base+"_array.sm2d";

    if(QFile::exists(path+base+"_array.sm2d")) QFile::remove(path+base+"_array.sm2d");
    if(QFile::exists(path+base+"_array.sm2p")) QFile::remove(path+base+"_array.sm2p");
    if(QFile::exists(path+base+"_array.opd")) QFile::remove(path+base+"_array.opd");
    if(QFile::exists(path+base+"_array.opp")) QFile::remove(path+base+"_array.opp");


  //  if(QFile::exists(path+base+"_unfinished.sm2d")) QFile::remove(path+base+"_unfinished.sm2d");
  //  if(QFile::exists(path+base+"_unfinished.sm2p")) QFile::remove(path+base+"_unfinished.sm2p");
//    }
}
//----------------------------------------------------------------------------
bool TfpgaTerminal::checkPath()
{
    bool ok=true;
    QString fn=expSettings->pathLineEdit->text().trimmed();
    QDir dir;
    if(!dir.exists(fn))
    {
       if(!dir.mkdir(fn))
       {
         QMessageBox::warning(this,tr("dir error"),tr("directory does not exist."));
         ok=false;
       }
    }

    if(expSettings->nameLineEdit->text().trimmed().isEmpty())
    {
        QMessageBox::warning(this,tr("filename error"),tr("filename is empty."));
        ok=false;
    }

    return ok;
}
//----------------------------------------------------------------------------
void TfpgaTerminal::onSaveButtonClicked()
{

    if(expSettings->pathLineEdit->text().trimmed().isEmpty())
    {
        QMessageBox::warning(this,tr("error"),tr("Path is empty."));
        return;
    }
    if(expSettings->nameLineEdit->text().trimmed().isEmpty())
    {
        QMessageBox::warning(this,tr("error"),tr("Filename is empty."));
        return;
    }

    QString fn=expSettings->pathLineEdit->text().trimmed() + '/'
               + expSettings->nameLineEdit->text() + '/'
               + expSettings->nameLineEdit->text();

    QFileInfo fi;
    fi.setFile(fn);
    QString base = fi.baseName();
    QString path = fi.absolutePath()+'/';
    QString sm2d = path+base+"_unfinished.sm2d";
    QString sm2p = path+base+"_unfinished.sm2p";
    QString asm2d = path+base+"_array.sm2d";
    QString opd = path+base+"_unfinished.opd";
    QString opp = path+base+"_unfinished.opp";
    QString aopd = path+base+"_array.opd";

    nmrData->comments=expSettings->commentTextEdit->toPlainText().split(QChar::ParagraphSeparator);

    QMutexLocker locker(&mutex);

    if(!arrayQ)
    {

        if(!QFile::exists(sm2p)) nmrData->Writesm2pFile(sm2p);
        if(!QFile::exists(opp)) nmrData->Writesm2pFile(opp);
        // We do not overwrite the parameter file, to retain any modifications made by other applications,
        // such as TakeNMR.

        if(!nmrData->Writesm2dFile(sm2d))
        {
            QMessageBox::warning(this,tr(""),tr("<p>Failed to save data: ") + sm2d );
            return;
        }

        if(!nmrData->WriteopdFile(opd))
        {
            QMessageBox::warning(this,tr(""),tr("<p>Failed to save data: ") + opd );
            return;
        }
        expSettings->onJobSaveButtonClicked();
        if(!expSettings->ok) return;

        disableSaveButton();
        return;
    }


    if(QFile::exists(opd)) QFile::remove(opd);
    if(QFile::exists(aopd))
    {
        QFile::copy(aopd,opd);
        nmrData->WriteopdFile(opd,QIODevice::Append);
        if(!QFile::exists(opp)) nmrData->WriteoppFile(opp);
        // We do not overwrite the parameter file, to retain any modifications made by other applications,
        // such as TakeNMR.
    }
    else
    {
        nmrData->WriteopdFile(opd);
        if(!QFile::exists(opp)) nmrData->WriteoppFile(opp);
        // We do not overwrite the parameter file, to retain any modifications made by other applications,
        // such as TakeNMR.
    }



        if(QFile::exists(sm2d)) QFile::remove(sm2d);
        if(QFile::exists(asm2d))
        {
            QFile::copy(asm2d,sm2d);
            nmrData->Writesm2dFile(sm2d,QIODevice::Append);
            if(!QFile::exists(sm2p)) nmrData->Writesm2pFile(sm2p);
            // We do not overwrite the parameter file, to retain any modifications made by other applications,
            // such as TakeNMR.
        }
        else
        {
            nmrData->Writesm2dFile(sm2d);
            if(!QFile::exists(sm2p)) nmrData->Writesm2pFile(sm2p);
            // We do not overwrite the parameter file, to retain any modifications made by other applications,
            // such as TakeNMR.
        }
    disableSaveButton();

}
//----------------------------------------------------------------------------
void TfpgaTerminal::saveFID()
{
    QMutexLocker locker(&mutex);
    QString fn=expSettings->pathLineEdit->text().trimmed();

  #if defined(__APPLE__)
    if(!fn.endsWith('/')) fn+='/';
  #elif defined(_WIN32)
    if(!fn.endsWith('/')) fn+='/';
  #endif

    expSettings->onJobSaveButtonClicked();
    if(!expSettings->ok) return;

    nmrData->comments=expSettings->commentTextEdit->toPlainText().split(QChar::ParagraphSeparator);
    fn += expSettings->nameLineEdit->text() + '/' + expSettings->nameLineEdit->text();
//    if(!nmrData->Writesm2Files(fn))
    if(!nmrData->WriteopFiles(fn))
    {
        QMessageBox::warning(this,tr(""),
                           tr("<p>Failed to save data. "
                              ));
        return;
    }


}
//----------------------------------------------------------------------------
void TfpgaTerminal::setCarrierFreq(double f)
{
    nmrData->setSF1(f);
}
//----------------------------------------------------------------------------
void TfpgaTerminal::setAR(int ar)
{
    if (!device->ppgConnected) return;  // Do nothing if connection to FPGA has not been established.
    ppg->receiverInfo.setAR(ar);
    transferPPG(QStringList()<<TreceiverCommand::updateAR(ar));
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
