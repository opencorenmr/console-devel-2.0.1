#include <QBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QDebug>
#include <QThread>
#include <QPushButton>
#include "gpib488console.h"

TGPIB488Console::TGPIB488Console(QWidget *parent) : QWidget(parent)
{
    //this->hide();
    this->setWindowTitle(tr("GPIB-488"));
    this->setWindowFlags(Qt::Dialog);
    this->setFixedWidth(400);
    this->setFixedHeight(300);

    FDeviceHandle=0;
    FDevPad=4;
    console = new Console;
    FMessage.clear();
    FBufSize=10000;

    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(new QLabel(tr("Primary address")));
      padSpinBox = new QSpinBox;
      padSpinBox->setMaximum(30);
      padSpinBox->setValue(4);
      QPushButton *openGPIBPushButton = new QPushButton(tr("Open GPIB"));
      writeOnArrayCheckBox = new QCheckBox(tr("Write on Array Exp."));
      writeOnArrayCheckBox->setChecked(false);
      writeOnArrayTextEdit = new QPlainTextEdit;
      writeOnArrayTextEdit->setWindowTitle("Write GPIB-488 on Array");
    hLayout->addWidget(padSpinBox);
    hLayout->addStretch();
    hLayout->addWidget(openGPIBPushButton);
    QVBoxLayout *vLayout = new QVBoxLayout;
    //vLayout->setMargin(0);
    vLayout->addLayout(hLayout);
    vLayout->addWidget(console);
    vLayout->addWidget(writeOnArrayCheckBox);
    this->setLayout(vLayout);

    //this->show();

    connect(openGPIBPushButton,SIGNAL(clicked()),this,SLOT(open()));
    connect(writeOnArrayCheckBox,SIGNAL(toggled(bool)),this,SLOT(onWriteOnArrayCheckBoxToggled(bool)));

    //this->open();
}

void TGPIB488Console::onWriteOnArrayCheckBoxToggled(bool b)
{
  if(!b)
  {

      writeOnArrayTextEdit->hide();
      return;
  }

  writeOnArrayTextEdit->show();


}

void TGPIB488Console::open()
{
    FDeviceHandle = 0;
    FDeviceHandle =
//      ibdev(GPIB_BOARD_NUM,4,GPIB_DEV_SAD,10,0,0); // Open the device
//    ibdev(GPIB_BOARD_NUM,devPad(),GPIB_DEV_SAD,10,0,0); // Open the device
    ibdev(GPIB_BOARD_NUM,padSpinBox->value(),GPIB_DEV_SAD,10,0,0); // Open the device

//    qDebug() << QString(Q_FUNC_INFO) << deviceHandle();
//    qDebug() << QString(Q_FUNC_INFO) << ibsta;
      if ((deviceHandle() < 0)) //|| (ibsta and ERR != 0))
      {
         console->insertPlainText("Failed to open.");
      }
      else
      {
     //     gpibConsole->putData(QByteArray("0"));
          console->insertPlainText("Device handle: "+QString::number(deviceHandle())+"\r\n");
          connect(console,SIGNAL(sendCommand(QString)),this,SLOT(write(QString)));
          write("");

      }

    return;
}

void TGPIB488Console::write(QString str)
{
    QByteArray ba=(str+"\r\n").toLatin1();
    ibwrt(deviceHandle(),ba.data(),ba.length());
//    ibwait(deviceHandle(),CMPL);Write on Array
    QThread::msleep(250);
   // qDebug() << QString(Q_FUNC_INFO) << "ibcnt:" << ibcnt;
   // qDebug() << QString(Q_FUNC_INFO) << "ibsta:" << ibsta;
   // qDebug() << QString(Q_FUNC_INFO) << "iberr:" << iberr;

    if (ibcnt!=ba.length() || ibsta==ERR) console->insertPlainText("\r\nWrite Error");

    console->insertPlainText("\r\n");
    console->moveCursor(QTextCursor::End);

    read();
    return;
}



void TGPIB488Console::read()
{
    char buffer[FBufSize];
    ibrd(deviceHandle(),buffer,FBufSize);
    QThread::msleep(250);
//    ibwait(deviceHandle(),CMPL);
    buffer[ibcnt-1] = '\0';
    FMessage=QString(buffer);
    console->insertPlainText(message());
    console->insertPlainText("$");
    console->moveCursor(QTextCursor::End);

    return;
}

