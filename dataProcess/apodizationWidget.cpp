
#include <QLabel>
#include <QLayout>
#include <QGroupBox>
#include <QMessageBox>

#include "apodizationWidget.h"
#include "processPanelWidget.h"


TApodizationWidget::TApodizationWidget()
{

  // We create process element.
    apodization = new TApodization;

  // We create widgets.
    inverseCheckbox = new QCheckBox(tr("Inverse operation"));
    inverseCheckbox -> setChecked(false);
    apodizationTypeComboBox = new QComboBox;
    apodizationTypeComboBox -> addItems(QStringList()<<"Gaussian"<<"Exponential"<<"sinc");
    applyApodizationPushButton = new QPushButton("Apply Apodization");
    widthDoubleSpinBox = new QDoubleSpinBox;
    widthDoubleSpinBox->setSuffix(" Hz");
    widthDoubleSpinBox->setMinimum(0);
    widthDoubleSpinBox->setMaximum(1e100);
    widthDoubleSpinBox->setValue(0);

  // We create panel.
    QVBoxLayout *vLayout1 = new QVBoxLayout;
    QGroupBox *groupBox1 = new QGroupBox(tr("Apodization"));
    QGridLayout *gLayout1 = new QGridLayout;
    gLayout1->addWidget(new QLabel("Apodization type"),0,0,1,1);
    gLayout1->addWidget(apodizationTypeComboBox,0,1,1,1);
    gLayout1->addWidget(new QLabel("Width"),1,0,1,1);
    gLayout1->addWidget(widthDoubleSpinBox,1,1,1,1);
    gLayout1->addWidget(inverseCheckbox,2,1,1,2);
    gLayout1->addWidget(applyApodizationPushButton,3,0,2,2);

    groupBox1->setLayout(gLayout1);
    vLayout1->addWidget(groupBox1);
    vLayout1->addWidget(applyModeWidget);
    vLayout1->addStretch();
    setLayout(vLayout1);

  // We create connections;
    connect(applyApodizationPushButton,SIGNAL(clicked(bool)),this,SLOT(onApplyButtonClicked()));

    applyModeWidget->applyModeComboBox->setCurrentIndex(0);  //default
    apodization->setApplyMode(0); // default

}


void TApodizationWidget::onApplyButtonClicked()
{
    if(!isAncestorDefined())
    {
        QMessageBox::warning(this,tr(""),"Ancestor is not defined.");
        return;
    }
    if(ancestor()->FID_2D->FID.isEmpty())
    {
        QMessageBox::warning(this,tr(""),"Data is empty.");
        return;
    }

    switch (apodizationTypeComboBox->currentIndex())
    {
      case 0:
        apodization->setApodizationType(Gaussian);
        break;
      case 1:
        apodization->setApodizationType(Exponential);
        break;
      case 2:
        apodization->setApodizationType(Sinc);
        break;
      default:
        QMessageBox::warning(this,tr(""),"Invalid apodization type.");
        return;

    } //switch
    apodization->setWidth(widthDoubleSpinBox->value());
    apodization->setInverse(inverseCheckbox->isChecked());

    apodization->setApplyMode(applyModeWidget->applyModeComboBox->currentIndex());
    apodization->setApplyIndex(ancestor()->plotters
                                   ->FIDPlotters.at(applyModeWidget->currentPlotter())
                                   ->FIDSelectSpinBox->value()-1);

    if(!performOperation())
    // in case of error...
    {
        QMessageBox::warning(this,tr(""), apodization->errorMessage);
        return;
    }

    // With success, we add the operation to the operation list.
    addOperation();

}

bool TApodizationWidget::performOperation()
{
    bool ok;

    switch(apodization->applyMode())
    {
      case ApplyToAll:
        ok=apodization->process(ancestor()->FID_2D);
        break;
      case ApplyToOne:
        if(apodization->applyIndex()<=ancestor()->FID_2D->FID.size()-1)
        {
          ok = apodization->process(ancestor()->FID_2D->FID[apodization->applyIndex()]);
        }
        else ok=false;
        break;
      case ApplyToOthers:
        for(int k=0; k<ancestor()->FID_2D->FID.size(); k++)
        {
          if(k!=apodization->applyIndex())
          {
             ok=apodization->process(ancestor()->FID_2D->FID[k]);
             if(!ok) break;
          }
        }
        break;
      default: // None
        return true;

    }

    if(!ok)
    {
        setMessage(apodization->errorMessage);
        return false;
    }
    ancestor()->plotters->update();
    return true;
}

void TApodizationWidget::addOperation()
{
    if(!isAncestorDefined()) return;
    if(ancestor()->FID_2D->FID.isEmpty()) return;
    TApodization *apo = new TApodization;
      apo->setApodizationType(apodization->apodizationType());
      apo->setWidth(apodization->width());
      apo->setInverse(apodization->inverse());
      apo->setApplyMode(apodization->applyMode());
      apo->setApplyIndex(apodization->applyIndex());
    ancestor()->processOperations->processElements.append(apo);
    // common settings
    ancestor()->updateProcessSettings();
    //
    createSettings(ancestor()->processSettings,
                   QString::number(ancestor()->processOperations->processElements.size()-1));
    return;
}

void TApodizationWidget::createSettings(QSettings *settings, QString section)
{
    settings->beginGroup(section);
      settings->setValue("type", apodization->apodizationType());
      //settings->setValue("type_text",apodizationTypeComboBox->currentText());
      settings->setValue("width",apodization->width());
      settings->setValue("inverse",apodization->inverse());
      settings->setValue("applymode",apodization->applyMode());
      settings->setValue("applyindex",apodization->applyIndex());
    settings->endGroup();
    settings->sync();
}

void TApodizationWidget::readSettings(QSettings *settings, QString section)
{
    settings->beginGroup(section);
      apodization->setApodizationType(settings->value("type",0).toInt());
      apodization->setWidth(settings->value("width",0).toDouble());
      apodization->setInverse(settings->value("inverse",false).toBool());
      apodization->setApplyMode(settings->value("applymode",0).toInt());
      apodization->setApplyIndex(settings->value("applyindex",0).toInt());
    settings->endGroup();
}

