#ifndef TPROCESSOPERATIONS_H
#define TPROCESSOPERATIONS_H

#include <QObject>
#include <QSettings>
#include "processElement.h"
#include "axisStyle.h"
#include "arraySum.h"
#include "transpose.h"
#include "flatten.h"
#include "addCutPoints.h"
#include "apodization.h"
#include "fft.h"
#include "fidDomain.h"
#include "ifft.h"
#include "phase.h"
#include "replaceRealWithAbsolute.h"
#include "cartesianMap.h"

class TProcessOperations : public QObject
{
    Q_OBJECT
public:
    explicit TProcessOperations(QObject *parent = nullptr);
    ~TProcessOperations();

    QList<TProcessElement*> processElements;
    bool saveToFile(QString filename);
    bool loadFromFile(QString filename);
    bool applyTo(TFID_2D *fid_2d);

    QString errorMessage() {return FErrorMessage;}
    void setErrorMessage(QString qs) {FErrorMessage=qs;}


signals:

public slots:


private:

    QString FErrorMessage;

};

#endif // TPROCESSOPERATIONS_H
