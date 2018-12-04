#include <QDebug>
#include <QApplication>

#include <QProgressDialog>
#include "TxRxThread.h"

USBTxThread::USBTxThread(QObject *parent):QThread(parent)
{
  strList.clear();
  abort=false;
  ignoreRunPrompt=false;
}

USBTxThread::~USBTxThread()
{
    mutex.lock();
    abort=true;
    condition.wakeOne();
    mutex.unlock();
    wait();
}


void USBTxThread::send(QString s)
{
    mutex.lock();
      strList.clear();
      strList.append(s);
    mutex.unlock();

    QMutexLocker locker(&mutex);
    if(!isRunning()){
        start(LowPriority);
    } else {
        condition.wakeOne();
    }
}

void USBTxThread::send(QStringList sList)
{
    mutex.lock();
      strList=sList;
    mutex.unlock();

    QMutexLocker locker(&mutex);

    if(!isRunning()){
        start(LowPriority);
    } else {
        condition.wakeOne();
    }

}

void USBTxThread::proceed(QChar qc)
{
    if(ignoreRunPrompt && qc==QChar('#')) return;

    QMutexLocker locker(&mutex);

    if(!isRunning()){
        start(LowPriority);
    } else {
        condition.wakeOne();
    }

}

void USBTxThread::run()
{
  int i,j;
  char c[1000];
  FT_STATUS ftStatus;
  DWORD dwBytesWritten;
  QChar *data;

  forever{

    if (abort) return;
    mutex.lock();

    i=0;
    while(i<strList.count())
    {
      qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

      emit currentLine(i);

      QString str=strList[i];
      data = str.data();

      j=0;
      while (!data->isNull()) {
        c[j] = data->toUpper().toLatin1();
        ++data;
        j++;
      } // while (j)
      c[j]=QChar(0x0d).toLatin1();  // Lastly, "return" is added to c.
      j++;  // Now, j carries the number of characters, including the return (0x0d).
      ftStatus = FT_Write(deviceHandle(), c, j, &dwBytesWritten);
      if(ftStatus==FT_OK) emit sent(str);

      // Here, we let the thread sleep, which will be woken up by proceed(), which, in turn,
      // called upon reception of the prompt (*) by the rThread
      condition.wait(&mutex);
      i++;
    } // while (i)

    strList.clear();
    emit transferComplete();

    condition.wait(&mutex);   // Let the thread sleep.
    mutex.unlock();

  } // forever;
} // run();


USBRxThread::USBRxThread(QObject *parent):QThread(parent)
{
    stopped=false;
}

USBRxThread::~USBRxThread()
{
    mutex.lock();
      stopped=true;
      condition.wakeOne();
    mutex.unlock();
    wait();
}


/*
void USBRxThread::sleep(FT_HANDLE devHandle)
{
    QMutexLocker locker(&mutex);
    condition.wait(&mutex);
}
*/
void USBRxThread::stop()
{
    QMutexLocker locker(&mutex);
    stopped=true;
}

void USBRxThread::standBy()
{
    QMutexLocker locker(&mutex);
    stopped=false;
    if(!isRunning()) start(LowPriority); else condition.wakeOne();
}

void USBRxThread::run()
{
    QString sentence="";
    QChar ch;
    char pcBufRead[1];
    DWORD dwRxSize;
    DWORD dwBytesRead;

    FT_STATUS	ftStatus;

    dwRxSize=0;

    forever {

      if (stopped) return;

      while(dwRxSize==0) {
          if (stopped) return;
          ftStatus=FT_GetQueueStatus(deviceHandle(), &dwRxSize);

          msleep(10);  // 10 ms sleep

      }  // wait until the device has received at least one character

      ftStatus=FT_Read(deviceHandle(), pcBufRead, 1, &dwBytesRead);
      ch=ch.fromLatin1(pcBufRead[0]);
      if(ch!='*' && ch!='#' && ch!='%' && ch!=0x0D)  sentence.append(ch);
      emit gotChar(ch);

    //  if(ch=='*') {emit gotPrompt(ch); emit readyPrompt();qDebug()<<"sentence: " << sentence;sentence.clear();}
    //  if(ch=='#') {emit gotPrompt(ch); emit runPrompt();}
      if(ch=='*' || ch=='#' || ch=='%')
      {
         emit gotPrompt(ch);
         emit gotSentence(sentence);

         //qDebug()<<"sentence: " << sentence;

         sentence.clear();
         if(ch=='*') emit readyPrompt();
         else if(ch=='#') emit runPrompt();
         else if(ch=='%') emit arrayPrompt();
      }

      ftStatus=FT_GetQueueStatus(deviceHandle(), &dwRxSize);
    }

//    qDebug()<< "ftStatus: " <<ftStatus;

}
