#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>

#include "addCutPointsWidget.h"
#include "processPanelWidget.h"


TAddCutPointsWidget::TAddCutPointsWidget()
{
    addCutPoints = new TAddCutPoints;
   // FID_2D = new TFID_2D;

    createWidgets();
    createPanel();
    initialize();
    createConnections();
}

TAddCutPointsWidget::~TAddCutPointsWidget()
{
    delete addCutPoints;
}


void TAddCutPointsWidget::createWidgets()
{
    headPointsSpinBox = new QSpinBox;
    tailPointsSpinBox = new QSpinBox;

    headCheckBox = new QCheckBox(tr("Click-set"));
    tailCheckBox = new QCheckBox(tr("Click-set"));

    operationSelectComboBox = new QComboBox; // select cut/leave/zero-fill/extrapolate
    headTailComboBox = new QComboBox;        // select head/tail/head&tail

    applyPushButton = new QPushButton(tr("Apply Cut/Add"));
}

void TAddCutPointsWidget::setFID2D(TFID_2D *f)
{
    //FID_2D=f;
    //headPointsSpinBox->setMaximum(f->al()-1);
    //tailPointsSpinBox->setMaximum(f->al()-1);
}

void TAddCutPointsWidget::createPanel()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;

    QGroupBox *groupBox0=new QGroupBox(tr("Cut/Add Points"));

      QGridLayout *gridLayout0 = new QGridLayout;
      gridLayout0->addWidget(new QLabel(tr("Operation")),0,0,1,1);
      gridLayout0->addWidget(operationSelectComboBox,0,1,1,2);
      gridLayout0->addWidget(new QLabel(tr("Scope")),1,0,1,1);
      gridLayout0->addWidget(headTailComboBox,1,1,1,2);
      gridLayout0->addWidget(new QLabel(tr("Head Points")),2,0,1,1);
      gridLayout0->addWidget(headPointsSpinBox,2,1,1,1);
      gridLayout0->addWidget(headCheckBox,2,2,1,1);
      gridLayout0->addWidget(new QLabel(tr("Tail Points")),3,0,1,1);
      gridLayout0->addWidget(tailPointsSpinBox,3,1,1,1);
      gridLayout0->addWidget(tailCheckBox,3,2,1,1);
      gridLayout0->addWidget(applyPushButton,4,0,1,3);

    groupBox0->setLayout(gridLayout0);

      mainLayout->addWidget(groupBox0);
      mainLayout->addStretch();
      setLayout(mainLayout);

}


void TAddCutPointsWidget::initialize()
{
  operationSelectComboBox->addItems(QStringList() << "Cut"
                                                  << "Leave"
                                                  << "Zero Fill"
                                                  << "Extrapolate"
                                    );

  headTailComboBox->addItems(QStringList() << "Head"
                                           << "Tail"
                                           << "Head and Tail"
                             );

  operationSelectComboBox->setCurrentIndex(0);
  onOperationSelectComboBoxChanged();
  headTailComboBox->setCurrentIndex(0);
  headPointsSpinBox->setMinimum(0);
  headPointsSpinBox->setMaximum(65536);
  headPointsSpinBox->setValue(0);
  tailPointsSpinBox->setMinimum(0);
  tailPointsSpinBox->setMaximum(65536);
  tailPointsSpinBox->setValue(0);
  addCutPoints->setHeadPoints(0);
  addCutPoints->setTailPoints(0);

  headCheckBox->setChecked(false);
  tailCheckBox->setChecked(false);

  onOperationSelectComboBoxChanged();
  onHeadTailComboBoxChanged();
}

void TAddCutPointsWidget::createConnections()
{
  connect(operationSelectComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(onOperationSelectComboBoxChanged()));
  connect(headTailComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(onHeadTailComboBoxChanged()));
  connect(headPointsSpinBox,SIGNAL(valueChanged(int)),this,SLOT(setHeadPoints(int)));
  connect(tailPointsSpinBox,SIGNAL(valueChanged(int)),this,SLOT(setTailPoints(int)));
  connect(headCheckBox,SIGNAL(toggled(bool)),this,SLOT(clickSetHeadPoints()));
  connect(tailCheckBox,SIGNAL(toggled(bool)),this,SLOT(clickSetTailPoints()));
  connect(applyPushButton,SIGNAL(clicked(bool)),this,SLOT(onApplyButtonClicked()));
//  connect(applyPushButton,SIGNAL(clicked(bool)),this,SLOT(performAddCutPoints()));

}

void TAddCutPointsWidget::onHeadTailComboBoxChanged()
{
    switch(headTailComboBox->currentIndex())
    {
    case TAddCutPoints::Head:
        addCutPoints->setHeadTail(TAddCutPoints::Head);
        break;
    case TAddCutPoints::Tail:
        addCutPoints->setHeadTail(TAddCutPoints::Tail);
        break;
    case TAddCutPoints::Both:
        addCutPoints->setHeadTail(TAddCutPoints::Both);
        break;
    default:
        addCutPoints->setHeadTail(TAddCutPoints::headTailUndefined);
        break;

    }
}

void TAddCutPointsWidget::onOperationSelectComboBoxChanged()
{

   switch(operationSelectComboBox->currentIndex())
   {
   case TAddCutPoints::Cut:
       addCutPoints->setOperation(TAddCutPoints::Cut);
       headTailComboBox->setItemText(2,"Head and Tail");
       break;
   case TAddCutPoints::Leave:
       addCutPoints->setOperation(TAddCutPoints::Leave);
       headTailComboBox->setItemText(2,"Middle");
       break;
   case TAddCutPoints::ZeroFill:
       addCutPoints->setOperation(TAddCutPoints::ZeroFill);
       headTailComboBox->setItemText(2,"Head and Tail");
       break;
   case TAddCutPoints::Extrapolate:
       addCutPoints->setOperation(TAddCutPoints::Extrapolate);
       headTailComboBox->setItemText(2,"Head and Tail");
       break;
   default:
       addCutPoints->setOperation(TAddCutPoints::operationUndefined);
       break;

   }

}

void TAddCutPointsWidget::setHeadPoints(int p)
{
    headPointsSpinBox->setValue(p);
    addCutPoints->setHeadPoints(p);
}

void TAddCutPointsWidget::setTailPoints(int p)
{
    tailPointsSpinBox->setValue(p);
    addCutPoints->setTailPoints(p);
}

void TAddCutPointsWidget::setResidualTailPoints(int p)
{
    int q;
    if(!isAncestorDefined()) q=0;
    else
    {
        q=ancestor()->FID_2D->FID.at(ancestor()->FID_2D->currentFID())->al()-p;
    }
    setTailPoints(q);
}

void TAddCutPointsWidget::clickSetHeadPoints()
{
    if(headCheckBox->isChecked())
    {
      if(tailCheckBox->isChecked()) tailCheckBox->setChecked(false);
    }

    if(!isAncestorDefined()) return;
    if(ancestor()->FID_2D->FID.isEmpty()) return;
    if(headCheckBox->isChecked())
    {
      for(int k=0; k<ancestor()->plotters->FIDPlotters.size(); k++)
      {
        connect(ancestor()->plotters->FIDPlotters[k]->plotter,SIGNAL(clickedXPosition(int)),
                this,SLOT(setHeadPoints(int)));
      }
    }
    else
    {
      for(int k=0; k<ancestor()->plotters->FIDPlotters.size(); k++)
      {
         disconnect(ancestor()->plotters->FIDPlotters[k]->plotter,SIGNAL(clickedXPosition(int)),
                  this,SLOT(setHeadPoints(int)));
      }
    }

}

void TAddCutPointsWidget::clickSetTailPoints()
{
    if(tailCheckBox->isChecked())
    {
      if(headCheckBox->isChecked()) headCheckBox->setChecked(false);
    }
    if(!isAncestorDefined()) return;
    if(ancestor()->FID_2D->FID.isEmpty()) return;
    if(tailCheckBox->isChecked())
    {
      for(int k=0; k<ancestor()->plotters->FIDPlotters.size(); k++)
      {
        connect(ancestor()->plotters->FIDPlotters[k]->plotter,SIGNAL(clickedXPosition(int)),
                this,SLOT(setResidualTailPoints(int)));
      }
    }
    else
    {
      for(int k=0; k<ancestor()->plotters->FIDPlotters.size(); k++)
      {
         disconnect(ancestor()->plotters->FIDPlotters[k]->plotter,SIGNAL(clickedXPosition(int)),
                  this,SLOT(setResidualTailPoints(int)));
      }
    }

}

void TAddCutPointsWidget::cut()
{

}
void TAddCutPointsWidget::zeroFill()
{

}
void TAddCutPointsWidget::extrapolate()
{

}

void TAddCutPointsWidget::onApplyButtonClicked()
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

    if(!performOperation())
    // in case of error...
    {
        QMessageBox::warning(this,tr(""), addCutPoints->errorMessage);
        return;
    }

    // With success, we add the operation to the operation list.
    addOperation();
}

bool TAddCutPointsWidget::refresh()
{
    onOperationSelectComboBoxChanged();
    onHeadTailComboBoxChanged();
    return performOperation();
}

void TAddCutPointsWidget::addOperation()
{
    if(!isAncestorDefined()) return;
    if(ancestor()->FID_2D->FID.isEmpty()) return;
    TAddCutPoints *addCut = new TAddCutPoints;
      addCut->setHeadTail(TAddCutPoints::Head);
    ancestor()->processOperations->processElements.append(addCut);
    // common settings
    ancestor()->updateProcessSettings();
    // specific settings
    createSettings(ancestor()->processSettings,
                   QString::number(ancestor()->processOperations->processElements.size()-1));

    return;
}

bool TAddCutPointsWidget::performOperation()
{
    //qDebug() << QString(Q_FUNC_INFO) << "1" << FID_2D->FID.at(0)->al();

    if(!addCutPoints->process(ancestor()->FID_2D))
    {
        setMessage(addCutPoints->errorMessage);
        return false;
    }
   // qDebug() << QString(Q_FUNC_INFO) << "2" << FID_2D->FID.at(0)->al();

    for(int k=0; k<ancestor()->plotters->FIDPlotters.size(); k++)
    {
        int c=ancestor()->plotters->FIDPlotters[k]->FIDSelectSpinBox->value()-1; // current FID for k-th FIDPlotter
        int f=ancestor()->FID_2D->FID.at(c)->al()-1;
        ancestor()->plotters->FIDPlotters[k]->plotter->updatePlotRange(0,f);
    }
    //ancestor()->plotters->update();
    ancestor()->refresh();
  //  qDebug() << QString(Q_FUNC_INFO) << "2";

    return true;

}

void TAddCutPointsWidget::createSettings(QSettings *settings, QString section)
{
    settings->beginGroup(section);
      settings->setValue("type",operationSelectComboBox->currentIndex());
      settings->setValue("type_text",operationSelectComboBox->currentText());
      settings->setValue("headTail",headTailComboBox->currentIndex());
      settings->setValue("headTail_text",headTailComboBox->currentText());
      settings->setValue("headPoints",headPointsSpinBox->value());
      settings->setValue("tailPoints",tailPointsSpinBox->value());
    settings->endGroup();
    settings->sync();

}

void TAddCutPointsWidget::readSettings(QSettings *settings, QString section)
{
    settings->beginGroup(section);
      int k;
      k=settings->value("type",0).toInt();
      if(k>operationSelectComboBox->count()-1) k=0;
      operationSelectComboBox->setCurrentIndex(k);
      k=settings->value("headTail",0).toInt();
      if(k>headTailComboBox->count()-1) k=0;
      headTailComboBox->setCurrentIndex(k);
      headPointsSpinBox->setValue(settings->value("headPoints",0).toInt());
      tailPointsSpinBox->setValue(settings->value("tailPoints",0).toInt());
    settings->endGroup();
//    refresh();

}
