#include "processElement.h"

TProcessElement::TProcessElement(QObject *parent) : QObject(parent)
{
    errorQ=false;
    warningQ=false;
    FErrorMessage.clear();
    FWarningMessage.clear();

}

