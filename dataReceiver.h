#ifndef DATARECEIVER_H
#define DATARECEIVER_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>


#include "ftd2xx.h"
#include "fid.h"

class dataReceiver : public QThread
{
    Q_OBJECT
public:

    dataReceiver()
    {
        localFid = new TFID(16384);
        stopped=false;
    }
    void setDeviceHandle(FT_HANDLE devHandle) {FdevHandle=devHandle;}

    ~dataReceiver() {
      mutex.lock();
        stopped=true;
        condition.wakeOne();
      mutex.unlock();
      wait();
      delete localFid;
    }


    void stop() { QMutexLocker locker(&mutex); stopped=true;condition.wakeAll();}
    void standBy() {
        QMutexLocker locker(&mutex);
        stopped=false;
        if(!isRunning())
//          start(LowPriority);
//          start(HighPriority);
          start(HighestPriority);
        else
          condition.wakeAll();
  //        condition.wakeOne();
    }

    FT_HANDLE deviceHandle() {return FdevHandle;}
    TFID *localFid;

protected:
    void run();

signals:
    void gotFID(TFID *f);
    void copyCompleteReceived();
    void receiverStatusChanged(bool busy);

public slots:
    void wake() {condition.wakeOne();}
//    void setAL(int al) {localFid->setAL(al);}

private slots:

private:
    FT_HANDLE FdevHandle;
    FT_STATUS	ftStatus;

    DWORD dwRxSize;
    DWORD dwBytesRead,dwBytesWritten;
    unsigned char pcBufRead[8192];   // "unsigned" is important! 2015.1.22 Takeda
    char c[1];

    QMutex mutex;
    QWaitCondition condition;

    bool transferBusy;

    int count;
    bool volatile stopped;

};

#endif // DATARECEIVER_H
