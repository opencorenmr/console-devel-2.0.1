#ifndef TPROCESSPANELWIDGET_H
#define TPROCESSPANELWIDGET_H

#include <QWidget>
#include <QStackedLayout>
#include <QGridLayout>
#include <QComboBox>
#include <QSpinBox>
#include <QListWidget>
#include <QPlainTextEdit>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QLabel>
#include <QPushButton>

#include "../plotter.h"
#include "phaseWidget.h"
#include "processOperations.h"
#include "../fid.h"
#include "transformWidget.h"
#include "axisFormatWidget.h"
#include "../console.h"
#include "addCutPointsWidget.h"
#include "2DprocessWidget.h"

#include "covarianceWidget.h"
#include "nutationWidget.h"
#include "exportWidget.h"
#include "exportAbsWidget.h"
#include "apodizationWidget.h"
#include "createFIDWidget.h"
#include "fidMathWidget.h"
#include "peakPickWidget.h"
#include "interpolateWidget.h"
#include "imageGenWidget.h"
#include "cartesianMapWidget.h"
#include "3DrearrangementWidget.h"

class TFIDPlotters;
class TProcessFileWidget;


class TProcessPanelWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TProcessPanelWidget(QWidget *parent = 0);
    ~TProcessPanelWidget();
    void setDevicePixelRatio(int r);
    int devicePixelRatio() {return FDevicePixelRatio;}
    QListWidget *operationListWidget;
    QPlainTextEdit *commandTextEdit;
    Console *console;
    QListWidget *commandHistoryListWidget;
    QStackedWidget *stackedWidget;
    TProcessFileWidget *processFileWidget;
    TFIDPlotters *plotters;
    TFID_2D *FID_2D;
    TFID_2D *currentNMRData;
    TProcessOperations *processOperations;
    TApodizationWidget *apodizationWidget;
    TTransformWidget *transformWidget;
    T2DProcessWidget *twoDProcessWidget;
    //
    KCovarianceWidget *CovarianceWidget;
    KNutationWidget *NutationWidget;
    KExportWidget *exportWidget;
    SExportAbsWidget *exportAbsWidget;
    KPeakPickWidget *peakPickWidget;
    KInterpolateWidget *interpolateWidget;
    //
    TAxisFormatWidget *axisFormatWidget;
    TPhaseWidget *phaseWidget;
    TAddCutPointsWidget *addCutPointsWidget;
    TFIDMathWidget *FIDMathWidget;

    TImageGenWidget *imageGenWidget;

    TCreateFIDWidget *createFIDWidget;
    TCartesianMapWidget *cartesianMapWidget;
    S3DrearrangementWidget *threeDrearrangementWidget;

    QString settingDirPath() {return FSettingDirPath;}
    void setSettingDirPath(QString qs);
    QSettings *processSettings;

    QString processFilePath() {return FProcessFilePath;}
    void setProcessFilePath(QString qs) {FProcessFilePath=qs;}


signals:

public slots:
    void refresh();
    void initialize();
    void onFIDCreated();
    void applyProcess();

    void clearProcess();
    void updatePlotter();
    void initializePlotter();
    void resetProcessSettings();
    void updateProcessSettings();

    void updateNumberOfPlotters(int i);
    void exportProcess();
    void importProcess();
    void onVOffsetRequestReceived(double);

private:
    QString FSettingDirPath;

    int FDevicePixelRatio;
    QString FProcessFilePath;

    void createWidgets();
    void createPanel();
    void createConnections();
};

class TProcessFileWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TProcessFileWidget(QWidget *parent=nullptr);
    ~TProcessFileWidget(){}

    QCheckBox *openAndProcessCheckBox;
    QCheckBox *openProcessAndApplyCheckBox;
    QPushButton *openDataButton,*openAndProcessButton,*saveDataButton;
    QLineEdit *currentFileLineEdit;
    QPlainTextEdit *parameterPlainTextEdit;
    QPushButton *exportDataButton;
    QSpinBox *plotterIDSpinBox;
    QPushButton *saveProcessButton, *openProcessButton;
    TFID_2D *FID_2D;
    void setFID2D(TFID_2D *f) {FID_2D=f;}
    QString dataFilePath() {return FDataFilePath;}
    void setDataFilePath(QString qs) {FDataFilePath=qs;}

signals:
    void initializeRequest();
    void applyProcessRequest();
    void clearProcessRequest();

public slots:

private slots:
    void openFile();
    void saveFile();
    void openFileAndProcess();
    void setNOfPlotters(int n);
    void exportASCIIData();

private:

    void createWidgets();
    void createPanel();
    void createConnections();

    QString FDataFilePath;
    bool fidSetted;
    bool processSetted;

};




#endif // TPROCESSPANELWIDGET_H
