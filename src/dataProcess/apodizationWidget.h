#ifndef TAPODIZATIONWIDGET_H
#define TAPODIZATIONWIDGET_H

#include <QWidget>
#include <QCheckBox>
#include <QSettings>
#include <QComboBox>
//#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include "apodization.h"
#include "processBase.h"

class TApodizationWidget : public TProcessBase
{
    Q_OBJECT
public:
    explicit TApodizationWidget();
    ~TApodizationWidget() {;}

    TApodization *apodization;

    QCheckBox *inverseCheckbox;
    QComboBox *apodizationTypeComboBox;
    QPushButton *applyApodizationPushButton;
    QDoubleSpinBox *widthDoubleSpinBox;

public slots:

private slots:
    void onApplyButtonClicked();
    bool performOperation();
    void createSettings(QSettings *settings, QString section);
    void readSettings(QSettings *settings, QString section);



private:

    void addOperation();


};

#endif // TAPODIZATIONWIDGET_H
