#ifndef TPROCESSELEMENT_H
#define TPROCESSELEMENT_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QObject>
#include <QDebug>
#include "../fid.h"
//#include "applyMode.h"

#define TWOPI 6.28318530717958647692528676656

#define RESHAPE_AL "reshape_al"
#define RESHAPE_NOD "reshape_number_of_divisions"
#define RESHAPE_INVALID "reshape_invalid"


//class TProcessElement : public QObject
class TProcessElement : public QThread
{
    Q_OBJECT
public:
//    explicit TProcessElement(QObject *parent = nullptr);
    TProcessElement();
    ~TProcessElement()
    {
      mutex.lock();
        stopped=true;
        condition.wakeOne();
      mutex.unlock();
      wait();
    }



    enum TProcessType {CutAdd,
                       Apodization,
                       FFT,
                       IFFT,
                       Phase,
                       PhaseReverse,
                       AxisStyle,
                       ArraySum,
                       Transpose,
                       Flatten,
                       Reshape,
                       CartesianMap3D,
                       FFT3D,
                       Math,
                       InvalidType
                      };

    enum TFIDMathOperation {Add,Subtract,Multiply,Divide,
                            Normalize,Offset,PhaseOffset,
                            ReversePhase, InvalidMathOperation};

    enum TFIDMathOperationWith {Number,File,Buffer};

    enum TReshapeOption {ReshapeAL,
                         ReshapeNumberOfDivisions,
                         ReshapeInvalid};

    void stop() {QMutexLocker locker(&mutex); stopped=true; condition.wakeAll();}


    TProcessType processType() {return FProcessType;}
    QString processTypeStr();
    void setProcessType(TProcessType pt) {FProcessType=pt;}
    void setProcessTypeStr(QString);

    enum TApplyMode {ApplyToAll, ApplyToOne, ApplyToOthers, ApplytoNone};

    virtual bool process(TFID *fid) {qDebug() << QString(Q_FUNC_INFO) << fid->comment(); return true;}
    virtual bool process(TFID_2D *fid_2d, int k) {qDebug() << QString(Q_FUNC_INFO) << fid_2d->comments << k; return true;}
    virtual bool process(TFID_2D *fid_2d) {qDebug() << QString(Q_FUNC_INFO) << fid_2d->comments; return true;}
    virtual bool process(TFID_2D *fid_2d, int k, int l) {qDebug() << QString(Q_FUNC_INFO) << fid_2d->comments << k << l; return true;}
    virtual QString command() {return "";}

    virtual QStringList processInformation() {return QStringList();}
    QString warningMessage() {return FWarningMessage;}
    QString errorMessage() {return FErrorMessage;}
    bool errorQ;
    bool warningQ;

    int applyMode() {return FApplyMode;}
    int applyIndex() {return FApplyIndex;}


    // virtual functions for cutadd
    virtual int operation() {return 0;}
    virtual void setOperation(int) {;}
    virtual int headTail() {return 0;}
    virtual void setHeadTail(int) {;}
    virtual int headPoints() {return 0;}
    virtual void setHeadPoints(int) {;}
    virtual int tailPoints() {return 0;}
    virtual void setTailPoints(int){;}
    virtual int averagePoints() {return 0;}
    virtual void setAveragePoints(int) {;}

    // virtual functions for apodization
    virtual int apodizationType() {return 0;}
    virtual void setApodizationType(int) {;}
    virtual double width() {return 0;}
    virtual void setWidth(double) {;}
    virtual bool inverse() {return false;}
    virtual void setInverse(bool) {;}

    // virtual functions for reshape
    virtual int reshapeAL() {return 0;}
    virtual void setReshapeAL(int ) {return;}
    virtual int reshapeNOfDivisions() {return 0;}
    virtual void setReshapeNOfDivisions(int) {return;}
    virtual TReshapeOption reshapeOption() {return TReshapeOption::ReshapeAL;}
    virtual void setReshapeOption(TReshapeOption) {return;}
    virtual QString reshapeOptionStr() {return "";}
    virtual void setReshapeOptionStr(QString) {return;}

    // virtual functions for FFT, IFFT
    virtual bool Laplace() {return false;}
    virtual void setLaplace(bool) {;}
    virtual double LaplaceWidth() {return 0;}
    virtual void setLaplaceWidth(double) {;}
    virtual int axisDomain() {return 0;}
    virtual void setAxisDomain(int) {;}

    // virtual functions for phase
    virtual double initialPhase0() {return 0;}
    virtual double initialPhase1() {return 0;}
    virtual void setInitialPhase0(double ) {return;}
    virtual void setInitialPhase1(double ) {return;}
    virtual int pivot() {return 0;}
    virtual double phase0() {return 0;}
    virtual double phase1() {return 0;}
    virtual double accumPhase0() {return 0;}
    virtual double accumPhase1() {return 0;}
    virtual void setPivot(int ) {return;}
    virtual void setPhase0(double ) {return;}
    virtual void setPhase1(double ) {return;}
    virtual void setAccumPhase0(double ) {return;}
    virtual void setAccumPhase1(double ) {return;}
    virtual void changePhase0To(double ) {return;}
    virtual void changePhase1To(double ) {return;}

    // virtual functions for TAxisStyle
    virtual int axisStyle() {return 0;}
    virtual void setAxisStyle(int) {;}
    virtual int domain() {return 0;}
    virtual void setDomain(int) {;}
    virtual void setDomain(QString) {;}
    virtual int unit() {return 0;}
    virtual void setUnit(int) {;}
    virtual void setUnit(QString) {;}
    virtual QString label() {return "";}
    virtual void setLabel(QString) {;}
    virtual int referencePoint() {return 0;}
    virtual void setReferencePoint(int) {;}
    virtual double referenceValue() {return 0;}
    virtual void setReferenceValue(double) {;}

    // virtual functions for TCartesianMap3D
    virtual QString cartesianMap3DPolarAnglesStr() {return "";}
    virtual bool setCartesianMap3DPolarAnglesStr(QString) {return false;}

    //virtual functions for TFFT3D;
    virtual void FFT3D_setLengths(int,int) {;}
    virtual void FFT3D_setN1(int) {;}
    virtual void FFT3D_setN2(int) {;}
    virtual int FFT3D_n1() {return 0;}
    virtual int FFT3D_n2() {return 0;}

    //virtual functions for math
    virtual TFIDMathOperationWith FIDMathOperationWith(){return TProcessElement::Number;}
    virtual void setFIDMathOperationWith(TFIDMathOperationWith) {;}
    virtual TFIDMathOperation FIDMathOperation() {return TProcessElement::Add;}
    virtual void setFIDMathOperation(TFIDMathOperation) {;}
    virtual QString FIDMathOperationStr() {return "";}
    virtual void setFIDMathOperationStr(QString) {;}
    virtual QString FIDMathOperationWithStr() {return "";}
    virtual void setFIDMathOperationWithStr(QString) {;}
    virtual double FIDMathReal() {return 0;}
    virtual void setFIDMathReal(double) {;}
    virtual double FIDMathImag() {return 0;}
    virtual void setFIDMathImag(double) {;}
    virtual int FIDMathXIni() {return 0;}
    virtual int FIDMathXFin() {return 0;}
    virtual void setFIDMathXIni(int) {;}
    virtual void setFIDMathXFin(int) {;}
    virtual QString FIDMathDirName() {return "";}
    virtual QString FIDMathFileName() {return "";}
    virtual void setFIDMathDirName(QString) {;}
    virtual void setFIDMathFileName(QString) {;}


    QMutex mutex;
    QWaitCondition condition;
    bool volatile stopped;

public slots:
    void setApplyMode(int ap) {FApplyMode=ap;}
    void setApplyIndex(int i) {FApplyIndex=i;}

protected:
    void setWarningMessage(QString qs) {FWarningMessage=qs;}
    void setErrorMessage(QString qs) {FErrorMessage=qs;}

private:
    TProcessType FProcessType;
    QString FWarningMessage;
    QString FErrorMessage;
    int FApplyIndex;
    int FApplyMode;


};

#endif // TPROCESSELEMENT_H
