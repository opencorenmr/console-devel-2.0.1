#include <QLayout>
#include <QDebug>
#include <QtWidgets>
#include <QEventLoop>
#include "variableedit.h"

TvariableTable::TvariableTable(QWidget *parent) :
    QTableWidget(parent)
{

    setRowCount(0);
    setColumnCount(5);
    setHorizontalHeaderLabels(
                QStringList() << tr("name") << tr("type") << tr("numeric") << tr("unit") << tr("comment"));

  //  QHBoxLayout *mainLayout = new QHBoxLayout;
  //  mainLayout->addWidget(tableWidget);
  //  setLayout(mainLayout);

}

TvariableTable::~TvariableTable()
{
    setRowCount(0);
    setColumnCount(0);
  //  while (!variables.isEmpty()) delete variables.takeFirst();
}

void TvariableTable::initialize()
{
   setRowCount(0);
 //  while (!variables.isEmpty()) delete variables.takeFirst();
}

void TvariableTable::setEditable(bool b)
{
    disconnect(this,SIGNAL(itemChanged(QTableWidgetItem *)),
               this,SLOT(tryUpdateVariable(QTableWidgetItem *)));

    for(int k=0; k<rowCount(); k++)
    {
        if(!b)
        {
            item(k,2)->setFlags(item(k,2)->flags() & ~Qt::ItemIsEditable);
        }
        else
        {
            item(k,2)->setFlags(item(k,2)->flags() | Qt::ItemIsEditable);

        }
    }
    connect(this,SIGNAL(itemChanged(QTableWidgetItem *)),
               this,SLOT(tryUpdateVariable(QTableWidgetItem *)));

}

void TvariableTable::addRow(TVariable *variable)
{

    if (variable->isConst()) return;
    if(0==QString::compare(variable->name(),"ACQTIME",Qt::CaseInsensitive)) return;

    disconnect(this,SIGNAL(itemChanged(QTableWidgetItem *)),
               this,SLOT(tryUpdateVariable(QTableWidgetItem *)));

    addRow();

    item(rowCount()-1,0)->setText(variable->name());
//    item(rowCount()-1,1)->setText(TVariable::typeString(variable->type()));
    item(rowCount()-1,1)->setText(variable->typeString());
    item(rowCount()-1,2)->setText(variable->numericString());
    if(variable->unitString2()!="")
    {
        item(rowCount()-1,3)->setText(variable->unitString2());
    }
    else // no unit ---> readonly
    {
      item(rowCount()-1,3)->setFlags(item(rowCount()-1,3)->flags() & ~Qt::ItemIsEditable);
    }
    item(rowCount()-1,4)->setText(variable->caption());

/*    if(0==QString::compare(variable->name(),"ACQTIME",Qt::CaseInsensitive))
    {
        QTableWidgetItem *it = new QTableWidgetItem;
        it = item(rowCount()-1,2);
        it->setFlags(it->flags() & ~Qt::ItemIsEditable);
    }
*/
    connect(this,SIGNAL(itemChanged(QTableWidgetItem *)),
               this,SLOT(tryUpdateVariable(QTableWidgetItem *)));

}

void TvariableTable::addRow()
{
    int row = rowCount();
    insertRow(row);
#if defined(__APPLE__)
    setRowHeight(row,QFontMetrics(font()).height());
#elif defined(_WIN32)
    setRowHeight(row,1.5*QFontMetrics(font()).height());
#endif

    QTableWidgetItem *item0 = new QTableWidgetItem;
    item0->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    item0->setFlags(item0->flags() & ~Qt::ItemIsEditable);
    setItem(row,0,item0);

    QTableWidgetItem *item1 = new QTableWidgetItem;
    item1->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    item1->setFlags(item1->flags() & ~Qt::ItemIsEditable);
    setItem(row,1,item1);

    QTableWidgetItem *item2 = new QTableWidgetItem;
    item2->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    setItem(row,2,item2);

    QTableWidgetItem *item3 = new QTableWidgetItem;
    item3->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    item3->setFlags(item3->flags() & ~Qt::ItemIsEditable);
    setItem(row,3,item3);
    setColumnWidth(3,5*QFontMetrics(font()).width(QChar('M')));

    QTableWidgetItem *item4 = new QTableWidgetItem;
    item4->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    item4->setFlags(item4->flags() & ~Qt::ItemIsEditable);
    setItem(row,4,item4);
    int w=columnWidth(0)+columnWidth(1)+columnWidth(2)+columnWidth(3);
    setColumnWidth(4,width()-w);

    setCurrentItem(item0);

}
/*
int TvariableTable::varIndex(QString varName)
{
    int index=-1;
    for(int k=0; k<rowCount(); k++)
    {
        if(0==QString::compare(varName,item(k,0)->text(),Qt::CaseInsensitive))
        {index=k;}
    }
    return index;
}
*/

void TvariableTable::tryUpdateVariable(QTableWidgetItem *newItem)
{
    bool ok=true;
    QString s;
    currentRow=newItem->row(); currentColumn=newItem->column();
    int index=ppg->variableIndex(item(currentRow,0)->text());

    if(index<0) {ok=false;s="Item was not found.\n"; emit message(s); return;}

    QString orig=ppg->variables.at(index)->numericString();

    if(currentColumn==3) // unit
    {

    }
    else if(currentColumn==2) // numeric
    {
      if(newItem->text().trimmed()=="")
      {
          ok=false;
          s="Empty item is not allowed.\n";
          emit message(s);          
      }
      else
      {
        if(ppg->variables.at(index)->valueType()==TVariable::Integer)
        {
           int i=newItem->text().toInt(&ok);
           if(!ok) s="Invalid number: "+ newItem->text() + "\n";
           else ppg->variables[index]->setNumeric(QVariant(i));

           if(ppg->variables.at(index)->type()==TVariable::naVariable)
           {
               emit updateOrigNARequest(i);
               ppg->setOrigNA(i);
           }


        }
        else if(ppg->variables.at(index)->valueType()==TVariable::Double)
        {
           double d=newItem->text().toDouble(&ok);
           if(!ok) s="Invalid number: " + newItem->text() + "\n";
           else ppg->variables[index]->setNumeric(QVariant(d));
        }

        if(!ok)
        {
            QMessageBox::about(this,"",s);

            disconnect(this,SIGNAL(itemChanged(QTableWidgetItem *)),
                       this,SLOT(tryUpdateVariable(QTableWidgetItem *)));

            newItem->setText(orig);

            connect(this,SIGNAL(itemChanged(QTableWidgetItem *)),
                    this,SLOT(tryUpdateVariable(QTableWidgetItem *)));
            return;
        }

        QString separator;
        if(ppg->variables.at(index)->unitString()=="") separator=""; else separator=" ";

        s= ppg->variables.at(index)->name() + " is being updated"
            + " from " + orig + separator + ppg->variables.at(index)->unitString()
            + " to " + newItem->text() + separator + ppg->variables.at(index)->unitString() + " ... ";
        emit message(s);

        isSuccessful=ppg->updateVariable(ppg->variables.at(index)->name(),
                                           ppg->variables.at(index)->numeric(),
                                           ppg->variables.at(index)->unit());
        if(!isSuccessful)
        {
            ppg->variables[index]->undoUpdate();
            s="Failed. " + ppg->errorMessage + "\n";
            ok=false;
            QMessageBox::about(this,"",s);

            disconnect(this,SIGNAL(itemChanged(QTableWidgetItem *)),
                         this,SLOT(tryUpdateVariable(QTableWidgetItem *)));

            newItem->setText(orig);

            connect(this,SIGNAL(itemChanged(QTableWidgetItem *)),
                      this,SLOT(tryUpdateVariable(QTableWidgetItem *)));
        }
        else
        {
            ok=true;
            s="OK.\n";
            emit transferRequest(ppg->updatedPPG);
            emit updateAuxParamsRequest();
            emit modified();
            if(ppg->variables.at(index)->type()==TVariable::freqVariable)
            {
                emit freqVariableChanged(index);
            }
        }

        emit(message(s));
      }



    }

}

void TvariableTable::displayVariables()
{
    while (rowCount()>0) removeRow(0);

    for(int row=0; row<ppg->variables.size(); row++)
    {
        addRow(ppg->variables.at(row));
    }

}

QStringList TvariableTable::variableNames()
{
   QStringList sl; sl.clear();
   for(int k=0; k<ppg->variables.size(); k++)
    {
        if(!ppg->variables.at(k)->isConst()
            && 0!=QString::compare(ppg->variables.at(k)->name(),"ACQTIME",Qt::CaseInsensitive))
        {
            sl.append(ppg->variables.at(k)->name());
        }
    }
   return sl;
}

QStringList TvariableTable::variableScript()
{
    QStringList sl; sl.clear();

    for(int k=0; k<ppg->variables.size(); k++)
    {
        if(!ppg->variables.at(k)->isConst()
            && 0!=QString::compare(ppg->variables.at(k)->name(),"ACQTIME",Qt::CaseInsensitive))
        {
            sl.append(ppg->variables.at(k)->name() + " " + ppg->variables.at(k)->numericString());
        }

    }
    return sl;
}
