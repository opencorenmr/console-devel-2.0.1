#include "mainwindow.h"
#include <QApplication>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QDir>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

  path0=QDir::homePath()+"/.opencorenmr";

    processPanel = new TProcessPanelWidget;
    processPanel->setDevicePixelRatio(qApp->devicePixelRatio());
    processPanel->setSettingDirPath(QDir::homePath());
    processPanel->setWindowFlags(Qt::Dialog);

    // A layout cannot be set on the MainWindow.
    // Accordingly a central widget is set.  (2014.1.17 Takeda)
    plotterWidget = new QWidget;
    plotterWidget->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowMinMaxButtonsHint );
    plotterWidget->setWindowTitle(tr("Opencore NMR Process Plotter"));

//    this->setCentralWidget(centralWidget);
    this->setCentralWidget(processPanel);
    this->setWindowTitle(tr("Opencore NMR Process Control Panel"));

    QHBoxLayout *layout1 = new QHBoxLayout(plotterWidget);
    layout1->setMargin(0);
    layout1->addWidget(processPanel->plotters);

//    processPanel->show();
    plotterWidget->show();
    hasCloseEventCalled=false;
    loadSettings();

}

MainWindow::~MainWindow()
{
    saveSettings();
}

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
           plotterWidget->close();



           hasCloseEventCalled=true;
           event->accept();
       }
}

void MainWindow::loadSettings()
{
    QSettings settings(path0 + "/nmrProcess.ini", QSettings::IniFormat);

    settings.beginGroup("ProcessPanel");
      this->move(settings.value("position",QPoint(0,0)).toPoint());
    settings.endGroup();

    settings.beginGroup("Plotter");
      plotterWidget->move(settings.value("position",QPoint(400,200)).toPoint());
      plotterWidget->resize(settings.value("size",QSize(600,400)).toSize());
    settings.endGroup();

    QString defaultPath1;
    defaultPath1=QDir::homePath();
    settings.beginGroup("Path");
      processPanel->processFileWidget->setDataFilePath(settings.value("dataFilePath",defaultPath1).toString());
      processPanel->setProcessFilePath(settings.value("processFilePath",defaultPath1).toString());
    settings.endGroup();

}

void MainWindow::saveSettings()
{
    QSettings settings(path0 + "/nmrProcess.ini", QSettings::IniFormat);

    settings.beginGroup("ProcessPanel");
    #if defined(__linux__)
      settings.setValue("position", pos() + QPoint(0,23));
    #else
      settings.setValue("position", pos());
    #endif
    settings.endGroup();

    settings.beginGroup("Plotter");
    #if defined(__linux__)
      settings.setValue("position", plotterWidget->pos()+QPoint(0,23));
    #else
      settings.setValue("position", plotterWidget->pos());
    #endif
    settings.setValue("size",plotterWidget->size());
    settings.endGroup();

    settings.beginGroup("Path");
      settings.setValue("dataFilePath", processPanel->processFileWidget->dataFilePath());
      settings.setValue("processFilePath",processPanel->processFilePath());
    settings.endGroup();
}
