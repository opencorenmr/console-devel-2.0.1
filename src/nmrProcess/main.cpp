#include "mainwindow.h"
#include <QApplication>

#define VERSION_MAJOR 2
#define VERSION_MINOR 0
#define VERSION_PATCH 17
#define BUILD_NUMBER 20231028
#define RELEASE_YEAR 2023
#define RELEASE_MONTH "October"
#define RELEASE_DATE 28

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle(QObject::tr("Opencore NMR Process ")
                     + QString::number(VERSION_MAJOR)
                     +"."+QString::number(VERSION_MINOR)
                     +"."+QString::number(VERSION_PATCH)
                  //   + QObject::tr(" #") + QString::number(BUILD_NUMBER)
                     + QObject::tr(" (")
                     + QString::number(RELEASE_DATE) + QObject::tr(" ")
                     + QObject::tr(RELEASE_MONTH)  + QObject::tr(" ")
                     + QString::number(RELEASE_YEAR) + QObject::tr(")")
                     );

    w.show();

    w.processPanel->processOperations->setMajorVersion(VERSION_MAJOR);
    w.processPanel->processOperations->setMinorVersion(VERSION_MINOR);
    w.processPanel->processOperations->setPatchVersion(VERSION_PATCH);



    return a.exec();
}

