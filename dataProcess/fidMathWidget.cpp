#include <QLayout>
#include <QLabel>
#include <QDir>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>

#include "fidMathWidget.h"
#include "processPanelWidget.h"

TFIDMathWidget::TFIDMathWidget()
{

    FIDMath = new TFIDMath;

    createWidgets();
    createLayout();
    createConnections();
}


void TFIDMathWidget::createWidgets()
{
    applyButton = new QPushButton(tr("Apply Math Operation"));

    withComboBox = new QComboBox;
      withComboBox->addItems(QStringList() << "Number" << "File" );
    numberOperationComboBox = new QComboBox; // Options for "Number"
      numberOperationComboBox->addItems(QStringList()
                         << "Add" // 0
                         << "Subtract" // 1
                         << "Multiply" // 2
                         << "Divide" // 3
                         << "Normalize" // 4
                         << "Offset" // 5
                         << "Phase Offset" // 6
                         << "Reverse Phase" // 7
                         );
    fileOperationComboBox = new QComboBox; // Options for "File"
      fileOperationComboBox->addItems(QStringList()
                         << "Add"
                         << "Subtract"
                         << "Multiply"
                         << "Divide"
                         );
    stackedLayout = new QStackedLayout;

    withNumberStackedWidget = new QStackedWidget;
//    number1DoubleSpinBox = new QDoubleSpinBox;
//    number2DoubleSpinBox = new QDoubleSpinBox;

    number1LineEdit = new QLineEdit;
    number2LineEdit = new QLineEdit;
    xIniSpinBox = new QSpinBox;
      // xIniSpinBox->setPrefix("From ");
      xIniSpinBox->setMinimum(0);
    xFinSpinBox = new QSpinBox;
      // xFinSpinBox->setPrefix("To ");
      xFinSpinBox->setMinimum(0);

    fileNameLineEdit = new QLineEdit;
    filePathLineEdit = new QLineEdit;
    fileSelectPushButton = new QPushButton(tr("Select"));

}


void TFIDMathWidget::createLayout()
{
    QVBoxLayout *vLayout0 = new QVBoxLayout(this);
    QGridLayout *gLayout0 = new QGridLayout;

      QWidget *widget1 = new QWidget;
      QGridLayout *gLayout1 = new QGridLayout(widget1);
        gLayout1->addWidget(new QLabel(tr("Operation")),0,0,1,1);
        gLayout1->addWidget(numberOperationComboBox,0,1,1,1);
          QWidget *widget3 = new QWidget;
          QGridLayout *gLayout3 = new QGridLayout(widget3);
          gLayout3->setMargin(0);
          gLayout3->addWidget(new QLabel(tr("Real")),0,0,1,1);
          gLayout3->addWidget(number1LineEdit,0,1,1,1);
          gLayout3->addWidget(new QLabel(tr("Imag")),0,2,1,1);
          gLayout3->addWidget(number2LineEdit,0,3,1,1);

          QWidget *widget4 = new QWidget;
          QGridLayout *gLayout4 = new QGridLayout(widget4);
          gLayout4->setMargin(0);
          gLayout4->addWidget(new QLabel("Use the average of points"),0,0,1,4);
          gLayout4->addWidget(new QLabel(tr("From")),1,0,1,1);
          gLayout4->addWidget(xIniSpinBox,1,1,1,1);
          gLayout4->addWidget(new QLabel(tr("To")),1,2,1,1);
          gLayout4->addWidget(xFinSpinBox,1,3,1,1);

          QWidget *widget5 = new QWidget;

          withNumberStackedWidget->addWidget(widget3);
          withNumberStackedWidget->addWidget(widget4);
          withNumberStackedWidget->addWidget(widget5);

        gLayout1->addWidget(withNumberStackedWidget,1,0,1,2);

      QWidget *widget2 = new QWidget;
      QGridLayout *gLayout2 = new QGridLayout(widget2);
        gLayout2->setMargin(0);
        gLayout2->addWidget(fileSelectPushButton,0,0,1,1);
        gLayout2->addWidget(new QLabel(tr("Path")),1,0,1,1);
        gLayout2->addWidget(filePathLineEdit,1,1,1,2);
        gLayout2->addWidget(new QLabel(tr("File Name")),2,0,1,1);
        gLayout2->addWidget(fileNameLineEdit,2,1,1,2);
        gLayout2->addWidget(new QLabel(tr("Operation")),3,0,1,1);
        gLayout2->addWidget(fileOperationComboBox,3,1,1,1);

      stackedLayout->addWidget(widget1);
      stackedLayout->addWidget(widget2);

    gLayout0->addWidget(new QLabel(tr("Operation with")),0,0,1,1);
    gLayout0->addWidget(withComboBox,0,1,1,2);
    gLayout0->addLayout(stackedLayout,1,0,1,3);


    vLayout0->addLayout(gLayout0);
    vLayout0->addWidget(applyButton);
    vLayout0->addStretch();


}

void TFIDMathWidget::createConnections()
{
    connect(withComboBox,SIGNAL(activated(int)),stackedLayout,SLOT(setCurrentIndex(int)));
    connect(numberOperationComboBox,SIGNAL(activated(int)),this,SLOT(onNumberOperationComboBoxActivated()));
    connect(fileSelectPushButton,SIGNAL(clicked(bool)),this,SLOT(onFileSelectButtonClicked()));
    connect(applyButton,SIGNAL(clicked(bool)),this,SLOT(onApplyButtonClicked()));

}

void TFIDMathWidget::onFileSelectButtonClicked()
{
    QString path="~/";
    if(QDir(ancestor()->processFileWidget->dataFilePath()).exists())
        path=ancestor()->processFileWidget->dataFilePath()+'/';
    QString fileName=QFileDialog::getOpenFileName(this,tr("Select data"),
                                                  path,
                                                  tr("Opencore files (*.opp *.opd *.sm2p *.sm2d *.smd"));

    if(fileName.isEmpty()) return;
    filePathLineEdit->setText(QFileInfo(fileName).absolutePath());
    fileNameLineEdit->setText(QFileInfo(fileName).fileName());



}

void TFIDMathWidget::onNumberOperationComboBoxActivated()
{
    switch(numberOperationComboBox->currentIndex())
    {
        case 5:
        case 6:
          withNumberStackedWidget->setCurrentIndex(1);
          break;
        case 4:
        case 7:
          withNumberStackedWidget->setCurrentIndex(2);
          break;
        default:
          withNumberStackedWidget->setCurrentIndex(0);
    }

}


void TFIDMathWidget::onApplyButtonClicked()
{
    bool ok=true;
    QFileInfo qfi;

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

    double dr=0,di=0;
    dr=number1LineEdit->text().toDouble(&ok);
    di=number2LineEdit->text().toDouble(&ok);


    switch(withComboBox->currentIndex())
    {
      case 0: // with number
        FIDMath->setOperationWith(TFIDMath::Number);
        switch(numberOperationComboBox->currentIndex())
        {
          case 0: // Add
            FIDMath->setFIDMathOperation(TFIDMath::Add);
            dr=number1LineEdit->text().toDouble(&ok);
            di=number2LineEdit->text().toDouble(&ok);
            if(ok)
            {
              FIDMath->setNumber1(dr);
              FIDMath->setNumber2(di);
            }
            break;
          case 1: // Subtract
            FIDMath->setFIDMathOperation(TFIDMath::Subtract);
            dr=number1LineEdit->text().toDouble(&ok);
            di=number2LineEdit->text().toDouble(&ok);
            if(ok)
            {
              FIDMath->setNumber1(dr);
              FIDMath->setNumber2(di);
            }
            break;
          case 2: // Multiply
            FIDMath->setFIDMathOperation(TFIDMath::Multiply);
            dr=number1LineEdit->text().toDouble(&ok);
            di=number2LineEdit->text().toDouble(&ok);
            if(ok)
            {
              FIDMath->setNumber1(dr);
              FIDMath->setNumber2(di);
            }
            break;
          case 3: // Divide
            FIDMath->setFIDMathOperation(TFIDMath::Divide);
            dr=number1LineEdit->text().toDouble(&ok);
            di=number2LineEdit->text().toDouble(&ok);
            if(ok)
            {
              FIDMath->setNumber1(dr);
              FIDMath->setNumber2(di);
            }
            break;
          case 4: // Normalize
            FIDMath->setFIDMathOperation(TFIDMath::Normalize);
            ok=true; // we do not use number
            break;
          case 5: // CorrectOffset
            FIDMath->setFIDMathOperation(TFIDMath::CorrectOffset);
            FIDMath->setXIni(xIniSpinBox->value());
            FIDMath->setXFin(xFinSpinBox->value());
            break;
          case 6: // CorrectPhaseOffset
            FIDMath->setFIDMathOperation(TFIDMath::CorrectPhaseOffset);
            FIDMath->setXIni(xIniSpinBox->value());
            FIDMath->setXFin(xFinSpinBox->value());
            break;
          case 7: // ReversePhase
            FIDMath->setFIDMathOperation(TFIDMath::ReversePhase);
            ok=true; // we do not use number.
            break;
          default:
            QMessageBox::warning(this,tr(""),"Invalid operation.");
            return;
        }

        if(!ok)  // text -> double conversion error
        {
            QMessageBox::warning(this,tr(""),"Invalid number");
            return;
        }
        break;

      case 1: // with file

        FIDMath->setOperationWith(TFIDMath::File);
        qfi.setFile(filePathLineEdit->text(),fileNameLineEdit->text());
        if(!qfi.exists())
        {
            QMessageBox::warning(this,tr(""),"File does not exist.");
            return;
        }

       // qDebug() << QString(Q_FUNC_INFO) << qfi.suffix();

        if(0==QString::compare(qfi.suffix(),"sm2p",Qt::CaseInsensitive)
            || 0==QString::compare(qfi.suffix(),"sm2d",Qt::CaseInsensitive))
        {
            FIDMath->setFileType(TFIDMath::sm2d);
        }
        else if (0==QString::compare(qfi.suffix(),"opp",Qt::CaseInsensitive)
                 || 0==QString::compare(qfi.suffix(),"opd",Qt::CaseInsensitive))
        {
            FIDMath->setFileType(TFIDMath::opd);
            // qDebug() << QString(Q_FUNC_INFO) << "opd";
            // qDebug() << QString(Q_FUNC_INFO) << FIDMath->fileType();
        }
        else if (0==QString::compare(qfi.suffix(),"smd",Qt::CaseInsensitive))
        {
            FIDMath->setFileType(TFIDMath::smd);
        }
        else
        {
            QMessageBox::warning(this,tr(""),"Invalid file type.");
            return;
        }

        FIDMath->setFileName(qfi.absoluteFilePath());

        switch(fileOperationComboBox->currentIndex())
        {
          case 0: // Add
            FIDMath->setFIDMathOperation(TFIDMath::Add);
            break;
          case 1: // Subtract
            FIDMath->setFIDMathOperation(TFIDMath::Subtract);
            break;
          case 2: // Multiply
            FIDMath->setFIDMathOperation(TFIDMath::Multiply);
            break;
          case 3: // Divide
            FIDMath->setFIDMathOperation(TFIDMath::Divide);
            break;
          default:
            QMessageBox::warning(this,tr(""),"Invalid operation");
            return;

        }
        break;

      // case 2: // TODO: with buffer
      // case 3: // TODO: with function


      default:
        QMessageBox::warning(this,tr(""),"Invalid math type.");
        return;
    } // switch



    if(!FIDMath->process(ancestor()->FID_2D))
    {
        QMessageBox::warning(this,tr("TFIDMath error"), FIDMath->errorMessage);
        return;
    }


    // TODO: Plotter update
    ancestor()->plotters->update();

    addOperation();

}

void TFIDMathWidget::addOperation()
{
  if(!isAncestorDefined()) return;
  if(ancestor()->FID_2D->FID.isEmpty()) return;
  TFIDMath *fm = new TFIDMath;
  fm=FIDMath;

  ancestor()->processOperations->processElements.append(fm);

  ancestor()->updateProcessSettings();

  createSettings(ancestor()->processSettings,
                 QString::number(ancestor()->processOperations->processElements.size()-1));
  return;
}

void TFIDMathWidget::createSettings(QSettings *settings, QString section)
{

}

void TFIDMathWidget::readSettings(QSettings *settings, QString section)
{

}

