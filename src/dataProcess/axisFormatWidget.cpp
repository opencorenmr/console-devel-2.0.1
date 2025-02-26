#include <QGridLayout>
#include <QLabel>
#include "axisFormatWidget.h"
#include "processPanelWidget.h"


TAxisFormatWidget::TAxisFormatWidget() : TProcessBase()
{
    axisStyle = new TAxisStyle;

    //FfidSetted=false;
    timeStringList=QStringList()<<"usec"<<"msec"<<"sec";

    createWidgets();
    createPanel();
    createConnections();
}

void TAxisFormatWidget::createWidgets()
{
    domainComboBox = new QComboBox;
      domainComboBox->addItems(QStringList()<<"Time"<<"Frequency"<<"Other");
      domainComboBox->setCurrentIndex(0); axisStyle->setDomain("time");
     // domainComboBox->setEnabled(false);
    unitComboBox = new QComboBox;
      //unitComboBox->addItems(QStringList()<<"msec"<<"sec");
      unitComboBox->addItems(timeStringList);
      unitComboBox->setCurrentIndex(1); axisStyle->setUnit("msec");
    axisLabelLineEdit = new QLineEdit;
    setReferenceSpinBox = new QSpinBox;
    setReferencePushButton=new QPushButton(tr("Click Set"));
    setReferencePushButton->setCheckable(true);
    setReferencePushButton->setChecked(false);

    referenceValueLineEdit = new QLineEdit;
    applyPushButton = new QPushButton(tr("Apply"));
    cancelPushButton = new QPushButton(tr("Revert"));
}

void TAxisFormatWidget::createPanel()
{

    QGridLayout *mainLayout = new QGridLayout(this);

    mainLayout->addWidget(new QLabel(tr("X axis represents")),0,0,1,1);
    mainLayout->addWidget(domainComboBox,0,1,1,1);
    mainLayout->addWidget(new QLabel(tr("X axis unit")),1,0,1,1);
    mainLayout->addWidget(unitComboBox,1,1,1,1);
    mainLayout->addWidget(new QLabel(tr("X axis label")),2,0,1,1);
    mainLayout->addWidget(axisLabelLineEdit,2,1,1,1);
    mainLayout->addWidget(new QLabel(tr("Reference Point")),3,0,1,1);
    mainLayout->addWidget(setReferenceSpinBox,3,1,1,1);
    mainLayout->addWidget(setReferencePushButton,4,1,1,1);
    mainLayout->addWidget(new QLabel(tr("Reference Value")),5,0,1,1);
    mainLayout->addWidget(referenceValueLineEdit,5,1,1,1);
    mainLayout->addWidget(applyPushButton,6,0,1,2);

   // mainLayout->addWidget(cancelPushButton,7,1,1,1);

}

void TAxisFormatWidget::createSettings(QSettings *settings,QString section)
{
    settings->beginGroup(section);
      settings->setValue("domainComboBox",domainComboBox->currentText());
      settings->setValue("unitComboBox",unitComboBox->currentText());
      settings->setValue("axisLabelLineEdit",axisLabelLineEdit->text());
      settings->setValue("setReferenceSpinBox",setReferenceSpinBox->value());
      settings->setValue("referenceValueLineEdit",referenceValueLineEdit->text());
    settings->endGroup();
    settings->sync();
}


void TAxisFormatWidget::readSettings(QSettings *settings, QString section)
{
    settings->beginGroup(section);
      axisStyle->setDomain(settings->value("domainComboBox","").toString());
      if(axisStyle->domain()==TAxisStyle::TimeDomain) domainComboBox->setCurrentIndex(0);
      else if (axisStyle->domain()==TAxisStyle::FrequencyDomain) domainComboBox->setCurrentIndex(1);
      else domainComboBox->setCurrentIndex(2);
      setDomain();

      axisStyle->setUnit(settings->value("unitComboBox","").toString());

      unitComboBox->setCurrentIndex(
                  unitComboBox->findText(settings->value("unitComboBox","").toString()));

      axisLabelLineEdit->setText(settings->value("axisLabelLineEdit","").toString());
      setReferenceSpinBox->setValue(settings->value("setReferenceSpinBox",0).toInt());
      referenceValueLineEdit->setText(settings->value("referenceValueLineEdit","").toString());
    settings->endGroup();
    refresh();
}

void TAxisFormatWidget::onSetReferencePushButtonToggled()
{
    if(!isAncestorDefined()) return;

    if(setReferencePushButton->isChecked())
    {//emit requestClickSet();
        for(int k=0; k<ancestor()->plotters->FIDPlotters.size(); k++)
        {
            connect(ancestor()->plotters->FIDPlotters[k]->plotter,SIGNAL(clickedXPosition(int)),
                    setReferenceSpinBox,SLOT(setValue(int)));
          //  connect(ancestor()->plotters->FIDPlotters[k]->plotter,SIGNAL(clickedXPosition(int)),
          //          this,SLOT(disconnectFromAxisWidget()));

        }
    }
    else
    {//emit cancelClickSet();
        for(int k=0; k<ancestor()->plotters->FIDPlotters.size(); k++)
        {
            disconnect(ancestor()->plotters->FIDPlotters[k]->plotter,SIGNAL(clickedXPosition(int)),
                    setReferenceSpinBox,SLOT(setValue(int)));
        }
        setReferencePushButton->setChecked(false);

    }
}

void TAxisFormatWidget::createConnections()
{
    connect(setReferencePushButton,SIGNAL(toggled(bool)),this,SLOT(onSetReferencePushButtonToggled()));
    connect(cancelPushButton,SIGNAL(clicked()),this,SLOT(revert()));
    connect(domainComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(setDomain()));

    connect(unitComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(setUnit()));
    connect(axisLabelLineEdit,SIGNAL(returnPressed()),this,SLOT(setUnit()));
    connect(referenceValueLineEdit,SIGNAL(returnPressed()),this,SLOT(setUnit()));
    connect(applyPushButton,SIGNAL(clicked(bool)),this,SLOT(apply()));
 //   connect(applyPushButton,SIGNAL(clicked(bool)),this,SLOT(createSettings()));

}


void TAxisFormatWidget::setDomain()
{

    unitComboBox->clear();
    if(domainComboBox->currentIndex()==0)  // time
    {
        //unitComboBox->addItems(QStringList()<<"msec"<<"sec");
        unitComboBox->addItems(timeStringList);
        unitComboBox->setCurrentIndex(1);

    }
    else if(domainComboBox->currentIndex()==1) // frequency
    {
        unitComboBox->addItems(QStringList()<<"kHz"<<"Hz"<<"ppm");
    }
    else
    {
        unitComboBox->addItems(QStringList()<<"");
    }
}

void TAxisFormatWidget::setUnit()
{
    if(!isAncestorDefined())return;
    if(ancestor()->FID_2D->FID.isEmpty()) return;

    switch(domainComboBox->currentIndex())
    {
      case 0: setTimeUnit();
        break;
      case 1: setFrequencyUnit();
        break;
      case 2: setOtherUnit();
        break;
      default:
        break;
     }

     ancestor()->FID_2D->setXAxisLabel(axisLabelLineEdit->text());
     ancestor()->updatePlotter();
     //ancestor()->refresh();

}

void TAxisFormatWidget::setTimeUnit()
{
    if(!isAncestorDefined())return;
    if(ancestor()->FID_2D->FID.isEmpty()) return;

    for(int k=0; k<ancestor()->FID_2D->FID.size(); k++) setTimeUnit(k);
    return;
}


void TAxisFormatWidget::setTimeUnit(int k)
{
    if(!isAncestorDefined())return;
    if(ancestor()->FID_2D->FID.isEmpty()) return;


    ancestor()->FID_2D->FID[k]->setXUnit(TFIDXUnit::Second);
    ancestor()->FID_2D->FID[k]->setPrefix(TMetricPrefix::Micro);
    ancestor()->FID_2D->FID[k]->setDx(ancestor()->FID_2D->FID[k]->dw()*TMetricPrefix::Decimal(TMetricPrefix::Micro));

    ancestor()->FID_2D->FID[k]->setXAxisUnitSymbol("sec");
    switch(unitComboBox->currentIndex())
    {
      case 0: // usec
        axisStyle->setUnit(TAxisStyle::usec);
        ancestor()->FID_2D->FID[k]->setPlotPrefix(TMetricPrefix::Micro);
        ancestor()->FID_2D->FID[k]->setXInitialValue(referenceValueLineEdit->text().toDouble()*TMetricPrefix::Decimal(TMetricPrefix::Micro)
                                         -setReferenceSpinBox->value()*ancestor()->FID_2D->FID[k]->dx());
        break;
      case 1: // msec
        axisStyle->setUnit(TAxisStyle::msec);
        ancestor()->FID_2D->FID[k]->setPlotPrefix(TMetricPrefix::Milli);
        ancestor()->FID_2D->FID[k]->setXInitialValue(referenceValueLineEdit->text().toDouble()*TMetricPrefix::Decimal(TMetricPrefix::Milli)
                                         -setReferenceSpinBox->value()*ancestor()->FID_2D->FID[k]->dx());
        break;
      case 2: // sec
        axisStyle->setUnit(TAxisStyle::sec);
        ancestor()->FID_2D->FID[k]->setPlotPrefix(TMetricPrefix::None);
        ancestor()->FID_2D->FID[k]->setXInitialValue(referenceValueLineEdit->text().toDouble()*TMetricPrefix::Decimal(TMetricPrefix::None)
                                         -setReferenceSpinBox->value()*ancestor()->FID_2D->FID[k]->dx());
        break;
    }

    return;
}


void TAxisFormatWidget::setFrequencyUnit(int k)
{
    ancestor()->FID_2D->FID[k]->setXUnit(TFIDXUnit::Hz);

    switch(unitComboBox->currentIndex())
    {
      case 0: // kHz
        axisStyle->setUnit(TAxisStyle::kHz);
        ancestor()->FID_2D->FID[k]->setPrefix(TMetricPrefix::None);
        ancestor()->FID_2D->FID[k]->setPlotPrefix(TMetricPrefix::Kilo);
        ancestor()->FID_2D->FID[k]->setDx(
                    -1.0/(ancestor()->FID_2D->FID[k]->dw()*TMetricPrefix::Decimal(TMetricPrefix::Micro)
                                    *ancestor()->FID_2D->FID[k]->al())
                    );

        ancestor()->FID_2D->FID[k]->setXInitialValue(
                    referenceValueLineEdit->text().toDouble()*TMetricPrefix::Decimal(TMetricPrefix::Kilo)
                   -setReferenceSpinBox->value()*ancestor()->FID_2D->FID[k]->dx());

        ancestor()->FID_2D->FID[k]->setXAxisUnitSymbol("Hz");

        break;
      case 1: // Hz
        axisStyle->setUnit(TAxisStyle::Hz);

        ancestor()->FID_2D->FID[k]->setPlotPrefix(TMetricPrefix::None);
        ancestor()->FID_2D->FID[k]->setDx(
                    -1.0/(ancestor()->FID_2D->FID[k]->dw()*TMetricPrefix::Decimal(TMetricPrefix::Micro)
                                    *ancestor()->FID_2D->FID[k]->al())
                    );

        ancestor()->FID_2D->FID[k]->setXInitialValue(referenceValueLineEdit->text().toDouble()
                                         -setReferenceSpinBox->value()*ancestor()->FID_2D->FID[k]->dx());

        ancestor()->FID_2D->FID[k]->setXAxisUnitSymbol("Hz");

        break;
      case 2: //ppm
        axisStyle->setUnit(TAxisStyle::ppm);
        axisStyle->setAxisStyle(TAxisStyle::PPMStyle);

        ancestor()->FID_2D->FID[k]->setPlotPrefix(TMetricPrefix::None);
        ancestor()->FID_2D->FID[k]->setDx(
                          -1.0/(ancestor()->FID_2D->FID[k]->dw()*TMetricPrefix::Decimal(TMetricPrefix::Micro)
                                    *ancestor()->FID_2D->FID[k]->al()*ancestor()->FID_2D->sf1())
                    );
        ancestor()->FID_2D->FID[k]->setXInitialValue(referenceValueLineEdit->text().toDouble()
                                         -setReferenceSpinBox->value()*ancestor()->FID_2D->FID[k]->dx());

        ancestor()->FID_2D->FID[k]->setXAxisUnitSymbol("ppm");

        break;
    }

    return;
}

void TAxisFormatWidget::setFrequencyUnit()
{
    for(int k=0; k<ancestor()->FID_2D->FID.size(); k++) setFrequencyUnit(k);
    return;
}

void TAxisFormatWidget::setOtherUnit()
{
    for(int k=0; k<ancestor()->FID_2D->FID.size(); k++) setOtherUnit(k);
    return;
}

void TAxisFormatWidget::setOtherUnit(int k)
{
    ancestor()->FID_2D->FID[k]->setXUnit(TFIDXUnit::NoUnit);

    axisStyle->setUnit(TAxisStyle::sec);
    ancestor()->FID_2D->FID[k]->setPrefix(TMetricPrefix::None);
    ancestor()->FID_2D->FID[k]->setPlotPrefix(TMetricPrefix::None);
    ancestor()->FID_2D->FID[k]->setDx(1);

    ancestor()->FID_2D->FID[k]->setXInitialValue(
                referenceValueLineEdit->text().toDouble()*TMetricPrefix::Decimal(TMetricPrefix::Kilo)
                   -setReferenceSpinBox->value()*ancestor()->FID_2D->FID[k]->dx());

    ancestor()->FID_2D->FID[k]->setXAxisUnitSymbol("");

    return;
}

void TAxisFormatWidget::setUnitComboBox(int i)
{
    if(domainComboBox->currentIndex()==0)  // time
    {
        switch(i)
        {
        case TAxisStyle::usec: unitComboBox->setCurrentIndex(0); break;
        case TAxisStyle::msec: unitComboBox->setCurrentIndex(1); break;
        case TAxisStyle::sec: unitComboBox->setCurrentIndex(2); break;

        default: unitComboBox->setCurrentIndex(1);
        break;

        }
    }
    else if(domainComboBox->currentIndex()==1) // frequency
    {
        switch(i)
        {
        case TAxisStyle::kHz: unitComboBox->setCurrentIndex(0); break;
        case TAxisStyle::Hz: unitComboBox->setCurrentIndex(1); break;
        case TAxisStyle::ppm: unitComboBox->setCurrentIndex(2); break;

        default: unitComboBox->setCurrentIndex(1);
        break;

        }
    }
    else
    {
    }

}


void TAxisFormatWidget::init()
{

    setReferencePushButton->setChecked(false);

    unitComboBox->clear();
    if(domainComboBox->currentIndex()==0)  // time
    {
//        unitComboBox->addItems(QStringList()<<"msec"<<"sec");
        unitComboBox->addItems(timeStringList);
        unitComboBox->setCurrentIndex(1);
    }
    else // frequency
    {
        unitComboBox->addItems(QStringList()<<"kHz"<<"Hz"<<"ppm");
    }

    axisLabelLineEdit->clear();

    if(isAncestorDefined())
    {


      if(!ancestor()->FID_2D->FID.isEmpty())
      {
          //  setReferenceSpinBox->setMaximum(ancestor()->FID_2D->defaultAl()-1);
          //  2023.4.27: This causes a problem when you change the length of the FID
          //  by, e.g., zero-filling the data.
          //  Accordingly, we allow any integer value
          //  by setting the maximum to be std::numeric_limits<int32_t>::max()
          //  and the minimum to be std::numeric_limits<int32_t>::min()
        setReferenceSpinBox->setMaximum(std::numeric_limits<int32_t>::max());
        setReferenceSpinBox->setMinimum(std::numeric_limits<int32_t>::min());
        domainComboBox->setCurrentIndex(ancestor()->FID_2D->FID[ancestor()->FID_2D->currentFID()]->domain());

//    qDebug() << QString(Q_FUNC_INFO) << "0";
        if(ancestor()->FID_2D->FID[ancestor()->FID_2D->currentFID()]->domain()==TFID::TimeDomain)
            setReferenceSpinBox->setValue(0);
        else if(ancestor()->FID_2D->FID[ancestor()->FID_2D->currentFID()]->domain()==TFID::FrequencyDomain)
            setReferenceSpinBox->setValue(ancestor()->FID_2D->defaultAL()/2);

        referenceValueLineEdit->setText("0");
        axisLabelLineEdit->setText(ancestor()->FID_2D->FID[ancestor()->FID_2D->currentFID()]->xAxisLabel());

      }
    }


}

void TAxisFormatWidget::refresh()
{
    axisStyle->setDomain(domainComboBox->currentIndex());
    setUnit();
    axisStyle->setLabel(axisLabelLineEdit->text());
    axisStyle->setReferencePoint(setReferenceSpinBox->value());
    axisStyle->setReferenceValue(referenceValueLineEdit->text().toDouble());
}

void TAxisFormatWidget::apply()
{
    if(!isAncestorDefined()) return;
    if(ancestor()->FID_2D->FID.isEmpty()) return;
    refresh();
    TAxisStyle *as = new TAxisStyle;
      as->setDomain(axisStyle->domain());
      as->setAxisStyle(axisStyle->axisStyle());
      as->setUnit(axisStyle->unit());
      as->setLabel(axisStyle->label());
      as->setReferencePoint(axisStyle->referencePoint());
      as->setReferenceValue(axisStyle->referenceValue());

    // If the type of the last operation is also AxisStyle, we overwrite it.
    if(!ancestor()->processOperations->processElements.isEmpty())
    {
      if(ancestor()->processOperations->processElements.last()->processType()==TProcessElement::AxisStyle)
      {
        ancestor()->processOperations->processElements.removeLast();
      }
    }
    ancestor()->processOperations->processElements.append(as);
    // common settings
    ancestor()->updateProcessSettings();
    // settings specific to AxisStyle
    createSettings(ancestor()->processSettings,
                    QString::number(ancestor()->processOperations->processElements.size()-1));

}

void TAxisFormatWidget::revert()
{
    setReferencePushButton->setChecked(false);

    unitComboBox->clear();
    if(domainComboBox->currentIndex()==0)  // time
    {
        //unitComboBox->addItems(QStringList()<<"msec"<<"sec");
        unitComboBox->addItems(timeStringList);
        unitComboBox->setCurrentIndex(1);
    }
    else // frequency
    {
        unitComboBox->addItems(QStringList()<<"kHz"<<"Hz"<<"ppm");
    }

//    if(fidSetted())
    if(!ancestor()->FID_2D->FID.isEmpty())
    {
        setReferenceSpinBox->setMaximum(ancestor()->FID_2D->defaultAL()-1);

        if(domainComboBox->currentIndex()==0) setReferenceSpinBox->setValue(0);
        else
        {
            //qDebug() << QString(Q_FUNC_INFO) << FID_2D->al()/2;
            setReferenceSpinBox->setValue(ancestor()->FID_2D->defaultAL()/2);
        }
        referenceValueLineEdit->setText("");
        axisLabelLineEdit->setText(ancestor()->FID_2D->FID[ancestor()->FID_2D->currentFID()]->xAxisLabel());

        refresh();
    }
}
