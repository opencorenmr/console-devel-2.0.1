#include "toggleParams.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QDebug>


TToggleParams::TToggleParams(QWidget *parent) : QWidget(parent)
{
    this->hide();
    this->setWindowTitle(tr("Toggle Parameters"));
    this->setWindowFlags(Qt::Dialog);
    this->setFixedWidth(400);
    this->setFixedHeight(300);

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




}

void TToggleParams::createPanel()
{

    QHBoxLayout *hLayout1 = new QHBoxLayout; hLayout1->setContentsMargins(0,0,0,0);
    QHBoxLayout *hLayout2 = new QHBoxLayout; hLayout2->setContentsMargins(0,0,0,0);
    QVBoxLayout *vLayout1 = new QVBoxLayout; vLayout1->setContentsMargins(0,0,0,0);
    QGridLayout *gLayout1 = new QGridLayout; gLayout1->setContentsMargins(0,0,0,0);
    QGridLayout *gLayout2 = new QGridLayout; gLayout2->setContentsMargins(0,0,0,0);


    hLayout1->addLayout(gLayout1);
    hLayout2->addLayout(gLayout2);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->addLayout(hLayout1);
    mainLayout->addLayout(hLayout2);
    setLayout(mainLayout);


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
