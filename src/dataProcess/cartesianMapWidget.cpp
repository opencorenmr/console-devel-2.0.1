#include <QLabel>
#include <QLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QFileDialog>
#include <QEventLoop>
#include <QProgressDialog>

#include "cartesianMapWidget.h"
#include "processPanelWidget.h"

TCartesianMapWidget::TCartesianMapWidget()
{
    createWidgets();
    createLayout();
    createConnections();
}

void TCartesianMapWidget::createWidgets()
{
    thetaPhiTextEdit = new QPlainTextEdit;
    loadAngleTablePushButton = new QPushButton(tr("Load"));
    saveAngleTablePushButton = new QPushButton(tr("Save"));
//    setAngleTablePushButton = new QPushButton(tr("Set"));
    applyAngleTablePushButton = new QPushButton(tr("Apply"));

}

void TCartesianMapWidget::createLayout()
{
    QGridLayout *gLayout0 = new QGridLayout(this); // main layout


    gLayout0->addWidget(loadAngleTablePushButton,0,0,1,1);
    gLayout0->addWidget(saveAngleTablePushButton,0,1,1,1);
    gLayout0->addWidget(thetaPhiTextEdit,1,0,1,2);
//    gLayout0->addWidget(setAngleTablePushButton,2,0,1,1);
    gLayout0->addWidget(applyAngleTablePushButton,2,1,1,1);
}

void TCartesianMapWidget::createConnections()
{
    connect(loadAngleTablePushButton,SIGNAL(clicked()),this,SLOT(onLoadAngleTablePushButtonClicked()));
    connect(saveAngleTablePushButton,SIGNAL(clicked()),this,SLOT(onSaveAngleTablePushButtonClicked()));
//    connect(setAngleTablePushButton,SIGNAL(clicked()),this,SLOT(onSetAngleTablePushButtonClicked()));
    connect(applyAngleTablePushButton,SIGNAL(clicked()),this,SLOT(onApplyAngleTablePushButtonClicked()));

}

void TCartesianMapWidget::addOperation(TCartesianMap3D *cMap3D)
{


    ancestor()->processOperations->processElements.append(cMap3D);
    ancestor()->updateProcessSettings();

    ancestor()->processSettings
              ->beginGroup(
                  QString::number(ancestor()->processOperations->processElements.size()-1)
                );

    ancestor()->processSettings->setValue("polarAngles",cMap3D->origPolarAnglesStr());
    ancestor()->processSettings->endGroup();
    ancestor()->processSettings->sync();

    for(int k=0; k<ancestor()->plotters->FIDPlotters.size(); k++)
    {

      ancestor()->plotters->FIDPlotters[k]->plotter->xini=0;
      ancestor()->plotters->FIDPlotters[k]->plotter->xfin=ancestor()->FID_2D->FID.at(0)->al()-1;

    }

    ancestor()->plotters->update();

}


void TCartesianMapWidget::onApplyAngleTablePushButtonClicked()
{
    if(!isAncestorDefined()) return;
    if(ancestor()->FID_2D->FID.isEmpty())
    {
       QMessageBox::warning(this,"Data empty",
                            "Data is empty.");
       return;

    }

    TCartesianMap3D *cartesianMap3D = new TCartesianMap3D;
    QProgressDialog *progressDialog = new QProgressDialog("Processing cartesian map...",
                                                          "Cancel", 0, ancestor()->FID_2D->al());
    //progressDialog->setWindowModality(Qt::WindowModal);
    progressDialog->setMinimumDuration(100);


    connect(progressDialog, SIGNAL(canceled()), cartesianMap3D, SLOT(cancel()));



    connect(cartesianMap3D,SIGNAL(calcCount(int)), progressDialog, SLOT(setValue(int)));

    if (!cartesianMap3D->setOrigPolarAngles(thetaPhiTextEdit->toPlainText().trimmed()))
    {
        QMessageBox::warning(this,"error",cartesianMap3D->errorMessage());
        delete cartesianMap3D;
        return;
    }

    bool ok=cartesianMap3D->process(ancestor()->FID_2D);

    QEventLoop loop;
    loop.connect(cartesianMap3D, SIGNAL(calcComplete()), &loop, SLOT(quit()));
    loop.connect(cartesianMap3D, SIGNAL(canceled()), &loop, SLOT(quit()));
    loop.exec();

    disconnect(cartesianMap3D,SIGNAL(calcCount(int)), progressDialog, SLOT(setValue(int)));
    delete progressDialog;

    if(cartesianMap3D->wasCanceled)
    {
        QMessageBox::warning(this,"","Canceled.");
        delete cartesianMap3D;
        return;
    }

    if(!ok)
    {
      QMessageBox::warning(this,"error",cartesianMap3D->errorMessage());
      delete cartesianMap3D;
      return;
    }

  //  QStringList sl=thetaPhiTextEdit->toPlainText().split('\n');

    QEventLoop loop2;
    loop2.connect(cartesianMap3D, SIGNAL(copyComplete()), &loop, SLOT(quit()));
    loop.exec();

    addOperation(cartesianMap3D);

}

void TCartesianMapWidget::onSaveAngleTablePushButtonClicked()
{
    QString path="~/";
    if(QDir(ancestor()->processFileWidget->dataFilePath()).exists())
        path=ancestor()->processFileWidget->dataFilePath()+'/';
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save angle data"),
                                                    path
                                                    );
    if (fileName.isEmpty()) {return;}
    setDataFilePath(QFileInfo(fileName).absolutePath());

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("cartesianMapWidget"), tr("Failed to save file."));
        return;
    }

    QTextStream out(&file);

    QStringList sl=thetaPhiTextEdit->toPlainText().split('\n');
    for(int k=0; k<sl.size(); k++) out << sl.at(k) << "\n";

    return;
}

void TCartesianMapWidget::onLoadAngleTablePushButtonClicked()
{
    QString path="~/";
    if(QDir(ancestor()->processFileWidget->dataFilePath()).exists())
        path=ancestor()->processFileWidget->dataFilePath()+'/';
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open angle data"),
                                                    path
                                                    );
    if (fileName.isEmpty()) {return;}
    setDataFilePath(QFileInfo(fileName).absolutePath());

    QFile file(fileName);
    file.open(QFile::ReadOnly | QFile::Text);
    thetaPhiTextEdit->setPlainText(file.readAll());

    return;
}
