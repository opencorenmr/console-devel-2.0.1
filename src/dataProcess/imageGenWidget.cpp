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
    QVBoxLayout *vLayout1 = new QVBoxLayout(this);
    QHBoxLayout *hLayout1 = new QHBoxLayout();
      QComboBox *comboBox1 = new QComboBox();
      comboBox1->addItems(QStringList()<<"1"<<"2");
      hLayout1->addWidget(new QLabel(tr("Label")));
      hLayout1->addWidget(comboBox1);
      hLayout1->addStretch();

    QGridLayout *gLayout1 = new QGridLayout();
      gLayout1->addWidget(new QLabel(tr("Plotter ID")),1,0,1,1);
      gLayout1->addWidget(plotterIDSpinBox,1,1,1,1);

    vLayout1->addLayout(hLayout1);
    vLayout1->addLayout(gLayout1);
    vLayout1->addStretch();
}
void TImageGenWidget::createConnections()
{

}
