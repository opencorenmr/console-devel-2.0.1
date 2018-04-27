#ifndef PLOTTER_H
#define PLOTTER_H

#include <QMap>
#include <QSet>
#include <QPixmap>
#include <QVector>
#include <QWidget>
#include <QAction>
#include <QToolBar>
#include <QLabel>
#include <QSpinBox>
#include <QList>
#include <QSplitter>
#include <QMainWindow>
#include <QDebug>
#include <QLineEdit>
#include <QLabel>
#include <QCheckBox>
#include <QPlainTextEdit>

#include "fid.h"
#include "dataProcess/processOperations.h"
//#include "dataProcess/processPanelWidget.h"

class QPushButton;
class QToolButton;
class PlotSettings;
class Plotter;
class QComboBox;

class FIDPlotter;

class FIDPlotOption
{
    enum PlotScale {DataScalse,ManualScale,ReceiverScale};
};

//
//class TCursorInfo
//{
//public:
//    int xPosition;
//    double xValue;
//    int real;
//    int imag;
//    int abs;
//    int phase;
//};

class plotterDetailsWidget: public QWidget
{
    Q_OBJECT
public:
    plotterDetailsWidget(QWidget *parent = 0);
    QComboBox *detailsComboBox;
    QSpinBox *xIniSpinBox,*xFinSpinBox;
    QLineEdit *xIniValLineEdit,*xFinValLineEdit;
    QLabel *xIniUnitLabel,*xFinUnitLabel;

    QPlainTextEdit *deltaInfoTextEdit;
   // QSpinBox *xPosSpinBox;

signals:
    void xPlotRangeUpdateRequest(int i, int f);
    void xCursorPositionUpdateRequest(int xi);

public slots:
    void setCursorInfo(QStringList sl);
    void setPlotRangeInfo(QStringList sl);
    void onXIniValLineEditReturnPressed();
    void onXFinValLineEditReturnPressed();
    void onXIniSpinBoxValueChanged();
    void onXFinSpinBoxValueChanged();
    void onXPosLineEditReturnPressed();

private slots:

private:
    QLineEdit *realLineEdit, *imagLineEdit, *absLineEdit;
    QLineEdit *phaseLineEdit, *xValLineEdit;
    QLineEdit *xPosLineEdit;

    QString previousXPos;
    //QStringList FCurrentCursorInfo;
    //QStringList FPreviousCursorInfo;

};

class Plotter : public QWidget
{
    Q_OBJECT
public:
    enum TscaleSetting {ReceiverScale,DataScale,ManualScale};
    TscaleSetting scaleSetting;

    enum TPlotFormat {CartesianPlot,PolarPlot};
    TPlotFormat plotFormat() {return FplotFormat;}
    void setPlotFormat(TPlotFormat f)
    {
        FplotFormat=f;
      //  if(f==CartesianPlot) bottomMargin=40; else bottomMargin=0;
    }

    Plotter(QWidget *parent = 0);
    ~Plotter();
    
    void setPlotSettings(const PlotSettings &settings);
    void setCurveData(int id, const QVector<QPointF> &data);
    void clearCurve(int id);
    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    int devicePixelRatio() {return FDevicePixelRatio;}
    void setFID(TFID *f);
    bool fidSetted;
    bool rubberBandEnabled() {return FrubberBandEnabled;}
    bool vCursorEnabled() {return FvCursorEnabled;}
    bool vCursorIsShown() {return FvCursorIsShown;}
    bool rubberBandIsShown() {return FrubberBandIsShown;}
    bool bitLineIsShown() {return FbitLineIsShown;}

    void adjustScale();

    TFID *fid;
    TFID *localFID;

    int xini;
    int xfin;
    int vCenter;
    int vOffset() {return FvOffset;}
    void setVOffset(int v) {FvOffset=v;}

    QPixmap pixmap;

    int Margin;
    int topMargin;
    int bottomMargin;
    int leftMargin;
    int rightMargin;

    static double tic(double min, double max);

    enum TPlotItemOption {plotReal, plotImag, plotAbs};
    QSet<TPlotItemOption> plotItemOption;

    void refresh();

    double scale() {return Fscale;}
    void setScale(double s) {Fscale=s;}

    int penWidth() {return FPenWidth;}

    QColor backgroundColor0() {return FBackgroundColor0;}
    QColor backgroundColor1() {return FBackgroundColor1;}
    void setBackgroundColor0(QColor col) {FBackgroundColor0=col;}
    void setBackgroundColor1(QColor col) {FBackgroundColor1=col;}


signals:
    void rubberBandActionRequest(bool b);
    void xIniSignal(int xi);
    void xFinSignal(int xf);
    void clickedXPosition(int x);
    void vCursorXSignal(QString s);
    void yRealSignal(QString s);
    void yImagSignal(QString s);
    void yAbsSignal(QString s);
    void scaleSignal(QString s);
    void cursorInfo(QStringList sl);
    void deltaInfo(QString qs);
    void plotRangeInfo(QStringList sl);

public slots:
    void updatePlotRange(int i, int f);
    void updateXCursorPosition(int xi);
    void setPenWidth(int w) {if(w<1)FPenWidth=1; else if(w>10)FPenWidth=10; else FPenWidth=w;}
    void hExpand();
    void hShrink();
    void moveRight();
    void moveLeft();
    void setDevicePixedRatio(int r) {FDevicePixelRatio=r; pixmap.setDevicePixelRatio(r);}
    void zoomIn();
    void zoomOut();
    void setRubberBandEnabled(bool b) {FrubberBandEnabled=b;}
    void setVCursorEnabled(bool b)
    {
        FvCursorEnabled=b;
        setVCursorIsShown(b);
        refresh();
    }
    void setVCursorIsShown(bool b) {FvCursorIsShown=b;}
    void setRubberBandIsShown(bool b) {FrubberBandIsShown=b;}
    void setBitLineIsShown(bool b) {FbitLineIsShown=b;}


protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);

private:
    QColor FBackgroundColor0, FBackgroundColor1;
    TPlotFormat FplotFormat;
    void drawBitLines(QPainter *painter);
    void updateRubberBandRegion();
    void refreshPixmap();
    void drawGrid(QPainter *painter);
    void drawFrame(QPainter *painter);
    void drawCurves(QPainter *painter);
    void drawFID(QPainter *painter);
    void drawHalfFID(THalfFID *hFID, QPainter *painter, QColor color);
    void drawPolarFID(QPainter *painter, QColor color);
    void drawXTicks(QPainter *painter);
    void drawYZeroLine(QPainter *painter);
    void drawVCursor(QPainter *painter, int x);

   // enum {Margin=0};
    double Fscale;

    QMap<int, QVector<QPointF> > curveMap;
    QVector<PlotSettings> zoomStack;

    bool FrubberBandEnabled;
    bool FvCursorEnabled;
    bool FrubberBandIsShown;
    bool FvCursorIsShown;
    bool FbitLineIsShown;

    int FvOffset;

    int curZoom;
    QRect rubberBandRect;

//    bool cursorIsShown;

    double minimumXValue,maximumXValue;
    void setX();


    double xValue(int k) {return k*fid->dw()/1000;} // in ms
    double fxValue(int k) {return ((double)k/fid->al()-0.5)/fid->dw();}

    int FVCursorXPosition;
    void setVCursorXPosition(int i) {FPreviousVCursorXPosition=FVCursorXPosition; FVCursorXPosition=i;}
    int vCursorXPosition() {return FVCursorXPosition;}

    int FPreviousVCursorXPosition;
    int previousVCursorXPosition() {return FPreviousVCursorXPosition;}


    double plotOffset;
    int FPenWidth;

    int FDevicePixelRatio;
};

class PlotSettings
{
public:
    PlotSettings();
    void scroll(int dx, int dy);
    void xScroll(int dx);
    void yScroll(int dy);
    void adjust();
    double spanX() const {return maxX-minX;}
    double spanY() const {return maxY-minY;}

    double minX;
    double maxX;
    int numXTicks;
    double minY;
    double maxY;
    int numYTicks;

private:
    static void adjustAxis(double &min, double &max, int &numTicks);
};


class TPlotWindow: public QWidget
{
    Q_OBJECT

public:

signals:
    void deleteRequest(TPlotWindow *pw);
protected:
    void closeEvent(QCloseEvent *event);

};


class TFIDPlotters : public QWidget
{
    Q_OBJECT
public:
    enum PlotSplitMode {Horizontal,Vertical,NewWindow,RemoveSplit};

    TFIDPlotters(QWidget *parent=0);
    ~TFIDPlotters();
    QList<FIDPlotter*> FIDPlotters;
    QList<QSplitter*> plotSplitters;
    QList<QWidget*> plotWindows;
    TFID_2D *FID_2D;

    bool isFFTEnabled() {return FisFFTEnabled;}
    void setFFTEnabled(bool b);

    void setFID2D(TFID_2D *f2);
    int devicePixelRatio() {return FDevicePixelRatio;}
    void setDevicePixelRatio(int r);

    QColor backgroundColor0() {return FBackgroundColor0;}
    QColor backgroundColor1() {return FBackgroundColor1;}
    void setBackgroundColor0(QColor col);
    void setBackgroundColor1(QColor col);

signals:
    //void splitPlotUpdated(int nOfPlotters);
    void numberOfPlottersUpdated(int nOfPlotters);

public slots:
    void addAndCopyPlotter(FIDPlotter *fp);
    void splitPlot(FIDPlotter *fp, TFIDPlotters::PlotSplitMode splitMode);

    void update();
//    void test() {qDebug()<<"!!!";}

private:
    int FDevicePixelRatio;
    bool FFID2DSetted;
    bool FisFFTEnabled;
    int FN;
    QColor FBackgroundColor0,FBackgroundColor1;

};



//class FIDPlotter : public Plotter
class FIDPlotter : public QWidget
{
    Q_OBJECT
public:
    FIDPlotter(QWidget *parent=0);
    void setDevicePixelRatio(int r);
    int devicePixelRatio() {return FDevicePixelRatio;}
    Plotter *plotter;
    QComboBox *scaleComboBox;
    QComboBox *formatComboBox;
    TFID_2D *fid2d;
    QSpinBox *FIDSelectSpinBox;
    QComboBox *splitViewComboBox;
    plotterDetailsWidget *plotterDetails;
    QSpinBox *penWidthSpinBox;

    QCheckBox *FFTCheckBox;
    QCheckBox *processCheckBox;
    QSpinBox *phase0SpinBox;

    QAction *realAction;
    QAction *imagAction;
    QAction *absAction;
    QAction *vCursorAction;
    QAction *exportAsciiAction;
    QAction *exportImageAction;

    QLabel *IDLabel;

    int IDNumber() {return FIDNumber;}
    void setIDNumber(int i) {FIDNumber=i;}

    //TProcessPanelWidget *processPanelWidget;

   // QLabel *testLabel;

    bool isFID2DSetted() {return Ffid2dSetted;}
    void setFID2D(TFID_2D *f2) {fid2d=f2; Ffid2dSetted=true;}

    void setPath(QString qs) {FPath=qs;}
    QString path() {return FPath;}

signals:
//    void splitRequest(FIDPlotter* fp, int splitMode);
    void splitRequest(FIDPlotter *fp, TFIDPlotters::PlotSplitMode splitMode);

public slots:
    void setPlotItemOption();
    void setPlotScaleOption();
    void setPlotFormat();
    void setBitLineOption();
    void exportAscii();
    void exportImage();
    void update();
    void setVCursor(bool b);

private slots:
    void setRubberBand(bool b);
  //  void updateCursorInfo(int i) {vcxLabel->setText(QString::number(i));}
    void onFIDSelectSpinBoxChanged(int i);
    void onSplitViewRequest(int i);
    void thickkerLine();
    void thinnerLine();


private:
    bool Ffid2dSetted;
    int FIDNumber;
    QLabel *FID2DLengthLabel;
    QString FPath;

protected:
    int FDevicePixelRatio;

    QAction *thickkerAction;
    QAction *thinnerAction;

    QAction *bitLineAction;
    QAction *hExpandAction;
    QAction *hShrinkAction;
    QAction *moveRightAction, *moveLeftAction;

    QAction *exportAction;

    QToolBar *toolBar;
    QToolBar *toolBar2;
    QAction *rubberBandAction;
    void createActions();
    void createToolBar();
QLabel *vcxLabel;
//QLabel *yrLabel;

};




#endif // PLOTTER_H
