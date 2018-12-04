#include "mainwindow.h"
#include <QApplication>

#define VERSION_NUMBER 2
#define VERSION_NUMBER2 0
#define VERSION_NUMBER3 0
#define BUILD_NUMBER 204
#define RELEASE_YEAR 2017
#define RELEASE_MONTH October

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle(QObject::tr("Opencore NMR Process ") + QString::number(VERSION_NUMBER)
                     +"."+QString::number(VERSION_NUMBER2)
                     +"."+QString::number(VERSION_NUMBER3)
                     + QObject::tr(" Build #") + QString::number(BUILD_NUMBER));

    w.show();

    return a.exec();
}
