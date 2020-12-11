#include "toggleParams.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QSplitter>
#include <QDebug>


TToggleParams::TToggleParams(QWidget *parent) : QWidget(parent)
{
    createWidgets();
    createPanel();
    this->hide();
    this->setWindowTitle(tr("Toggle Parameters"));
    this->setWindowFlags(Qt::Dialog);
    this->setFixedWidth(500);
    this->setFixedHeight(500);

    ppgSetted=false;
}


void TToggleParams::createWidgets()
{
   paramComboBox = new QComboBox();
   paramTableWidget = new QTableWidget;
     paramTableWidget->setRowCount(0);
     paramTableWidget->setColumnCount(2);
     paramTableWidget->setHorizontalHeaderLabels(
                             QStringList() << tr("Parameter") << tr("Description") );
     paramTableWidget->setColumnWidth(0,6*QFontMetrics(font()).horizontalAdvance(QChar('M')));
     paramTableWidget->setColumnWidth(1,20*QFontMetrics(font()).horizontalAdvance(QChar('M')));

  paramPlainTextEdit = new QPlainTextEdit;

  messageTextEdit = new QPlainTextEdit;
  messageTextEdit->setReadOnly(false);

  valueLineEdit = new QLineEdit;
  addPushButton = new QPushButton(tr("Add"));
  editPushButton = new QPushButton(tr("Edit"));
  deletePuchButton = new QPushButton(tr("Delete"));
  moveUpPushButton = new QPushButton(tr("Up"));
  moveDownPushButton = new QPushButton(tr("Down"));
}

void TToggleParams::createPanel()
{

    QHBoxLayout *hLayout1 = new QHBoxLayout; hLayout1->setMargin(10);
    QHBoxLayout *hLayout2 = new QHBoxLayout; hLayout2->setMargin(0);
    QVBoxLayout *vLayout1 = new QVBoxLayout; vLayout1->setMargin(0);
    QGridLayout *gLayout1 = new QGridLayout; gLayout1->setMargin(0);
    QGridLayout *gLayout2 = new QGridLayout; gLayout2->setMargin(0);

    gLayout1->addWidget(new QLabel(tr("Parameter")),0,0,1,1);
    gLayout1->addWidget(paramComboBox,0,1,1,1);
    gLayout1->addWidget(new QLabel(tr("Value")),1,0,1,1);
//    gLayout1->addWidget(valueLineEdit,1,1,1,1);
    gLayout1->addWidget(paramPlainTextEdit,2,0,1,2);
    gLayout1->addWidget(addPushButton,3,0,1,1);
    gLayout1->addWidget(deletePuchButton,3,1,1,1);
//    gLayout1->addWidget(editPushButton,2,2,1,1);
//    gLayout1->addWidget(moveUpPushButton,3,2,1,1);
//    gLayout1->addWidget(moveDownPushButton,4,2,1,1);


    gLayout2->addWidget(messageTextEdit);

    hLayout1->setMargin(5);
    hLayout1->addLayout(gLayout1,2);
    hLayout1->addWidget(paramTableWidget,4);
    hLayout2->addLayout(gLayout2);

    QSplitter *mainSplitter = new QSplitter(Qt::Vertical,this);
    mainSplitter->setHandleWidth(1);
      QWidget *widget1 = new QWidget;
      widget1->setLayout(hLayout1);

      QWidget *widget2 = new QWidget;
      widget2->setLayout(hLayout2);
    mainSplitter->addWidget(widget1);
    mainSplitter->addWidget(widget2);

}

void TToggleParams::createConnections()
{

}


void TToggleParams::initialSetup(TpulseProgram *ppg)
{

    setPPG(ppg);

    while(paramTableWidget->rowCount()>0) paramTableWidget->removeRow(0);
    paramComboBox->clear();
    paramComboBox->addItem(tr("Select..."));
    for(int k=0; k<ppg->variables.size(); k++)
    {
        if( 0!=QString::compare(ppg->variables.at(k)->name(),"AL",Qt::CaseInsensitive)
              ||  0!=QString::compare(ppg->variables.at(k)->name(),"DW",Qt::CaseInsensitive)
          )
        paramComboBox->addItem(ppg->variables.at(k)->name());

    }
    paramComboBox->addItem("Filename");


}

//--------------------------------------------------------------------------------
void TToggleParams::refresh()
{
    if(!ppg->compiled) return;



}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void TToggleParams::onAddButtonClicked()
{
    if(paramComboBox->currentIndex()==0) // no select?
    {
      if(0==QString::compare(paramComboBox->itemText(0),"Select...",Qt::CaseInsensitive))
      {
        messageTextEdit->insertPlainText("Parameter is not selected.\n");
        messageTextEdit->moveCursor(QTextCursor::End);
        return;
      }
    }
    if(!ppgSetted)
    {
        messageTextEdit->insertPlainText("Toggle parameter is not available.\n");
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
    int i=ppg->variableIndex(paramComboBox->currentText());
    if(i<0)
    {
        messageTextEdit->insertPlainText("Parameter was not found.\n");
        messageTextEdit->moveCursor(QTextCursor::End);
        return;
    }


//    if(!setRandomArray(ppg->variables[i])) return;
//    ppg->variables[i]->setArrayDimension(arrayDimensionComboBox->currentIndex()+1);
//    addArray(ppg->variables[i]);
    refresh();

}
//--------------------------------------------------------------------------------
bool TToggleParams::setFileNameArray()
{

    QString s=paramPlainTextEdit->toPlainText().trimmed();
    QStringList sl=s.split(QRegExp("\\s+"));

    int n=sl.size();

    for(int k=0; k<paramTableWidget->rowCount(); k++)
    {
        QString vn=paramTableWidget->item(k,0)->text();
        int vi=ppg->variableIndex(vn);


    }//k

    paramPlainTextEdit->clear();

    return true;
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void TToggleParams::onDeleteButtonClicked()
{
    int p=paramTableWidget->currentRow();
    if (p<0) return;

    paramTableWidget->removeRow(p);
    refresh();
}
