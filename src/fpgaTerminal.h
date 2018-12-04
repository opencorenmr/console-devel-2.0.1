#ifndef FPGATERMINAL_H
#define FPGATERMINAL_H

#include <QWidget>
#include <QDebug>
#include <QTabWidget>
#include <QLineEdit>
#include <QSplitter>
#include <QComboBox>
#include <QList>

#include <QThread>
#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>


#include "TxRxThread.h"
#include "dataReceiver.h"
#include "rcvrcom.h"
#include "console.h"
#include "plotter.h"
#include "runStopButton.h"
#include "pulseProgram.h"


#include "expSettings.h"
#include "job.h"

#include "dataProcess/processOperations.h"

#if defined(__linux__)
  #include "gpib488console.h"
#endif

class QPlainTextEdit;
class QPushButton;


//class Tdevice : public QObject
class Tdevice : public QThread
{
    Q_OBJECT
public:
    Tdevice();
    ~Tdevice();
    FT_HANDLE pulserHandle;     // device handle for the 1st channel of FT2232C
                                // Used for communication with the pulse programmer
    FT_HANDLE receiverHandle;   // device handle for the 2ns channel of FT2232C
                                // Used for communication with the receiver

   // QStringList message;
    FT_STATUS createDeviceInfoList(DWORD &numDevs)
    {
        return FT_CreateDeviceInfoList(&numDevs);
    }

    QStringList message;
    bool error;
    bool ppgConnected,rcvrConnected;
    void stop() {mutex.lock(); stopped=true; mutex.unlock();}

signals:
    void sendMessage(QString qs);
    void finished();

public slots:
    void open();

    void onTimeout()
    {

    }
protected:
    void run();

private:
    QMutex mutex;
    QWaitCondition condition;

    volatile bool stopped;

    FT_STATUS ftStatus;
    FT_DEVICE_LIST_INFO_NODE *devInfo;
    DWORD numDevs;
    QString s;
    char deviceDescription[2][64];

};

class askOverride {
  public:
    enum TaskOverride {Yes, No};
};

class TfpgaTerminal : public QWidget
{
    Q_OBJECT
public:

    explicit TfpgaTerminal(QWidget *parent = 0);
    ~TfpgaTerminal();
   // void setFont(QFont font);

    Tdevice *device;

    USBRxThread rThread;  // a thread receiving messages from pulse programmer
    USBTxThread tThread;  // a thread sending commands to pulse programmer

    dataReceiver fidThread;

    TpulseProgram *ppg;

    QPushButton *loadJobButton;
    QPushButton *showExpSettingsButton, *showJobButton;
    QPushButton *showFavoriteJobButton;

    TExpSettings *expSettings;
    TJobQueueWidget *jobQueueWidget;
    TFavoriteJobWidget *favoriteJobWidget;

    QPlainTextEdit *FPGAStatusTextEdit;
    QPushButton *terminalOpenCloseButtion;
    // QCheckBox *openOnStartupCheckBox;
    QLabel *buildNumberLabel;
    QTabWidget *terminalTab;


    Console *pulserConsole;

    //TFIDPlotters *plotters;
    QList<FIDPlotter *> fidPlotters;
    QList<QSplitter *> plotSplitters;
    QList<TPlotWindow *> plotWindows;

    TFID_2D *nmrData;

    bool repeatScanQ;
    bool arrayQ;
    bool ppgTransferred;

    void FTDIOpen();
    void FTDIClose();

/*
#if defined(_WIN32)
    TGPIB488Console *GPIB488Console;
#endif
*/
#if defined(__linux__)
  TGPIB488Console *GPIB488Console;
#endif

    void setDevicePixelRatio(int r) {
        FDevicePixelRatio=r;
        for(int k=0; k<fidPlotters.size(); k++)
            fidPlotters[k]->setDevicePixelRatio(r);
    }

    void checkOverride();
    bool checkPath();
    void clearConsoleLine();
    void updateVariableTable();

signals:

    void updateVariableResult(bool ok, QString errorMessage);
    void setArrayTableRequest(TpulseProgram *p);
    void transferStarted();
    void usbClosed();
    void replyFromFPGACopied();
    void usbStatusChanged(bool opened);
    void copyComplete();
//    void plotComplete();
    void accumComplete();
    void jobComplete();

    void runStateChanged(TRunStopButton::State state);
    void scanModeChanged(int); // 0: accum, 1: repeat scan
    void dataSaveRequest();
    void disableTransferButton();
    void enableTransferButton();
    void hideArrayCounterRequest();

    void expBusy(bool b);
   // void buildNumber(QString s);

public slots:

    void updateAuxParams();
    void setAR(int ar);
// void updateVariable(TVariable *v);
    void onTransferComplete();
//    void fromEditor(TpulseProgram *p);
    void fromEditor(QString fn);
    void transferPPG(const QStringList &sl);
    void transferPPG(const QString &s);
    void sendToPulser(QString command);
    void displayPPGMessage(QChar c);
    void displaySentString(QString s);
    void displayLogMessage(QString s);
    void onRunPromptReceived();
    void onReadyPromptReceived();
    void onArrayPromptReceived();
    void copyPPG(TpulseProgram *p);

    void setNA(int na) {nmrData->setNA(na);}
    void setND(int nd) {nmrData->setND(nd);}
    void getReplyFromFPGA(QString qs);

//    void setFreqList(TpulseProgram *ppg);
    void setCarrierFreq(double f);
    void setJob();
    void setAndRunJob(QString qs);
    void showJobWidget()
    {
//        jobQueueWidget->setWindowFlags(Qt::WindowStaysOnTopHint);
//        jobQueueWidget->show();
        jobQueueWidget->setWindowFlags(Qt::Dialog);
        jobQueueWidget->show();

    }

    void showFavoriteJobWidget()
    {
//        favoriteJobWidget->setWindowFlags(Qt::WindowStaysOnTopHint);
//        favoriteJobWidget->show();
        favoriteJobWidget->setWindowFlags(Qt::Dialog);
        favoriteJobWidget->show();
    }

private slots:
    void FTDIOpenClose();

    void copyFID(TFID *f);
    void saveFID();
   // void setDirectory();
    void enableSaveButton();
    void disableSaveButton();
    void onSaveButtonClicked();
    void displayData();

    void promptReceived(QChar qc) {prompt=qc; isConnectionSuccessful=true;}
   // void setVariableTableState(TRunStopButton::State state);

    void disableOpenCloseButton() {terminalOpenCloseButtion->setEnabled(false);}
    void enableOpenCloseButton() {terminalOpenCloseButtion->setEnabled(true);}

    void FIDSelectSpinBoxUpdate(int i)
    {
        for(int k=0; k<fidPlotters.size(); k++)
        {
          fidPlotters[k]->FIDSelectSpinBox->setMaximum(i);
          if(i>1) fidPlotters[k]->FIDSelectSpinBox->show();
          else fidPlotters[k]->FIDSelectSpinBox->hide();
        }
    }

    void splitPlot(FIDPlotter* fp, TFIDPlotters::PlotSplitMode sMode);
    void addAndCopyPlotter(FIDPlotter *fp);

    void deletePlotWindow(TPlotWindow *pw);


private:
    int FDevicePixelRatio;
//    QWidget *optionPanel;
//    void createOptionPanel();
    bool isConnectionSuccessful;
    bool runQ;
    int nCopyOperations;

    QString replyFromFPGA;
    QMutex mutex;
    QChar prompt;

    bool repeatScan();
    bool accumulation();
    bool initData();
    void interrupt();

};

#endif // FPGATERMINAL_H
