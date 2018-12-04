#ifndef TPROCESSOPERATIONS_H
#define TPROCESSOPERATIONS_H

#include <QObject>
#include "processElement.h"
#include "fft.h"
#include "fidDomain.h"
#include "ifft.h"
#include "phase.h"
#include "replaceRealWithAbsolute.h"
#include "slowft.h"
#include "slowift.h"

class TProcessOperations : public QObject
{
    Q_OBJECT
public:
    explicit TProcessOperations(QObject *parent = 0);
    ~TProcessOperations();

    QList<TProcessElement*> processElements;

signals:

public slots:

};

#endif // TPROCESSOPERATIONS_H
