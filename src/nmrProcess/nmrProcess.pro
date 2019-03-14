#-------------------------------------------------
#
# Project created by QtCreator 2017-06-28T14:50:10
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = nmrProcess
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ../fid.cpp \
    ../plotter.cpp \
    ../metricPrefix.cpp \
    ../dataProcess/axisFormatWidget.cpp \
    ../dataProcess/axisStyle.cpp \
    ../dataProcess/fft.cpp \
    ../dataProcess/fidDomain.cpp \
    ../dataProcess/ifft.cpp \
    ../dataProcess/phase.cpp \
    ../dataProcess/phaseWidget.cpp \
    ../dataProcess/processElement.cpp \
    ../dataProcess/processOperations.cpp \
    ../dataProcess/processPanelWidget.cpp \
    ../dataProcess/replaceRealWithAbsolute.cpp \
    ../dataProcess/transformWidget.cpp \
    ../dataProcess/addCutPoints.cpp \
    ../dataProcess/addCutPointsWidget.cpp \
    ../dataProcess/2DprocessWidget.cpp \
    ../dataProcess/transpose.cpp \
    ../dataProcess/hyperComplex.cpp \
    ../dataProcess/exportWidget.cpp \
    ../dataProcess/apodization.cpp \
    ../dataProcess/apodizationWidget.cpp \
    ../dataProcess/processBase.cpp \
    ../dataProcess/applyMode.cpp \
    ../dataProcess/createFIDWidget.cpp \
    ../dataProcess/fidMath.cpp \
    ../dataProcess/fidMathWidget.cpp \
    ../dataProcess/fidSimulationWidget.cpp \
    ../dataProcess/export2dp.cpp \
    ../dataProcess/covariance.cpp \
    ../dataProcess/covarianceWidget.cpp \
    ../dataProcess/svd.cpp \
    ../dataProcess/peakPickWidget.cpp \
    ../dataProcess/peakPick.cpp



HEADERS  += mainwindow.h \
    ../fid.h \
    ../plotter.h \
    ../metricPrefix.h \
    ../dataProcess/axisFormatWidget.h \
    ../dataProcess/axisStyle.h \
    ../dataProcess/fft.h \
    ../dataProcess/fidDomain.h \
    ../dataProcess/ifft.h \
    ../dataProcess/phase.h \
    ../dataProcess/phaseWidget.h \
    ../dataProcess/processElement.h \
    ../dataProcess/processBase.h \
    ../dataProcess/processOperations.h \
    ../dataProcess/processPanelWidget.h \
    ../dataProcess/replaceRealWithAbsolute.h \
    ../dataProcess/transformWidget.h \
    ../dataProcess/addCutPoints.h \
    ../dataProcess/addCutPointsWidget.h \
    ../dataProcess/2DprocessWidget.h \
    ../dataProcess/transpose.h \
    ../dataProcess/hyperComplex.h \
    ../dataProcess/exportWidget.h \
    ../dataProcess/apodization.h \
    ../dataProcess/apodizationWidget.h \
    ../dataProcess/applyMode.h \
    ../dataProcess/createFIDWidget.h \
    ../dataProcess/fidMath.h \
    ../dataProcess/fidMathWidget.h \
    ../dataProcess/fidSimulationWidget.h \
    ../dataProcess/export2dp.h \
    ../dataProcess/covariance.h \
    ../dataProcess/covarianceWidget.h \
    ../dataProcess/svd.h \
    ../dataProcess/peakPickWidget.h \
    ../dataProcess/peakPick.h


win32: RC_ICONS = ../images/nmrProcess.ico

mac: QMAKE_MAC_SDK = macosx10.14
mac: ICON = ../images/nmrProcess.icns

RESOURCES += \
    nmrprocess.qrc

linux: LIBS += -L/usr/local/lib -llapack -llapacke -lblas -lcblas -lgfortran -lm

mac: LIBS += -framework Accelerate

win32: LIBS += -L"c:\lib" -llapack -llapacke -lblas -lcblas -lm
win32: INCLUDEPATH += c:/lib
