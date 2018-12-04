#ifndef VARIABLEEDIT_H
#define VARIABLEEDIT_H

#include <QWidget>
#include <QDialog>
#include <QTableWidget>
#include <QList>
#include <QFont>
#include "variables.h"
#include "pulseProgram.h"

class TvariableTable : public QTableWidget
{
    Q_OBJECT
public:
    explicit TvariableTable(QWidget *parent = 0);
    ~TvariableTable();

    void addRow(TVariable *variable);
//    QList<TVariable*> variables;
    void initialize();
//    int varIndex(QString varName);
    TpulseProgram *ppg;
    void setPPG(TpulseProgram *p) {ppg=p;}
    void setEditable(bool b);
    QStringList variableNames();
    QStringList variableScript();

signals:
    void updateAuxParamsRequest();
    void updateOrigNARequest(int);
 //   void updateRequest(TVariable *);
    void transferRequest(QStringList);
    void message(QString s);
    void updateComplete();
    void freqVariableChanged(int vIndex);
    void modified();

public slots:
    void updateResult(bool ok, QString msg) {isSuccessful=ok; messageFromCompiler=msg;
                                             setCurrentCell(currentRow,currentColumn);
                                             if(ok) emit updateComplete();}
    void tryUpdateVariable(QTableWidgetItem *item);
//    void displayVariables(TpulseProgram *ppg);
    void displayVariables();

protected:
    bool isSuccessful;
    QString messageFromCompiler;

private:
    void addRow();
    int currentRow,currentColumn;

};

#endif // VARIABLEEDIT_H
