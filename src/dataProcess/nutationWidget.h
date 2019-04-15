#ifndef KFIDSIMULATIONWIDGET_H
#define KFIDSIMULATIONWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QCheckBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>

#include "processBase.h"

class KFidSimulationWidget : public TProcessBase
{
    Q_OBJECT
public:
    explicit KFidSimulationWidget();
    ~KFidSimulationWidget() {;}

    QLineEdit *fid1_AmpLineEdit;
    QLineEdit *fid1_FreqLineEdit;
    QLineEdit *fid1_T2LineEdit;
    QLineEdit *fid1_t1IncrementLineEdit;

    QLineEdit *arrayLineEdit;
    QLineEdit *alLineEdit;
    QLineEdit *dwLineEdit;

    QCheckBox *noiseCheckBox;
    QLineEdit *noiseLineEdit;
    QCheckBox *addCheckBox;
    QPushButton *addFidButton;

signals:


public slots:
    void performFidCalculation();

private:
    void createWidgets();
    void createPanel();
    void createConnections();
};

#endif // KFIDSIMULATIONWIDGET_H
