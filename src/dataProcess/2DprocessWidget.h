#ifndef T2DPROCESSWIDGET_H
#define T2DPROCESSWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QComboBox>

#include "processBase.h"
#include "../fid.h"

class T2DProcessWidget : public TProcessBase
{
    Q_OBJECT
public:
    explicit T2DProcessWidget();
    ~T2DProcessWidget() {;}
    QPushButton *transposeButton;

    QComboBox *hyperComplexPhaseComboBox;
    QPushButton *hyperComplexButton;
    QPushButton *appendDataButton;

    QString dataFilePath() {return FDataFilePath;}
    void setDataFilePath(QString qs) {FDataFilePath=qs;}

    QStringList parameters;

    int AL;
    double DW, FSF1;

signals:

public slots:
    void performTranspose();
    void performHyperComplexCompression();
    void performAppendData();

    bool Readsm2FileforAdd(QString fn);
    bool ReadopFileforAdd(QString fn);

    bool Readsm2pFileforAdd(QString fn);
    bool Readsm2dFileforAdd(QString fn);
    bool ReadoppFileforAdd(QString fn);
    bool ReadopdFileforAdd(QString fn);
    //

private:
    void createWidgets();
    void createPanel();
    void createConnections();

    QString FDataFilePath;
};

#endif // T2DPROCESSWIDGET_H
