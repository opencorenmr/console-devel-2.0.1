#ifndef TCREATEFIDWIDGET_H
#define TCREATEFIDWIDGET_H

#include <QWidget>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QComboBox>
#include <QLineEdit>
#include <QStackedWidget>
#include <QGroupBox>

#include "../fid.h"

class TCreateFIDWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TCreateFIDWidget(QWidget *parent = 0);

    QPlainTextEdit *plainTextEdit;
    QDoubleSpinBox *dwDoubleSpinBox, *sf1DoubleSpinBox;
    QDoubleSpinBox *freqDoubleSpinBox;
    QComboBox *createModeComboBox, *decayFunctionComboBox;
    QSpinBox *alSpinBox;
    QPushButton *createFIDPushButton;
    QPushButton *loadFromFileButton;

    QStackedWidget *stackedWidget;
    QGroupBox *groupBox1;
    QGroupBox *groupBox2;


    TFID_2D *FID_2D;
    void setFID2D(TFID_2D *f) {FID_2D=f;}
    QString dataFilePath() {return FDataFilePath;}
    void setDataFilePath(QString qs) {FDataFilePath=qs;}


signals:
    void updateRequest();

public slots:

    void onCreateFIDButtonClicked();
    void createFIDFromAsci();
    void createFIDFromPropeties();
    void onLoadFromFileButtonClicked();

private:
    void createWidgets();
    void createPanel();
    void createConnections();
    QString FDataFilePath;

};

#endif // TCREATEFIDWIDGET_H
