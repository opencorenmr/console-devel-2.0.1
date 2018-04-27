#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "../dataProcess/processPanelWidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    TProcessPanelWidget *processPanel;
    QWidget *plotterWidget;

public slots:
    void loadSettings();
    void saveSettings();


protected:
    void closeEvent(QCloseEvent *event);
    bool hasCloseEventCalled;

    QString path0;


};

#endif // MAINWINDOW_H
