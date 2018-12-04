#include "processOperations.h"
#include <QDebug>


TProcessOperations::TProcessOperations(QObject *parent) : QObject(parent)
{
   while(!processElements.isEmpty()) processElements.removeLast();
}

TProcessOperations::~TProcessOperations()
{
    while(!processElements.isEmpty()) processElements.removeLast();
}
