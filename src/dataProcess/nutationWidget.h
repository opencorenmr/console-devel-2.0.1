#ifndef KNutationWIDGET_H
#define KNutationWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QCheckBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QPlainTextEdit>
#include <QComboBox>
#include <QLabel>
#include <QStackedWidget>

#include "processBase.h"

class KNutationWidget : public TProcessBase
{
    Q_OBJECT
public:
    explicit KNutationWidget();
    ~KNutationWidget() {;}

    QPlainTextEdit *resultTextEdit;

    QLineEdit *arrayInitLineEdit;
    QLineEdit *arrayDeltaLineEdit;

    QLabel *fromAtLabel, *toLabel;
    QComboBox *styleComboBox;

    QSpinBox *startPointSpinBox;
    QSpinBox *endPointSpinBox;
    QCheckBox *startClickSetCheckBox;
    QCheckBox *endClickSetCheckBox;

    QPushButton *processArrayButton;
    QPushButton *saveButton;

signals:


public slots:
    void processArray();
    void setStartPoints(int p);
    void setEndPoints(int p);
    void clickSetStartPoints();
    void clickSetEndPoints();

    void onStyleComboBoxChanged(int index);
    void onSaveButtonClicked();
//    void enableArea();

private:
    void createWidgets();
    void createPanel();
    void createConnections();
};

#endif // KNutationWIDGET_H
