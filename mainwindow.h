#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>
#include <QMainWindow>
#include <QAction>
#include <QToolBar>
#include <QToolButton>
#include <QPlainTextEdit>
#include <QComboBox>
#include "ppgEdit.h"
#include "fpgaTerminal.h"
#include "fid.h"
#include "plotter.h"
#include "runStopButton.h"
//#include "dataProcess.h"
#include "codeeditor.h"
#include "highlighter.h"
//#include "dataProcess/processPanelWidget.h"

#define VERSION_NUMBER 2
#define VERSION_NUMBER2 0
#define VERSION_NUMBER3 0
#define BUILD_NUMBER 204
#define RELEASE_YEAR 2018
#define RELEASE_MONTH MARCH

class TwelcomeWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
   // MainWindow();
    virtual ~MainWindow(){;}

    QWidget *centralWidget;

    QLabel *usbStatusLabel;
   // QLabel *expStatusLabel;
  //  QLabel *receiverStatusLabel;

    QLabel *acqCounterLabel, *arrayCounterLabel;
    QLabel *remainingTimeLabel, *finishTimeLabel;

    TwelcomeWidget *welcomeWidget;
    TfpgaTerminal *fpgaTerminal;


signals:
    void init();


private slots:

//    void gateFileRead();

    void setWelcome();
    void setCompiler();
    void setTerminal();
//    void setProcess();

    void transferToProcess();
    void saveSettings();


    void showUSBNotOpend() {usbStatusLabel->setText("USB Closed");}
    void showUSBBusy() {usbStatusLabel->setText("USB Busy"); runStopButton->setEnabled(false);}
    void showUSBReady() {usbStatusLabel->setText("USB Ready"); runStopButton->setEnabled(true);}
    void updateReceiverStatus(bool busy)
    {
        if(busy)
        {
          //  receiverStatusLabel->setText(tr("..."));
             lightningBoltButton->setIcon(QIcon(":/images/arrow.png"));
        }

        else
        {
          //  receiverStatusLabel->setText(tr(""));
             lightningBoltButton->setIcon(QIcon());
        }
    }

    void updateReceiverCounter();
   // void updateArrayCounter(QString ccs) {arrayCounterLabel->setText(ccs);}
    void hideArrayCounter() {arrayCounterLabel->setText("");}

    void onRunStopButtonClicked();
    void onExpBusy() {scanModeComboBox->setEnabled(false);}
    void onExpReady() {scanModeComboBox->setEnabled(true);}

    void addJobList(QString jobFilename);
    void onSetupJobButtonClicked();
    void onEditJobButtonClicked();
    void onClearJobButtonClicked();

protected:
    void closeEvent(QCloseEvent *event);
    TRunStopButton *runStopButton;
    QComboBox *scanModeComboBox;
    QToolButton *lightningBoltButton;
    QPushButton *FIDReadButton;
    FIDPlotter *plotter2;
    QString dataFilePath, processFilePath;

    int devicePixelRatio;
    QFont defaultFont;

    TppgEdit *ppgEdit;
//    TprocessTerminal *processTerminal;
//    TProcessPanelWidget *processPanel;

private:
    QString path0;
    // path for .ini and example files
    // linux and mac: home dir + "/.opencorenmr"
    // win: application path

    QStackedLayout *stackedLayout;

    QToolBar *leftToolBar;
    QToolBar *leftToolBar2;
  //  QToolButton *welcomeToolButton;
    QAction *welcomeAction;
    QAction *ppgCompilerAction;
    QAction *terminalAction;
//    QAction *processAction;
    void createActionsAndButtons();
    void createLeftToolBar();

   // QStringList recentJobList;
   // QListWidget *welcomeJobListWidget;
   // QPushButton *setupJobButton;
   // QPushButton *clearJobButton;

    bool hasCloseEventCalled;

};

//class TJobCategory
//{
//    enum Category {BuiltInJob,FavoriteJob};
//};

class TwelcomeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TwelcomeWidget(QWidget *parent = 0);
    ~TwelcomeWidget();

    QPushButton *topButton, *pulseProgramButton, *jobButton;
    QStringList builtInJobList, recentJobList, favoriteJobList;
    QListWidget *welcomeJobListWidget;
    QComboBox *jobCategoryCombobox;
//    TJobCategory::Category jobCategoty;
    QPlainTextEdit *jobPathPlainTextEdit, *jobPlainTextEdit;
    QPushButton *setupJobButton, *editJobButton;
    QPushButton *clearJobButton;
    QPushButton *showFavoriteJobWidgetButton;

    QListWidget *ppgListWidget;
    QPushButton *openWithPPGEditButton;
    QStringList ppgList, ppgPathList, ppgCaptionList;
    QStackedLayout *stackedLayout;
    CodeEditor *ppgPlainTextEdit;
    Highlighter *highlighter;
    QPlainTextEdit *ppgPathPlainTextEdit, *ppgCaptionPlainTextEdit;

    //QTabWidget *pulseProgramTabWidget;

    QString path0;
    // path for .ini and example files
    // linux: home dir + "/.opencorenmr"
    // mac and win: application path

signals:
    void openPPGRequest(QString);

private:

    void loadBuiltInPPGs();


public slots:
    void setJobCategoty();

private slots:
    void setAbout() {topButton->setFocus();stackedLayout->setCurrentIndex(0);}
    void setPulseProgram() {pulseProgramButton->setFocus();stackedLayout->setCurrentIndex(1);}
    void setJob() {jobButton->setFocus();stackedLayout->setCurrentIndex(2);}

    void displayPulseProgram(int index);
    void onOpenWithPPGEditButtonClicked();

    void displayJob(int index);

};

#endif // MAINWINDOW_H
