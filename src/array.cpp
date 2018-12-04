
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QStackedLayout>
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QTableWidget>
#include <QGroupBox>
#include <QThread>
#include "array.h"

//--------------------------------------------------------------------------------
TArrayWidget::TArrayWidget(QWidget *parent) :
    QWidget(parent)
{

    ppgSetted=false;
    messageTextEdit = new QPlainTextEdit;
    messageTextEdit->document()->setMaximumBlockCount(400);

    iniEdit = new QLineEdit;
    incEdit = new QLineEdit;
    nEdit = new QLineEdit;

    arrayVariableComboBox = new QComboBox;
    arrayTypeComboBox = new QComboBox;
    arrayTypeComboBox->addItem(tr("Linear"));
    arrayTypeComboBox->addItem(tr("Random"));
    arrayTypeComboBox->setCurrentIndex(0);
    arrayDimensionComboBox = new QComboBox;
    addButton = new QPushButton(tr("Add"));
    deleteButton =new QPushButton(tr("Delete"));
    deleteButton->setEnabled(false);
    requestButton = new QPushButton(tr("Request Array"));
    requestButton->setEnabled(false);
    arrayCheckBox = new QCheckBox(tr("Array Experiment"));
    arrayCheckBox->setEnabled(false);


    QGridLayout *leftLayout = new QGridLayout; leftLayout->setMargin(0);
    QVBoxLayout *rightLayout = new QVBoxLayout; rightLayout->setMargin(0);
    QWidget *widget0 = new QWidget;
    QStackedLayout *leftStackedLayout = new QStackedLayout; leftStackedLayout->setMargin(0);

    widget0->setLayout(leftStackedLayout);

    QVBoxLayout *linearArrayLayout = new QVBoxLayout;
    linearArrayLayout->setMargin(10); linearArrayLayout->setSpacing(0);

   // linearArrayLayout->addWidget(new QLabel(tr("Variable")));
   // linearArrayLayout->addWidget(arrayVariableComboBox);
   // linearArrayLayout->addWidget(new QLabel(tr("Array Type")));
   // linearArrayLayout->addWidget(arrayTypeComboBox);
   // linearArrayLayout->addSpacing(20);

      linearArrayLayout->addWidget(new QLabel(tr("Initial value")));
      QHBoxLayout *lo1= new QHBoxLayout;
      lo1->addWidget(iniEdit);
      iniUnitLabel = new QLabel(tr(""));
      lo1->addWidget(iniUnitLabel);
      linearArrayLayout->addLayout(lo1);
      linearArrayLayout->addWidget(new QLabel(tr("Increment")));
      QHBoxLayout *lo2= new QHBoxLayout;
      lo2->addWidget(incEdit);
      incUnitLabel = new QLabel("");
      lo2->addWidget(incUnitLabel);
      linearArrayLayout->addLayout(lo2);
      linearArrayLayout->addWidget(new QLabel(tr("Length")));
      linearArrayLayout->addWidget(nEdit);
      linearArrayLayout->addStretch();

  //    linearArrayLayout->addWidget(new QLabel(tr("Order")));
  //    linearArrayLayout->addWidget(arrayDimensionComboBox);

    QGridLayout *randomArrayLayout = new QGridLayout; randomArrayLayout->setMargin(5);
      randomArrayTextEdit = new QPlainTextEdit;
      randomArrayLayout->addWidget(randomArrayTextEdit,0,0,1,1);

    QWidget *widget1 = new QWidget; widget1->setLayout(linearArrayLayout);
    QWidget *widget2 = new QWidget; widget2->setLayout(randomArrayLayout);
    leftStackedLayout->addWidget(widget1);
    leftStackedLayout->addWidget(widget2);


    leftLayout->addWidget(new QLabel(tr("Variable")),0,0,1,1);
    leftLayout->addWidget(arrayVariableComboBox,0,1,1,1);
    leftLayout->addWidget(new QLabel(tr("Array type")),1,0,1,1);
    leftLayout->addWidget(arrayTypeComboBox,1,1,1,1);
    leftLayout->addWidget(widget0,2,0,1,2);
    leftLayout->addWidget(new QLabel(tr("Order")),3,0,1,1);
    leftLayout->addWidget(arrayDimensionComboBox,3,1,1,1);

      QWidget *widget3=new QWidget;
      QHBoxLayout *bl=new QHBoxLayout(widget3);
      bl->addWidget(addButton);
      bl->addWidget(deleteButton);
    leftLayout->addWidget(widget3,4,0,1,2);


    arrayTableWidget = new QTableWidget;
//#if defined(_WIN32)
//arrayTableWidget->setFont(QFont("Consolas",9));
//#endif
    arrayTableWidget->setRowCount(0);
    arrayTableWidget->setColumnCount(3);
    arrayTableWidget->setHorizontalHeaderLabels(
                QStringList() << tr("Variable") << tr("Order") << tr("Description") );
    arrayTableWidget->setColumnWidth(0,6*QFontMetrics(font()).width(QChar('M')));
    arrayTableWidget->setColumnWidth(1,4*QFontMetrics(font()).width(QChar('M')));
    arrayTableWidget->setColumnWidth(2,20*QFontMetrics(font()).width(QChar('M')));

    QWidget *widget4=new QWidget;
    QHBoxLayout *bl2=new QHBoxLayout(widget4);
    bl2->setMargin(0);
    bl2->addWidget(requestButton);
    bl2->addWidget(arrayCheckBox);

    rightLayout->addWidget(arrayTableWidget,4);
    rightLayout->addWidget(widget4,0);
    rightLayout->addWidget(messageTextEdit,1);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setMargin(5);
    mainLayout->addLayout(leftLayout,2);
    mainLayout->addLayout(rightLayout,4);
    setLayout(mainLayout);

    connect(arrayTypeComboBox,SIGNAL(currentIndexChanged(int)),leftStackedLayout,SLOT(setCurrentIndex(int)));
    connect(addButton,SIGNAL(clicked()),this,SLOT(onAddButtonClicked()));
    connect(deleteButton,SIGNAL(clicked()),this,SLOT(onDeleteButtonClicked()));
    connect(arrayTableWidget,SIGNAL(itemSelectionChanged()),this,SLOT(onItemSelected()));
    connect(arrayVariableComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(onVariableSelected()));

    connect(requestButton,SIGNAL(clicked()),this,SLOT(onRequestButtonClicked()));

    connect(arrayCheckBox,SIGNAL(toggled(bool)),this,SLOT(onArrayCheckBoxClicked(bool)));

}
//--------------------------------------------------------------------------------
void TArrayWidget::onArrayCheckBoxClicked(bool checked)
{
    if(!checked)
    {
        arrayCheckBox->setEnabled(false);
        emit setAR(1);
        emit modified();
    }
}
//--------------------------------------------------------------------------------
void TArrayWidget::initialSetup(TpulseProgram *ppg)
{

    setPPG(ppg);
    arrayCounter.clear();

    while(arrayTableWidget->rowCount()>0) arrayTableWidget->removeRow(0);
    arrayVariableComboBox->clear();
    arrayVariableComboBox->addItem(tr("Select..."));
    for(int k=0; k<ppg->variables.size(); k++)
    {
        if(!ppg->variables.at(k)->isConst()
              &&  0!=QString::compare(ppg->variables.at(k)->name(),"ACQTIME",Qt::CaseInsensitive)
              &&  0!=QString::compare(ppg->variables.at(k)->name(),"AL",Qt::CaseInsensitive)
              &&  0!=QString::compare(ppg->variables.at(k)->name(),"DW",Qt::CaseInsensitive)
                )
        arrayVariableComboBox->addItem(ppg->variables.at(k)->name());

    }

    arrayDimensionComboBox->clear();
    arrayDimensionComboBox->addItem(QString::number(1));

    arrayCheckBox->setChecked(false);
    arrayCheckBox->setEnabled(false);

    connect(arrayVariableComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(prepareArray(int)));

}
//--------------------------------------------------------------------------------
void TArrayWidget::prepareArray(int vIndex)
{
    if(vIndex==0) // "no item selected!"
    {
        return;
    }

    QString vName=arrayVariableComboBox->itemText(vIndex);
    int p = ppg->variableIndex(vName);
    if(p<0) return;

    iniUnitLabel->setText(ppg->variables.at(p)->unitString());
    incUnitLabel->setText(ppg->variables.at(p)->unitString());

    iniEdit->setText(ppg->variables.at(p)->numericString());
    incEdit->clear();

}
//--------------------------------------------------------------------------------
void TArrayWidget::onAddButtonClicked()
{
    if(arrayVariableComboBox->currentIndex()==0) // no select?
    {
      if(0==QString::compare(arrayVariableComboBox->itemText(0),"Select...",Qt::CaseInsensitive))
      {
        messageTextEdit->insertPlainText("Variable is not selected.\n");
        messageTextEdit->moveCursor(QTextCursor::End);
        return;
      }
    }
    if(!ppgSetted)
    {
        messageTextEdit->insertPlainText("Array is not available.\n");
        messageTextEdit->moveCursor(QTextCursor::End);
        return;
    }
    if(!ppg->compiled)
    {
        messageTextEdit->insertPlainText("Pulse program has not been complied yet.\n");
        messageTextEdit->moveCursor(QTextCursor::End);
        return;
    }

//    bool ok;
    int i=ppg->variableIndex(arrayVariableComboBox->currentText());
    if(i<0)
    {
        messageTextEdit->insertPlainText("Variable was not found.\n");
        messageTextEdit->moveCursor(QTextCursor::End);
        return;
    }


    if(arrayTypeComboBox->currentIndex()==0)// Linear
    {
        if(!setLinearArray(ppg->variables[i])) return;
    }
    else if(arrayTypeComboBox->currentIndex()==1) //
    {
        if(!setRandomArray(ppg->variables[i])) return;
    }

    ppg->variables[i]->setArrayDimension(arrayDimensionComboBox->currentIndex()+1);
    addArray(ppg->variables[i]);
    refresh();
}
//--------------------------------------------------------------------------------
bool TArrayWidget::setLinearArray(TVariable *variable)
{
    variable->setArrayMode(TVariable::LinearArray);

    if(iniEdit->text().trimmed().isEmpty() || incEdit->text().trimmed().isEmpty()
          || nEdit->text().trimmed().isEmpty())
    {
      messageTextEdit->insertPlainText("Item(s) is(are) empty.\n");
      messageTextEdit->moveCursor(QTextCursor::End);
      return false;
    }

    bool ok;
    variable->setIniValue(iniEdit->text(),&ok);
    if(!ok)
    {
      messageTextEdit->insertPlainText("Invalid expression (initial value): " + iniEdit->text() + "\n");
      messageTextEdit->moveCursor(QTextCursor::End);
      return false;
    }

    variable->setIncValue(incEdit->text(),&ok);
    if(!ok)
    {
      messageTextEdit->insertPlainText("Invalid expression (increment): " + incEdit->text() + "\n");
      messageTextEdit->moveCursor(QTextCursor::End);
      return false;
    }

    int n=nEdit->text().toInt(&ok);
    if(!ok)
    {
      messageTextEdit->insertPlainText("Invalid number (Length): " + nEdit->text() + "\n");
      messageTextEdit->moveCursor(QTextCursor::End);
      return false;
    }

    if(n<2)
    {
      messageTextEdit->insertPlainText("Length (" + nEdit->text() + ") must be larger than 1.\n");
      messageTextEdit->moveCursor(QTextCursor::End);
      return false;
    }

    variable->setArrayCount(n);

    for(int k=0; k<arrayTableWidget->rowCount(); k++)
    {
        QString vn=arrayTableWidget->item(k,0)->text();
        int vi=ppg->variableIndex(vn);
        if(ppg->variables.at(vi)->arrayDimension()==arrayDimensionComboBox->currentIndex()+1)
        {
            if(ppg->variables.at(vi)->arrayMode()==TVariable::LinearArray)
            {
              ppg->variables[vi]->setArrayCount(n);
            }
            else if(ppg->variables.at(vi)->arrayMode()==TVariable::RandomArray
                    && ppg->variables.at(vi)->arrayCount()!=n)
            {
                messageTextEdit->insertPlainText("Length (" + nEdit->text() + ") does not match "
                                                        +  "with that of " + ppg->variables.at(vi)->name() + ".\n");
                messageTextEdit->moveCursor(QTextCursor::End);
                return false;

            }
        }
    }//k

    return true;
}


bool TArrayWidget::setRandomArray(TVariable *variable)
{
    variable->setArrayMode(TVariable::RandomArray);

    QString s=randomArrayTextEdit->toPlainText().trimmed();
    QStringList sl=s.split(QRegExp("\\s+"));

    if(!variable->setRandomArrayValues(sl))
    {
        messageTextEdit->insertPlainText(variable->errorMessage());
        messageTextEdit->moveCursor(QTextCursor::End);
        return false;
    }

    int n=sl.size();


    for(int k=0; k<arrayTableWidget->rowCount(); k++)
    {
        QString vn=arrayTableWidget->item(k,0)->text();
        int vi=ppg->variableIndex(vn);
        if(ppg->variables.at(vi)->arrayDimension()==arrayDimensionComboBox->currentIndex()+1)
        {
            if(ppg->variables.at(vi)->arrayMode()==TVariable::LinearArray)
            {
              ppg->variables[vi]->setArrayCount(n);
            }
            else if(ppg->variables.at(vi)->arrayMode()==TVariable::RandomArray
                    && ppg->variables.at(vi)->arrayCount()!=n)
            {
                messageTextEdit->insertPlainText("Length (" + nEdit->text() + ") does not match "
                                                        +  "with that of " + ppg->variables.at(vi)->name() + ".\n");
                messageTextEdit->moveCursor(QTextCursor::End);
                return false;

            }
        }
    }//k

   randomArrayTextEdit->clear();
   return true;
}
//--------------------------------------------------------------------------------
void TArrayWidget::addArray(TVariable *variable)
{
    int row=variableIndex(variable->name());
    if(row<0) // new array variable?
    {
      row=arrayTableWidget->rowCount();
      arrayTableWidget->insertRow(row);

    #if defined(__APPLE__)
      arrayTableWidget->setRowHeight(row,QFontMetrics(font()).height());
    #elif defined(_WIN32)
      arrayTableWidget->setRowHeight(row,1.5*QFontMetrics(font()).height());
    #endif

      QTableWidgetItem *item0 = new QTableWidgetItem;
      item0->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
      item0->setFlags(item0->flags() & ~Qt::ItemIsEditable);
      arrayTableWidget->setItem(row,0,item0);

      QTableWidgetItem *item1 = new QTableWidgetItem;
      item1->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
      item1->setFlags(item1->flags() & ~Qt::ItemIsEditable);
      arrayTableWidget->setItem(row,1,item1);

      QTableWidgetItem *item2 = new QTableWidgetItem;
      item2->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
      item2->setFlags(item2->flags() & ~Qt::ItemIsEditable);
      arrayTableWidget->setItem(row,2,item2);
    }
    //

    arrayTableWidget->item(row,0)->setText(variable->name());
    arrayTableWidget->item(row,1)->setText(QString::number(variable->arrayDimension()));
    arrayTableWidget->item(row,2)->setText(variable->arrayInfo());

    int n=variable->arrayCount();

    if(arrayCounter.NOfCounts.size()<arrayDimensionComboBox->currentIndex()+1)
      arrayCounter.append(n);
    else
    {
      arrayCounter.NOfCounts[arrayDimensionComboBox->currentIndex()]=n;
      for(int k=0; k<arrayTableWidget->rowCount(); k++)
      {
          QString vn=arrayTableWidget->item(k,0)->text();
          int vi=ppg->variableIndex(vn);
          if(ppg->variables.at(vi)->arrayDimension()==arrayDimensionComboBox->currentIndex()+1)
          {
              ppg->variables[vi]->setArrayCount(n);
              arrayTableWidget->item(k,2)->setText(ppg->variables[vi]->arrayInfo());
          }

        //  qDebug() << "arrayCount "<< vn << " " << ppg->variables[vi]->arrayCount();

      }//k
    }
}
//--------------------------------------------------------------------------------
//void TArrayWidget::setEditable(bool b)
//{
//   for(int k=0; k<arrayTableWidget->rowCount(); k++)
//   if(!b)
//   {

//   }
//}

//--------------------------------------------------------------------------------
int TArrayWidget::variableIndex(QString v)
{
    int p=-1;
    for(int k=0; k<arrayTableWidget->rowCount(); k++)
    {
        if(0==QString::compare(arrayTableWidget->item(k,0)->text(),v,Qt::CaseInsensitive))
        {
            p=k;
            break;
        }
    }

    return p;
}
//--------------------------------------------------------------------------------
void TArrayWidget::onItemSelected()
{
    int cur=arrayTableWidget->currentRow();
    if(cur<0) return;

    arrayTableWidget->selectRow(cur);
    QString vn=arrayTableWidget->item(cur,0)->text();
    int vi=ppg->variableIndex(vn);

    arrayVariableComboBox->setCurrentIndex(0);
    arrayVariableComboBox->setItemText(0, ppg->variables.at(vi)->name());

    if(ppg->variables.at(vi)->arrayMode()==TVariable::LinearArray)
    {
        arrayTypeComboBox->setCurrentIndex(0);
        iniEdit->setText(ppg->variables.at(vi)->iniNumericString());
        incEdit->setText(ppg->variables.at(vi)->incNumericString());
        nEdit->setText(QString::number(ppg->variables.at(vi)->arrayCount()));
    }
    else  // Random array
    {
        arrayTypeComboBox->setCurrentIndex(1);
        randomArrayTextEdit->setPlainText(ppg->variables.at(vi)->arrayInfo());

    }

    arrayDimensionComboBox->setCurrentIndex(ppg->variables.at(vi)->arrayDimension()-1);

    deleteButton->setEnabled(true);
    addButton->setText(tr("Change"));

}
//--------------------------------------------------------------------------------
void TArrayWidget::onDeleteButtonClicked()
{
    int p=arrayTableWidget->currentRow();
    if (p<0) return;

    int vIndex=ppg->variableIndex(arrayTableWidget->item(p,0)->text());
    ppg->variables[vIndex]->setArrayDimension(0);
    arrayTableWidget->removeRow(p);

    refresh();

}
//--------------------------------------------------------------------------------
void TArrayWidget::onVariableSelected()
{
    iniEdit->clear(); incEdit->clear(); nEdit->clear();
    addButton->setText(tr("Add"));
    deleteButton->setEnabled(false);
    arrayVariableComboBox->setItemText(0,tr("Select..."));
}
//--------------------------------------------------------------------------------
void TArrayWidget::refresh()
{
    if(!ppg->compiled) return;

    arrayVariableComboBox->clear();
    arrayVariableComboBox->addItem(tr("Select..."));
    iniEdit->clear(); incEdit->clear(); nEdit->clear();

    for(int k=0; k<ppg->variables.size(); k++)
    {

//        qDebug()<< arrayCounter.ppg->variables[k]->name()
//                 << ": " <<arrayCounter.ppg->variables[k]->numericString();

        if(!ppg->variables.at(k)->isConst()
             && 0!=QString::compare(ppg->variables.at(k)->name(),"ACQTIME",Qt::CaseInsensitive)
             && ppg->variables.at(k)->arrayDimension()==0)
        arrayVariableComboBox->addItem(ppg->variables.at(k)->name());
    }

    arrayCounter.clear();
    for(int k=0; k<arrayTableWidget->rowCount(); k++)
    {
       int vi=ppg->variableIndex(arrayTableWidget->item(k,0)->text());
       if (vi<0)
       {
          messageTextEdit->insertPlainText("Invalid index.\n");
          messageTextEdit->moveCursor(QTextCursor::End);
          return;
       }

       while(arrayCounter.NOfCounts.size()<ppg->variables.at(vi)->arrayDimension())
          arrayCounter.append(0);

       arrayCounter.NOfCounts[ppg->variables.at(vi)->arrayDimension()-1]
         = ppg->variables.at(vi)->arrayCount();
    }


    arrayDimensionComboBox->clear();
    for(int k=0; k<=arrayCounter.NOfCounts.size(); k++) arrayDimensionComboBox->addItem(QString::number(k+1));
    arrayDimensionComboBox->setCurrentIndex(0);

    deleteButton->setEnabled(false);
    addButton->setText(tr("Add"));

    if(arrayTableWidget->rowCount()>0)
    {
        requestButton->setEnabled(true);
    }
    else
    {
        requestButton->setEnabled(false);
    }

    arrayCheckBox->setChecked(false);
    arrayCheckBox->setEnabled(false);

    randomArrayTextEdit->clear();
    messageTextEdit->clear();
}
//--------------------------------------------------------------------------------
void TArrayWidget::onRequestButtonClicked()
{
    if(arrayTableWidget->rowCount()<1) return;
    messageTextEdit->clear();
    messageTextEdit->insertPlainText("Checking variables... ");
    messageTextEdit->moveCursor(QTextCursor::End);

    arrayCounter.init(ppg);
    ppg->errorCheckOnly=true;

    bool ok=true;

    QProgressDialog *progressDialog=new QProgressDialog("Checking array variables...", "Cancel", 0 ,arrayCounter.arrayCount());
    progressDialog->setWindowModality(Qt::WindowModal);
    progressDialog->setMinimumDuration(100);
    int count=0;

    while(!arrayCounter.hasFinished)
    {
        progressDialog->setValue(count);

        messageTextEdit->clear();
        messageTextEdit->insertPlainText("Checking variables... " + arrayCounter.currentCountString());
        messageTextEdit->moveCursor(QTextCursor::End);

        ok=ppg->processMainPPG();
        if(ok) ok=ppg->processAsyncPPG();
        if(!ok) {arrayCounter.init(ppg); break;}

        arrayCounter.increment(ppg);

        if(progressDialog->wasCanceled())
        {
            messageTextEdit->insertPlainText(" Array setup was cancelled.\n");
            messageTextEdit->moveCursor(QTextCursor::End);
            arrayCheckBox->setEnabled(false);
            arrayCheckBox->setChecked(false);
            return;
        }
        count++;
    }

    delete progressDialog;

    ppg->errorCheckOnly=false;

    if(!ok)
    {
        messageTextEdit->insertPlainText(" Error. " + ppg->errorMessage+"\n");
        messageTextEdit->moveCursor(QTextCursor::End);
        arrayCheckBox->setEnabled(false);
        arrayCheckBox->setChecked(false);
    }
    else
    {
        messageTextEdit->insertPlainText("\nOk. We are ready for the array experiment.");
        messageTextEdit->moveCursor(QTextCursor::End);
        arrayCheckBox->setEnabled(true);
        arrayCheckBox->setChecked(true);

        emit setAR(arrayCounter.arrayCount());
        emit modified();
    }

}
//--------------------------------------------------------------------------------
QStringList TArrayWidget::arrayScript()
{

    QStringList sl;
    sl.clear();
    if(!ppgSetted) return sl;
    if(!ppg->compiled) return sl;

    if(arrayCounter.NOfCounts.size()<1) return sl;

    sl.append("[Array]");
    sl.append("dimensions=" + QString::number(arrayCounter.NOfCounts.size()));
    for(int k=0; k<arrayCounter.NOfCounts.size(); k++)
    {
      sl.append("arrayCount"+QString::number(k+1)+ "=" + QString::number(arrayCounter.NOfCounts.at(k)));
    }

    for(int a=0; a<ppg->variables.size();a++)
    {
        if(ppg->variables.at(a)->arrayDimension()>0)  // update
        {
          sl.append("[" + ppg->variables.at(a)->name() + "]");
          sl.append("dimension=" + QString::number(ppg->variables.at(a)->arrayDimension()));
          sl.append("mode="+ppg->variables.at(a)->arrayModeString());
          if(ppg->variables.at(a)->arrayMode()==TVariable::LinearArray)
          {
            sl.append("initial=" + ppg->variables.at(a)->iniNumericString());
            sl.append("increment=" + ppg->variables.at(a)->incNumericString());
          }
          else if(ppg->variables.at(a)->arrayMode()==TVariable::RandomArray)
          {
            sl.append(ppg->variables.at(a)->randomArrayNumericString());
          }
        }
    } // for(a)

    return sl;
}
