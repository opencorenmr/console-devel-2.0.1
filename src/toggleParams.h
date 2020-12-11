#ifndef TTOGGLEPARAMS_H
#define TTOGGLEPARAMS_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QToolButton>
#include <QLineEdit>
#include <QList>
#include <QComboBox>
#include <QCheckBox>
#include <QTextEdit>
#include <QTableWidget>
#include <QPlainTextEdit>
#include <QSpinBox>

#include "pulseProgram.h"

class TToggleParams : public QWidget
{
    Q_OBJECT
public:
    explicit TToggleParams(QWidget *parent = nullptr);
    void refresh();
    TpulseProgram *ppg;
    QTableWidget *paramTableWidget;
    QComboBox *paramComboBox;
    QLineEdit *valueLineEdit;
    QPushButton *addPushButton, *editPushButton, *deletePuchButton,
                *moveUpPushButton, *moveDownPushButton;
    QPlainTextEdit *paramPlainTextEdit;
    QPlainTextEdit *messageTextEdit;

    void setPPG(TpulseProgram *p) {ppg=p; ppgSetted=true;}

public slots:
   void initialSetup(TpulseProgram *ppg);

private slots:
   void onAddButtonClicked();
   void onDeleteButtonClicked();

signals:


private:
    void createWidgets();
    void createPanel();
    void createConnections();
    bool setRandomArray();
    bool setFileNameArray();

    bool ppgSetted;


};

#endif // TTOGGLEPARAMS_H
