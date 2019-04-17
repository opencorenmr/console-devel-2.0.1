#ifndef KNutationWIDGET_H
#define KNutationWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QCheckBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>

#include "processBase.h"

class KNutationWidget : public TProcessBase
{
    Q_OBJECT
public:
    explicit KNutationWidget();
    ~KNutationWidget() {;}


    QLineEdit *arrayInitLineEdit;
    QLineEdit *arrayDeltaLineEdit;

    QComboBox *processStyleComboBox;

    QSpinBox *startPointSpinBox;
    QSpinBox *endPointSpinBox;
    QCheckBox *startClickSetCheckBox;
    QCheckBox *endClickSetCheckBox;

    QPushButton *processArrayButton;

signals:


public slots:
    void processArray();

private:
    void createWidgets();
    void createPanel();
    void createConnections();
};

#endif // KNutationWIDGET_H
