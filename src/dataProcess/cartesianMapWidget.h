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
//    QPushButton *setAngleTablePushButton;
    QPushButton *applyAngleTablePushButton;
    QString dataFilePath() {return FDataFilePath;}
    void setDataFilePath(QString qs) {FDataFilePath=qs;}

signals:
    void isCartesianMapIdle(bool);

public slots:
    void onLoadAngleTablePushButtonClicked();
    void onSaveAngleTablePushButtonClicked();
//    void onSetAngleTablePushButtonClicked();
    void onApplyAngleTablePushButtonClicked();

private:

    void createWidgets();
    void createLayout();
    void createConnections();
    void addOperation(TCartesianMap3D *cMap3D);
    QString FDataFilePath;
};

#endif // TCARTESIANMAPWIDGET_H
