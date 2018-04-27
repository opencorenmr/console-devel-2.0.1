#-------------------------------------------------
#
# Project created by QtCreator 2014-01-17T14:50:56
#
#-------------------------------------------------

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = nmr
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    codeeditor.cpp \
    highlighter.cpp \
    gates.cpp \
    inifile.cpp \
    variables.cpp \
    pulseProgram.cpp \
    TxRxThread.cpp \
    console.cpp \
    plotter.cpp \
    fid.cpp \
    digitalfilter.cpp \
    intToHex.cpp \
    rcvrcom.cpp \
    ppgEdit.cpp \
    fpgaTerminal.cpp \
    variableedit.cpp \
    dataReceiver.cpp \
    dataProcess.cpp \
    runStopButton.cpp \
    array.cpp \
    arrayCounter.cpp \
    acquisition.cpp \
    acquisitionWidget.cpp \
    job.cpp \
    expSettings.cpp \
    metricPrefix.cpp \
    dataProcess/processElement.cpp \
    dataProcess/fft.cpp \
    dataProcess/ifft.cpp \
    dataProcess/phase.cpp \
    dataProcess/fidDomain.cpp \
    dataProcess/exportWidget.cpp \
    dataProcess/replaceRealWithAbsolute.cpp \
    dataProcess/processOperations.cpp \
    dataProcess/phaseWidget.cpp \
    dataProcess/processPanelWidget.cpp \
    dataProcess/transformWidget.cpp \
    dataProcess/axisStyle.cpp \
    dataProcess/hyperComplex.cpp \
    dataProcess/axisFormatWidget.cpp \
    dataProcess/addCutPoints.cpp \
    dataProcess/addCutPointsWidget.cpp \
    dataProcess/2DprocessWidget.cpp \
    dataProcess/transpose.cpp \
    dataProcess/apodizationWidget.cpp \
    dataProcess/processBase.cpp \
    dataProcess/createFIDWidget.cpp \
    dataProcess/fidMath.cpp \
    dataProcess/fidMathWidget.cpp \
    dataProcess/fidSimulationWidget.cpp \
    dataProcess/apodization.cpp \
    dataProcess/applyMode.cpp \
    dataProcess/export2dp.cpp \
    dataProcess/covariance.cpp \
    dataProcess/covarianceWidget.cpp \
    dataProcess/svd.cpp


linux: SOURCES += gpib488console.cpp

HEADERS  += mainwindow.h \
    codeeditor.h \
    highlighter.h \
    gates.h \
    inifile.h \
    variables.h \
    pulseProgram.h \
    TxRxThread.h \
    console.h \
    plotter.h \
    fid.h \
    digitalfilter.h \
    intToHex.h \
    rcvrcom.h \
    ppgEdit.h \
    fpgaTerminal.h \
    variableedit.h \
    dataReceiver.h \
    dataProcess.h \
    runStopButton.h \
    array.h \
    arrayCounter.h \
    acquisition.h \
    acquisitionWidget.h \
    job.h \
    expSettings.h \
    metricPrefix.h \
    dataProcess/processElement.h \
    dataProcess/ifft.h \
    dataProcess/fft.h \
    dataProcess/phase.h \
    dataProcess/fidDomain.h \
    dataProcess/replaceRealWithAbsolute.h \
    dataProcess/processOperations.h \
    dataProcess/phaseWidget.h \
    dataProcess/processPanelWidget.h \
    dataProcess/transformWidget.h \
    dataProcess/axisStyle.h \
    dataProcess/axisFormatWidget.h \
    dataProcess/exportWidget.h \
    dataProcess/hyperComplex.h \
    dataProcess/processBase.h \
    dataProcess/addCutPoints.h \
    dataProcess/addCutPointsWidget.h \
    dataProcess/2DprocessWidget.h \
    dataProcess/transpose.h \
    dataProcess/apodizationWidget.h \
    dataProcess/createFIDWidget.h \
    dataProcess/fidMath.h \
    dataProcess/fidMathWidget.h \
    dataProcess/fidSimulationWidget.h \
    dataProcess/apodization.h \
    dataProcess/applyMode.h \
    dataProcess/export2dp.h \
    dataProcess/covariance.h \
    dataProcess/covarianceWidget.h \
    dataProcess/svd.h


linux: HEADERS += gpib488console.h

mac: QMAKE_MAC_SDK = macosx10.13
mac: ICON = images/opencoreNMR.icns

mac:INCLUDEPATH += /usr/local/include
mac:LIBS += -L/usr/local/lib -lftd2xx
mac: LIBS += -framework Accelerate

win32: LIBS += -L"c:\lib" -lftd2xx #-lgpib488
win32: INCLUDEPATH += c:\lib
# (Windows:) We assume that the downloaded ftd2xx.lib and ftd2xx.h (from FTDI)
#            have been copied to c:\lib

win32: RC_ICONS = images/opencoreNMR.ico

linux: INCLUDEPATH += /usr/local/include/gpib /usr/local/include
linux: LIBS += -L/usr/local/lib -lftd2xx -lgpib

linux: LIBS += -L/usr/local/lib -llapack -llapacke -lblas -lcblas -lgfortran -lm

win32: LIBS += -L"c:\lib" -llapack -llapacke -lblas -lcblas -lm
win32: INCLUDEPATH += c:/lib


RESOURCES += \
    opencoreNMR.qrc

OTHER_FILES +=

DISTFILES +=
