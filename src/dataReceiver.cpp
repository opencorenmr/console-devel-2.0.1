#include <QDebug>
#include <QEventLoop>
#include "dataReceiver.h"

void dataReceiver::run()
{
    emit receiverStatusChanged(false); // receiver ready
    dwRxSize=0;

    forever {
      count=0;
      if (stopped) break;
      ftStatus=FT_GetQueueStatus(deviceHandle(), &dwRxSize);

      while(dwRxSize==0) {
          if (stopped) break;
          ftStatus=FT_GetQueueStatus(deviceHandle(), &dwRxSize);
          msleep(1);  // 1 ms sleep
      }  // wait until the device has received at least one character

      if (stopped) break;
      emit receiverStatusChanged(true); // receiver busy

      while(count<localFid->al())
      {

          if(stopped) break;

         // mutex.lock();
          ftStatus=FT_GetQueueStatus(deviceHandle(), &dwRxSize);
         // mutex.unlock();

          while(dwRxSize<8)
          {
              if(stopped) break;
            //  mutex.lock();
              ftStatus=FT_GetQueueStatus(deviceHandle(), &dwRxSize);
              usleep(100);
            //  mutex.unlock();
          }
          if(stopped) break;
          int nOfB=dwRxSize / 8;
          if(nOfB>1024) nOfB=1024;
         // mutex.lock();
          ftStatus=FT_Read(deviceHandle(), pcBufRead, 8*nOfB, &dwBytesRead);
         // mutex.unlock();
         // qDebug() << dwBytesRead << " bytes read.";

          int offset=0;

          for(int k=0; k<nOfB; k++)
          {
            if(stopped) break;

            if(count+k>localFid->al()-1) {

                qDebug()<<"Index out of range (dataReceiver::run)   count: " << count << ", k: " << k;
                stopped=true;
                break;
            }

            mutex.lock();
//            localFid->real->sig[count+k]=(float)((pcBufRead[0+offset] << 24) + (pcBufRead[1+offset] << 16)
//                                        + (pcBufRead[2+offset] << 8) + (pcBufRead[3+offset]));
//            localFid->imag->sig[count+k]=(float)((pcBufRead[4+offset] << 24) + (pcBufRead[5+offset] << 16)
//                                        + (pcBufRead[6+offset] << 8) + (pcBufRead[7+offset]));
            localFid->real->sig[count+k]=(double)((pcBufRead[0+offset] << 24) + (pcBufRead[1+offset] << 16)
                                        + (pcBufRead[2+offset] << 8) + (pcBufRead[3+offset]));
            localFid->imag->sig[count+k]=(double)((pcBufRead[4+offset] << 24) + (pcBufRead[5+offset] << 16)
                                        + (pcBufRead[6+offset] << 8) + (pcBufRead[7+offset]));
            offset += 8;

            mutex.unlock();
          }

          count+=nOfB;

      }
      if (stopped) break;


      emit gotFID(localFid);  //qDebug() << " gotFID ";
      // The gotFID signal is connected to slot fpgaTerminal::copyFID
   //   QEventLoop loop;
   //   loop.connect(this, SIGNAL(copyCompleteReceived()), &loop, SLOT(quit()));
   //   loop.exec();
      // We let the thread wait for the copyComplete signal from fpgaTermina::copyFID

      //qDebug() << " copyComplete received. ";


     // mutex.lock();

      c[0]=QChar(0x0d).toLatin1();
      ftStatus = FT_Write(deviceHandle(), c, 1, &dwBytesWritten);

     // mutex.unlock();
      emit receiverStatusChanged(false);


    } // forever

    emit receiverStatusChanged(false);
}

