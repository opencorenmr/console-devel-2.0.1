#include "2DprocessWidget.h"
#include "processPanelWidget.h"
#include "arraySum.h"
#include "flatten.h"
#include "transpose.h"
#include "hyperComplex.h"
#include "slow2dft.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>

#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include "../fid.h"

#include <float.h> // DBL_EPSILON
#include <math.h>  // fabs, fmax

T2DProcessWidget::T2DProcessWidget()
{
   createWidgets();
   createPanel();
   createConnections();
}



void T2DProcessWidget::createWidgets()
{
    transposeButton = new QPushButton(tr("Transpose"));
    arraySumButton = new QPushButton(tr("Array Sum"));
    flattenButton = new QPushButton(tr("Flatten"));

    hyperComplexPhaseComboBox = new QComboBox();
    hyperComplexPhaseComboBox->addItems(QStringList() << "As it is" << "Reverse");
    hyperComplexButton = new QPushButton(tr("Apply"));
    appendDataButton = new QPushButton(tr("open file"));
}

void T2DProcessWidget::createPanel()
{
    QVBoxLayout *vLayout= new QVBoxLayout(this);
      QGroupBox *transposeGroupBox = new QGroupBox(tr("Array manipulation"));
      QHBoxLayout *transposeLayout = new QHBoxLayout;
      transposeLayout->addWidget(transposeButton);
      transposeLayout->addWidget(arraySumButton);
      transposeLayout->addWidget(flattenButton);
      transposeGroupBox->setLayout(transposeLayout);
    vLayout->addWidget(transposeGroupBox);

      QGroupBox *hyperComplexGroupBox = new QGroupBox(tr("Hypercomplex compression"));
      QHBoxLayout *hyperComplexLayout = new QHBoxLayout;
      hyperComplexLayout->addWidget(new QLabel(tr("Phase")));
      hyperComplexLayout->addWidget(hyperComplexPhaseComboBox);
      hyperComplexLayout->addWidget(hyperComplexButton);
      hyperComplexGroupBox->setLayout(hyperComplexLayout);
    vLayout->addWidget(hyperComplexGroupBox);


    //Kobayashi
      QGroupBox *appendDataGroupBox = new QGroupBox(tr("Append Experiment Data"));
      QHBoxLayout *appendDataLayout = new QHBoxLayout;
      appendDataLayout->addWidget(appendDataButton);
      appendDataGroupBox->setLayout(appendDataLayout);
    vLayout->addWidget(appendDataGroupBox);
    vLayout->addStretch();
}

void T2DProcessWidget::createConnections()
{

    connect(transposeButton,SIGNAL(clicked()),this,SLOT(performTranspose()));
    connect(arraySumButton,SIGNAL(clicked()),this,SLOT(performArraySum()));
    connect(flattenButton,SIGNAL(clicked()),this,SLOT(performFlatten()));
    connect(hyperComplexButton,SIGNAL(clicked()),this,SLOT(performHyperComplexCompression()));
    connect(appendDataButton,SIGNAL(clicked()),this,SLOT(performAppendData()));

}

void T2DProcessWidget::performHyperComplexCompression()
{
    if(!isAncestorDefined()) return;
    if(ancestor()->FID_2D->FID.isEmpty()) return;

    bool ok;
    THyperComplex *hyperComplex = new THyperComplex;
    if(hyperComplexPhaseComboBox->currentIndex()==0) hyperComplex->setPhaseReverse(false);
    else hyperComplex->setPhaseReverse(true);

    ok=hyperComplex->process(ancestor()->FID_2D);
    if(!ok)
    {
      delete hyperComplex;
      return;
    }

    ancestor()->processOperations->processElements.append(hyperComplex);
    ancestor()->updateProcessSettings();


    for(int k=0; k<ancestor()->plotters->FIDPlotters.size(); k++)
    {
      ancestor()->plotters->FIDPlotters[k]->plotter->xini=0;
      ancestor()->plotters->FIDPlotters[k]->plotter->xfin=ancestor()->FID_2D->FID.at(0)->al()-1;
    }

    ancestor()->plotters->update();

}

void T2DProcessWidget::performFlatten()
{
    if(!isAncestorDefined()) return;
    if(ancestor()->FID_2D->FID.isEmpty()) return;

    bool ok;
    TFlatten *flatten = new TFlatten;
    ok=flatten->process(ancestor()->FID_2D);
    if(!ok)
    {
        delete flatten;
        return;
    }
    ancestor()->processOperations->processElements.append(flatten);

    ancestor()->updateProcessSettings();

    for(int k=0; k<ancestor()->plotters->FIDPlotters.size(); k++)
    {

      ancestor()->plotters->FIDPlotters[k]->plotter->xini=0;
      ancestor()->plotters->FIDPlotters[k]->plotter->xfin=ancestor()->FID_2D->FID.at(0)->al()-1;

    }

    ancestor()->plotters->update();



}
void T2DProcessWidget::performArraySum()
{
    if(!isAncestorDefined()) return;
    if(ancestor()->FID_2D->FID.isEmpty()) return;

    bool ok;
    TArraySum *arraySum = new TArraySum;
    ok=arraySum->process(ancestor()->FID_2D);
    if(!ok)
    {
        delete arraySum;
        return;
    }
    ancestor()->processOperations->processElements.append(arraySum);

    ancestor()->updateProcessSettings();

    for(int k=0; k<ancestor()->plotters->FIDPlotters.size(); k++)
    {

      ancestor()->plotters->FIDPlotters[k]->plotter->xini=0;
      ancestor()->plotters->FIDPlotters[k]->plotter->xfin=ancestor()->FID_2D->FID.at(0)->al()-1;

    }

    ancestor()->plotters->update();

}
void T2DProcessWidget::performTranspose()
{
    if(!isAncestorDefined()) return;
    if(ancestor()->FID_2D->FID.isEmpty()) return;

    bool ok;
    TTranspose *transpose = new TTranspose;
    ok=transpose->process(ancestor()->FID_2D);
    if(!ok)
    {
      delete transpose;
      return;
    }

    ancestor()->processOperations->processElements.append(transpose);
    ancestor()->updateProcessSettings();

//    ancestor()->axisFormatWidget->init();
//    ancestor()->axisFormatWidget->refresh();
//    ancestor()->phaseWidget->phasePivotSpinBox->setValue(
//                    ancestor()->FID_2D->FID.at(0)->abs->maxAt()
//                    );


    for(int k=0; k<ancestor()->plotters->FIDPlotters.size(); k++)
    {

      ancestor()->plotters->FIDPlotters[k]->plotter->xini=0;
      ancestor()->plotters->FIDPlotters[k]->plotter->xfin=ancestor()->FID_2D->FID.at(0)->al()-1;

    }

    ancestor()->plotters->update();

}

void T2DProcessWidget::performAppendData()
{

    if(!isAncestorDefined()) return;
    if(ancestor()->FID_2D->FID.isEmpty()) return;

    //-----acquire file path-------------
    QString path="~/";
    if(QDir(dataFilePath()).exists()) path = ancestor()->processFileWidget->dataFilePath()+'/';
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open data"),
                                                    path,
                                                    tr("Opencore files (*.opp *.opd *.sm2p *.sm2d)"));

    if (fileName.isEmpty()) {return;}

    setDataFilePath(QFileInfo(fileName).absolutePath());
    QString fileExt=QFileInfo(fileName).suffix();

    //-----read and add opp or sm2p to FID--------------
    if(0==QString::compare(fileExt,"sm2p") || 0==QString::compare(fileExt,"sm2d")){
      if(!Readsm2FileforAdd(fileName)){return;}

    } else if(0==QString::compare(fileExt,"opp") || 0==QString::compare(fileExt,"opd")){
      if(!ReadopFileforAdd(fileName)){return;}

    } else {
        QMessageBox::warning(this,tr(""), "." + fileExt + " is not supported."); return;
    }


    //-----plotter update----------------
    for(int k=0; k<ancestor()->plotters->FIDPlotters.size(); k++)
    {
      ancestor()->plotters->FIDPlotters[k]->plotter->xini=0;
      ancestor()->plotters->FIDPlotters[k]->plotter->xfin=ancestor()->FID_2D->FID.at(0)->al()-1;
    }

    ancestor()->plotters->update();

}

//-----------------------------------------------------------------------------
bool T2DProcessWidget::Readsm2FileforAdd(QString fn)
{
    QFileInfo fi;
    fi.setFile(fn);
    QString base = fi.completeBaseName();
    QString path = fi.absolutePath()+'/';
    QString sm2p = path+base+".sm2p";
    QString sm2d = path+base+".sm2d";

    if(!Readsm2pFileforAdd(sm2p)) return false;
    double org_dw=ancestor()->FID_2D->dw();
    double org_fsf1=ancestor()->FID_2D->sf1();
    if(AL!=ancestor()->FID_2D->defaultAl()){
        ancestor()->FID_2D->errorMessage="point=xxx is different."; return false;
    }
    if(fabs(DW-org_dw)>DBL_EPSILON*fmax(1, fmax(fabs(DW), fabs(org_dw)))){
        ancestor()->FID_2D->errorMessage="dw=xxx is different."; return false;
    }
    if(fabs(FSF1-org_fsf1)>DBL_EPSILON*fmax(1, fmax(fabs(FSF1), fabs(org_fsf1)))){
        ancestor()->FID_2D->errorMessage="sf1=xxx is different."; return false;
    }
    qDebug() << "read OK";


    if(!Readsm2dFileforAdd(sm2d)) return false;

    return true;
}

bool T2DProcessWidget::ReadopFileforAdd(QString fn)
{

    QFileInfo fi;
    fi.setFile(fn);
    QString base = fi.completeBaseName();
    QString path = fi.absolutePath()+'/';
    QString opp = path+base+".opp";
    QString opd = path+base+".opd";

    if(!ReadoppFileforAdd(opp)) return false;

    double org_dw=ancestor()->FID_2D->dw();
    double org_fsf1=ancestor()->FID_2D->sf1();
    if(AL!=ancestor()->FID_2D->defaultAl()){
        ancestor()->FID_2D->errorMessage="point=xxx is different."; return false;
    }
    if(fabs(DW-org_dw)>DBL_EPSILON*fmax(1, fmax(fabs(DW), fabs(org_dw)))){
        ancestor()->FID_2D->errorMessage="dw=xxx is different."; return false;
    }
    if(fabs(FSF1-org_fsf1)>DBL_EPSILON*fmax(1, fmax(fabs(FSF1), fabs(org_fsf1)))){
        ancestor()->FID_2D->errorMessage="sf1=xxx is different."; return false;
    }


    if(!ReadopdFileforAdd(opd)) return false;

    return true;
}

bool T2DProcessWidget::Readsm2pFileforAdd(QString fn)
{

    //QMutexLocker locker(&mutex);
    QFile file(fn);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){return false;}

    QString source; source.clear();
    QTextStream in(&file);

    bool pointDefined=false;
    bool dwDefined=false;
    bool sf1Defined=false;

    parameters.clear();

    bool ok;

    while (!in.atEnd())
    {
        source=in.readLine();
        parameters.append(source);
        source.remove(QChar(' '), Qt::CaseInsensitive);
        if(source.startsWith('#')) break;
        if(source.startsWith("point=",Qt::CaseInsensitive))
        {
            pointDefined=true;
            AL=source.mid(6).toInt(&ok);
            if(!ok) {file.close(); return false;}
        }
        if(source.startsWith("dw=",Qt::CaseInsensitive))
        {
            dwDefined=true;
            DW=source.mid(3).toDouble(&ok);
            if(!ok) {file.close(); return false;}
        }
        if(source.startsWith("sf1=",Qt::CaseInsensitive))
        {
            sf1Defined=true;
            FSF1=source.mid(4).toDouble(&ok);
            if(!ok) {file.close(); return false;}
        }

    }
    return true;
}

bool T2DProcessWidget::ReadoppFileforAdd(QString fn){

    //QMutexLocker locker(&mutex);
    QFile file(fn);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){return false;}

    QString source; source.clear();
    QTextStream in(&file);

    bool pointDefined=false;
    bool dwDefined=false;
    bool sf1Defined=false;

    parameters.clear();

    bool ok;

    while (!in.atEnd())
    {
        source=in.readLine();
        parameters.append(source);
        source.remove(QChar(' '), Qt::CaseInsensitive);
        if(source.startsWith('#')) break;
        if(source.startsWith("point=",Qt::CaseInsensitive))
        {
            pointDefined=true;
            AL=source.mid(6).toInt(&ok);
            if(!ok) {file.close(); return false;}
        }
        if(source.startsWith("dw=",Qt::CaseInsensitive))
        {
            dwDefined=true;
            DW=source.mid(3).toDouble(&ok);
            if(!ok) {file.close(); return false;}
        }
        if(source.startsWith("sf1=",Qt::CaseInsensitive))
        {
            sf1Defined=true;
            FSF1=source.mid(4).toDouble(&ok);
            if(!ok) {file.close(); return false;}
        }

    }

    return true;
}

bool T2DProcessWidget::Readsm2dFileforAdd(QString fn){

    QFile file(fn);
    if (!file.open(QIODevice::ReadOnly))
    {
         ancestor()->FID_2D->errorMessage=QString(Q_FUNC_INFO)+ ": Failed to open " + fn;
         return false;
    }

    QFileInfo fInfo;
    fInfo.setFile(file);
    int nByte=fInfo.size();
    int n=nByte/(2*AL*sizeof(double));

    QDataStream in(&file);
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);
    in.setByteOrder(QDataStream::LittleEndian);

    int org_FIDsize=ancestor()->FID_2D->FID.size();

    for(int k=org_FIDsize; k<n+org_FIDsize; k++)
    {
        ancestor()->FID_2D->FID.append(new TFID(AL));
        ancestor()->FID_2D->FID[ancestor()->FID_2D->FID.size()-1]->setDW(DW);
        for(int m=0; m<AL; m++) in >> ancestor()->FID_2D->FID[k]->real->sig[m] >> ancestor()->FID_2D->FID[k]->imag->sig[m];
        ancestor()->FID_2D->FID[k]->updateAbs();
        ancestor()->FID_2D->FID[k]->setCustomXAxis(ancestor()->FID_2D->isXAxisCustom());
        ancestor()->FID_2D->FID[k]->setXInitialValue(ancestor()->FID_2D->xInitialValue());
        ancestor()->FID_2D->FID[k]->setDx(ancestor()->FID_2D->dx());
        ancestor()->FID_2D->FID[k]->setXAxisLabel(ancestor()->FID_2D->xAxisLabel());
        ancestor()->FID_2D->FID[k]->setXAxisUnitSymbol(ancestor()->FID_2D->xAxisUnitSymbol());
        ancestor()->FID_2D->FID[k]->setPrefix(ancestor()->FID_2D->prefix());
        ancestor()->FID_2D->FID[k]->setPlotPrefix(ancestor()->FID_2D->plotPrefix());

        ancestor()->FID_2D->FID[k]->setDomain(TFID::TimeDomain);

        ancestor()->FID_2D->FID[k]->setEmpty(false);
    } // k

    file.close();
    ancestor()->FID_2D->setCurrentFID(0);

    return true;
}

bool T2DProcessWidget::ReadopdFileforAdd(QString fn){


    QFile file(fn);
    if (!file.open(QIODevice::ReadOnly))
    {
         ancestor()->FID_2D->errorMessage=QString(Q_FUNC_INFO)+ ": Failed to open " + fn;
         return false;
    }

    QFileInfo fInfo;
    fInfo.setFile(file);
    int nByte=fInfo.size();
    int n=nByte/(2*AL*sizeof(double));

    QDataStream in(&file);
    in.setFloatingPointPrecision(QDataStream::DoublePrecision);
    in.setByteOrder(QDataStream::LittleEndian);

    int org_FIDsize=ancestor()->FID_2D->FID.size();

    for(int k=org_FIDsize; k<n+org_FIDsize; k++)
    {
        ancestor()->FID_2D->FID.append(new TFID(AL));
        ancestor()->FID_2D->FID[ancestor()->FID_2D->FID.size()-1]->setDW(DW);
        for(int m=0; m<AL; m++) in >> ancestor()->FID_2D->FID[k]->real->sig[m] >> ancestor()->FID_2D->FID[k]->imag->sig[m];
        ancestor()->FID_2D->FID[k]->updateAbs();
        ancestor()->FID_2D->FID[k]->setCustomXAxis(ancestor()->FID_2D->isXAxisCustom());
        ancestor()->FID_2D->FID[k]->setXInitialValue(ancestor()->FID_2D->xInitialValue());
        ancestor()->FID_2D->FID[k]->setDx(ancestor()->FID_2D->dx());
        ancestor()->FID_2D->FID[k]->setXAxisLabel(ancestor()->FID_2D->xAxisLabel());
        ancestor()->FID_2D->FID[k]->setXAxisUnitSymbol(ancestor()->FID_2D->xAxisUnitSymbol());
        ancestor()->FID_2D->FID[k]->setPrefix(ancestor()->FID_2D->prefix());
        ancestor()->FID_2D->FID[k]->setPlotPrefix(ancestor()->FID_2D->plotPrefix());

        ancestor()->FID_2D->FID[k]->setDomain(TFID::TimeDomain);

        ancestor()->FID_2D->FID[k]->setEmpty(false);
    } // k

    file.close();
    ancestor()->FID_2D->setCurrentFID(0);

    return true;
}
