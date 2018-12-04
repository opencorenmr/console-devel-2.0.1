#ifndef KSLOWFTWIDGET_H
#define KSLOWFTWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include <QPlainTextEdit>

#include "processBase.h"

class KSlowFTWidget : public TProcessBase
{
    Q_OBJECT
public:
    explicit KSlowFTWidget();
    ~KSlowFTWidget() {;}

    QPlainTextEdit *timeAcquireTextEdit;

    QPushButton *timeAcquireButton;
    QComboBox *timeTypeComboBox;
    QPushButton *slowFTButton;

signals:

public slots:
    void timeAcquisition();
    void performSlowFT();

private:
    void createWidgets();
    void createPanel();
    void createConnections();
};

#endif // KSLOWFTWIDGET_H
