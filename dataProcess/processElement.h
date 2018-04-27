#ifndef TPROCESSELEMENT_H
#define TPROCESSELEMENT_H

#include <QObject>
#include <QDebug>
#include "../fid.h"
#include "applyMode.h"

#define TWOPI 6.28318530717958647692528676656


class TProcessElement : public QObject
{
    Q_OBJECT
public:
    explicit TProcessElement(QObject *parent = 0);
    ~TProcessElement() { ;}

    virtual bool process(TFID *fid) {qDebug() << QString(Q_FUNC_INFO) << fid->comment(); return true;}
    virtual bool process(TFID_2D *fid_2d, int k) {qDebug() << QString(Q_FUNC_INFO) << fid_2d->comments << k; return true;}
    virtual bool process(TFID_2D *fid_2d) {qDebug() << QString(Q_FUNC_INFO) << fid_2d->comments; return true;}
    virtual QString command() {return "";}

    virtual QStringList processInformation() {return QStringList();}
    QString warningMessage;
    QString errorMessage;
    bool errorQ;
    bool warningQ;

    int applyMode() {return FApplyMode;}
    int applyIndex() {return FApplyIndex;}

public slots:
    void setApplyMode(int ap) {FApplyMode=ap;}
    void setApplyIndex(int i) {FApplyIndex=i;}

protected:

private:
    int FApplyIndex;
    int FApplyMode;

};

#endif // TPROCESSELEMENT_H
