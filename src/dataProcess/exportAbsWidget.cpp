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

    onlyRealCheckBox = new QCheckBox(tr("Abs of real only"));
    SumlayersCheckBox = new QCheckBox(tr("Sum layers"));
    SumlayersthicknessSpinBox = new QSpinBox;
        SumlayersthicknessSpinBox->setMinimum(1);
        SumlayersthicknessSpinBox->setMaximum(65536);
        SumlayersthicknessSpinBox->setValue(1);
        SumlayersthicknessSpinBox->setEnabled(false);
    SumlayersmodeComboBox = new QComboBox();
        SumlayersmodeComboBox->setEnabled(false);
    SumlayersmodeComboBox->addItems(QStringList() << tr("even") << tr("odd"));
        SumlayersmodeComboBox->setEnabled(false);
    Sumlayers3DCheckBox = new QCheckBox(tr("3D process (unworkable)"));
        Sumlayers3DCheckBox->setEnabled(false);
}

void SExportAbsWidget::createPanel()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;

    QGroupBox *groupBox0 = new QGroupBox(tr("export for ImageJ"));
        QGridLayout *gridLayout0 = new QGridLayout;
        gridLayout0->addWidget(new QLabel(tr("Layers")),0,0,1,1);
        gridLayout0->addWidget(layersSpinBox,0,1,1,1);
        gridLayout0->addWidget(onlyRealCheckBox,1,0,1,2);
        gridLayout0->addWidget(SumlayersCheckBox,2,0,1,2);
        gridLayout0->addWidget(new QLabel(tr(" thickness")),3,0,1,1);
        gridLayout0->addWidget(SumlayersthicknessSpinBox,3,1,1,1);
        gridLayout0->addWidget(new QLabel(tr(" mode")),4,0,1,1);
        gridLayout0->addWidget(SumlayersmodeComboBox,4,1,1,1);
      //  gridLayout0->addWidget(Sumlayers3DCheckBox,5,0,1,2);
        gridLayout0->addWidget(exportAbsButton,6,0,1,2);
    groupBox0->setLayout(gridLayout0);

    mainLayout->addWidget(groupBox0);

    mainLayout->addStretch();
    setLayout(mainLayout);
}

void SExportAbsWidget::createConnections()
{
    connect(exportAbsButton,SIGNAL(clicked()),this,SLOT(exportAbs()));
    connect(SumlayersCheckBox,SIGNAL(toggled(bool)),SumlayersthicknessSpinBox,SLOT(setEnabled(bool)));
    connect(SumlayersCheckBox,SIGNAL(toggled(bool)),SumlayersmodeComboBox,SLOT(setEnabled(bool)));
//    connect(SumlayersCheckBox,SIGNAL(toggled(bool)),Sumlayers3DCheckBox,SLOT(setEnabled(bool)));
}


void SExportAbsWidget::exportAbs(){

    if(!isAncestorDefined())
    {
        QMessageBox::warning(this,tr("exportAbs error"),"Ancestor is not defined.");
        return;
    }
    if(ancestor()->FID_2D->FID.isEmpty())
    {
        QMessageBox::warning(this,tr("exportAbs error"),"Data is empty.");
        return;
    }

    int fidsize = ancestor()->FID_2D->FID.size();
    int layernum = layersSpinBox->value();

    // layernum = 1: slice 2D image
    // In the case of layernum>1, we deal with a 3D image

    bool onlyRealFlag = onlyRealCheckBox->isChecked();

    if(fidsize%layernum)
    {
        QMessageBox::warning(this,tr("exportAbs error"),
                             "The size of the array data must be"
                             "integral multiple of the number of layers"
                             );
        return;
    }

    int thickness,edge,bundlenum;
    if(SumlayersCheckBox->isChecked())
    {
        thickness = SumlayersthicknessSpinBox->value();
        if(thickness>layernum)
        {
            QMessageBox::warning(this,tr("exportAbs error"),
                                 "Thickness cannot be larger than number of layers.");
            return;
        }

        if(SumlayersmodeComboBox->currentIndex()) // currentIndex != 0 ---> odd
        {
            if((layernum+thickness)%2) return;
            bundlenum=(((layernum+thickness)/2-1)/thickness)*2+1;
        }
        else // currentIndex==0 ---> even
        {
            if(layernum%2) return;
            bundlenum=((layernum/2-1)/thickness+1)*2;
        }
        edge = (layernum-(bundlenum-2)*thickness)/2;
    }
    else // no sum layers
    {
        thickness = 1;
        edge = 1;
        bundlenum = layernum;
    }

    QString dirPath = ancestor()->processFileWidget->dataFilePath()+'/';
    QString dirName = QFileDialog::getExistingDirectory(this, "Export Ascii Data", dirPath);

    if(dirName.isEmpty()) return;
    //qDebug() << dirName << endl;

    int digit = 1;
    while (bundlenum>=pow(10,digit)){
        digit += 1;
    }

    int digitl = 1;
    int startlayer = 0,layernuminbundle = 0;

    for(int l=0; l<bundlenum; l++)
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

        startlayer += layernuminbundle;
        if((0<l)and(l<bundlenum-1)){
            layernuminbundle = thickness;
        }else{
            layernuminbundle = edge;
        }

        double Sum = 0;
        for(int i=0;i<fidsize/layernum;i++){
            for(int j=0;j<ancestor()->FID_2D->al();j++){
                Sum = 0;
                for(int iSum=0;iSum<layernuminbundle;iSum++){
                    if (onlyRealFlag){
                        Sum += abs(ancestor()->FID_2D->FID.at((startlayer+iSum)*fidsize/layernum+i)->real->sig.at(j));
                    }else{
                        Sum += ancestor()->FID_2D->FID.at((startlayer+iSum)*fidsize/layernum+i)->abs->sig.at(j);
                    }
                }
                out << Sum;
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
