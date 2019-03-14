#ifndef T2DPROCESSWIDGET_H
#define T2DPROCESSWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QComboBox>

#include "processBase.h"

class T2DProcessWidget : public TProcessBase
{
    Q_OBJECT
public:
    explicit T2DProcessWidget();
    ~T2DProcessWidget() {;}
    QPushButton *transposeButton;

    QComboBox *hyperComplexPhaseComboBox;
    QPushButton *hyperComplexButton;

signals:

public slots:
    void performTranspose();
    void performHyperComplexCompression();
    //

private:
    void createWidgets();
    void createPanel();
    void createConnections();
};

#endif // T2DPROCESSWIDGET_H
