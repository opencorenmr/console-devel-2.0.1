#ifndef TAPPLYMODE_H
#define TAPPLYMODE_H

#include <QWidget>
#include <QComboBox>
#include <QSpinBox>

enum ApplyMode
{
     ApplyToAll,
     ApplyToOne,
     ApplyToOthers,
     ApplyToNone
};

class TApplyModeWidget: public QWidget
{
    Q_OBJECT
public:
    explicit TApplyModeWidget(QWidget *parent=0);

    QComboBox *applyModeComboBox;
    QSpinBox *currentPlotterSpinBox;

    int currentPlotter() {return FCurrentPlotter;}

public slots:
    void setCurrentPlotter(int i) {FCurrentPlotter=i;}

    void onApplyModeComboBoxCurrentIndexChanged();

private:
    int FCurrentPlotter;
};


#endif // TAPPLYMODE_H
