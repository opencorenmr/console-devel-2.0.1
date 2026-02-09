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
    toggleParams.cpp \
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
    dataProcess/3Drearrangement.cpp \
    dataProcess/3DrearrangementWidget.cpp \
    dataProcess/arraySum.cpp \
    dataProcess/flatten.cpp \
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
    dataProcess/nutationWidget.cpp \
    dataProcess/apodization.cpp \
    dataProcess/applyMode.cpp \
    dataProcess/export2dp.cpp \
    dataProcess/covariance.cpp \
    dataProcess/covarianceWidget.cpp \
    dataProcess/svd.cpp \
    dataProcess/peakPickWidget.cpp \
    dataProcess/peakPick.cpp \
    dataProcess/interpolate.cpp \
    dataProcess/interpolateWidget.cpp \
    dataProcess/imageGenWidget.cpp \
    dataProcess/cartesianMap.cpp \
    dataProcess/cartesianMapWidget.cpp \
    dataProcess/cartesianMapWeight.cpp \
    dataProcess/exportAbsWidget.cpp \
    dataProcess/reshape.cpp



linux: SOURCES += gpib488console.cpp

HEADERS  += mainwindow.h \
    codeeditor.h \
    highlighter.h \
    gates.h \
    inifile.h \
    toggleParams.h \
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
    dataProcess/3Drearrangement.h \
    dataProcess/3DrearrangementWidget.h \
    dataProcess/arraySum.h \
    dataProcess/flatten.h \
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
    dataProcess/nutationWidget.h \
    dataProcess/apodization.h \
    dataProcess/applyMode.h \
    dataProcess/export2dp.h \
    dataProcess/covariance.h \
    dataProcess/covarianceWidget.h \
    dataProcess/svd.h \
    dataProcess/peakPickWidget.h \
    dataProcess/peakPick.h \
    dataProcess/interpolate.h \
    dataProcess/interpolateWidget.h \
    dataProcess/imageGenWidget.h \
    dataProcess/cartesianMap.h \
    dataProcess/cartesianMapWidget.h \
    dataProcess/cartesianMapWeight.h \
    dataProcess/exportAbsWidget.h \
    dataProcess/reshape.h


linux: HEADERS += gpib488console.h

#mac: QMAKE_MAC_SDK = macosx10.14
mac: QMAKE_APPLE_DEVICE_ARCHS=x86_64 arm64
mac: ICON = images/opencoreNMR.icns
mac: INCLUDEPATH += /usr/local/include
mac: LIBS += -L/usr/local/lib -lftd2xx
mac: LIBS += -framework Accelerate
#mac:LIBS += -L/usr/local/lib -llapacke -lrefblas -lcblas

# (Windows:) We assume that the downloaded ftd2xx.lib and ftd2xx.h (from FTDI)
#            have been copied to c:\lib
win32: LIBS += -L"c:\lib" -lftd2xx #-lgpib488
win32: INCLUDEPATH += c:\lib
win32: LIBS += -L"c:\lib" -llapack -llapacke -lrefblas -lcblas -lm -lgfortran
win32: RC_ICONS = images/opencoreNMR.ico

linux: INCLUDEPATH += /usr/local/include/gpib /usr/local/include
linux: LIBS += -L/usr/local/lib -lftd2xx -lgpib
linux: LIBS += -L/usr/local/lib -llapack -llapacke -lrefblas -lcblas -lgfortran -lm

RESOURCES += \
    opencoreNMR.qrc

OTHER_FILES +=

DISTFILES +=
