#include "expSettings.h"
#include <QVBoxLayout>
#include <QTabWidget>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QSettings>
#include <QMessageBox>


TXAxisOptionWidget::TXAxisOptionWidget(QWidget *parent) :
    QWidget(parent)
{
    defaultAxisRadioButton = new QRadioButton(tr("Default x axis"));
    customAxisRadioButton = new QRadioButton(tr("Customize x axis"));
    customAxisRadioButton->setChecked(false);
    xInitialValueLineEdit = new QLineEdit;
    xIncrementLineEdit = new QLineEdit;
    xAxisLabelLineEdit = new QLineEdit;

    metricPrefixCombobox = new QComboBox;
    metricPrefixCombobox->addItems(TMetricPrefix::prefixList());
    metricPrefixCombobox->setCurrentIndex(10);
    unitSymbolLineEdit = new QLineEdit;

    customAxisApplyButton = new QPushButton(tr("Apply"));
    infoLabel = new QLabel(tr(""));

    FInitialValue=0;
    FIncrement=1;

    ppgSetted = false;

    //TlogicSwitch *sw1 = new TlogicSwitch;
    //connect(defaultAxisCheckBox,SIGNAL(toggled(bool)),sw1,SLOT(stateSlot(bool)));
    //connect(sw1,SIGNAL(stateSignal(bool)),customAxisCheckBox,SLOT(setChecked(bool)));
    connect(defaultAxisRadioButton,SIGNAL(toggled(bool)),this,SLOT(onDefaultAxisRadioButtonToggled(bool)));
    connect(customAxisRadioButton,SIGNAL(toggled(bool)),this,SLOT(onCustomAxisRadioButtonToggled(bool)));

    //TlogicSwitch *sw2 = new TlogicSwitch;
    //connect(customAxisCheckBox,SIGNAL(toggled(bool)),sw2,SLOT(stateSlot(bool)));
    //connect(sw2,SIGNAL(stateSignal(bool)),defaultAxisCheckBox,SLOT(setChecked(bool)));

    QGridLayout *gLayout1 = new QGridLayout;
    gLayout1->addWidget(new QLabel(tr("Initial value")),0,0,1,1);
    gLayout1->addWidget(xInitialValueLineEdit,0,1,1,3);
    gLayout1->addWidget(new QLabel(tr("Increment")),1,0,1,1);
    gLayout1->addWidget(xIncrementLineEdit,1,1,1,3);
    gLayout1->addWidget(new QLabel(tr("Axis label")),2,0,1,1);
    gLayout1->addWidget(xAxisLabelLineEdit,2,1,1,3);
    gLayout1->addWidget(new QLabel(tr("Metric prefix")),3,0,1,1);
    gLayout1->addWidget(metricPrefixCombobox,3,1,1,1);
    gLayout1->addWidget(new QLabel(tr("Unit symbol")),3,2,1,1);
    gLayout1->addWidget(unitSymbolLineEdit,3,3,1,1);

    QHBoxLayout *hLayout1 = new QHBoxLayout;
    hLayout1->addStretch();
    hLayout1->addWidget(customAxisApplyButton);
    connect(customAxisApplyButton,SIGNAL(clicked()),this,SLOT(onApplyButtonClicked()));

//    QWidget *widget1 = new QWidget;
    QVBoxLayout *vLayout1 = new QVBoxLayout;
    vLayout1->addWidget(defaultAxisRadioButton);
    vLayout1->addSpacing(10);
    vLayout1->addWidget(customAxisRadioButton);
    vLayout1->addSpacing(10);
    vLayout1->addLayout(gLayout1);
    vLayout1->addLayout(hLayout1);
    vLayout1->addWidget(infoLabel);

    vLayout1->addStretch();


    setLayout(vLayout1);

    defaultAxisRadioButton->setChecked(true);
    onDefaultAxisRadioButtonToggled(true);
    onCustomAxisRadioButtonToggled(false);

}

TMetricPrefix::prefixes TXAxisOptionWidget::getPrefix()
{
  TMetricPrefix::prefixes p;
  switch(metricPrefixCombobox->currentIndex())
  {
    case 0: p=TMetricPrefix::Yotta; break;
    case 1: p=TMetricPrefix::Zetta; break;
    case 2: p=TMetricPrefix::Exa; break;
    case 3: p=TMetricPrefix::Peta; break;
    case 4: p=TMetricPrefix::Tera; break;
    case 5: p=TMetricPrefix::Giga; break;
    case 6: p=TMetricPrefix::Mega; break;
    case 7: p=TMetricPrefix::Kilo; break;
    case 8: p=TMetricPrefix::Hecto; break;
    case 9: p=TMetricPrefix::Deca; break;
    case 10: p=TMetricPrefix::None; break;
    case 11: p=TMetricPrefix::Deci; break;
    case 12: p=TMetricPrefix::Centi; break;
    case 13: p=TMetricPrefix::Milli; break;
    case 14: p=TMetricPrefix::Micro; break;
    case 15: p=TMetricPrefix::Nano; break;
    case 16: p=TMetricPrefix::Pico; break;
    case 17: p=TMetricPrefix::Femto; break;
    case 18: p=TMetricPrefix::Atto; break;
    case 19: p=TMetricPrefix::Zepto; break;
    case 20: p=TMetricPrefix::Yocto; break;
    default: p=TMetricPrefix::None; break;
  }
  return p;
}

void TXAxisOptionWidget::onDefaultAxisRadioButtonToggled(bool b)
{
  emit modified();
  if(b)
  {
    xInitialValueLineEdit->setText("0");
    xIncrementLineEdit->setText("dw");
    xAxisLabelLineEdit->setText("");
    metricPrefixCombobox->setCurrentIndex(13);
    unitSymbolLineEdit->setText("sec");

  }
}

void TXAxisOptionWidget::onCustomAxisRadioButtonToggled(bool b)
{
    emit modified();

    xInitialValueLineEdit->setEnabled(b);
    xIncrementLineEdit->setEnabled(b);
    xAxisLabelLineEdit->setEnabled(b);
    metricPrefixCombobox->setEnabled(b);
    unitSymbolLineEdit->setEnabled(b);
    customAxisApplyButton->setEnabled(b);

    if(b) onApplyButtonClicked();
}


void TXAxisOptionWidget::onApplyButtonClicked()
{
    if(!ppgSetted)
    {
        infoLabel->setText("Pulse program has not been set yet.");
        return;
    }
    bool ok;

    QString qs=xInitialValueLineEdit->text();
    ok=ppg->evalArg(qs);
    if(ok) {setInitialValue(ppg->evalArgResult);}
    else
    {
        infoLabel->setText("Invalid expression: " + qs);
        return;
    }

    qs=xIncrementLineEdit->text();
    ok=ppg->evalArg(qs);
    if(ok) {setIncrement(ppg->evalArgResult);}
    else
    {
        infoLabel->setText("Invalid expression: " + qs);
        return;
    }

    infoLabel->setText("X axis re-definition successful.");
    emit modified();

}

TExpSettings::TExpSettings(QWidget *parent) :
    QWidget(parent)
{
    this->hide();
    this->setTitle(tr("Experimental Settings")); //default window title;
    this->setWindowFlags(Qt::Dialog);
   // this->setFixedWidth(500);
   // this->setFixedHeight(400);
    ppgSetted = false;
    createWidgets();
    createPanel();
    setModified(false);
}


void TExpSettings::createWidgets()
{
    fileWidget = new QWidget;
    QHBoxLayout *fileLayout = new QHBoxLayout(fileWidget);
    fileLayout->setMargin(0);
    fileLayout->setSpacing(0);
    QWidget *fileManageWidget = new QWidget;
      pathLineEdit = new QLineEdit;
        #if defined(__APPLE__)
          pathLineEdit->setText(QDir::homePath());
       //   pathLineEdit->setText("");
        #elif defined(_WIN32)
          pathLineEdit->setText("C:/temp");
        #endif
      setDirButton = new QPushButton("...");
      connect(setDirButton,SIGNAL(clicked()),this,SLOT(setDirectory()));
//      qDebug() << "TExpSettings::createWidgets 1";

      nameLineEdit = new QLineEdit; nameLineEdit->setText("Untitled");
      dataSaveButton = new QPushButton(tr("Save Data")); dataSaveButton->setEnabled(false);
      obsFreqLineEdit = new QLineEdit; obsFreqLineEdit->setText("10.0");
      obsFreqCheckBox = new QCheckBox(tr("Choose from frequency variables"));
      obsFreqComboBox = new QComboBox;
      obsFreqComboBox->clear();
      commentTextEdit = new QPlainTextEdit;
      commentTextEdit->document()->setMaximumBlockCount(1000);
      jobSaveButton = new QPushButton(tr("Save job")); jobSaveButton->setEnabled(false);
      connect(jobSaveButton,SIGNAL(clicked()),this,SLOT(onJobSaveButtonClicked()));
      jobLoadButton = new QPushButton(tr("Load job")); jobLoadButton->setEnabled(false);
      connect(jobLoadButton,SIGNAL(clicked()),this,SLOT(onJobLoadButtonClicked()));
      jobQueueButton = new QPushButton(tr("Queue job")); jobQueueButton->setEnabled(false);
      connect(jobQueueButton,SIGNAL(clicked()),this,SLOT(onJobQueueButtonClicked()));


//      qDebug() << "TExpSettings::createWidgets 2";

      connect(obsFreqCheckBox,SIGNAL(toggled(bool)),obsFreqLineEdit,SLOT(setDisabled(bool)));
      connect(obsFreqCheckBox,SIGNAL(toggled(bool)),obsFreqComboBox,SLOT(setEnabled(bool)));
      connect(obsFreqLineEdit,SIGNAL(textChanged(QString)),this,SLOT(setCarrierFreq()));
      connect(obsFreqCheckBox,SIGNAL(toggled(bool)),this,SLOT(setCarrierFreq()));
      connect(obsFreqComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(setCarrierFreq()));
      obsFreqCheckBox->setChecked(true);

//      qDebug() << "TExpSettings::createWidgets 3";

      QGridLayout *obsFreqLayout = new QGridLayout;
      obsFreqLayout->addWidget(new QLabel(tr("Carrier Frequency")),0,0,1,1);
      obsFreqLayout->addWidget(obsFreqLineEdit,0,1,1,1);
      obsFreqLayout->addWidget(new QLabel(tr("MHz")),0,2,1,1);
        QHBoxLayout *ofLayout = new QHBoxLayout;
        ofLayout->addWidget(obsFreqCheckBox);
        ofLayout->addWidget(obsFreqComboBox);
      obsFreqLayout->addLayout(ofLayout,1,1,1,2);
      //obsFreqLayout->addWidget(obsFreqCheckBox,1,1,1,2);
      //obsFreqLayout->addWidget(obsFreqComboBox,2,1,1,1);

      QGridLayout *fileManageLayout = new QGridLayout(fileManageWidget);
      fileManageLayout->addWidget(new QLabel(tr("Working Dir")),0,0);
      fileManageLayout->addWidget(pathLineEdit,0,1);
      fileManageLayout->addWidget(setDirButton,0,2);
      fileManageLayout->addWidget(new QLabel(tr("Job Name")),1,0);
      fileManageLayout->addWidget(nameLineEdit,1,1);
      fileManageLayout->addWidget(dataSaveButton,1,2);
      fileManageLayout->addLayout(obsFreqLayout,2,0,1,3);
      fileManageLayout->addWidget(new QLabel(tr("Comment")),3,0);
      fileManageLayout->addWidget(commentTextEdit,4,0,1,3);

      QHBoxLayout *jobLayout = new QHBoxLayout;
      jobLayout->addWidget(jobLoadButton);
      jobLayout->addStretch();
      jobLayout->addWidget(jobSaveButton);
      jobLayout->addWidget(jobQueueButton);
      fileManageLayout->addLayout(jobLayout,5,0,1,3);

    fileLayout->addWidget(fileManageWidget);

//    qDebug() << "TExpSettings::createWidgets 4";

    variableTable =new TvariableTable;
    connect(variableTable,SIGNAL(freqVariableChanged(int)),this,SLOT(onFreqVariableChanged(int)));
    connect(variableTable,SIGNAL(modified()),this,SLOT(onModified()));

    arrayWidget = new TArrayWidget;
    connect(arrayWidget,SIGNAL(modified()),this,SLOT(onModified()));

    acquisitionWidget = new TAcquisitionWidget;
    connect(acquisitionWidget,SIGNAL(commandRequest(QStringList)),this,SIGNAL(transferRequest(QStringList)));
    connect(acquisitionWidget,SIGNAL(modified()),this,SLOT(onModified()));


    connect(variableTable,SIGNAL(updateOrigNARequest(int)),acquisitionWidget,SLOT(setOrigNA(int)));
    // This must appear AFTER creating acquisitionWidget!


    xAxisOptionWidget = new TXAxisOptionWidget;
    connect(xAxisOptionWidget,SIGNAL(modified()),this,SLOT(onModified()));

    optionTab=new QTabWidget;
//    qDebug() << "TExpSettings::createWidgets 5";

}

void TExpSettings::createPanel()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    optionTab->setTabPosition(QTabWidget::North);
    optionTab->addTab(fileWidget,tr("File"));
    optionTab->addTab(variableTable,tr("Variables"));
    optionTab->addTab(arrayWidget,tr("Array"));
    optionTab->addTab(acquisitionWidget,tr("Acquisition"));
    optionTab->addTab(xAxisOptionWidget,tr("X Axis"));
    mainLayout->addWidget(optionTab);
    setLayout(mainLayout);
}

//----------------------------------------------------------------------------
void TExpSettings::showWindow()
{
    //this->setWindowFlags(Qt::WindowStaysOnTopHint);
    //this->show();
    this->setWindowFlags(Qt::Dialog);
    this->show();
}
//----------------------------------------------------------------------------
void TExpSettings::setDirectory()
{
   QFileDialog *dialog = new QFileDialog;

   dialog->setFileMode(QFileDialog::Directory);
   dialog->setOption(QFileDialog::ShowDirsOnly);
   dialog->setViewMode(QFileDialog::Detail);
   dialog->setLabelText(QFileDialog::Accept, tr("Choose directory"));

   if(dialog->exec())
   {
       pathLineEdit->setText(dialog->selectedFiles().at(0));
   }
}
//----------------------------------------------------------------------------
void TExpSettings::onJobLoadButtonClicked()
{

//    QString path="~/";
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open job"),
                                                    pathLineEdit->text(),
                                                    tr("job file (*.nmrjob)"));

    if (fileName.isEmpty()) return;

    readJob(fileName);
}
//----------------------------------------------------------------------------
void TExpSettings::readJob(QString fileName)
{
    disconnect(obsFreqLineEdit,SIGNAL(textChanged(QString)),this,SLOT(setCarrierFreq()));
    disconnect(obsFreqCheckBox,SIGNAL(toggled(bool)),this,SLOT(setCarrierFreq()));
    disconnect(obsFreqComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(setCarrierFreq()));

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
       QMessageBox::warning(this,tr(""),
                            "Failed to open file: " + fileName + "\n"
                            );
       return;
    }
    file.close();

    QSettings settings(fileName, QSettings::IniFormat);

    QFileInfo fInfo(fileName);
    QString path=fInfo.canonicalPath(); // link -> real file
    QString grandParentPath=QFileInfo(fInfo.dir().absolutePath()).dir().absolutePath();

    settings.beginGroup("File");
      QString ppgFileName=settings.value("ppgFilename").toString();

      // ppg->setppgFileName(settings.value("ppgFilename").toString());
      ppg->setppgFileName(path+"/"+ppgFileName);
      // int n=settings.value("numberOfImportedFiles",QVariant(0)).toInt();   // not necessary?

      pathLineEdit->setText(grandParentPath);
      nameLineEdit->setText(settings.value("nameLineEdit").toString());
      obsFreqLineEdit->setText(settings.value("obsFreqLineEdit").toString());
      obsFreqCheckBox->setChecked(settings.value("obsFreqCheckBox").toBool());
      int obsFreqComboBoxValue=settings.value("obsFreqComboBox").toInt();
      // we need to set the index later, after ppg has been compiled!
      commentTextEdit->setPlainText(settings.value("commentTextEdit").toString());
    settings.endGroup();

    if(!ppg->loadPPG()) {
        QMessageBox::warning(this,tr("load error"),tr("Failed to load pulse program."));
        return;
    }
    // if successful, sourcePPG has been set so that you can implement ppg.compile();
    if(!ppg->compilePPG()) {
        QMessageBox::warning(this,tr("Compile error. "),
                                tr("Failed to compile pulse program: ") + ppg->errorMessage);
//
        emit message(ppg->errorMessage);
        return;
    }
    setFreqList();
    emit ppgCompiled();

    obsFreqComboBox->setCurrentIndex(obsFreqComboBoxValue);



    acquisitionWidget->setOrigNA(ppg->variables.at(ppg->variableIndex("NA"))->numeric().toInt());
    acquisitionWidget->setOrigAL(ppg->variables.at(ppg->variableIndex("AL"))->numeric().toInt());

    settings.beginGroup("Acquisition");
      acquisitionWidget->separateDataStorageCheckBox
                       ->setChecked(settings.value("separateDataStorageCheckBox").toBool());
      acquisitionWidget->separateDataStorageSpinBox
                       ->setValue(settings.value("separateDataStorageSpinBox").toInt());
     // acquisitionWidget->separateDataStorageComboBox
     //                  ->setCurrentIndex(settings.value("separateDataStorageComboBox").toInt());
      acquisitionWidget->multipleAcquisitionsCheckBox
                       ->setChecked(settings.value("multipleAcquisitionCheckBox").toBool());
      acquisitionWidget->multipleAcquisitionSpinBox
                       ->setValue(settings.value("multipleAcquisitionSpinBox").toInt());
      acquisitionWidget->multipleAcquisitionComboBox
                       ->setCurrentIndex(settings.value("multipleAcquisitionComboBox").toInt());
      acquisitionWidget->inFPGAAccumCheckBox
                       ->setChecked(settings.value("inFPGAAccumCheckBox").toBool());
      acquisitionWidget->inFPGAAccumSpinBox
                       ->setValue(settings.value("inFPGAAccumSpinBox").toInt());
//      acquisitionWidget->spectralDensityAccumCheckBox
//                       ->setChecked(settings.value("spectralDensityAccumCheckBox").toBool());
      acquisitionWidget->phaseReverseCheckBox
                       ->setChecked(settings.value("phaseReverseCheckBox").toBool());
      acquisitionWidget->phaseRotationCheckBox
                       ->setChecked(settings.value("phaseRotationCheckBox").toBool());
      acquisitionWidget->phaseRotationSpinBox
                       ->setValue(settings.value("phaseRotationSpinBox").toDouble());
      acquisitionWidget->FFTCheckBox
                       ->setChecked(settings.value("FFTCheckBox").toBool());
      acquisitionWidget->replaceRealWithAbsCheckBox
                       ->setChecked(settings.value("replaceRealWithAbsCheckBox","false").toBool());
      acquisitionWidget->QDCheckBox
                       ->setChecked(settings.value("QDCheckBox").toBool());
      acquisitionWidget->offsetCorrectionCheckBox
                       ->setChecked(settings.value("offsetCorrectionCheckBox").toBool());
      acquisitionWidget->offsetCorrectionSpinBox
                       ->setValue(settings.value("offsetCorrectionSpinBox").toInt());
      acquisitionWidget->digitalFilterCheckBox
                       ->setChecked(settings.value("digitalFilterCheckBox").toBool());

      acquisitionWidget->onOffsetChanged(); // Added (2016.10.3 K.T)

    settings.endGroup();

    settings.beginGroup("xAxis");
      xAxisOptionWidget->defaultAxisRadioButton
              ->setChecked(settings.value("defaultAxisRadioButton").toBool());
      xAxisOptionWidget->customAxisRadioButton
              ->setChecked(settings.value("customAxisRadioButton").toBool());
      xAxisOptionWidget->xInitialValueLineEdit
              ->setText(settings.value("xInitialValueLineEdit").toString());
      xAxisOptionWidget->xIncrementLineEdit
              ->setText(settings.value("xIncrementLineEdit").toString());
      xAxisOptionWidget->xAxisLabelLineEdit
              ->setText(settings.value("xAxisLabelLineEdit").toString());

      int kx=settings.value("metricPrefixCombobox").toInt();
      if(kx<0)kx=0;
      if(kx>xAxisOptionWidget->metricPrefixCombobox->count()-1) kx=xAxisOptionWidget->metricPrefixCombobox->count()-1;
      xAxisOptionWidget->metricPrefixCombobox
              ->setCurrentIndex(kx);

      xAxisOptionWidget->unitSymbolLineEdit
              ->setText(settings.value("unitSymbolLineEdit").toString());


    settings.endGroup();

    settings.beginGroup("Variables");
      QStringList variableNames=settings.value("names").toStringList();
      for(int a=0; a<variableNames.size(); a++)
      {
         int vIndex=ppg->variableIndex(variableNames.at(a));
         if(vIndex>-1)
         {
             ppg->variables[vIndex]->setNumeric(settings.value(variableNames.at(a)));
             if(0==QString::compare(ppg->variables.at(vIndex)->name(),"NA",Qt::CaseInsensitive))
             {
                 acquisitionWidget->setOrigNA(settings.value(variableNames.at(a)).toInt());
                 ppg->setOrigNA(settings.value(variableNames.at(a)).toInt());
             }
         }
         else
         {
   //          QMessageBox::warning(this,tr(""),
   //                               "Variable " + variableNames.at(a) + " was not found."
   //                               "<p> The pulse program may have been modified after creation of the job file."
   //                               );
   //          return;
         }
      }
    settings.endGroup();

    while(variableTable->rowCount()>0) variableTable->removeRow(0);
    for(int row=0; row<ppg->variables.size(); row++)
    {
        variableTable->addRow(ppg->variables.at(row));
    }


    settings.beginGroup("Array");
      arrayWidget->initialSetup(ppg);
      bool arrayCheckBoxChecked=settings.value("arrayCheckBox").toBool();
      int arrayDimensions=settings.value("dimensions",QVariant(0)).toInt();
      QList<int> arrayCounts;
      for(int i=0; i<arrayDimensions; i++)
      {
          arrayCounts.append(settings.value("arrayCount"+QString::number(i)).toInt());
      }
      QStringList arrayVariables=settings.value("arrayVariables").toStringList();
    settings.endGroup();

    for(int i=0; i<arrayVariables.size(); i++)
    {
       settings.beginGroup(arrayVariables.at(i));
         int avIndex=ppg->variableIndex(arrayVariables.at(i));
         ppg->variables[avIndex]->setArrayDimension(settings.value("dimension").toInt());

         arrayWidget->arrayDimensionComboBox->setCurrentIndex(ppg->variables[avIndex]->arrayDimension()-1);
         ppg->variables[avIndex]->setArrayMode(static_cast<TVariable::ArrayMode>(settings.value("mode").toUInt()));
         if(ppg->variables[avIndex]->arrayMode()==TVariable::LinearArray)
         {
             ppg->variables[avIndex]->setIniValue(settings.value("initial"));
             ppg->variables[avIndex]->setIncValue(settings.value("increment"));
             ppg->variables[avIndex]
                     ->setArrayCount(arrayCounts.at(ppg->variables[avIndex]->arrayDimension()-1));
         }
         else if(ppg->variables[avIndex]->arrayMode()==TVariable::RandomArray)
         {
             ppg->variables[avIndex]->randomArrayValues=settings.value("values").toList();
             ppg->variables[avIndex]->setArrayCount(
                         ppg->variables[avIndex]->randomArrayValues.size()
                         );
         }
       settings.endGroup();
       arrayWidget->addArray(ppg->variables[avIndex]);
       arrayWidget->refresh();
    }

      arrayWidget->arrayCheckBox->setChecked(arrayCheckBoxChecked);
      if(arrayCheckBoxChecked)
      {
        arrayWidget->onRequestButtonClicked();
      }

    // We transfer compiled PPG
    emit transferRequest(ppg->toFPGA);
    // We clear toFPGA after transfer
    // ppg->toFPGA.clear();

    // We update variables
    ppg->updateVariable();
    // We transfer updated variables
    emit transferRequest(ppg->updatedPPG);


    emit setJobRequest();

    if(xAxisOptionWidget->customAxisRadioButton->isChecked()) xAxisOptionWidget->onApplyButtonClicked();

    jobSaveButton->setEnabled(true);

    connect(obsFreqLineEdit,SIGNAL(textChanged(QString)),this,SLOT(setCarrierFreq()));
    connect(obsFreqCheckBox,SIGNAL(toggled(bool)),this,SLOT(setCarrierFreq()));
    connect(obsFreqComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(setCarrierFreq()));

    emit addJobListRequest(fileName);

    setCarrierFreq();  // Added in build 168.  2016.3.3 KT

    setModified(false);
    // Having read a job, we set FIsModified to false.
    // This should be implemented AFTER setCarrierFreq().  20160306 KT

}

void TExpSettings::onFreqVariableChanged(int vIndex)
{
    if(!ppgSetted || !ppg->compiled) {qDebug()<<"ppg set error."; return;}
    int cIndex=-1;
    for(int k=0; k<obsFreqComboBox->count(); k++)
    {
        if(0==QString::compare(ppg->variables.at(vIndex)->name(),obsFreqComboBox->itemText(k),Qt::CaseInsensitive))
        {
            cIndex=k;
        }
    }
    if(cIndex<0) {qDebug()<<"variable not found." ;return;}
    if(obsFreqCheckBox->isChecked() && cIndex==obsFreqComboBox->currentIndex())
    {
        setCarrierFreq();
    }

    setModified(true);

    return;
}

void TExpSettings::setFreqList()
{
     if(!ppg->compiled) return;

     disconnect(obsFreqComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(setCarrierFreq()));

     obsFreqComboBox->clear();
     int c=0;
     for(int k=0; k<ppg->variables.size()-1; k++)
     {
     if(ppg->variables.at(k)->type()==TVariable::freqVariable)
     {
         obsFreqComboBox->addItem(ppg->variables.at(k)->name());
         c++;
     }
    }
    if(c==0) {obsFreqComboBox->addItem("Unavailable");}
    obsFreqComboBox->setCurrentIndex(0);
    connect(obsFreqComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(setCarrierFreq()));

    setCarrierFreq();
    return;
}

void TExpSettings::setCarrierFreq()
{

//    qDebug()<<"TExpSettings::setCarrierFreq 0";
    double f;
    if(!ppgSetted) return;
    if(!ppg->compiled) return;
//    qDebug()<<"TExpSettings::setCarrierFreq 1";
    disconnect(obsFreqLineEdit,SIGNAL(textChanged(QString)),this,SLOT(setCarrierFreq()));
    disconnect(obsFreqCheckBox,SIGNAL(toggled(bool)),this,SLOT(setCarrierFreq()));
    disconnect(obsFreqComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(setCarrierFreq()));

    if(obsFreqCheckBox->isChecked())
    {
        int vIndex=ppg->variableIndex(obsFreqComboBox->currentText());
        if(vIndex<0)
        {
            QMessageBox::warning(this,tr("error"),tr("Failed to set carrier frequency."));
            return;
        }
        obsFreqLineEdit->setText(ppg->variables.at(vIndex)->numericString());
        f=ppg->variables.at(vIndex)->numeric().toDouble();
    }
    else
    {
        if(obsFreqLineEdit->text().trimmed().isEmpty())
        {
            obsFreqLineEdit->setText("10.0");
        }
        bool ok;
        f=obsFreqLineEdit->text().toDouble(&ok);
        if(!ok) f=10.0;
    }
//    qDebug()<<"TExpSettings::setCarrierFreq 2";

    emit setCarrierFreqRequest(f);
    setModified(true);
//    qDebug()<<"TExpSettings::setCarrierFreq 3";
    connect(obsFreqLineEdit,SIGNAL(textChanged(QString)),this,SLOT(setCarrierFreq()));
    connect(obsFreqCheckBox,SIGNAL(toggled(bool)),this,SLOT(setCarrierFreq()));
    connect(obsFreqComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(setCarrierFreq()));

}
//----------------------------------------------------------------------------
void TExpSettings::updateVariableTable()
{
    while(variableTable->rowCount()>0) variableTable->removeRow(0);
    for(int row=0; row<ppg->variables.size(); row++)
    {
      if(!ppg->variables.at(row)->isConst())
      {
        if(0==QString::compare(ppg->variables.at(row)->name(),"NA",Qt::CaseInsensitive))
        {
          TVariable *originalNA=new TVariable("NA",TVariable::naVariable,QVariant(ppg->origNA()),TVariable::Unity);
          variableTable->addRow(originalNA);
          delete originalNA;
        }
        else
        {
          variableTable->addRow(ppg->variables.at(row));
        }
      }
    }
}
//----------------------------------------------------------------------------
void TExpSettings::onJobQueueButtonClicked()
{
   onJobSaveButtonClicked();
   if(!ok) return;
   emit queueJobRequest(jobFileName);
}
//----------------------------------------------------------------------------
void TExpSettings::onJobSaveButtonClicked()
{

    if(!ppg->compiled) return;

    QString jobName=nameLineEdit->text().trimmed();

    if(jobName.isEmpty())
    {
        QMessageBox::warning(this,tr("error"), tr("job name is empty."));
        ok=false;
        return;
    }

    QString workingDirectory=pathLineEdit->text().trimmed();
    if(!workingDirectory.endsWith('/')) workingDirectory+='/';

    QString dirName=workingDirectory+jobName;
    QDir dir;
    if(!dir.exists(dirName))
    {
       if(!dir.mkpath(dirName))
       // if(!dir.mkdir(dirName))
       {
         QMessageBox::warning(this,tr("dir error"),
                              tr("Directory ")+ dirName+ tr(" could not be made."));
         ok=false;
         return;
       }
    }

    jobFileName = dirName + '/' + jobName + ".nmrjob";

    if(QFile::exists(jobFileName))
        // What to do if we find the job file with the same name in the same path...
    {
        if(isModified())
        {
            if (QMessageBox::No == QMessageBox::question(this, "Overwrite?",
                                           "A job file " + jobFileName + " already exists."
                                           "<p> Do you want to overwrite it and DELETE obsolete data files?",
                                           QMessageBox::Yes|QMessageBox::No))
            {
                ok=false;
                return; // we do not overwrite. The user should rename it.
            }
            else
            {
                dir.setPath(dirName);
                QStringList filters;
                filters << "*.opp" << "*.opd" << "*.sm2p" << "*.sm2d" << "*.ppg";
                dir.setNameFilters(filters);
                dir.setFilter(QDir::Files | QDir::NoSymLinks);
                QFileInfoList infolist = dir.entryInfoList();

                foreach(QFileInfo fileInfo, infolist)
                {
                    if(QFile::remove(fileInfo.filePath()))
                    {
                    //    qDebug("%s %d: QFile::remove(path = %s)",__FILE__,__LINE__, fileInfo.filePath());
                    }
                    else
                    {
                    //    qDebug("%s %d: Error:QFile::remove(path = %s)",__FILE__,__LINE__, fileInfo.filePath());
                    }
                }


            }
        }
    }

    QFileInfo sourceFileInfo;
    sourceFileInfo.setFile(ppg->sourceFileName());
    QString sPath=sourceFileInfo.absolutePath();

    ok=true;

    //sourceFileInfo.fileName() -> the name of the ppg source file, excluding the path.
    if(QFile::exists(dirName+'/'+sourceFileInfo.fileName())
       && ppg->sourceFileName()!=dirName+'/'+sourceFileInfo.fileName())
    {

         ok=QFile::remove(dirName+'/'+sourceFileInfo.fileName());
         if(!ok) {QMessageBox::warning(this,tr("error"),tr("Failed to delete the old ppg file.")); return;}

         ppg->updateSourcePPG();
         ppg->setppgFileName(dirName+'/'+sourceFileInfo.fileName());

         QFile file(dirName+'/'+sourceFileInfo.fileName());
         if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
         {
             QMessageBox::warning(this,tr("error"),tr("Failed to copy the new ppg file."));
             return;
         }

         QTextStream out(&file);
         for(int k=0; k<ppg->sourcePPG.size(); k++) out << ppg->sourcePPG.at(k) << "\n";
    }

    if(!QFile::exists(dirName+'/'+sourceFileInfo.fileName()))
    {
        ppg->updateSourcePPG();
        ppg->setppgFileName(dirName+'/'+sourceFileInfo.fileName());

        QFile file(dirName+'/'+sourceFileInfo.fileName());
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(this,tr("error"),tr("Failed to copy the new ppg file."));
            return;
        }

        QTextStream out(&file);
        for(int k=0; k<ppg->sourcePPG.size(); k++) out << ppg->sourcePPG.at(k) << "\n";

    }

    QFileInfo gateFileInfo;
    gateFileInfo.setFile(ppg->gateFileName());
    QString gPath=gateFileInfo.absolutePath();

    //qDebug() << QString(Q_FUNC_INFO) << sPath << gPath;

    // If the ppg source file and the included .gate file are located in the same directory,
    // we judge the custom gate file is used.
    if(0==QString::compare(sPath,gPath))
    {
        if(!QFile::exists(dirName+'/'+gateFileInfo.fileName()))
        {
          ok=QFile::copy(ppg->gateFileName(),dirName+'/'+gateFileInfo.fileName());

          ppg->setGateFileName(dirName+'/'+gateFileInfo.fileName());
          //qDebug() << QString(Q_FUNC_INFO) << "ppg gateFileName: " << ppg->gateFileName();

        }
    // qDebug() << ppg->gateFileName() << fn+gateFileInfo.fileName();

          // If the gate file does not exist in the same directory, we copy it.
    }
    if(!ok) {QMessageBox::warning(this,tr("error"),tr("Failed to copy gate-definition file."));return;}

    QFileInfo loadedFileInfo;

    //qDebug() << QString(Q_FUNC_INFO) << "loaded files to be copied:" << ppg->loadedFiles;

    for(int k=0; k<ppg->loadedFiles.size();k++)
    {
        loadedFileInfo.setFile(ppg->loadedFiles.at(k));
        QString lPath=loadedFileInfo.absolutePath();
        //qDebug() << QString(Q_FUNC_INFO) << sPath << lPath;
        if(0==QString::compare(sPath,lPath))
        {
            if(!QFile::exists(dirName+'/'+loadedFileInfo.fileName()))
              ok=QFile::copy(ppg->loadedFiles.at(k),dirName+'/'+loadedFileInfo.fileName());
            // If the loaded file exist in the same directory, we copy them.
            if(!ok) {QMessageBox::warning(this,tr("error"),tr("Failed to copy file(s)."));return;}
            ppg->loadedFiles[k]=dirName+'/'+loadedFileInfo.fileName();
        }
    }

    writeJob(jobFileName);

    return;

}
//----------------------------------------------------------------------------
void TExpSettings::writeJob(QString fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
       QMessageBox::warning(this,tr(""),
                            "Failed to open file: " + fileName + "\n"
                            );
       return;
    }
    file.close();

    QSettings settings(fileName, QSettings::IniFormat);

    settings.beginGroup("File");
      settings.setValue("numberOfImportedFiles",ppg->loadedFiles.size());
      //for(int i=0; i<ppg->loadedFiles.size(); i++)
      //    settings.setValue("loadedFile"+QString::number(i),ppg->loadedFiles.at(i));
      QFileInfo fInfo(ppg->sourceFileName());
//      settings.setValue("ppgFilename",ppg->sourceFileName());
      settings.setValue("ppgFilename", fInfo.fileName()); // We save the file name without path.
      //settings.setValue("pathLineEdit", pathLineEdit->text()); commented out (path should be that of the job file)
      settings.setValue("nameLineEdit", nameLineEdit->text());
      settings.setValue("obsFreqLineEdit",obsFreqLineEdit->text());
      settings.setValue("obsFreqCheckBox",obsFreqCheckBox->isChecked());
      settings.setValue("obsFreqComboBox",obsFreqComboBox->currentIndex());
      settings.setValue("commentTextEdit",commentTextEdit->toPlainText());
    settings.endGroup();

    settings.beginGroup("Variables");
      settings.setValue("names",variableTable->variableNames());
      for(int k=0; k<ppg->variables.size(); k++)
      {
          if(0==QString::compare(ppg->variables.at(k)->name(),"NA",Qt::CaseInsensitive))
          {
             // If we use separate acq (in, e.g., 2D experiments), NA may have been altered.
             // Accordingly we write the original value.
             // setOrigNA is called either when ppg is compiled (including updataVariable)
             // or job is read.
             settings.setValue(ppg->variables.at(k)->name(), acquisitionWidget->origNA());
          }
          if(!ppg->variables.at(k)->isConst()
             && 0!=QString::compare(ppg->variables.at(k)->name(),"NA",Qt::CaseInsensitive)
             && 0!=QString::compare(ppg->variables.at(k)->name(),"ACQTIME",Qt::CaseInsensitive))
          {
             settings.setValue(ppg->variables.at(k)->name(), ppg->variables.at(k)->numeric());
          }
      }
    settings.endGroup();


    settings.beginGroup("Array");
      settings.setValue("arrayCheckBox",arrayWidget->arrayCheckBox->isChecked());
      settings.setValue("dimensions",
                        arrayWidget->arrayCounter.NOfCounts.size());
      for(int i=0; i<arrayWidget->arrayCounter.NOfCounts.size(); i++)
      {
         settings.setValue("arrayCount"+QString::number(i),
                           arrayWidget->arrayCounter.NOfCounts.at(i));
      }
      QStringList sl; sl.clear();

      for(int row=0; row<arrayWidget->arrayTableWidget->rowCount(); row++)
      {
          sl.append(arrayWidget->arrayTableWidget->item(row,0)->text());
      }

//      qDebug() << sl;
      settings.setValue("arrayVariables",sl);
      settings.endGroup();

      for(int a=0; a<sl.size(); a++)
      {
          int vIndex=ppg->variableIndex(sl.at(a));
          settings.beginGroup(ppg->variables.at(vIndex)->name());
          settings.setValue("dimension",
                             ppg->variables.at(vIndex)->arrayDimension());
          settings.setValue("mode",
                                 ppg->variables.at(vIndex)->arrayMode());
          if(ppg->variables.at(vIndex)->arrayMode()==TVariable::LinearArray)
          {
            settings.setValue("initial", ppg->variables.at(vIndex)->iniValue());
            settings.setValue("increment", ppg->variables.at(vIndex)->incValue());
          }  // TODO: check value <---> numeric   OK?
          else if(ppg->variables.at(vIndex)->arrayMode()==TVariable::RandomArray)
          {
             settings.setValue("values",
                                   ppg->variables.at(vIndex)->randomArrayValues);
                 //settings.setValue("values",
                              //     ppg->variables.at(a)->randomArrayNumericString());
          }
          settings.endGroup();
      }

    settings.beginGroup("Acquisition");
      settings.setValue("separateDataStorageCheckBox",
                        acquisitionWidget->separateDataStorageCheckBox->isChecked());
      settings.setValue("separateDataStorageSpinBox",
                        acquisitionWidget->separateDataStorageSpinBox->value());
     // settings.setValue("separateDataStorageComboBox",
     //                   acquisitionWidget->separateDataStorageComboBox->currentIndex());
      settings.setValue("multipleAcquisitionCheckBox",
                        acquisitionWidget->multipleAcquisitionsCheckBox->isChecked());
      settings.setValue("multipleAcquisitionSpinBox",
                        acquisitionWidget->multipleAcquisitionSpinBox->value());
      settings.setValue("multipleAcquisitionComboBox",
                        acquisitionWidget->multipleAcquisitionComboBox->currentIndex());
      settings.setValue("inFPGAAccumCheckBox",
                        acquisitionWidget->inFPGAAccumCheckBox->isChecked());
      settings.setValue("inFPGAAccumSpinBox",
                        acquisitionWidget->inFPGAAccumSpinBox->value());
//      settings.setValue("spectralDensityAccumCheckBox",
//                        acquisitionWidget->spectralDensityAccumCheckBox->isChecked());
      settings.setValue("phaseReverseCheckBox",
                        acquisitionWidget->phaseReverseCheckBox->isChecked());
      settings.setValue("phaseRotationCheckBox",
                        acquisitionWidget->phaseRotationCheckBox->isChecked());
      settings.setValue("phaseRotationSpinBox",
                        acquisitionWidget->phaseRotationSpinBox->value());
      settings.setValue("FFTCheckBox",
                        acquisitionWidget->FFTCheckBox->isChecked());
      settings.setValue("replaceRealWithAbsCheckBox",
                        acquisitionWidget->replaceRealWithAbsCheckBox->isChecked());
      settings.setValue("QDCheckBox",
                        acquisitionWidget->QDCheckBox->isChecked());
      settings.setValue("offsetCorrectionCheckBox",
                        acquisitionWidget->offsetCorrectionCheckBox->isChecked());
      settings.setValue("offsetCorrectionSpinBox",
                        acquisitionWidget->offsetCorrectionSpinBox->value());
      settings.setValue("digitalFilterCheckBox",
                        acquisitionWidget->digitalFilterCheckBox->isChecked());

    settings.endGroup();

    settings.beginGroup("xAxis");
      settings.setValue("defaultAxisRadioButton",xAxisOptionWidget->defaultAxisRadioButton->isChecked());
      settings.setValue("customAxisRadioButton",xAxisOptionWidget->customAxisRadioButton->isChecked());
      settings.setValue("xInitialValueLineEdit",xAxisOptionWidget->xInitialValueLineEdit->text());
      settings.setValue("xIncrementLineEdit",xAxisOptionWidget->xIncrementLineEdit->text());
      settings.setValue("xAxisLabelLineEdit",xAxisOptionWidget->xAxisLabelLineEdit->text());
      settings.setValue("metricPrefixCombobox",xAxisOptionWidget->metricPrefixCombobox->currentIndex());
      settings.setValue("unitSymbolLineEdit",xAxisOptionWidget->unitSymbolLineEdit->text());
    settings.endGroup();

    settings.sync();

  //  QFile::link(fileName,
  //              QFileInfo(QFileInfo(fileName).dir().absolutePath()).dir().absolutePath()
  //              +'/'+QFileInfo(fileName).fileName());

    emit addJobListRequest(fileName);
    setModified(false);

}

void TExpSettings::onRepeatScan()
{
    variableTable->setEditable(false);
    arrayWidget->setEnabled(false);
    acquisitionWidget->setEnabled(true);
    jobLoadButton->setEnabled(false);

    acquisitionWidget->separateDataStorageCheckBox->setEnabled(false);
    acquisitionWidget->separateDataStorageSpinBox->setEnabled(false);
   // acquisitionWidget->separateDataStorageComboBox->setEnabled(false);
    acquisitionWidget->multipleAcquisitionsCheckBox->setEnabled(false);
    acquisitionWidget->inFPGAAccumCheckBox->setEnabled(false);
    //acquisitionWidget->spectralDensityAccumCheckBox->setEnabled(false);
    acquisitionWidget->phaseReverseCheckBox->setEnabled(true);
    acquisitionWidget->phaseRotationCheckBox->setEnabled(true);
    acquisitionWidget->phaseRotationSpinBox->setEnabled(true);
    acquisitionWidget->FFTCheckBox->setEnabled(true);
    acquisitionWidget->digitalFilterCheckBox->setEnabled(false);
    acquisitionWidget->QDCheckBox->setEnabled(false);
    acquisitionWidget->offsetCorrectionCheckBox->setEnabled(false);
    acquisitionWidget->offsetCorrectionSpinBox->setEnabled(false);
    acquisitionWidget->defaultPushButton->setEnabled(false);
}

void TExpSettings::onAccum()
{
    variableTable->setEditable(false);
    arrayWidget->setEnabled(false);
    acquisitionWidget->setEnabled(false);
    jobLoadButton->setEnabled(false);
}

void TExpSettings::onExpFinished()
{
    variableTable->setEditable(true);
    arrayWidget->setEnabled(true);
    acquisitionWidget->setEnabled(true);
    jobLoadButton->setEnabled(true);

    acquisitionWidget->separateDataStorageCheckBox->setEnabled(true);
    acquisitionWidget->separateDataStorageSpinBox->setEnabled(true);
   // acquisitionWidget->separateDataStorageComboBox->setEnabled(true);
    acquisitionWidget->multipleAcquisitionsCheckBox->setEnabled(true);
    acquisitionWidget->inFPGAAccumCheckBox->setEnabled(true);
    //acquisitionWidget->spectralDensityAccumCheckBox->setEnabled(true);
    acquisitionWidget->phaseReverseCheckBox->setEnabled(true);
    acquisitionWidget->phaseRotationCheckBox->setEnabled(true);
    acquisitionWidget->phaseRotationSpinBox->setEnabled(true);
    acquisitionWidget->FFTCheckBox->setEnabled(true);
    acquisitionWidget->digitalFilterCheckBox->setEnabled(true);
    acquisitionWidget->QDCheckBox->setEnabled(true);
    acquisitionWidget->offsetCorrectionCheckBox->setEnabled(true);
    acquisitionWidget->offsetCorrectionSpinBox->setEnabled(true);
    acquisitionWidget->defaultPushButton->setEnabled(true);

}
