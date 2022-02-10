#ifndef SEXPORTABSWIDGET_H
#define SEXPORTABSWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QSpinBox>
#include <QPlainTextEdit>
#include <QLineEdit>

#include "processBase.h"
#include "fidDomain.h"

class SExportAbsWidget : public TProcessBase
{
    Q_OBJECT
public:
    explicit SExportAbsWidget();
    ~SExportAbsWidget() {;}

    QString dataFilePath() {return FDataFilePath;}
    void setDataFilePath(QString qs) {FDataFilePath=qs;}

    QSpinBox *layersSpinBox;
    QPushButton *exportAbsButton;

signals:

public slots:
    void exportAbs();

private:
    void createWidgets();
    void createPanel();
    void createConnections();

    QString FDataFilePath;
};

#endif // SEXPORTABSWIDGET_H
