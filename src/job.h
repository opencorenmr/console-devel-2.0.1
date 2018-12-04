#ifndef JOB_H
#define JOB_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QToolButton>
#include <QLineEdit>
#include <QMutex>
#include <QMutexLocker>

class TJob : public QObject
{
    Q_OBJECT
public:
    TJob();

    QString jobName() {return FjobName;}
    void setJobName(QString jn) {FjobName=jn;}
    bool hasError() {return Ferror;}
    QString errorMessage() {return FerrorMessage;}

    QString filePath;
    QString fileName;
    QString carrierFrequency;
    bool obsFreqComboBoxChecked;
    int obsFreqComboBoxIndex;
    QString comment;

    QStringList variableInfo;

    QStringList arrayInfo;

    bool separateDataStorageCheckBoxChecked;
    int separateDataStorageSpinBoxValue;
  //  int separateDataStorageComboBoxIndex;
    bool multipleAcquisitionCheckBoxChecked;
    int multipleAcquisitionSpinBoxValue;
    int multipleAcquisitionComboBoxValue;

    bool inFPGAAccumCheckBox;
    int inFPGAAccumSpinBoxValue;

    bool spectralDensityAccumCheckBoxChecked;

    bool phaseReverseCheckBoxChecked;
    bool phaseRotationCheckBoxChecked;
    double phaseRotationSpinBoxValue;
    bool FFTCheckBoxChecked;

    bool digitalFilterCheckBoxChecked;
    bool QDCheckBoxChecked;
    bool offsetCorrectionCheckBoxChecked;
    int offsetCorrectionSpinBoxValue;

signals:

public slots:

private:
    QString FjobName;
    bool Ferror;
    QString FerrorMessage;

    QString FPPGFileName;



};




class TJobQueueWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TJobQueueWidget(QWidget *parent = 0);

    QLabel *currentJobLabel,*queuedJobLabel;
    QPushButton *runJobPushButton;
    QListWidget *jobListWidget;
    QLineEdit *currentJobLineEdit;

    bool isRunning() {return running;}
signals:
    void runJobRequest(QString qs);

public slots:
    void addJob(QString jobFilename);
    void onAddJobPushButtonClicked();
    void onRemoveJobPushButtonClicked();
    void onRunJobPushButtonClicked();
    void onUpButtonClicked();
    void onDownButtonClicked();

    void onJobFinished();
    void onJobAborted();

private:
    QPushButton *addJobPushButton,*removeJobPushButton;
    QToolButton *upButton,*downButton;
    void createWidgets();
    void createPanel();

    QString lastPath;

    bool running;
    QMutex mutex;
};

class TFavoriteJobWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TFavoriteJobWidget(QWidget *parent = 0);

//    QLabel *currentJobLabel,*queuedJobLabel;
    QPushButton *loadJobPushButton;
    QListWidget *jobListWidget;
//    QLineEdit *currentJobLineEdit;

//    bool isRunning() {return running;}
signals:
    void loadJobRequest(QString qs);

public slots:
    void addJob(QString jobFilename);
    void onAddJobPushButtonClicked();
    void onRemoveJobPushButtonClicked();
    void onLoadJobPushButtonClicked();
    void onUpButtonClicked();
    void onDownButtonClicked();

private:
    QPushButton *addJobPushButton,*removeJobPushButton;
    QToolButton *upButton,*downButton;
    void createWidgets();
    void createPanel();

    QString lastPath;

//    bool running;
//    QMutex mutex;
};


#endif // JOB_H
