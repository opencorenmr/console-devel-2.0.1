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

    void setPPG(TpulseProgram *p) {ppg=p; ppgSetted=true;}

public slots:
   void initialSetup(TpulseProgram *ppg);

signals:


private:
    void createWidgets();
    void createPanel();

    bool ppgSetted;


};

#endif // TTOGGLEPARAMS_H
