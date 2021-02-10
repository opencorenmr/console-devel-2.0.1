#ifndef TCARTESIANMAPWIDGET_H
#define TCARTESIANMAPWIDGET_H

#include <QWidget>
#include <QCheckBox>
#include <QSettings>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <QPlainTextEdit>
#include "cartesianMap.h"
#include "processBase.h"

class TCartesianMapWidget : public TProcessBase
{
    Q_OBJECT
public:
    TCartesianMapWidget();

    QPlainTextEdit *thetaPhiTextEdit;
    QPushButton *loadAngleTablePushButton;
    QPushButton *saveAngleTablePushButton;
    QPushButton *applyAngleTablePushButton;
    QString dataFilePath() {return FDataFilePath;}
    void setDataFilePath(QString qs) {FDataFilePath=qs;}

public slots:
    void onLoadAngleTablePushButtonClicked();
    void onSaveAngleTablePushButtonClicked();
    void onApplyAngleTablePushButtonClicked();

private:

    void createWidgets();
    void createLayout();
    void createConnections();
    void addOperation();
    QString FDataFilePath;
};

#endif // TCARTESIANMAPWIDGET_H
