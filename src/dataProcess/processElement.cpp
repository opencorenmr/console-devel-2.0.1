#include "processElement.h"

//TProcessElement::TProcessElement(QObject *parent) : QObject(parent)
TProcessElement::TProcessElement()
{
    errorQ=false;
    warningQ=false;
    FErrorMessage.clear();
    FWarningMessage.clear();
    setApplyMode(0);
    setApplyIndex(0);

}

