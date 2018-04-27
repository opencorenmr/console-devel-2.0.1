#-------------------------------------------------
#
# Project created by QtCreator 2014-09-08T15:56:25
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qd
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ../fid.cpp \
    ../metricPrefix.cpp \
    ../intToHex.cpp \
    ../digitalfilter.cpp \
    ../inifile.cpp \
    ../plotter.cpp \
    ../dataProcess/fft.cpp \
    ../dataProcess/processElement.cpp \
    ../dataProcess/fidDomain.cpp

HEADERS  += mainwindow.h \
    ../fid.h \
    ../metricPrefix.h \
    ../intToHex.cpp \
    ../digitalfilter.h \
    ../inifile.h \
    ../plotter.h \
    ../dataProcess/fft.h \
    ../dataProcess/processElement.h \
    ../dataProcess/fidDomain.h


mac: QMAKE_MAC_SDK = macosx10.13

#RESOURCES +=
