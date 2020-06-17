#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include "imageGenWidget.h"


TImageGenWidget::TImageGenWidget()
{
   createWidgets();
   createPanel();
   createConnections();
}



void TImageGenWidget::createWidgets()
{
    plotterIDSpinBox = new QSpinBox;
    plotterIDSpinBox->setMinimum(0);
    plotterIDSpinBox->setMaximum(0);

}

void TImageGenWidget::createPanel()
{
    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->addWidget(new QLabel(tr("Plotter ID")),1,0,1,1);
    mainLayout->addWidget(plotterIDSpinBox,1,1,1,1);

}
void TImageGenWidget::createConnections()
{

}
