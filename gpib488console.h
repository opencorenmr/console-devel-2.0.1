#ifndef TGPIB488CONSOLE_H
#define TGPIB488CONSOLE_H

#include <QWidget>
#include <QSpinBox>
#include <QCheckBox>
#include <QPlainTextEdit>

#include "console.h"
#include "ib.h"

#define GPIB_BOARD_NUM 0
//#define GPIB_DEV_PAD       4
#define GPIB_DEV_SAD       0

class TGPIB488Console : public QWidget
{
    Q_OBJECT
public:
    explicit TGPIB488Console(QWidget *parent = 0);
    Console *console;
    int deviceHandle() {return FDeviceHandle;}
    int devPad() {return FDevPad;}
    void setDevPad(int p) {FDevPad=p;}
    QString message() {return FMessage;}
    QSpinBox *padSpinBox;
    QCheckBox *writeOnArrayCheckBox;
    QPlainTextEdit *writeOnArrayTextEdit;


signals:

public slots:
    void open();
    void write(QString str);
    void read();
    void onWriteOnArrayCheckBoxToggled(bool b);

private:
    int FDeviceHandle;
    int FDevPad;
    QString FMessage;
    int FBufSize;

};

#endif // TGPIB488CONSOLE_H


