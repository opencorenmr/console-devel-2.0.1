#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QtWidgets>
#include <QMainWindow>
#include "../fid.h"
#include "../plotter.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
        MainWindow(QWidget *parent = 0);
    //MainWindow();
    ~MainWindow();

    QWidget *centralWidget;
    QFont defaultFont, editorFont;
    QString errorMessage;

    QVector<QPointF> inData;
    TFID_2D FID_2D;

private slots:

    void openData();
    void cosSinMultiply();
    void applyDigitalFilter();
    void exportData();

private:

    QPushButton *openDataButton;
    QPushButton *cosSinMultiplyButtion;
    QPushButton *applyDigitalFilterButtion;
    QPushButton *exportDataButton;

    QLineEdit *cosSinLineEdit;
    QLineEdit *digitalFilterLineEdit;

    QLabel *currentFileLabel;

    Plotter *plotter;

    QString dataFilePath, exportFilePath;
    int devicePixelRatio;


};

#endif // MAINWINDOW_H
