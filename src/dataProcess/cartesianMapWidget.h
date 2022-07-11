#ifndef TCARTESIANMAPWIDGET_H
#define TCARTESIANMAPWIDGET_H

#include <QWidget>
#include <QDialog>
#include <QCheckBox>
#include <QSettings>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <QPlainTextEdit>
#include "cartesianMap.h"
#include "cartesianMapWeight.h"
#include "processBase.h"

class SCartesianMapGaussianOption : public QDialog
{
    Q_OBJECT
public:
    SCartesianMapGaussianOption(QWidget *parent);
    ~SCartesianMapGaussianOption();

public slots:
    void onAccept();
    void onReject();

signals:
    void sendSigma(double d);

private:
    QDoubleSpinBox *sigmaDoubleSpinBox;
    QPushButton *okPushButton, *cancelPushButton;
};

class SCartesianMapWeightOption : public QDialog
{
    Q_OBJECT
public:
    SCartesianMapWeightOption(QWidget *parent);
    ~SCartesianMapWeightOption();

public slots:
    void onAccept();
    void onReject();

signals:
    void sendIteration(int i);

private:
    QSpinBox *iterationSpinBox;
    QPushButton *calculatePushButton, *cancelPushButton;
};

class TCartesianMapWidget : public TProcessBase
{
    Q_OBJECT
public:
    TCartesianMapWidget();

    QPlainTextEdit *thetaPhiTextEdit;
    QPushButton *loadAngleTablePushButton;
    QPushButton *saveAngleTablePushButton;
//    QPushButton *setAngleTablePushButton;
    QSpinBox *setCubeSidePointsSpinBox;
    QDoubleSpinBox *setDistanceBetweenPointsDoubleSpinBox;
    QComboBox *selectModeComboBox;
    QPushButton *calcWeightPushButton;
    QPushButton *applyAngleTablePushButton;
    QString dataFilePath() {return FDataFilePath;}
    void setDataFilePath(QString qs) {FDataFilePath=qs;}

signals:
    void isCartesianMapIdle(bool);

public slots:
    void onLoadAngleTablePushButtonClicked();
    void onSaveAngleTablePushButtonClicked();
//    void onSetAngleTablePushButtonClicked();
    void changeCalcWeightPushButtonEnabled(int);
    void onCalcWeightPushButtonClicked();
    void onApplyAngleTablePushButtonClicked();

private:
    void createWidgets();
    void createLayout();
    void createConnections();
    void addOperation(TCartesianMap3D *cMap3D);
    QString FDataFilePath;

    SCartesianMapWeightOption *weightOption = 0;
    SCartesianMapGaussianOption *gaussianOption = 0;
};

#endif // TCARTESIANMAPWIDGET_H
