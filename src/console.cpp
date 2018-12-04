#include <QDebug>
#include "console.h"

#define MAXCOMLENGTH 100

Console::Console(QWidget *parent): QPlainTextEdit(parent)
{
    document()->setMaximumBlockCount(1000);
//    QPalette p = palette();
//    p.setColor(QPalette::Base, Qt::white);
//    p.setColor(QPalette::Text, Qt::blue);
//    setPalette(p);
    command.clear();
    commandHistory.clear();
    comCounter=0;
    echoEnter=false;
}
void Console::putData(const QByteArray &data)
{
    insertPlainText(QString(data));

    //QScrollBar *bar = verticalScrollBar();
    //bar->setValue(bar->maximum());
}

void Console::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
    case Qt::Key_Backspace:
        if (command.size()>0)
        {
            command.chop(1);
            QPlainTextEdit::keyPressEvent(e);
        }
        break;
    case Qt::Key_Return:
        if(echoEnter) QPlainTextEdit::keyPressEvent(e);
        if(!command.isEmpty())
        {
            commandHistory.append(command);
            if(commandHistory.size()>MAXCOMLENGTH) commandHistory.removeFirst();
            comCounter=0;
        }
        emit sendCommand(command);
        command.clear();
        break;
    case Qt::Key_Up:

        if(!commandHistory.isEmpty())
        {
          if(comCounter<commandHistory.size())
          {
            moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
            moveCursor(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
            moveCursor(QTextCursor::Right, QTextCursor::MoveAnchor);
            moveCursor(QTextCursor::End, QTextCursor::KeepAnchor);
            textCursor().removeSelectedText();

            comCounter++;
            command=commandHistory.at(commandHistory.size()-comCounter);

            insertPlainText(command);
          }
        }
        break;
    case Qt::Key_Down:
        if(!commandHistory.isEmpty())
        {
            moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
            moveCursor(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
            moveCursor(QTextCursor::Right, QTextCursor::MoveAnchor);
            moveCursor(QTextCursor::End, QTextCursor::KeepAnchor);
            textCursor().removeSelectedText();

            if(comCounter>1)
            {
              comCounter--;
              command=commandHistory.at(commandHistory.size()-comCounter);
            }
            else
            {
              command.clear();
            }

            insertPlainText(command);
        }

        break;
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Shift:
        // skip processing
        break;
    default:
        QPlainTextEdit::keyPressEvent(e);
        // command.append(e->key());
        //    key() return the keyboard that was pressed, so that
        //    the case sensitivity (e.g., "A" or "a") is lost.
        //    Use text() instead.
        command.append(e->text());
        emit getData(e->text().toLocal8Bit());
    }
}

void Console::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
    setFocus();
}

void Console::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
}

void Console::contextMenuEvent(QContextMenuEvent *e)
{
    Q_UNUSED(e)
}

