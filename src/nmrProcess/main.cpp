#include "mainwindow.h"
#include <QApplication>

#define VERSION_NUMBER 2
#define VERSION_NUMBER2 0
#define VERSION_NUMBER3 1
#define BUILD_NUMBER 20200617
#define RELEASE_YEAR 2020
#define RELEASE_MONTH "Jun"

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle(QObject::tr("Opencore NMR Process ") + QString::number(VERSION_NUMBER)
                     +"."+QString::number(VERSION_NUMBER2)
                     +"."+QString::number(VERSION_NUMBER3)
                  //   + QObject::tr(" #") + QString::number(BUILD_NUMBER)
                     + QObject::tr(" (") + QString::number(RELEASE_YEAR) + QObject::tr(" ")
                     + QObject::tr(RELEASE_MONTH) + QObject::tr(")")
                     );

    w.show();

    return a.exec();
}

