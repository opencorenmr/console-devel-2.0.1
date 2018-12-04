#ifndef KCOVARIANCEWIDGET_H
#define KCOVARIANCEWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QCheckBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QComboBox>

#include "processBase.h"

class KCovarianceWidget : public TProcessBase
{
    Q_OBJECT
public:
    explicit KCovarianceWidget();
    ~KCovarianceWidget() {;}

    QSpinBox *cutArraySpinBox;
    QLineEdit *cutArrayLineEdit;
    QPushButton *cutArrayButton;

    QComboBox *svdSettingComboBox;
    QPushButton *svdButton;
    QCheckBox *imagKeepCheckBox;

    QComboBox *covarianceComboBox;
    QPushButton *covarianceButton;
    QCheckBox *imagKeepCovCheckBox;

    QPushButton *rootMatrixButton;

signals:

public slots:
    void performCutArray();
    void performSVD();
    void performCovariance();
//    void performRootMatrix();
    //void setCutArray();

private:
    void createWidgets();
    void createPanel();
    void createConnections();
};

#endif // KCOVARIANCEWIDGET_H
