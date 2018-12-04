#ifndef KPEAKPICKWIDGET_H
#define KPEAKPICKWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QCheckBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QComboBox>

#include "processBase.h"

class KPeakPickWidget : public TProcessBase
{
    Q_OBJECT
public:
    explicit KPeakPickWidget();
    ~KPeakPickWidget() {;}

    QDoubleSpinBox *intensitySpinBox;
    QSpinBox *whichFidSpinBox;
    QPushButton *peakPickButton;

    QDoubleSpinBox *intensity2DSpinBox;
    QCheckBox *setAreaCheckBox;
    QSpinBox *targetAreaStartSpinBox;
    QSpinBox *targetAreaEndSpinBox;
    QPushButton *peakPick2DButton;


signals:

public slots:
    void performPeakPick();
    void performPeakPick2D();

private:
    void createWidgets();
    void createPanel();
    void createConnections();
};

#endif // KPEAKPICKWIDGET_H
