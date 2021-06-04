#ifndef TTRANSFORMWIDGET_H
#define TTRANSFORMWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QSpinBox>


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

    QSpinBox *n1SpinBox, *n2SpinBox;
    QPushButton *FFT3DButton;


signals:
    void vOffsetRequest(double);

public slots:
    void performFFT();
    void performIFFT();
    void restoreToDefault();
    void perform3DFFT();
private:

    void createWidgets();
    void createPanel();
    void createConnections();
};

#endif // TTRANSFORMWIDGET_H
