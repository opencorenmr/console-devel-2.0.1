#ifndef DATAPROCESS_H
#define DATAPROCESS_H

#include <QtWidgets>

//#include <QObject>
//#include <QList>
#include <QWidget>
#include <QDebug>
//#include <QTabWidget>
#include <QLineEdit>
//#include <QSPlitter>
#include <QComboBox>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>

#include "plotter.h"
#include "fid.h"

#include "dataProcess/processElement.h"
#include "dataProcess/fft.h"
#include "dataProcess/ifft.h"



class TProcess : public QObject
{
    Q_OBJECT
public:
    explicit TProcess(QObject *parent = 0);

    //~TProcess() {while(!processElement.isEmpty()) {processElement.removeLast();}}
    QList<TProcessElement> processElement;
    int loadFromFile(QString fileName);
    int saveToFile(QString fileName);


signals:

public slots:

};



class TransformWindow : public QWidget
{
    Q_OBJECT
public:
//    TransformWindow(QWidget *parent = 0) {;}
    TransformWindow()
    {
        FFTButton = new QPushButton(tr("FFT"));
        invFFTButton = new QPushButton(tr("iFFT"));

        QVBoxLayout *mainLayout = new QVBoxLayout;
        mainLayout->setMargin(0);
        mainLayout->setSpacing(0);
        mainLayout->addWidget(new QLabel(tr("Fourier Transformation")));
        mainLayout->addWidget(FFTButton);
        mainLayout->addWidget(invFFTButton);
        mainLayout->addStretch();
        setLayout(mainLayout);

        connect(FFTButton,SIGNAL(clicked()),this,SIGNAL(FFTRequest()));
        connect(invFFTButton,SIGNAL(clicked()),this,SIGNAL(iFFTRequest()));

    }
    ~TransformWindow() {;}
signals:
    void FFTRequest();
    void iFFTRequest();

private slots:

private:

    QPushButton *FFTButton;
    QPushButton *invFFTButton;


};


class TprocessTerminal : public QWidget
{
    Q_OBJECT
public:

    explicit TprocessTerminal(QWidget *parent = 0);
    ~TprocessTerminal();

    QString dataFilePath;

    int devicePixelRatio() {return FdevicePixelRatio;}
    void setDevicePixelRatio(int r) {FdevicePixelRatio=r; fp->setDevicePixelRatio(FdevicePixelRatio);}

    TransformWindow *tw;

signals:

public slots:
    void readFID();
    void saveFID();
    void plotUpdate();
    void showTransformPanel();

private slots:
    void fft();
    void ifft();

private:
    QPushButton *FIDReadButton;
    QPushButton *FIDSaveButton;
    QPushButton *transformButton;
    FIDPlotter *fp;
    QLabel *dataFilePathLabel;

    int FdevicePixelRatio;
    int currentFIDIndex;

    TFID_2D *FID_2D;
};

#endif // DATAPROCESS_H
