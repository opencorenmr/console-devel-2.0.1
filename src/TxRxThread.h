#ifndef TXRXTHREAD_H
#define TXRXTHREAD_H


#include <QMutex>
#include <QThread>
#include <QWaitCondition>
#include <QString>
#include <QStringList>

#include "ftd2xx.h"

class USBTxThread :  public QThread
{
    Q_OBJECT
    Q_DISABLE_COPY(USBTxThread)

public:
    USBTxThread(QObject *parent=0);
    ~USBTxThread();

    QStringList strList;
    void setDeviceHandle(FT_HANDLE devHandle) {FdevHandle=devHandle;}
    FT_HANDLE deviceHandle(){return FdevHandle;}
    void send(QString s);
    void send(QStringList sList);
    bool ignoreRunPrompt;

protected:
    void run();

public slots:
    void proceed(QChar qc);

signals:
    void sent(QString);
    void transferComplete();
    void currentLine(int);

private:
    QMutex mutex;
    QWaitCondition condition;
    volatile bool abort;
    volatile bool restart;
    FT_HANDLE FdevHandle;
 //   QString str;

};




class USBRxThread : public QThread
{
    Q_OBJECT
    Q_DISABLE_COPY(USBRxThread)

public:
    USBRxThread(QObject *parent=0);
    ~USBRxThread();

    void setDeviceHandle(FT_HANDLE devHandle) {FdevHandle=devHandle;}
    FT_HANDLE deviceHandle(){return FdevHandle;}
    void standBy();
    void stop();

signals:
    void gotChar(QChar);
    void gotPrompt(QChar);
    void gotSentence(QString);
    void readyPrompt();
    void runPrompt();
    void arrayPrompt();

protected:
    void run();

private:
    QMutex mutex;
    QWaitCondition condition;
    bool volatile stopped;
    FT_HANDLE FdevHandle;



};





#endif // TXRXTHREAD_H
