#include <QLayout>
#include <QComboBox>
#include <QLabel>

#include "processBase.h"

TProcessBase::TProcessBase()
{
   FIsAncestorDefined=false;
   applyModeWidget = new TApplyModeWidget;
   // By default, we set the master plotter to the current plotter
   applyModeWidget->setCurrentPlotter(0);
}
