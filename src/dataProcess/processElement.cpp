#include "processElement.h"

TProcessElement::TProcessElement(QObject *parent) : QObject(parent)
{
    errorQ=false;
    warningQ=false;
    FErrorMessage.clear();
    FWarningMessage.clear();
    setApplyMode(0);
    setApplyIndex(0);

}

