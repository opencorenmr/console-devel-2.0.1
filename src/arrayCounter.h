#ifndef ARRAYCOUNTER_H
#define ARRAYCOUNTER_H

#include <QList>
#include <QString>
#include <QObject>
#include "pulseProgram.h"

class TArrayCounter : public QObject
{
    Q_OBJECT
public:
    TArrayCounter();

    QList<int> NOfCounts;
    QList<int> currentCount;
    QList<bool> updated;
    void append(int k);
    void removeAt(int k);
    void init(TpulseProgram *ppg);
    void clear();
    bool hasFinished;
    QString currentCountString();
    int arrayCount();
    int remainingArrayCount() {return FRemainingArrayCount;}

  //  QStringList arrayScript(TpulseProgram *ppg);

signals:

    void incremented(int dimension);
    void finished();
    void updateCurrentCountRequest(QString ccs);

public slots:
    void increment(TpulseProgram *ppg);

protected:

private:
    int FTotalArrayCount;
    int FRemainingArrayCount;

};



#endif // ARRAYCOUNTER_H
