#ifndef KInterpolateWIDGET_H
#define KInterpolateWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QCheckBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QStackedWidget>

#include "processBase.h"

class KInterpolateWidget : public TProcessBase
{
    Q_OBJECT
public:
    explicit KInterpolateWidget();
    ~KInterpolateWidget() {;}


    QLineEdit *initialAngleLineEdit;
    QLineEdit *deltaAngleLineEdit;

    QPushButton *processButton;

signals:


public slots:
    void processInterpolate();

private:
    void createWidgets();
    void createPanel();
    void createConnections();
};

#endif // KInterpolateWIDGET_H
