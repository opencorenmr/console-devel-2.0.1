#include "mainwindow.h"
#include <QApplication>

#define VERSION_NUMBER 2
#define VERSION_NUMBER2 0
#define VERSION_NUMBER3 2
#define BUILD_NUMBER 20210212
#define RELEASE_YEAR 2021
#define RELEASE_MONTH "Feb"
#define RELEASE_DATE 12

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle(QObject::tr("Opencore NMR Process ") + QString::number(VERSION_NUMBER)
                     +"."+QString::number(VERSION_NUMBER2)
                     +"."+QString::number(VERSION_NUMBER3)
                  //   + QObject::tr(" #") + QString::number(BUILD_NUMBER)
                     + QObject::tr(" (")
                     + QString::number(RELEASE_DATE) + QObject::tr(" ")
                     + QObject::tr(RELEASE_MONTH)  + QObject::tr(" ")
                     + QString::number(RELEASE_YEAR) + QObject::tr(")")
                     );

    w.show();

    return a.exec();
}

