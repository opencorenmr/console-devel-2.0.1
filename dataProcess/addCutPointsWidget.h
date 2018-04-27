#ifndef TADDCUTPOINTSWIDGET_H
#define TADDCUTPOINTSWIDGET_H

#include <QWidget>
#include <QCheckBox>
#include <QSettings>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>
#include "addCutPoints.h"
#include "processBase.h"



class TAddCutPointsWidget : public TProcessBase
{
    Q_OBJECT
public:
    explicit TAddCutPointsWidget();
    ~TAddCutPointsWidget();
  //  TFID_2D *FID_2D;
    void setFID2D(TFID_2D *f);

    TAddCutPoints *addCutPoints;

    QSpinBox *headPointsSpinBox;
    QSpinBox *tailPointsSpinBox;
    QCheckBox *headCheckBox;
    QCheckBox *tailCheckBox;

    QComboBox *operationSelectComboBox; // select cut/leave/zero-fill/extrapolate
    QComboBox *headTailComboBox;        // select head/tail/head&tail

    QPushButton *applyPushButton;

signals:

public slots:
    void initialize();
    bool refresh();
    void addOperation();


    void setHeadPoints(int p);
    void setTailPoints(int p);
    void setResidualTailPoints(int p);
    void cut();
    void zeroFill();
    void extrapolate();
    void clickSetHeadPoints();
    void clickSetTailPoints();


private slots:
    void onApplyButtonClicked();
    bool performOperation();
    void createSettings(QSettings *settings, QString section);
    void readSettings(QSettings *settings, QString section);

    void onOperationSelectComboBoxChanged();
    void onHeadTailComboBoxChanged();


private:

    void createWidgets();
    void createPanel();
    void createConnections();

};

#endif // TADDCUTPOINTSWIDGET_H
