#include "transformWidget.h"
#include "processPanelWidget.h"
#include <QGridLayout>
#include <QGroupBox>
#include <QMessageBox>

//TTransformWidget::TTransformWidget(QWidget *parent) : TProcessPanelBase(parent)
TTransformWidget::TTransformWidget() : TProcessBase()
{

    createWidgets();
    createPanel();
    createConnections();

}

void TTransformWidget::createWidgets()
{

    FFTButton=new QPushButton(tr("Transform"));
    IFFTButton=new QPushButton(tr("Inversion"));

    LaplaceCheckBox = new QCheckBox(tr("Laplace transformation"));
    LaplaceWidthDoubleSpinBox = new QDoubleSpinBox;
    LaplaceWidthDoubleSpinBox->setMinimum(0);
    LaplaceWidthDoubleSpinBox->setMaximum(1e100);
    LaplaceWidthDoubleSpinBox->setSuffix(" Hz");

   // slowFTButton = new QPushButton(tr("slow FT"));
   // slowIFTButton = new QPushButton(tr("slow inverse FT"));

    restoreToDefaultButton = new QPushButton(tr("Default"));

    restoreToDefault();

}

void TTransformWidget::restoreToDefault()
{
    LaplaceCheckBox->setChecked(false);
    LaplaceWidthDoubleSpinBox->setEnabled(false);
    LaplaceWidthDoubleSpinBox->setValue(0);
}

void TTransformWidget::createPanel()
{
    QVBoxLayout *vLayout1 = new QVBoxLayout(this);

    QGridLayout *gLayout1= new QGridLayout;
    gLayout1->addWidget(FFTButton,0,0,2,1);
    gLayout1->addWidget(IFFTButton,0,1,2,1);
    gLayout1->addWidget(LaplaceCheckBox,2,0,1,2);
    gLayout1->addWidget(LaplaceWidthDoubleSpinBox,3,1,1,1);
    gLayout1->addWidget(restoreToDefaultButton,4,1,1,1);

    QGroupBox *groupBox1 = new QGroupBox(tr("Fourier/Laplace Transformation"));
    groupBox1->setLayout(gLayout1);

    vLayout1->addWidget(groupBox1);
    vLayout1->addWidget(applyModeWidget);
    vLayout1->addStretch();

}

void TTransformWidget::createConnections()
{
    connect(FFTButton,SIGNAL(clicked()),this,SLOT(performFFT()));
    connect(IFFTButton,SIGNAL(clicked()),this,SLOT(performIFFT()));

    connect(restoreToDefaultButton,SIGNAL(clicked()),this,SLOT(restoreToDefault()));
    connect(LaplaceCheckBox,SIGNAL(toggled(bool)),LaplaceWidthDoubleSpinBox,SLOT(setEnabled(bool)));


}


void TTransformWidget::performFFT()
{
   if(!isAncestorDefined()) return;
   if(ancestor()->FID_2D->FID.isEmpty()) return;

   bool ok=true;
   TFFT *fft=new TFFT;
   // We check if we apply Laplace transfomation
   fft->setLaplace(LaplaceCheckBox->isChecked());
   // If we apply Laplace transformation, we set the width.
   if(fft->Laplace())
   {
       fft->setLaplaceWidth(LaplaceWidthDoubleSpinBox->value());
   }

   if(applyModeWidget->currentPlotter() > ancestor()->plotters->FIDPlotters.size()-1)
   {
       QMessageBox::warning(this,"","plotter index out of range.");
   }

   fft->setApplyIndex(ancestor()->plotters
                                  ->FIDPlotters.at(applyModeWidget->currentPlotter())
                                  ->FIDSelectSpinBox->value()-1);

   fft->setApplyMode(applyModeWidget->applyModeComboBox->currentIndex());


   ok=fft->process(ancestor()->FID_2D);
   if(!ok)
   {
     QMessageBox::warning(this,"error",fft->errorMessage());
     delete fft;
     return;
   }

   emit vOffsetRequest(0.1);

   ancestor()->processOperations->processElements.append(fft);
   ancestor()->updateProcessSettings();

   ancestor()->processSettings
             ->beginGroup(
                 QString::number(ancestor()->processOperations->processElements.size()-1)
               );
     ancestor()->processSettings->setValue("laplace",fft->Laplace());
     ancestor()->processSettings->setValue("laplacewidth",fft->LaplaceWidth());
     ancestor()->processSettings->setValue("axisdomain",fft->axisDomain());
     ancestor()->processSettings->setValue("applymode",fft->applyMode());
     ancestor()->processSettings->setValue("applyIndex",fft->applyIndex());

   ancestor()->processSettings->endGroup();
   ancestor()->processSettings->sync();


   ancestor()->axisFormatWidget->domainComboBox->setCurrentIndex(1);
   ancestor()->axisFormatWidget->init();
   ancestor()->axisFormatWidget->refresh();

   ancestor()->phaseWidget->phasePivotSpinBox->setValue(
                   ancestor()->FID_2D->FID.at(0)->abs->maxAt()
                   );
   ancestor()->plotters->update();


}



void TTransformWidget::performIFFT()
{
   if(!isAncestorDefined()) return;
   if(ancestor()->FID_2D->FID.isEmpty()) return;

   bool ok=false;
   TIFFT *ifft=new TIFFT;

   // We check if we apply Laplace transfomation
   ifft->setLaplace(LaplaceCheckBox->isChecked());
   // If we apply Laplace transformation, we set the width.
   if(ifft->Laplace())
   {
       ifft->setLaplaceWidth(LaplaceWidthDoubleSpinBox->value());
   }


   ifft->setApplyIndex(ancestor()->plotters
                                  ->FIDPlotters.at(applyModeWidget->currentPlotter())
                                  ->FIDSelectSpinBox->value()-1);

   ifft->setApplyMode(applyModeWidget->applyModeComboBox->currentIndex());

   ok=ifft->process(ancestor()->FID_2D);

  // qDebug() << QString(Q_FUNC_INFO) << ok;

   if(!ok)
   {
     QMessageBox::warning(this,"error",ifft->errorMessage());
     delete ifft;
     return;
   }

   emit vOffsetRequest(0.5);

   ancestor()->processOperations->processElements.append(ifft);
   ancestor()->updateProcessSettings();
   ancestor()->processSettings
             ->beginGroup(
                 QString::number(ancestor()->processOperations->processElements.size()-1)
               );
     ancestor()->processSettings->setValue("laplace",ifft->Laplace());
     ancestor()->processSettings->setValue("laplacewidth",ifft->LaplaceWidth());
     ancestor()->processSettings->setValue("axisdomain",ifft->axisDomain());
     ancestor()->processSettings->setValue("applymode",ifft->applyMode());
     ancestor()->processSettings->setValue("applyIndex",ifft->applyIndex());

   ancestor()->processSettings->endGroup();
   ancestor()->processSettings->sync();

   ancestor()->axisFormatWidget->init();
   ancestor()->axisFormatWidget->refresh();

   ancestor()->plotters->update();

}


