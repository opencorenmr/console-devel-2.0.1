#include <QLayout>
#include <QLabel>
#include <QDebug>

#include "applyMode.h"

TApplyModeWidget::TApplyModeWidget(QWidget *parent) : QWidget(parent)
{
    applyModeComboBox = new QComboBox;
      applyModeComboBox->addItems(QStringList()<<"All"<<"One"<<"Other"<<"No");
    currentPlotterSpinBox = new QSpinBox;
      currentPlotterSpinBox->setMinimum(0);
      currentPlotterSpinBox->setMaximum(0);
    QHBoxLayout *hLayout1 = new QHBoxLayout;
    hLayout1->addStretch();
    hLayout1->addWidget(new QLabel(tr("Apply to ")));
    hLayout1->addWidget(applyModeComboBox);
    hLayout1->addWidget(new QLabel(tr(" data")));
    QHBoxLayout *hLayout2 = new QHBoxLayout;
    hLayout2->addStretch();
    hLayout2->addWidget(new QLabel(tr("in plotter #")));
    hLayout2->addWidget(currentPlotterSpinBox);

    QVBoxLayout *vLayout1 = new QVBoxLayout(this);
    vLayout1->addLayout(hLayout1);
    vLayout1->addLayout(hLayout2);

    connect(applyModeComboBox,SIGNAL(currentIndexChanged(int)),
            this,SLOT(onApplyModeComboBoxCurrentIndexChanged()));

    applyModeComboBox->setCurrentIndex(0);
    onApplyModeComboBoxCurrentIndexChanged();

    connect(currentPlotterSpinBox,SIGNAL(valueChanged(int)),
            this,SLOT(setCurrentPlotter(int)));


}



void TApplyModeWidget::onApplyModeComboBoxCurrentIndexChanged()
{
   switch(applyModeComboBox->currentIndex())
   {
     case 0:
       currentPlotterSpinBox->setEnabled(false);
       break;
     case 1:
       currentPlotterSpinBox->setEnabled(true);
       break;
     case 2:
       currentPlotterSpinBox->setEnabled(true);
       break;
     case 3:
       currentPlotterSpinBox->setEnabled(false);
       break;
   }
}


