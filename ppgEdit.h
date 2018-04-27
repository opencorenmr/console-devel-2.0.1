#ifndef PPGEDIT_H
#define PPGEDIT_H

#include <QWidget>
#include <QPushButton>
#include <QStackedLayout>
#include "codeeditor.h"
#include "highlighter.h"
#include "pulseProgram.h"
#include "variableedit.h"
#include "array.h"
#include "runStopButton.h"
#include "expSettings.h"

// forward declaration
class QLabel;
class QListWidget;
//class QPushButton;
class QTextEdit;
class QSplitter;
class QVBoxLayout;
class QHBoxLayout;

class TppgEdit : public QWidget
{
    Q_OBJECT
public:
    explicit TppgEdit(QWidget *parent = 0);
    ~TppgEdit();

    TExpSettings *expSettings;
    TpulseProgram *ppg;
    QStringList receiverCommands;
    QPushButton *ppgTransferButton;
    QSplitter *ppgCompilerMainSplitter;
    QSplitter *ppgCompilerRightSplitter;

signals:
//    void toTerminal(TpulseProgram *ppg);
    void toTerminal(QString fn);
    void transferRequest(const QStringList &sl);
    void rTransferRequest(const QStringList &sl);
    void viewRequest();

    void setALRequest(int al);
    void setDWRequest(double dw);
    void setNARequest(int na);
    void setNDRequest(int nd);

    void updateVariableResult(bool ok, QString errorMessage);

    void setFreqListRequest(TpulseProgram *ppg);
    void setVariableTableRequest(TpulseProgram *ppg);
    void setArrayTableRequest(TpulseProgram *ppg);

public slots:
    void onUSBStatusChanged(bool opened) {ppgTransferButton->setEnabled(opened);}
    void disableTransferButton() {ppgTransferButton->setEnabled(false);}
    void enableTransferButton() {ppgTransferButton->setEnabled(true);}
    void onPPGCompiled();

private slots:
    void ppgRead();
    void ppgReadFromFile(QString fileName);
    void ppgSave();
    void ppgSaveAs();
    void ppgCompile();
    void displayPPGPart(int j);
    void showMessageFromVariableTable(QString s) {
        compilerReport->insertPlainText(s);
        compilerReport->moveCursor(QTextCursor::End);
    }
    void transferClicked();
    void setppgSaveButtonEnabled() {ppgSaveButton->setEnabled(true);}
    void showEditor();
    void showExpSettings();

private:
    QPushButton *loadJobButton, *saveJobButton, *queueJobButton;

    QLabel *ppgFilenameLabel;
    QPlainTextEdit *compilerReport;
    QFont editorFont;
    Highlighter *highlighter;
    QPushButton *ppgCompileButton;
    QListWidget *ppgPartListWidget;
    CodeEditor *ppgPartTextEdit;
    QPushButton *ppgOpenButton;
    QPushButton *ppgSaveButton;
    QPushButton *ppgSaveAsButton;
    QPushButton *showExpSettingsButton;
    QVBoxLayout *layout0,*layout1;
    QStackedLayout *stackedLayout;
    TvariableTable *variableTable;
    TArrayWidget *arrayWidget;
    QStringList updatedCommands;

};

#endif // PPGEDIT_H
