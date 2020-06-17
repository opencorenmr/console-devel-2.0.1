#ifndef TIMAGEGENWIDGET_H
#define TIMAGEGENWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QDebug>
#include <QComboBox>
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

    void createWidgets();
    void createPanel();
    void createConnections();

signals:

public slots:

private:



};

#endif // TIMAGEGENWIDGET_H
