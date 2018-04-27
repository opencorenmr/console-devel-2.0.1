#ifndef EXPSETTINGS_H
#define EXPSETTINGS_H

#include <QWidget>
#include <QString>
#include <QStringList>
#include <QComboBox>
#include <QRadioButton>
#include "pulseProgram.h"
#include "acquisitionWidget.h"
#include "variableedit.h"
#include "array.h"
#include "job.h"
#include "metricPrefix.h"

//class TXAxisOptionWidget;


class TYAxisOptionWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TYAxisOptionWidget(QWidget *parent = 0);

signals:

public slots:

private slots:

private:


};

class TXAxisOptionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TXAxisOptionWidget(QWidget *parent = 0);
    TpulseProgram *ppg;
    void setPPG(TpulseProgram *p)
    {
        ppg=p; ppgSetted=true;
    }

    bool isCustom() {return customAxisRadioButton->isChecked();}
    QString axisLabel() {return xAxisLabelLineEdit->text();}
    QString unitSymbol() {return unitSymbolLineEdit->text();}

    double initialValue() {return FInitialValue;}
    void setInitialValue(double d) {FInitialValue=d;}
    double increment() {return FIncrement;}
    void setIncrement(double d) {FIncrement=d;}
    QRadioButton *defaultAxisRadioButton;
    QRadioButton *customAxisRadioButton;

    QLineEdit *xInitialValueLineEdit;
    QLineEdit *xIncrementLineEdit;
    QLineEdit *xAxisLabelLineEdit;

    QLineEdit *unitSymbolLineEdit;

    TMetricPrefix metricPrefix;
    QComboBox *metricPrefixCombobox;
    TMetricPrefix::prefixes getPrefix();

signals:
    void modified();

public slots:

    void onDefaultAxisRadioButtonToggled(bool b);
    void onCustomAxisRadioButtonToggled(bool b);
    void onApplyButtonClicked();

private slots:

private:

    QPushButton *customAxisApplyButton;
    QLabel *infoLabel;

    double FInitialValue;
    double FIncrement;

    bool ppgSetted;

};


class TExpSettings : public QWidget
{
    Q_OBJECT
public:
    explicit TExpSettings(QWidget *parent = 0);
    TpulseProgram *ppg;
    void setPPG(TpulseProgram *p)
    {
        ppg=p; ppgSetted=true;
        acquisitionWidget->setPPG(ppg);
        arrayWidget->setPPG(ppg);
        variableTable->setPPG(ppg);
        xAxisOptionWidget->setPPG(ppg);
    }


    void setOrigAL(int a) {ForigAL=a;}
    int origAL() {return ForigAL;}

    void writeJob(QString fileName);
//    TJob *job;
    bool ok;

    QString title() {return FTitle;}
    void setTitle(QString qs) {FTitle=qs; setWindowTitle(title());}

    bool isModified() {return FIsModified;}
    void setModified(bool b)
    {
        FIsModified=b;
        if(!b) this->setTitle(title());
        else this->setWindowTitle(title() + tr(" (updated)"));
    }

    QTabWidget *optionTab;
    QLineEdit *pathLineEdit, *nameLineEdit, *obsFreqLineEdit;
    QCheckBox *obsFreqCheckBox;
    QComboBox *obsFreqComboBox;
    QPushButton *setDirButton;
    QPushButton *dataSaveButton;
    QPlainTextEdit *commentTextEdit;
    QPushButton *jobSaveButton;
    QPushButton *jobLoadButton;
    QPushButton *jobQueueButton;

    QWidget *fileWidget;
    TvariableTable *variableTable;
    TArrayWidget *arrayWidget;
    TAcquisitionWidget *acquisitionWidget;
    TXAxisOptionWidget *xAxisOptionWidget;

    QString jobFileName;


signals:
    void message(QString qs);
    void ppgCompiled();
    void dataSaveRequest();
    void setCarrierFreqRequest(double f);
    void transferRequest(const QStringList &sl);
    void setJobRequest();
    void addJobListRequest(QString jobFilename);
    void queueJobRequest(QString jobFileName);

public slots:
    void readJob(QString fileName);
    void showWindow();
    void setDirectory();
    void onJobSaveButtonClicked();
    void onJobLoadButtonClicked();
    void onJobQueueButtonClicked();
    void onRepeatScan();
    void onAccum();
    void onExpFinished();
    void setFreqList();
    void setCarrierFreq();
    void onFreqVariableChanged(int vIndex);
    void updateVariableTable();

private slots:
    void onModified() {setModified(true);}


private:
    bool ppgSetted;
//    int ForigNA;
    int ForigAL;

    void createWidgets();
    void createPanel();

    bool FIsModified;
    QString FTitle;

};


#endif // EXPSETTINGS_H
