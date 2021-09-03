#include "exportAbsWidget.h"
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
#include <math.h>

SExportAbsWidget::SExportAbsWidget()
{
   createWidgets();
   createPanel();
   createConnections();
}



void SExportAbsWidget::createWidgets()
{
    layersSpinBox = new QSpinBox;
        layersSpinBox->setMinimum(1);
        layersSpinBox->setMaximum(65536);
        layersSpinBox->setValue(1);
    exportAbsButton = new QPushButton(tr("Export all abs for ImageJ"));
}

void SExportAbsWidget::createPanel()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;

    QGroupBox *groupBox0 = new QGroupBox(tr("export for ImageJ"));
        QGridLayout *gridLayout0 = new QGridLayout;
        gridLayout0->addWidget(new QLabel(tr("Layers")),0,0,1,1);
        gridLayout0->addWidget(layersSpinBox,0,1,1,1);
        gridLayout0->addWidget(exportAbsButton,1,0,1,2);
    groupBox0->setLayout(gridLayout0);

    mainLayout->addWidget(groupBox0);

    mainLayout->addStretch();
    setLayout(mainLayout);
}

void SExportAbsWidget::createConnections()
{
    connect(exportAbsButton,SIGNAL(clicked()),this,SLOT(exportAbs()));
}


void SExportAbsWidget::exportAbs(){

    if(!isAncestorDefined()) return;
    if(ancestor()->FID_2D->FID.isEmpty()) return;

    int fidsize = ancestor()->FID_2D->FID.size();
    int layernum = layersSpinBox->value();

    if(fidsize%layernum) return;

    QString dirPath = ancestor()->processFileWidget->dataFilePath()+'/';
    QString dirName = QFileDialog::getExistingDirectory(this, "Export Ascii Data", dirPath);

    if(dirName.isEmpty()) return;
    //qDebug() << dirName << endl;

    int digit = 1;
    while (layernum>=pow(10,digit)){
        digit += 1;
    }

    int digitl = 1;

    for(int l=0; l<layernum; l++)
    {
        while (l>=pow(10,digitl)){
            digitl += 1;
        }
        QString fileName = dirName + '/';
        int zerofill = digit - digitl;
        while (zerofill){
            fileName += QString::number(0);
            zerofill -= 1;
        }
        fileName += QString::number(l)+ ".txt";
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))break;

        QTextStream out(&file);

    //------------export absolute value------------
        for(int i=0;i<fidsize/layernum;i++){
            for(int j=0;j<ancestor()->FID_2D->al();j++){
                out << ancestor()->FID_2D->FID.at(fidsize*l/layernum+i)->abs->sig.at(j);
                if(j!=ancestor()->FID_2D->al()-1){out << " ";}
            }
            out << '\n'; // Qt::endl;
        }

        file.close();

    }

    for(int k=0; k<ancestor()->plotters->FIDPlotters.size(); k++)
    {
      ancestor()->plotters->FIDPlotters[k]->plotter->xini=0;
      ancestor()->plotters->FIDPlotters[k]->plotter->xfin=ancestor()->FID_2D->FID.at(0)->al()-1;
    }

    ancestor()->plotters->update();

    //qDebug() << "end";
    return;
}
