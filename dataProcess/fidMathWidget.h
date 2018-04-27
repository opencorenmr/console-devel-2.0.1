#ifndef TFIDMATHWIDGET_H
#define TFIDMATHWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QStackedLayout>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QSettings>

#include "processBase.h"
#include "fidMath.h"

class TFIDMathWidget : public TProcessBase
{
    Q_OBJECT
public:
    TFIDMathWidget();

    TFIDMath *FIDMath;

    QStackedLayout *stackedLayout;
    QComboBox *withComboBox,
              *numberOperationComboBox,
              *fileOperationComboBox;
    QCheckBox *checkBox1;
    QSpinBox *xIniSpinBox, *xFinSpinBox;
    //QDoubleSpinBox *number1DoubleSpinBox, *number2DoubleSpinBox;
    QLabel *number1Label, *number2Label;
    QLineEdit *number1LineEdit, *number2LineEdit;
    QPushButton *applyButton;
    QStackedWidget *withNumberStackedWidget;
    QLineEdit *fileNameLineEdit;
    QLineEdit *filePathLineEdit;
    QPushButton *fileSelectPushButton;

public slots:
    void onNumberOperationComboBoxActivated();
    void onFileSelectButtonClicked();

    void onApplyButtonClicked();
    //bool performOperation();
    void createSettings(QSettings *settings, QString section);
    void readSettings(QSettings *settings, QString section);

signals:

private:

    void createWidgets();
    void createLayout();
    void createConnections();
    void addOperation();

};

#endif // TFIDMATHWIDGET_H
