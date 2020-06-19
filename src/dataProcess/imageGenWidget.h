#ifndef TIMAGEGENWIDGET_H
#define TIMAGEGENWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QDebug>
#include <QComboBox>
#include <QPlainTextEdit>
#include <QStringList>
#include <QSettings>
#include "../fid.h"
#include "processBase.h"

class TImageGenWidget : public TProcessBase
{
    Q_OBJECT
public:
    explicit TImageGenWidget();
    ~TImageGenWidget() {;}

    QSpinBox *plotterIDSpinBox;
    QPlainTextEdit *epsPlainTextEdit;
    QPushButton *saveEPSPushButton;
    QPushButton *createEPSPushButton;
    QLineEdit *boundingBox1LineEdit;
    QLineEdit *boundingBox2LineEdit;
    QLineEdit *boundingBox3LineEdit;
    QLineEdit *boundingBox4LineEdit;

    void createWidgets();
    void createPanel();
    void createConnections();

signals:

public slots:
    void createEPS();


private:



};

#endif // TIMAGEGENWIDGET_H
