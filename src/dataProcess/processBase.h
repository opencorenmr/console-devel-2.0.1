#ifndef PROCESSBASE_H
#define PROCESSBASE_H

#include <QWidget>
#include <QComboBox>

#include "applyMode.h"

class TProcessPanelWidget;

class TProcessBase: public QWidget
{
    Q_OBJECT
public:
    explicit TProcessBase();
    TProcessPanelWidget *ancestor() {return FAncestor;}
    void setAncestor(TProcessPanelWidget *ppw) {FAncestor=ppw; FIsAncestorDefined=true;}
    bool isAncestorDefined() {return FIsAncestorDefined;}
    QString message() {return FMessage;}
    void setMessage(QString qs) {FMessage=qs;}

    bool applyToSelectable() {return FApplyToSelectable;}
    void setApplyToSelectable(bool b) {FApplyToSelectable=b;}
    TApplyModeWidget *applyModeWidget;

private:
    QString FMessage;
    TProcessPanelWidget *FAncestor;

    bool FIsAncestorDefined;
    bool FApplyToSelectable;
};


#endif // PROCESSBASE_H
