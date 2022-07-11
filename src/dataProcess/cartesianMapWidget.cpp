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
    setCubeSidePointsSpinBox = new QSpinBox;
        setCubeSidePointsSpinBox->setMinimum(1);
        setCubeSidePointsSpinBox->setMaximum(1024);
        setCubeSidePointsSpinBox->setValue(256);
    setDistanceBetweenPointsDoubleSpinBox = new QDoubleSpinBox;
        setDistanceBetweenPointsDoubleSpinBox->setSingleStep(0.01);
        setDistanceBetweenPointsDoubleSpinBox->setMinimum(0.01);
        setDistanceBetweenPointsDoubleSpinBox->setMaximum(100.00);
        setDistanceBetweenPointsDoubleSpinBox->setValue(1.00);
    selectModeComboBox = new QComboBox;
        selectModeComboBox->addItems(QStringList() << tr("gridding (gaus)") << tr("gridding (sinc)") << tr("vector, linear") << tr("d^(-1) coef."));
    calcWeightPushButton = new QPushButton(tr("Calc. weight"));
    applyAngleTablePushButton = new QPushButton(tr("Apply"));

}

void TCartesianMapWidget::createLayout()
{
    QGridLayout *gLayout0 = new QGridLayout(this); // main layout


    gLayout0->addWidget(loadAngleTablePushButton,0,0,1,1);
    gLayout0->addWidget(saveAngleTablePushButton,0,1,1,1);
    gLayout0->addWidget(thetaPhiTextEdit,1,0,1,2);
    gLayout0->addWidget(new QLabel(tr("Mode")),2,0,1,1);
    gLayout0->addWidget(selectModeComboBox,2,1,1,1);
    gLayout0->addWidget(new QLabel(tr("Points on cube side")),3,0,1,1);
    gLayout0->addWidget(setCubeSidePointsSpinBox,3,1,1,1);
    gLayout0->addWidget(new QLabel(tr("Distance between points")),4,0,1,1);
    gLayout0->addWidget(setDistanceBetweenPointsDoubleSpinBox,4,1,1,1);
//    gLayout0->addWidget(setAngleTablePushButton,5,0,1,1);
    gLayout0->addWidget(calcWeightPushButton,5,0,1,1);
    gLayout0->addWidget(applyAngleTablePushButton,5,1,1,1);
}

void TCartesianMapWidget::createConnections()
{
    connect(loadAngleTablePushButton,SIGNAL(clicked()),this,SLOT(onLoadAngleTablePushButtonClicked()));
    connect(saveAngleTablePushButton,SIGNAL(clicked()),this,SLOT(onSaveAngleTablePushButtonClicked()));
//    connect(setAngleTablePushButton,SIGNAL(clicked()),this,SLOT(onSetAngleTablePushButtonClicked()));
    connect(selectModeComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(changeCalcWeightPushButtonEnabled(int)));
    connect(calcWeightPushButton,SIGNAL(clicked()),this,SLOT(onCalcWeightPushButtonClicked()));
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
    cartesianMap3D->interpolateMode = selectModeComboBox->currentIndex();
    cartesianMap3D->numberofPointsonCubeSide = setCubeSidePointsSpinBox->value();
    cartesianMap3D->ratioofDistanceBetweenPoints = setDistanceBetweenPointsDoubleSpinBox->value();

    switch(selectModeComboBox->currentIndex()){
    case TCartesianMap3D::gridGaus:
        if(!gaussianOption){
            gaussianOption = new SCartesianMapGaussianOption(this);
            connect(gaussianOption,SIGNAL(sendSigma(double)),cartesianMap3D,SLOT(getSigma(double)));
        }
        if(gaussianOption->exec() != QDialog::Accepted){
            delete cartesianMap3D;
            return;
        }
        break;
    case TCartesianMap3D::gridSinc:
        break;
    case TCartesianMap3D::vector:
        break;
    case TCartesianMap3D::dInverse:
        break;
    default:
        break;
    }

    emit isCartesianMapIdle(false);
    if (!cartesianMap3D->setOrigPolarAngles(thetaPhiTextEdit->toPlainText().trimmed()))
    {
        QMessageBox::warning(this,"error",cartesianMap3D->errorMessage());
        delete cartesianMap3D;
        emit isCartesianMapIdle(true);
        return;
    }

    QProgressDialog *progressDialog0 = new QProgressDialog("Preparing...",
                                                          QString(), 0, ancestor()->FID_2D->FID.at(0)->al());

    progressDialog0->setMinimumDuration(10);
    progressDialog0->setWindowTitle("Cartesian map");
    connect(cartesianMap3D,SIGNAL(tableCount(int)), progressDialog0, SLOT(setValue(int)));



    bool ok=cartesianMap3D->process(ancestor()->FID_2D);

    QEventLoop loop0;
    loop0.connect(cartesianMap3D, SIGNAL(genTableComplete()), & loop0, SLOT(quit()));
    loop0.exec();

    disconnect(cartesianMap3D,SIGNAL(tableCount(int)), progressDialog0, SLOT(setValue(int)));
    delete progressDialog0;



    QString qs1="Processing...";
    QProgressDialog *progressDialog1 = new QProgressDialog(qs1,
                                                          "Cancel", 0, cartesianMap3D->numberofPointsonCubeSide);
    progressDialog1->setMinimumDuration(10);
    progressDialog1->setWindowTitle("Cartesian map");

    connect(progressDialog1, SIGNAL(canceled()), cartesianMap3D, SLOT(cancel()));
    connect(cartesianMap3D,SIGNAL(calcCount(int)), progressDialog1, SLOT(setValue(int)));
//    connect(cartesianMap3D,SIGNAL(info(QString)), progressDialog1, SLOT(setLabelText(QString)));

    QEventLoop loop1;
    loop1.connect(cartesianMap3D, SIGNAL(calcComplete()), &loop1, SLOT(quit()));
    loop1.connect(cartesianMap3D, SIGNAL(canceled()), &loop1, SLOT(quit()));
    loop1.exec();

    disconnect(cartesianMap3D,SIGNAL(calcCount(int)), progressDialog1, SLOT(setValue(int)));
//    disconnect(cartesianMap3D,SIGNAL(info(QString)), progressDialog1, SLOT(setLabelText(QString)));
    delete progressDialog1;

    if(cartesianMap3D->wasCanceled)
    {
        QMessageBox::warning(this,"Cartesian map","Canceled.");
        delete cartesianMap3D;
        emit isCartesianMapIdle(true);
        return;
    }

    if(!ok)
    {
      QMessageBox::warning(this,"error",cartesianMap3D->errorMessage());
      delete cartesianMap3D;
      emit isCartesianMapIdle(true);
      return;
    }


//    QProgressDialog *progressDialog2 = new QProgressDialog("Applying change ...",
//                                                          QString(), 0,
//                                                          ancestor()->FID_2D->FID.at(0)->al()*ancestor()->FID_2D->FID.at(0)->al());

//    progressDialog2->setMinimumDuration(10);
//    connect(cartesianMap3D,SIGNAL(copyCount(int)), progressDialog2, SLOT(setValue(int)));

    QEventLoop loop2;
    loop2.connect(cartesianMap3D, SIGNAL(copyComplete()), &loop2, SLOT(quit()));
    loop2.exec();
//    disconnect(cartesianMap3D,SIGNAL(copyCount(int)), progressDialog2, SLOT(setValue(int)));
//    delete progressDialog2;


    addOperation(cartesianMap3D);

    emit isCartesianMapIdle(true);

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

void TCartesianMapWidget::changeCalcWeightPushButtonEnabled(int index)
{
    switch(index){
    case TCartesianMap3D::gridGaus:
        calcWeightPushButton->setEnabled(true);
        break;
    case TCartesianMap3D::gridSinc:
        calcWeightPushButton->setEnabled(true);
        break;
    case TCartesianMap3D::vector:
        calcWeightPushButton->setEnabled(false);
        break;
    case TCartesianMap3D::dInverse:
        calcWeightPushButton->setEnabled(false);
        break;
    default:
        calcWeightPushButton->setEnabled(false);
    }
}

void TCartesianMapWidget::onCalcWeightPushButtonClicked()
{
    if(!isAncestorDefined()) return;
    if(ancestor()->FID_2D->FID.isEmpty())
    {
       QMessageBox::warning(this,"Data empty",
                            "Data is empty.");
       return;

    }

    SCartesianMapWeight3D *cartesianMapWeight3D = new SCartesianMapWeight3D;
    cartesianMapWeight3D->interpolateMode = selectModeComboBox->currentIndex();
    cartesianMapWeight3D->numberofPointsonCubeSide = setCubeSidePointsSpinBox->value();
    cartesianMapWeight3D->ratioofDistanceBetweenPoints = setDistanceBetweenPointsDoubleSpinBox->value();

    switch(selectModeComboBox->currentIndex()){
    case TCartesianMap3D::gridGaus:
        if(!gaussianOption){
            gaussianOption = new SCartesianMapGaussianOption(this);
            connect(gaussianOption,SIGNAL(sendSigma(double)),cartesianMapWeight3D,SLOT(getSigma(double)));
        }
        if(gaussianOption->exec() != QDialog::Accepted){
            delete cartesianMapWeight3D;
            return;
        }
        break;
    case TCartesianMap3D::gridSinc:
        break;
    case TCartesianMap3D::vector:
        delete cartesianMapWeight3D;
        return;
    case TCartesianMap3D::dInverse:
        delete cartesianMapWeight3D;
        return;
    default:
        delete cartesianMapWeight3D;
        return;
    }

    if(!weightOption){
        weightOption = new SCartesianMapWeightOption(this);
        connect(weightOption,SIGNAL(sendIteration(int)),cartesianMapWeight3D,SLOT(getIteration(int)));
    }
    if(weightOption->exec() != QDialog::Accepted){
        delete cartesianMapWeight3D;
        return;
    }

    emit isCartesianMapIdle(false);
    if (!cartesianMapWeight3D->setOrigPolarAngles(thetaPhiTextEdit->toPlainText().trimmed()))
    {
        QMessageBox::warning(this,"error",cartesianMapWeight3D->errorMessage());
        delete cartesianMapWeight3D;
        emit isCartesianMapIdle(true);
        return;
    }

//    QProgressDialog *progressDialog0 = new QProgressDialog("Preparing...",
//                                                          QString(), 0, ancestor()->FID_2D->FID.at(0)->al());

//    progressDialog0->setMinimumDuration(10);
//    progressDialog0->setWindowTitle("Cartesian map");
//    connect(cartesianMap3D,SIGNAL(tableCount(int)), progressDialog0, SLOT(setValue(int)));



//    bool ok=cartesianMap3D->process(ancestor()->FID_2D);

//    QEventLoop loop0;
//    loop0.connect(cartesianMap3D, SIGNAL(genTableComplete()), & loop0, SLOT(quit()));
//    loop0.exec();

//    disconnect(cartesianMap3D,SIGNAL(tableCount(int)), progressDialog0, SLOT(setValue(int)));
//    delete progressDialog0;



    QString qs1="Processing...";
    QProgressDialog *progressDialog1 = new QProgressDialog(qs1,
                                                          "Cancel", 0, cartesianMapWeight3D->iteration);
    progressDialog1->setMinimumDuration(10);
    progressDialog1->setWindowTitle("Calculate weight");

    connect(progressDialog1, SIGNAL(canceled()), cartesianMapWeight3D, SLOT(cancel()));
    connect(cartesianMapWeight3D,SIGNAL(calcCount(int)), progressDialog1, SLOT(setValue(int)));
//    connect(cartesianMapWeight3D,SIGNAL(info(QString)), progressDialog1, SLOT(setLabelText(QString)));

    bool ok=cartesianMapWeight3D->process(ancestor()->FID_2D);

    QEventLoop loop1;
    loop1.connect(cartesianMapWeight3D, SIGNAL(calcComplete()), &loop1, SLOT(quit()));
    loop1.connect(cartesianMapWeight3D, SIGNAL(canceled()), &loop1, SLOT(quit()));
    loop1.exec();

    disconnect(cartesianMapWeight3D,SIGNAL(calcCount(int)), progressDialog1, SLOT(setValue(int)));
//    disconnect(cartesianMapWeight3D,SIGNAL(info(QString)), progressDialog1, SLOT(setLabelText(QString)));
    delete progressDialog1;

    if(cartesianMapWeight3D->wasCanceled)
    {
        QMessageBox::warning(this,"Cartesian map","Canceled.");
        delete cartesianMapWeight3D;
        emit isCartesianMapIdle(true);
        return;
    }

    if(!ok)
    {
      QMessageBox::warning(this,"error",cartesianMapWeight3D->errorMessage());
      delete cartesianMapWeight3D;
      emit isCartesianMapIdle(true);
      return;
    }


//    QProgressDialog *progressDialog2 = new QProgressDialog("Applying change ...",
//                                                          QString(), 0,
//                                                          ancestor()->FID_2D->FID.at(0)->al()*ancestor()->FID_2D->FID.at(0)->al());

//    progressDialog2->setMinimumDuration(10);
//    connect(cartesianMap3D,SIGNAL(copyCount(int)), progressDialog2, SLOT(setValue(int)));

//    QEventLoop loop2;
//    loop2.connect(cartesianMapWeight3D, SIGNAL(copyComplete()), &loop2, SLOT(quit()));
//    loop2.exec();
//    disconnect(cartesianMap3D,SIGNAL(copyCount(int)), progressDialog2, SLOT(setValue(int)));
//    delete progressDialog2;


    addOperation(cartesianMapWeight3D);

    emit isCartesianMapIdle(true);
}


SCartesianMapGaussianOption::SCartesianMapGaussianOption(QWidget *parent)
    :QDialog(parent)
{
    sigmaDoubleSpinBox = new QDoubleSpinBox;
        sigmaDoubleSpinBox->setDecimals(2);
        sigmaDoubleSpinBox->setSingleStep(0.01);
        sigmaDoubleSpinBox->setMinimum(0.01);
        sigmaDoubleSpinBox->setMaximum(10.00);
        sigmaDoubleSpinBox->setValue(1.00);
    okPushButton = new QPushButton(tr("OK"));
    cancelPushButton = new QPushButton(tr("Cancel"));

    QGridLayout *centerLayout = new QGridLayout;
    centerLayout->addWidget(new QLabel(tr("Sigma")),0,0,1,1);
    centerLayout->addWidget(sigmaDoubleSpinBox,0,1,1,1);
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(okPushButton);
    buttonLayout->addWidget(cancelPushButton);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(centerLayout);
    mainLayout->addLayout(buttonLayout);
    this->setLayout(mainLayout);

    connect(okPushButton,SIGNAL(clicked()),this,SLOT(onAccept()));
    connect(cancelPushButton,SIGNAL(clicked()),this,SLOT(onReject()));
}

SCartesianMapGaussianOption::~SCartesianMapGaussianOption()
{

}

void SCartesianMapGaussianOption::onAccept()
{
    emit sendSigma(sigmaDoubleSpinBox->value());
    accept();
}

void SCartesianMapGaussianOption::onReject()
{
    reject();
}


SCartesianMapWeightOption::SCartesianMapWeightOption(QWidget *parent)
    :QDialog(parent)
{
    iterationSpinBox = new QSpinBox;
        iterationSpinBox->setMinimum(1);
        iterationSpinBox->setMaximum(128);
        iterationSpinBox->setValue(10);
    calculatePushButton = new QPushButton(tr("Calculate"));
    cancelPushButton = new QPushButton(tr("Cancel"));

    QGridLayout *centerLayout = new QGridLayout;
    centerLayout->addWidget(new QLabel(tr("Iteration")),0,0,1,1);
    centerLayout->addWidget(iterationSpinBox,0,1,1,1);
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(calculatePushButton);
    buttonLayout->addWidget(cancelPushButton);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(centerLayout);
    mainLayout->addLayout(buttonLayout);
    this->setLayout(mainLayout);

    connect(calculatePushButton,SIGNAL(clicked()),this,SLOT(onAccept()));
    connect(cancelPushButton,SIGNAL(clicked()),this,SLOT(onReject()));
}

SCartesianMapWeightOption::~SCartesianMapWeightOption()
{

}

void SCartesianMapWeightOption::onAccept()
{
    emit sendIteration(iterationSpinBox->value());
    accept();
}

void SCartesianMapWeightOption::onReject()
{
    reject();
}
