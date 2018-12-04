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
      domainComboBox->setEnabled(false);
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
    mainLayout->addWidget(applyPushButton,6,1,1,1);
    mainLayout->addWidget(cancelPushButton,7,1,1,1);

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
    connect(axisLabelLineEdit,SIGNAL(returnPressed()),this,SLOT(updateXAxis()));
    connect(referenceValueLineEdit,SIGNAL(returnPressed()),this,SLOT(setUnit()));
    connect(applyPushButton,SIGNAL(clicked(bool)),this,SLOT(apply()));
 //   connect(applyPushButton,SIGNAL(clicked(bool)),this,SLOT(createSettings()));

}

void TAxisFormatWidget::updateXAxis()
{
    if(!isAncestorDefined())return;
    if(ancestor()->FID_2D->FID.isEmpty()) return;

    ancestor()->FID_2D->FID[0]->setXAxisLabel(axisLabelLineEdit->text());
    ancestor()->refresh();
}

void TAxisFormatWidget::setDomain()
{

    if(domainComboBox->currentIndex()>1) return;

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
}

void TAxisFormatWidget::setUnit()
{
    if(!isAncestorDefined())return;
    if(ancestor()->FID_2D->FID.isEmpty()) return;

//    if(!fidSetted()) return;

    //qDebug() << QString(Q_FUNC_INFO) << unitComboBox->currentIndex();

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
    updateXAxis();
}

void TAxisFormatWidget::setTimeUnit()
{
    if(!isAncestorDefined())return;
    if(ancestor()->FID_2D->FID.isEmpty()) return;


    ancestor()->FID_2D->FID[0]->xunit=TFID::Second;
    ancestor()->FID_2D->FID[0]->metricPrefix.setPrefix(TMetricPrefix::Micro);
    ancestor()->FID_2D->FID[0]->setDx(ancestor()->FID_2D->FID[0]->dw()*TMetricPrefix::Decimal(TMetricPrefix::Micro));
    ancestor()->FID_2D->FID[0]->setXInitialValue(referenceValueLineEdit->text().toDouble()
                                     -setReferenceSpinBox->value()*ancestor()->FID_2D->FID[0]->dx());

    ancestor()->FID_2D->FID[0]->setXAxisUnitSymbol("sec");
    switch(unitComboBox->currentIndex())
    {
      case 0: // usec
        axisStyle->setUnit(TAxisStyle::usec);
        ancestor()->FID_2D->FID[0]->plotMetricPrefix.setPrefix(TMetricPrefix::Micro);
        break;
      case 1: // msec
        axisStyle->setUnit(TAxisStyle::msec);
        ancestor()->FID_2D->FID[0]->plotMetricPrefix.setPrefix(TMetricPrefix::Milli);
        break;
      case 2: // sec
        axisStyle->setUnit(TAxisStyle::sec);
        ancestor()->FID_2D->FID[0]->plotMetricPrefix.setPrefix(TMetricPrefix::None);
        break;
    }

    return;
}
void TAxisFormatWidget::setFrequencyUnit()
{
    ancestor()->FID_2D->FID[0]->xunit=TFID::Hz;

    switch(unitComboBox->currentIndex())
    {
      case 0: // kHz
        axisStyle->setUnit(TAxisStyle::kHz);
        ancestor()->FID_2D->FID[0]->metricPrefix.setPrefix(TMetricPrefix::None);
        ancestor()->FID_2D->FID[0]->plotMetricPrefix.setPrefix(TMetricPrefix::Kilo);
        ancestor()->FID_2D->FID[0]->setDx(
                    -1.0/(ancestor()->FID_2D->FID[0]->dw()*TMetricPrefix::Decimal(TMetricPrefix::Micro)
                                    *ancestor()->FID_2D->FID[0]->al())
                    );

        //qDebug() << "kHz  dx: " << FID_2D->FID[0]->dx();
//        FID_2D->FID[0]->setXInitialValue(
//                    0.5/(FID_2D->FID[0]->dw()*TMetricPrefix::Decimal(TMetricPrefix::Micro))
//                    );
        ancestor()->FID_2D->FID[0]->setXInitialValue(
                    referenceValueLineEdit->text().toDouble()*TMetricPrefix::Decimal(TMetricPrefix::Kilo)
                   -setReferenceSpinBox->value()*ancestor()->FID_2D->FID[0]->dx());

       // qDebug() << "xini: " << referenceValueLineEdit->text().toDouble()
       //             -setReferenceSpinBox->value()*FID_2D->FID[0]->dx()*TMetricPrefix::Decimal(TMetricPrefix::Kilo);

        ancestor()->FID_2D->FID[0]->setXAxisUnitSymbol("Hz");

        break;
      case 1: // Hz
        axisStyle->setUnit(TAxisStyle::Hz);

        ancestor()->FID_2D->FID[0]->plotMetricPrefix.setPrefix(TMetricPrefix::None);
        ancestor()->FID_2D->FID[0]->setDx(
                    -1.0/(ancestor()->FID_2D->FID[0]->dw()*TMetricPrefix::Decimal(TMetricPrefix::Micro)
                                    *ancestor()->FID_2D->FID[0]->al())
                    );
       // qDebug() << "Hz  dx: " << FID_2D->FID[0]->dx();

//        FID_2D->FID[0]->setXInitialValue(
//                    0.5/(FID_2D->FID[0]->dw()*TMetricPrefix::Decimal(TMetricPrefix::Micro))
//                    );
        ancestor()->FID_2D->FID[0]->setXInitialValue(referenceValueLineEdit->text().toDouble()
                                         -setReferenceSpinBox->value()*ancestor()->FID_2D->FID[0]->dx());

        ancestor()->FID_2D->FID[0]->setXAxisUnitSymbol("Hz");

        break;
      case 2: //ppm
        axisStyle->setUnit(TAxisStyle::ppm);

        ancestor()->FID_2D->FID[0]->plotMetricPrefix.setPrefix(TMetricPrefix::None);
        ancestor()->FID_2D->FID[0]->setDx(
                          -1.0/(ancestor()->FID_2D->FID[0]->dw()*TMetricPrefix::Decimal(TMetricPrefix::Micro)
                                    *ancestor()->FID_2D->FID[0]->al()*ancestor()->FID_2D->sf1())
                    );
        ancestor()->FID_2D->FID[0]->setXInitialValue(referenceValueLineEdit->text().toDouble()
                                         -setReferenceSpinBox->value()*ancestor()->FID_2D->FID[0]->dx());

        ancestor()->FID_2D->FID[0]->setXAxisUnitSymbol("ppm");

        break;
    }

    //FID_2D->FID[0]
    return;
}
void TAxisFormatWidget::setOtherUnit()
{

    return;
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

    if(isAncestorDefined())
    {


      if(!ancestor()->FID_2D->FID.isEmpty())
      {
        setReferenceSpinBox->setMaximum(ancestor()->FID_2D->al()-1);
        domainComboBox->setCurrentIndex(ancestor()->FID_2D->FID[ancestor()->FID_2D->currentFID()]->domain);

    //    qDebug() << QString(Q_FUNC_INFO) << "0";
        if(ancestor()->FID_2D->FID[ancestor()->FID_2D->currentFID()]->domain==TFID::TimeDomain)
            setReferenceSpinBox->setValue(0);
        else if(ancestor()->FID_2D->FID[ancestor()->FID_2D->currentFID()]->domain==TFID::FrequencyDomain)
            setReferenceSpinBox->setValue(ancestor()->FID_2D->al()/2);

        referenceValueLineEdit->setText("0");
        axisLabelLineEdit->setText(ancestor()->FID_2D->FID[ancestor()->FID_2D->currentFID()]->xAxisLabel());

      }
    }


}

void TAxisFormatWidget::refresh()
{
    axisStyle->setDomain(domainComboBox->currentText());
    setUnit();
    axisStyle->setLabel(axisLabelLineEdit->text());
    axisStyle->setReferencePoint(setReferenceSpinBox->value());
    axisStyle->setReferenceValue(referenceValueLineEdit->text().toDouble());
}

void TAxisFormatWidget::apply()
{
    refresh();
    if(!isAncestorDefined()) return;
    if(ancestor()->FID_2D->FID.isEmpty()) return;
    updateXAxis();
    TAxisStyle *as = new TAxisStyle;
    as=this->axisStyle;
    ancestor()->processOperations->processElements.append(as);
    ancestor()->updateProcessSettings();

    createSettings(ancestor()->processSettings,
                    QString::number(ancestor()->processOperations->processElements.size()-1));
    //emit performRequest();


    /*
    if(!isAncestorDefined()) return;
    if(ancestor()->FID_2D->FID.isEmpty()) return;
    TPhaseRotation *phRot = new TPhaseRotation; //qDebug() << phRot;
      phRot->setPhase0(phaseRotation->phase0());
      phRot->setPhase1(phaseRotation->phase1());
      phRot->setPivot(phaseRotation->pivot());
    ancestor()->processOperations->processElements.append(phRot);

    // common settings
    ancestor()->updateProcessSettings();
    // settings specific to phase rotation
    ancestor()->processSettings->beginGroup(QString::number(ancestor()->processOperations->processElements.size()-1));
      ancestor()->processSettings->setValue("parameters",QStringList()<<"phase0"<<"phase1"<<"pivot");
      ancestor()->processSettings->setValue("phase0",phaseRotation->phase0());
      ancestor()->processSettings->setValue("phase1",phaseRotation->phase1());
      ancestor()->processSettings->setValue("pivot",phaseRotation->pivot());
    ancestor()->processSettings->endGroup();
    ancestor()->processSettings->sync();
    createSettings(ancestor()->processSettings,QString::number(ancestor()->processOperations->processElements.size()-1));
}
     */
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
        setReferenceSpinBox->setMaximum(ancestor()->FID_2D->al()-1);

        if(domainComboBox->currentIndex()==0) setReferenceSpinBox->setValue(0);
        else
        {
            //qDebug() << QString(Q_FUNC_INFO) << FID_2D->al()/2;
            setReferenceSpinBox->setValue(ancestor()->FID_2D->al()/2);
        }
        referenceValueLineEdit->setText(0);
        axisLabelLineEdit->setText(ancestor()->FID_2D->FID[ancestor()->FID_2D->currentFID()]->xAxisLabel());

        refresh();
    }
}
