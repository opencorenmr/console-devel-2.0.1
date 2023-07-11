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
#include <QDebug>
#include <QSet>

KExportWidget::KExportWidget()
{
   createWidgets();
   createPanel();
   createConnections();
}



void KExportWidget::createWidgets()
{
    //exportOpFileButton = new QPushButton(tr("export .opp & .opd"));
    //exportSm2FileButton = new QPushButton(tr("export .sm2p & .sm2d"));
    //exportCSVButton = new QPushButton(tr("export .csv"));

    arrayOptionComboBox = new QComboBox();
    arrayOptionComboBox->addItems(QStringList() << "Separate file"
                                                << "Single file, no separator"
                                                << "Single file, blank-line separator");

    xCheckBox = new QCheckBox(tr("x data"));
    xCheckBox->setChecked(true);
    inPhaseCheckBox = new QCheckBox(tr("in-phase data"));
    inPhaseCheckBox->setChecked(true);
    quadratureCheckBox = new QCheckBox(tr("quadrature data"));
    quadratureCheckBox->setChecked(true);
    absoluteCheckBox = new QCheckBox(tr("absolute data"));
    absoluteCheckBox->setChecked(true);

    exportAsciiButton = new QPushButton(tr("Export"));
    separatorCombobox= new QComboBox();
    separatorCombobox->addItems(QStringList()<<"[space]"<<",");
    separatorCombobox->setCurrentIndex(0);

    export2DButton = new QPushButton(tr("Export .2dp and .2dd for takeNMR"));

//    exportAbsButton = new QPushButton(tr("Export all abs for ImageJ"));

    startf1LineEdit = new QLineEdit();
    endf1LineEdit = new QLineEdit();
    startf2LineEdit = new QLineEdit();
    endf2LineEdit = new QLineEdit();
    contNumberLineEdit = new QLineEdit();
    contHighLineEdit = new QLineEdit();
    contLowLineEdit = new QLineEdit();
    //exportFirstButton = new QPushButton(tr("First Array value"));
    //exportDiagButton = new QPushButton(tr("Diagonal value"));
}

void KExportWidget::createPanel()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;

    QGroupBox *groupBox00 = new QGroupBox(tr("export ascii"));
        QGridLayout *gridLayout00 = new QGridLayout;
        gridLayout00->addWidget(new QLabel("Separator"));
        gridLayout00->addWidget(separatorCombobox,0,1,1,1);
        gridLayout00->addWidget(new QLabel("Arrayed data"),1,0,1,1);
        gridLayout00->addWidget(arrayOptionComboBox,1,1,1,1);

        gridLayout00->addWidget(xCheckBox,2,0,1,1);
        gridLayout00->addWidget(inPhaseCheckBox,2,1,1,1);
        gridLayout00->addWidget(quadratureCheckBox,3,1,1,1);
        gridLayout00->addWidget(absoluteCheckBox,4,1,1,1);

        gridLayout00->addWidget(exportAsciiButton,5,0,1,2);
    groupBox00->setLayout(gridLayout00);

//    QGroupBox *groupBox0 = new QGroupBox(tr("export for ImageJ"));
//        QGridLayout *gridLayout0 = new QGridLayout;
//        gridLayout0->addWidget(exportAbsButton,0,0,1,1);
//        //gridLayout0->addWidget(exportOpFileButton,0,0,1,1);
//        //gridLayout0->addWidget(exportSm2FileButton,1,0,1,1);
//        //gridLayout0->addWidget(exportCSVButton,2,0,1,1);
//    groupBox0->setLayout(gridLayout0);

    QGroupBox *groupBox1 = new QGroupBox(tr("export 2D data"));
        QGridLayout *gridLayout1 = new QGridLayout;
        gridLayout1->addWidget(new QLabel(tr("Area")),0,0,1,1);
        gridLayout1->addWidget(startf1LineEdit,0,1,1,1);
        gridLayout1->addWidget(new QLabel(tr("-")),0,2,1,1);
        gridLayout1->addWidget(endf1LineEdit,0,3,1,1);
        gridLayout1->addWidget(new QLabel(tr("Cont. Line")),1,0,1,1);
        gridLayout1->addWidget(contNumberLineEdit,1,1,1,1);
        gridLayout1->addWidget(new QLabel(tr("Cont. Range (%)")),2,0,1,1);
        gridLayout1->addWidget(contLowLineEdit,2,1,1,1);
        gridLayout1->addWidget(new QLabel(tr("to")),2,2,1,1);
        gridLayout1->addWidget(contHighLineEdit,2,3,1,1);
        gridLayout1->addWidget(export2DButton,3,0,1,4);
    groupBox1->setLayout(gridLayout1);

    mainLayout->addWidget(groupBox00);
//    mainLayout->addWidget(groupBox0);
    mainLayout->addWidget(groupBox1);

    mainLayout->addStretch();
    setLayout(mainLayout);
}

void KExportWidget::createConnections()
{
    connect(exportAsciiButton,SIGNAL(clicked()),this,SLOT(performExportAscii()));
//    connect(exportAbsButton,SIGNAL(clicked()),this,SLOT(exportAbs()));
     //connect(exportOpFileButton,SIGNAL(clicked()),this,SLOT(performExportOpFile()));
     //connect(exportSm2FileButton,SIGNAL(clicked()),this,SLOT(performExportSm2File()));
     //connect(exportCSVButton,SIGNAL(clicked()),this,SLOT(performExportCSVFile()));
     connect(export2DButton,SIGNAL(clicked()),this,SLOT(performExport2DFile()));
     //connect(exportFirstButton,SIGNAL(clicked()),this,SLOT(performExportFirstFile()));
     //connect(exportDiagButton,SIGNAL(clicked()),this,SLOT(performExportDiagFile()));
}


void KExportWidget::performExportAscii()
{

    if(!isAncestorDefined()) {return;}
    if(ancestor()->FID_2D->FID.isEmpty()) {return;}

   // qDebug() << QString(Q_FUNC_INFO) << dataFilePath();
    QSet<QString> dset;
    dset.clear();
    if(xCheckBox->isChecked()) dset << "x";
    if(inPhaseCheckBox->isChecked()) dset << "i";
    if(quadratureCheckBox->isChecked()) dset << "q";
    if(absoluteCheckBox->isChecked()) dset << "a";


    if(dset.size()==0)
    {
        QMessageBox::warning(this,tr(""),
                    QString(Q_FUNC_INFO)+ "Please select (check) at least one item.");

        return;
    }


    QString path="~/";
    if(QDir(dataFilePath()).exists()) path=dataFilePath()+'/';
    QString fileName=QFileDialog::getSaveFileName(this,tr("Save"),path);
    if(fileName.isEmpty()) {return;}


    QString sep,sep1,sep2,sep3;

    if(separatorCombobox->currentIndex()==0) {sep=" ";sep1=" "; sep2=" "; sep3=" ";}
    else if(separatorCombobox->currentIndex()==1) {sep=",";sep1=",";sep2=",";sep3=",";}
    else {sep=" "; sep1=" "; sep2=" "; sep3=" ";}

    if(dset.contains("a")) sep3="";
    else if(dset.contains("q")) sep2="";
    else if(dset.contains("i")) sep1="";
    else sep="";



    QMutex mutex;
    QMutexLocker locker(&mutex);



    if(arrayOptionComboBox->currentIndex()!=0 || ancestor()->FID_2D->FID.size()==1) // single file
    {
      QFile file(fileName);
      if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
      {
         QMessageBox::warning(this,tr(""),
                     QString(Q_FUNC_INFO)+ ": Failed to open " + fileName);
         return;
      }
      QTextStream out(&file);

      for(int j=0; j<ancestor()->FID_2D->FID.size(); j++)
      {
        for(int k=0; k<ancestor()->FID_2D->FID.at(j)->al(); k++)
        {
          if(dset.contains("x"))
          {
              out << QString::number(ancestor()->FID_2D->FID.at(j)->xValue(k),'g',12);
              out << sep;
          }
          if(dset.contains("i"))
          {
              out << QString::number(ancestor()->FID_2D->FID.at(j)->real->sig.at(k),'g',12);
              out << sep1;
          }
          if(dset.contains("q"))
          {
              out << QString::number(ancestor()->FID_2D->FID.at(j)->imag->sig.at(k),'g',12);
              out << sep2;
          }
          if(dset.contains("a"))
          {
              out << QString::number(ancestor()->FID_2D->FID.at(j)->abs->sig.at(k),'g',12);
              out << sep3;
          }
          out << "\n";

    //      out << QString::number(ancestor()->FID_2D->FID.at(j)->xValue(k),'g',12) << sep
    //          << QString::number(ancestor()->FID_2D->FID.at(j)->real->sig.at(k),'g',12) << sep
    //          << QString::number(ancestor()->FID_2D->FID.at(j)->imag->sig.at(k),'g',12) << sep
    //          << QString::number(ancestor()->FID_2D->FID.at(j)->abs->sig.at(k),'g',12)
    //          << "\n";
        } // k
        if(j<ancestor()->FID_2D->FID.size()-1)
        {
          if(arrayOptionComboBox->currentIndex()==2) // blank-line separator
          {
              out << "\n";
          }
        }
      } // j
      file.close();
    }
    else // separate file for arrayed data
    {
        QFileInfo finfo=QFileInfo(fileName);
        QDir dir=finfo.dir();
        QString dirName=dir.absolutePath();
        QString completeBaseName=finfo.completeBaseName();
        QString suffix=finfo.suffix();

        int a=ancestor()->FID_2D->FID.size()-1;
        int maxDigit=QString::number(a,10).length();

        for(int j=0; j<ancestor()->FID_2D->FID.size(); j++)
        {
          int currentDigit=QString::number(j).length();
          int extraDigit=maxDigit-currentDigit;
          QString z;
          for(int k=0; k<extraDigit; k++) z+='0';
          z+=QString::number(j);

          QString fileName2=dirName+'/'+completeBaseName+'_'+z+'.'+suffix;
          QFile file2(fileName2);

          if (!file2.open(QIODevice::WriteOnly | QIODevice::Text))
          {
             QMessageBox::warning(this,tr(""),
                         QString(Q_FUNC_INFO)+ ": Failed to open " + fileName2);
             return;
          }
          QTextStream out2(&file2);

          dset.clear();
          if(xCheckBox->isChecked()) dset << "x";
          if(inPhaseCheckBox->isChecked()) dset << "i";
          if(quadratureCheckBox->isChecked()) dset << "q";
          if(absoluteCheckBox->isChecked()) dset << "a";

          for(int k=0; k<ancestor()->FID_2D->FID.at(j)->al(); k++)
          {
              if(dset.contains("x"))
              {
                  out2 << QString::number(ancestor()->FID_2D->FID.at(j)->xValue(k),'g',12);
                  out2 << sep;
              }
              if(dset.contains("i"))
              {
                  out2 << QString::number(ancestor()->FID_2D->FID.at(j)->real->sig.at(k),'g',12);
                  out2 << sep1;
              }
              if(dset.contains("q"))
              {
                  out2 << QString::number(ancestor()->FID_2D->FID.at(j)->imag->sig.at(k),'g',12);
                  out2 << sep2;
              }
              if(dset.contains("a"))
              {
                  out2 << QString::number(ancestor()->FID_2D->FID.at(j)->abs->sig.at(k),'g',12);
                  out2 << sep3;
              }
              out2 << "\n";


       //    out2 << QString::number(ancestor()->FID_2D->FID.at(j)->xValue(k),'g',12) << sep
       //         << QString::number(ancestor()->FID_2D->FID.at(j)->real->sig.at(k),'g',12) << sep
       //         << QString::number(ancestor()->FID_2D->FID.at(j)->imag->sig.at(k),'g',12) << sep
       //         << QString::number(ancestor()->FID_2D->FID.at(j)->abs->sig.at(k),'g',12)
       //         << "\n";
          } // k

          file2.close();
        } // j
    }

    return;
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
        for(int j=0;j<ancestor()->FID_2D->defaultAl();j++)
        {
            if(j!=ancestor()->FID_2D->defaultAl()-1)
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
//    if(ancestor()->FID_2D->al() != ancestor()->FID_2D->FID.size()) {return;}


    QString path="~/";
    if(QDir(dataFilePath()).exists()) path=dataFilePath()+'/';
    QString fileName=QFileDialog::getSaveFileName(this,tr("Save"),path,tr("*.2DD and *.2DP"));
    if(fileName.isEmpty()) {return;}

    QFileInfo fi;
    fi.setFile(fileName);
    QString base = fi.completeBaseName();
    QString path_1 = fi.absolutePath()+'/';
    QString data2dp = path_1+base+".2DP";
    QString data2dd = path_1+base+".2DD";



    //-------------------------------------

    //----export contour parameter as *.2DP------------

    int sf1, ef1;
    int line;
    double contL, contH;

    QString s = startf1LineEdit->text();
    sf1 = s.toInt();
    s = endf1LineEdit->text();
    ef1 = s.toInt();
    s = contNumberLineEdit->text();
    line = s.toInt();
    s = contLowLineEdit->text();
    contL = s.toDouble();
    s = contHighLineEdit->text();
    contH = s.toDouble();

    if(sf1==0 || sf1 < 0){sf1=1;}
    if(ef1==0 || ef1 > ancestor()->FID_2D->FID.size()){ef1=ancestor()->FID_2D->FID.size();}
    if(line<1 || line>100){line=5;}
    if(contL==0. || contL < 1.0){contL=5.0;}
    if(contH==0. || contH > 99.0){contH=99.0;}

    if(contL>=contH){contL=5.0; contH=99.0;}

    if(sf1<ef1 && contL<contH)
    {
        bool ok;
        KExport2DP *exp = new KExport2DP;

        ok=exp->process(ancestor()->FID_2D, data2dp, sf1, ef1, line, contL, contH);

        if(!ok)
        {

          delete exp;
          return;
        }
    }

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
            out << ancestor()->FID_2D->FID.at(k)->real->sig.at(m);
        }
    }

    file.close();

}

//void KExportWidget::exportAbs(){

//    //-----------Check FID opened?-----------
//    if(!isAncestorDefined()) return;
//    if(ancestor()->FID_2D->FID.isEmpty()) return;

//    int fidsize = ancestor()->FID_2D->FID.size();

//    //-----acquire path and set output file---------
//    QString path = ancestor()->processFileWidget->dataFilePath()+'/';
//    QString fileName = QFileDialog::getSaveFileName(this, "Export Ascii Data", path, "txt(*.txt)");

//    if(fileName.isEmpty()) return;
//    //qDebug() << fileName << endl;

//    QFile file(fileName);
//    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;

//    QTextStream out(&file);

//    //------------export absolute value------------
//    for(int i=0;i<fidsize;i++){
//        for(int j=0;j<ancestor()->FID_2D->al();j++){
//            out << ancestor()->FID_2D->FID.at(i)->abs->sig.at(j);
//            if(j!=ancestor()->FID_2D->al()-1){out << " ";}
//        }
//        out << '\n'; // Qt::endl;
//    }

//    file.close();

//    for(int k=0; k<ancestor()->plotters->FIDPlotters.size(); k++)
//    {
//      ancestor()->plotters->FIDPlotters[k]->plotter->xini=0;
//      ancestor()->plotters->FIDPlotters[k]->plotter->xfin=ancestor()->FID_2D->FID.at(0)->al()-1;
//    }

//    ancestor()->plotters->update();

//    //qDebug() << "end";
//    return;
//}
