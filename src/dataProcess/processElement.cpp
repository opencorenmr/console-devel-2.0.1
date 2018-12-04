#include "processElement.h"

TProcessElement::TProcessElement(QObject *parent) : QObject(parent)
{
    errorQ=false;
    warningQ=false;
    errorMessage.clear();
    warningMessage.clear();

}

