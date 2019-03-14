#ifndef ARRAY_H
#define ARRAY_H

#include <QList>
#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QTextEdit>
#include <QTableWidget>
#include <QPlainTextEdit>

#include "pulseProgram.h"
#include "arrayCounter.h"


class TArrayWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TArrayWidget(QWidget *parent = 0);

    QTableWidget *arrayTableWidget;
    QPlainTextEdit *messageTextEdit, *randomArrayTextEdit;
    TArrayCounter arrayCounter;
    void refresh();
    TpulseProgram *ppg;
    void setPPG(TpulseProgram *p) {ppg=p; ppgSetted=true;}
    QCheckBox *arrayCheckBox;
    QComboBox *arrayVariableComboBox,*arrayTypeComboBox,*arrayDimensionComboBox;

    QStringList arrayScript();
    void loadArrayScript(QString filename);
    void addArray(TVariable *variable);
    bool setLinearArray(TVariable *variable);
    bool setRandomArray(TVariable *variable);


signals:
    void setAR(int ar);
    void modified();

public slots:
   void initialSetup(TpulseProgram *ppg);
   void prepareArray(int vIndex);
//   void setEditable(bool b);
   void onRequestButtonClicked();


private slots:
   void onAddButtonClicked();
   void onDeleteButtonClicked();
   void onItemSelected();
   void onVariableSelected();
   void onArrayCheckBoxClicked(bool);
protected:

    QLabel *iniUnitLabel, *incUnitLabel;
    QPushButton *addButton, *deleteButton, *requestButton;

    QLineEdit *iniEdit, *incEdit, *nEdit;

    bool ppgSetted;
    void addArrayVariable(TVariable v);
    int variableIndex(QString v);

};

#endif // ARRAY_H
