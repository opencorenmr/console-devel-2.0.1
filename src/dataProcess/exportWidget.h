#ifndef KEXPORTWIDGET_H
#define KEXPORTWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QCheckBox>

#include "processBase.h"
#include "fidDomain.h"

class KExportWidget : public TProcessBase
{
    Q_OBJECT
public:
    explicit KExportWidget();
    ~KExportWidget() {;}

    QString dataFilePath() {return FDataFilePath;}
    void setDataFilePath(QString qs) {FDataFilePath=qs;}

    //QPushButton *exportOpFileButton;
    //QPushButton *exportSm2FileButton;
    //QPushButton *exportCSVButton;

    QComboBox *arrayOptionComboBox;
    QCheckBox *xCheckBox;
    QCheckBox *inPhaseCheckBox;
    QCheckBox *quadratureCheckBox;
    QCheckBox *absoluteCheckBox;

    QPushButton *exportAsciiButton;
    QComboBox *separatorCombobox;

    QPushButton *export2DButton;


    QLineEdit *startf1LineEdit;
    QLineEdit *endf1LineEdit;

    QLineEdit *contNumberLineEdit;

    QLineEdit *contHighLineEdit;
    QLineEdit *contLowLineEdit;

    QLineEdit *startf2LineEdit;
    QLineEdit *endf2LineEdit;
    //QPushButton *exportFirstButton;
    //QPushButton *exportDiagButton;


signals:

public slots:
    void performExportAscii();
    void performExportOpFile();
    void performExportSm2File();
    void performExportCSVFile();
    void performExport2DFile();

//    void exportAbs();
    //void performExportFirstFile();
    //void performExportDiagFile();

private:
    void createWidgets();
    void createPanel();
    void createConnections();

    QString FDataFilePath;
};

#endif // KEXPORTWIDGET_H
