#ifndef TPROCESSELEMENT_H
#define TPROCESSELEMENT_H

#include <QObject>
#include <QDebug>
#include "../fid.h"
//#include "applyMode.h"

#define TWOPI 6.28318530717958647692528676656


class TProcessElement : public QObject
{
    Q_OBJECT
public:
    explicit TProcessElement(QObject *parent = nullptr);
    ~TProcessElement() { ;}

    enum TProcessType {CutAdd,
                       Apodization,
                       FFT,
                       IFFT,
                       Phase,
                       AxisStyle,
                       ArraySum,
                       Transpose,
                       Flatten
                      };


    TProcessType processType() {return FProcessType;}
    void setProcessType(TProcessType pt) {FProcessType=pt;}

    enum TApplyMode {ApplyToAll, ApplyToOne, ApplyToOthers, ApplytoNone};

    virtual bool process(TFID *fid) {qDebug() << QString(Q_FUNC_INFO) << fid->comment(); return true;}
    virtual bool process(TFID_2D *fid_2d, int k) {qDebug() << QString(Q_FUNC_INFO) << fid_2d->comments << k; return true;}
    virtual bool process(TFID_2D *fid_2d) {qDebug() << QString(Q_FUNC_INFO) << fid_2d->comments; return true;}
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

    // virtual functions for FFT, IFFT
    virtual bool Laplace() {return false;}
    virtual void setLaplace(bool) {;}
    virtual double LaplaceWidth() {return 0;}
    virtual void setLaplaceWidth(double) {;}
    virtual int axisDomain() {return 0;}
    virtual void setAxisDomain(int) {;}

    // virtual functions for phase
    virtual int pivot() {return 0;}
    virtual double phase0() {return 0;}
    virtual double phase1() {return 0;}
    virtual void setPivot(int ) {return;}
    virtual void setPhase0(double ) {return;}
    virtual void setPhase1(double ) {return;}

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
