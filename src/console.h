#ifndef CONSOLE_H
#define CONSOLE_H

#include <QPlainTextEdit>

class Console : public QPlainTextEdit
{
    Q_OBJECT

signals:
    void getData(const QByteArray &data);
    void sendCommand(QString s);

public:
    explicit Console(QWidget *parent = 0);
    void putData(const QByteArray &data);
    QString command;
    QStringList commandHistory;
    bool echoEnter; // if '\n' is added or not

public slots:

protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseDoubleClickEvent(QMouseEvent *e);
    virtual void contextMenuEvent(QContextMenuEvent *e);

private:

    int comCounter;

};


#endif // CONSOLE_H
