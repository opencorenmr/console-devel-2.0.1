#include "exportWidget.h"
#include "processPanelWidget.h"
#include "export2dp.h"
#include "fidDomain.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QDir>
#include <QDataStream>
#include <QFileDialog>
#include <QFileInfo>
#include <QString>
#include <QStringList>
#include <QMessageBox>
#include <QTextStream>
#include <QFile>

KExportWidget::KExportWidget()
{
   createWidgets();
   createPanel();
   createConnections();
}



void KExportWidget::createWidgets()
{
    exportOpFileButton = new QPushButton(tr("export .opp & .opd"));
    exportSm2FileButton = new QPushButton(tr("export .sm2p & .sm2d"));
    exportCSVButton = new QPushButton(tr("export .csv"));
    export2DButton = new QPushButton(tr("export .2dp & .2dd"));

    startf1LineEdit = new QLineEdit();
    endf1LineEdit = new QLineEdit();
    startf2LineEdit = new QLineEdit();
    endf2LineEdit = new QLineEdit();
    //exportFirstButton = new QPushButton(tr("First Array value"));
    //exportDiagButton = new QPushButton(tr("Diagonal value"));
}

void KExportWidget::createPanel()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;

    QGroupBox *groupBox0 = new QGroupBox(tr(".op & .csv"));
        QGridLayout *gridLayout0 = new QGridLayout;
        gridLayout0->addWidget(exportOpFileButton,0,0,1,1);
        gridLayout0->addWidget(exportSm2FileButton,1,0,1,1);
        gridLayout0->addWidget(exportCSVButton,2,0,1,1);
    groupBox0->setLayout(gridLayout0);

    QGroupBox *groupBox1 = new QGroupBox(tr("export 2D data"));
        QGridLayout *gridLayout1 = new QGridLayout;
        gridLayout1->addWidget(new QLabel(tr("Display Area ")),0,0,1,1);
        gridLayout1->addWidget(new QLabel(tr("F1")),1,0,1,1);
        gridLayout1->addWidget(startf1LineEdit,1,1,1,1);
        gridLayout1->addWidget(new QLabel(tr("-")),1,2,1,1);
        gridLayout1->addWidget(endf1LineEdit,1,3,1,1);
        gridLayout1->addWidget(new QLabel(tr("F2")),2,0,1,1);
        gridLayout1->addWidget(startf2LineEdit,2,1,1,1);
        gridLayout1->addWidget(new QLabel(tr("-")),2,2,1,1);
        gridLayout1->addWidget(endf2LineEdit,2,3,1,1);
        gridLayout1->addWidget(export2DButton,3,1,1,3);
    groupBox1->setLayout(gridLayout1);

    mainLayout->addWidget(groupBox0);
    mainLayout->addWidget(groupBox1);

    mainLayout->addStretch();
    setLayout(mainLayout);
}

void KExportWidget::createConnections()
{
     connect(exportOpFileButton,SIGNAL(clicked()),this,SLOT(performExportOpFile()));
     connect(exportSm2FileButton,SIGNAL(clicked()),this,SLOT(performExportSm2File()));
     connect(exportCSVButton,SIGNAL(clicked()),this,SLOT(performExportCSVFile()));
     connect(export2DButton,SIGNAL(clicked()),this,SLOT(performExport2DFile()));
     //connect(exportFirstButton,SIGNAL(clicked()),this,SLOT(performExportFirstFile()));
     //connect(exportDiagButton,SIGNAL(clicked()),this,SLOT(performExportDiagFile()));
}

void KExportWidget::performExportOpFile()
{

  if(!isAncestorDefined()) {return;}
  if(ancestor()->FID_2D->FID.isEmpty()) {return;}

  QString path="~/";
    if(QDir(dataFilePath()).exists()) path=dataFilePath()+'/';
    QString fileName=QFileDialog::getSaveFileName(this,tr("Save"),path,tr("*.opp or *.opd"));
    if(fileName.isEmpty()) {return;}

    if(!ancestor()->FID_2D->WriteopFiles(fileName))
    {
        QMessageBox::warning(this,tr(""),tr("<p>Failed to save data."));
        return;
    }
}

void KExportWidget::performExportSm2File()
{

  if(!isAncestorDefined()) {return;}
  if(ancestor()->FID_2D->FID.isEmpty()) {return;}

  QString path="~/";
    if(QDir(dataFilePath()).exists()) path=dataFilePath()+'/';
    QString fileName=QFileDialog::getSaveFileName(this,tr("Save"),path,tr("*.sm2p or *.sm2d"));
    if(fileName.isEmpty()) {return;}

    if(!ancestor()->FID_2D->Writesm2Files(fileName))
    {
        QMessageBox::warning(this,tr(""),tr("<p>Failed to save data."));
        return;
    }
}

void KExportWidget::performExportCSVFile()
{
  if(!isAncestorDefined()) {return;}
  if(ancestor()->FID_2D->FID.isEmpty()) {return;}

  QString path="~/";
    if(QDir(dataFilePath()).exists()) path=dataFilePath()+'/';
    QString fn=QFileDialog::getSaveFileName(this,tr("Save CSV"),path,tr("*.csv"));
    if (fn.isEmpty()) return;

    //-------save as csv----------
    QStringList ss;
    for(int i=0;i<ancestor()->FID_2D->FID.size();i++)
    {
        for(int j=0;j<ancestor()->FID_2D->al();j++)
        {
            if(j!=ancestor()->FID_2D->al()-1)
            {
                ss.append(QString::number(ancestor()->FID_2D->FID.at(i)->real->sig.at(j)) + ",");
            }
            else
            {
                ss.append(QString::number(ancestor()->FID_2D->FID.at(i)->real->sig.at(j)) + "\n");
            }
        }
    }

    QFile file(fn);

    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::information(this, tr("Unable to open file"),file.errorString());
        return;
    }

    QTextStream out(&file);
    foreach (QString item, ss)
    {
        out << item;
    }
    return;

}

void KExportWidget::performExport2DFile()
{
    if(!isAncestorDefined()) {return;}
    if(ancestor()->FID_2D->FID.isEmpty()) {return;}

    if(ancestor()->FID_2D->al() != ancestor()->FID_2D->FID.size()) {return;}

    QString path="~/";
    if(QDir(dataFilePath()).exists()) path=dataFilePath()+'/';
    QString fileName=QFileDialog::getSaveFileName(this,tr("Save"),path,tr("*.2DD and *.2DP"));
    if(fileName.isEmpty()) {return;}

    QFileInfo fi;
    fi.setFile(fileName);
    QString base = fi.baseName();
    QString path_1 = fi.absolutePath()+'/';
    QString data2dp = path_1+base+".2DP";
    QString data2dd = path_1+base+".2DD";

    //----export binary as *.2DD------------
    QFile file(data2dd);
    QIODevice::OpenModeFlag flag=QIODevice::WriteOnly;
    if (!file.open(flag))
    {
        ancestor()->FID_2D->errorMessage=QString(Q_FUNC_INFO)+ ": Failed to open " + data2dd;
        return;
    }

    QDataStream out(&file);
    out.setFloatingPointPrecision(QDataStream::SinglePrecision);
    out.setByteOrder(QDataStream::LittleEndian);

    for(int k=0; k<ancestor()->FID_2D->FID.size(); k++)
    {
        for(int m=0; m<ancestor()->FID_2D->FID.at(k)->al(); m++)
        {
            out << (float) ancestor()->FID_2D->FID.at(k)->real->sig.at(m);
        }
    }

    file.close();


    //-------------------------------------

    //----export contour parameter as *.2DP------------

    int sf1, ef1, sf2, ef2;

    QString s = startf1LineEdit->text();
    sf1 = s.toInt();
    s = endf1LineEdit->text();
    ef1 = s.toInt();
    s = startf2LineEdit->text();
    sf2 = s.toInt();
    s = endf2LineEdit->text();
    ef2 = s.toInt();

    if(sf1==0 || sf1 < 0){sf1=1;}
    if(sf2==0 || sf2 < 0){sf2=1;}
    if(ef1==0 || ef1 > ancestor()->FID_2D->FID.size()){ef1=ancestor()->FID_2D->FID.size();}
    if(ef2==0 || ef2 > ancestor()->FID_2D->al())      {ef2=ancestor()->FID_2D->al();}

    if(sf1<ef1 && sf2<ef2)
    {
        bool ok;
        KExport2DP *exp = new KExport2DP;
        ok=exp->process(ancestor()->FID_2D, data2dp, sf1, ef1, sf2, ef2);
        if(!ok)
        {
          delete exp;
          return;
        }
    }

}
