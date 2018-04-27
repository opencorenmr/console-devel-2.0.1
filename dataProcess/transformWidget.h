#ifndef TTRANSFORMWIDGET_H
#define TTRANSFORMWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QCheckBox>
#include <QDoubleSpinBox>


#include "processBase.h"


class TTransformWidget : public TProcessBase
{
    Q_OBJECT
public:
    explicit TTransformWidget();
    ~TTransformWidget() {;}

    QPushButton *FFTButton, *IFFTButton, *restoreToDefaultButton;
    QCheckBox *LaplaceCheckBox;
    QDoubleSpinBox *LaplaceWidthDoubleSpinBox;

//    QPushButton *slowFTButton, *slowIFTButton;

signals:


public slots:
    void performFFT();
    void performIFFT();
    void restoreToDefault();
private:

    void createWidgets();
    void createPanel();
    void createConnections();
};

#endif // TTRANSFORMWIDGET_H
