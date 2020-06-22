#ifndef TIMAGEGENWIDGET_H
#define TIMAGEGENWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QDebug>
#include <QComboBox>
#include <QPlainTextEdit>
#include <QStringList>
#include <QSettings>
#include "../fid.h"
#include "processBase.h"

class TImageGenWidget : public TProcessBase
{
    Q_OBJECT
public:
    explicit TImageGenWidget();
    ~TImageGenWidget() {;}

    QSpinBox *plotterIDSpinBox;
    QPlainTextEdit *epsPlainTextEdit;
    QPushButton *saveEPSPushButton;
    QPushButton *createEPSPushButton;
    QLineEdit *boundingBox1LineEdit;
    QLineEdit *boundingBox2LineEdit;
    QLineEdit *boundingBox3LineEdit;
    QLineEdit *boundingBox4LineEdit;

    void createWidgets();
    void createPanel();
    void createConnections();

    int boundingBox1() {return FBoundingBox1;}
    int boundingBox2() {return FBoundingBox2;}
    int boundingBox3() {return FBoundingBox3;}
    int boundingBox4() {return FBoundingBox4;}
    void setBoundingBox1(int i) {FBoundingBox1=i;}
    void setBoundingBox2(int i) {FBoundingBox2=i;}
    void setBoundingBox3(int i) {FBoundingBox3=i;}
    void setBoundingBox4(int i) {FBoundingBox4=i;}

    int boxHeight() {return boundingBox4()-boundingBox2();}
    int boxWidth() {return boundingBox3()-boundingBox1();}
    int plotWidth() {return boxWidth()-leftMargin()-rightMargin();}
    int plotHeight() {return boxHeight()-topMargin()-bottomMargin();}

    int xRangeIni() {return FXRangeIni;}
    int xRangeFin() {return FXRangeFin;}
    int xPlotIni() {return FXPlotIni;}
    int xPlotFin() {return FXPlotFin;}
    void setXRangeIni(int i) {FXRangeIni=i;}
    void setXRangeFin(int i) {FXRangeFin=i;}
    void setXPlotIni(int i) {FXPlotIni=i;}
    void setXPlotFin(int i) {FXPlotFin=i;}

    double scale() {return FScale;}
    void setScale(double d) {FScale=d;}
    double offset() {return FOffset;}
    void setOffset(double d) {FOffset=d;}

    int leftMargin() {return FLeftMargin;}
    int rightMargin() {return FRightMargin;}
    int topMargin() {return FTopMargin;}
    int bottomMargin() {return FBottomMargin;}
    void setLeftMargin(int i) {FLeftMargin=i;}
    void setRightMargin(int i) {FRightMargin=i;}
    void setTopMargin(int i) {FTopMargin=i;}
    void setBottomMargin(int i) {FBottomMargin=i;}

    bool drawEPSHalfFID(THalfFID *hf);
    bool drawEPSXAxis(TFID *fid);

    bool EPSCartesian(TFID *fid);
  //  bool EPSPolar(TFID *fid);

signals:

public slots:
    void createEPS();
    void saveEPS();


private:
    int FBoundingBox1;
    int FBoundingBox2;
    int FBoundingBox3;
    int FBoundingBox4;
    int FXRangeIni; // Plot Range (graph)
    int FXRangeFin; // Plot Range (graph)
    int FXPlotIni; // Plot Ragne (data)
    int FXPlotFin; // Plot Range (data)
    double FScale;
    double FOffset;

    int FLeftMargin;
    int FRightMargin;
    int FTopMargin;
    int FBottomMargin;

    QString errorMessage;
    QStringList eps0;

    bool setParams();


};

#endif // TIMAGEGENWIDGET_H
