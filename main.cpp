#include "mainwindow.h"
#include <QApplication>
#include <QSplashScreen>
#include <QFileInfo>
#include <QMessageBox>




int main(int argc, char *argv[])
{
  //  qDebug()<<QString(Q_FUNC_INFO)<<"1";
    QApplication a(argc, argv);

  //  qDebug()<<QString(Q_FUNC_INFO)<<"2";
    QSplashScreen *splash = new QSplashScreen;
//    Qt::Alignment bottomCenter = Qt::AlignCenter | Qt::AlignBottom;
    splash->setPixmap(QPixmap(":/images/splashScreen.png"));
    splash->showMessage(QObject::tr("Initial setup..."));
    splash->show();
  //  qDebug()<<QString(Q_FUNC_INFO)<<"3";

    //QTimer timer;
    //timer.setSingleShot(true);
    //loop.connect(&timer,SIGNAL(timeout()), &loop, SLOT(quit()));
    //timer.start(1000);
    //loop.exec();

    MainWindow w;
    w.setWindowTitle(QObject::tr("Opencore NMR ") + QString::number(VERSION_NUMBER)
                     +"."+QString::number(VERSION_NUMBER2)
                     +"."+QString::number(VERSION_NUMBER3)
                     + QObject::tr(" Build #") + QString::number(BUILD_NUMBER));

//    if(w.fpgaTerminal->openOnStartupCheckBox->isChecked())
    if(true)
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
            splash->finish(&w);
            QMessageBox::warning(&w,QObject::tr(""),
                               QObject::tr("<p>Built-in VCP driver detected."
                                  "You need to disable it to open USB connection. "
                                  "Open a terminal and execute the following commands:"
                                  "<p> sudo kextunload -bundle com.apple.driver.AppleUSBFTDI"
                                  ));
          //  return;
        }
        else
        {
          QObject::connect(w.fpgaTerminal->device,SIGNAL(sendMessage(QString)),splash,SLOT(showMessage(QString)));
          w.fpgaTerminal->FTDIOpen();
        }
      #endif
      #if !defined(__APPLE__)
        QObject::connect(w.fpgaTerminal->device,SIGNAL(sendMessage(QString)),splash,SLOT(showMessage(QString)));
        w.fpgaTerminal->FTDIOpen();
      #endif
    }
    else
    {
        QThread::msleep(1000);
    }

    w.show();

    splash->finish(&w);
    delete splash;
    
    return a.exec();
}
