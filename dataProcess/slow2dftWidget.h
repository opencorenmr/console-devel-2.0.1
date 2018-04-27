#ifndef KSLOW2DFTWIDGET_H
#define KSLOW2DFTWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include <QPlainTextEdit>

#include "processBase.h"

class Kslow2DFTWidget : public TProcessBase
{
    Q_OBJECT
public:
    explicit Kslow2DFTWidget();
    ~Kslow2DFTWidget() {;}

    QPlainTextEdit *indirectTimeAcquireTextEdit;

    QPushButton *indirectTimeAcquireButton;
    QComboBox *indirectTimeTypeComboBox;
    QLineEdit *arrayAxisInitialLineEdit;
    QLineEdit *arrayAxisIntervalLineEdit;
    QLineEdit *arrayAxisPointsLineEdit;
    QPushButton *indirectSlowFTButton;

signals:

public slots:
    void indirectTimeAcquisition();
    void performIndirectSlowFT();

private:
    void createWidgets();
    void createPanel();
    void createConnections();
};

#endif // KSLOW2DFTWIDGET_H
