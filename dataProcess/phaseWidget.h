#ifndef PHASEWIDGET_H
#define PHASEWIDGET_H

#include <QWidget>
#include <QCheckBox>
#include <QSettings>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>
#include "phase.h"
#include "processBase.h"

//class TPhaseWidget : public QWidget
class TPhaseWidget : public TProcessBase
{
    Q_OBJECT

public:
//    explicit TPhaseWidget(QWidget *parent = 0);
    explicit TPhaseWidget();
    ~TPhaseWidget();
   // TFID_2D *FID_2D;
    void setFID2D(TFID_2D *f);
   // bool fidSetted() {return FFIDSetted;}

    TPhaseRotation *phaseRotation;

    QSpinBox *phasePivotSpinBox;
    QSpinBox *phaseOrderSpinBox;
    QCheckBox *phasePivotCheckBox;
    QDoubleSpinBox *phase0ValueDoubleSpinBox,*phase1ValueDoubleSpinBox;
    QComboBox *phase0ResolutionComboBox,*phase1ResolutionComboBox;

    QPushButton *applyPushButton;

signals:
//    void updatePlottersRequest();

public slots:
    void initialize();
    void refresh();
    void addOperation();


    void setPivot(int p);
    void setPhase0();
    void setPhase1();
    void setPhase();
    void setResolution0();
    void setResolution1();
    void clickSetPhasePivot();

private slots:
    void performOperation();
    void createSettings(QSettings *settings, QString section);
    void readSettings(QSettings *settings, QString section);

private:

   // bool FFIDSetted;
    void createWidgets();
    void createPanel();
    void createConnections();
    double previousPhase0Value,previousPhase1Value;

};

#endif // PHASEWIDGET_H
