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

    FFTButton=new QPushButton(tr("FFT"));
    IFFTButton=new QPushButton(tr("Inverse FFT"));

    LaplaceCheckBox = new QCheckBox(tr("Laplace transformation"));
    LaplaceWidthDoubleSpinBox = new QDoubleSpinBox;
    LaplaceWidthDoubleSpinBox->setMinimum(0);
    LaplaceWidthDoubleSpinBox->setMaximum(1e100);
    LaplaceWidthDoubleSpinBox->setSuffix(" Hz");

    restoreToDefaultButton = new QPushButton(tr("Default"));
    restoreToDefault();

    n1SpinBox=new QSpinBox;
    n2SpinBox=new QSpinBox;
    n1SpinBox->setMinimum(1);
    n1SpinBox->setMaximum(65536);
    n2SpinBox->setMinimum(1);
    n2SpinBox->setMaximum(65536);
    n1SpinBox->setValue(1);
    n2SpinBox->setValue(1);

    FFT3DButton=new QPushButton(tr("3D FFT"));
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
    gLayout1->addWidget(FFTButton,0,0,1,1);
    gLayout1->addWidget(IFFTButton,0,1,1,1);
    gLayout1->addWidget(LaplaceCheckBox,1,0,1,2);
    gLayout1->addWidget(LaplaceWidthDoubleSpinBox,1,2,1,1);
 //   gLayout1->addWidget(restoreToDefaultButton,2,1,1,1);
    gLayout1->addWidget(applyModeWidget,2,0,1,3);

    QGroupBox *groupBox1 = new QGroupBox(tr("Fourier/Laplace Transformation"));
    groupBox1->setLayout(gLayout1);

    QGridLayout *gLayout2 = new QGridLayout;
    gLayout2->addWidget(new QLabel("Size (1st dimension)"), 0,0,1,1);
    gLayout2->addWidget(n1SpinBox,0,1,1,1);
    gLayout2->addWidget(new QLabel("Size (2nd dimension)"), 1,0,1,1);
    gLayout2->addWidget(n2SpinBox,1,1,1,1);
    gLayout2->addWidget(FFT3DButton,2,1,1,1);
    QGroupBox *groupBox2 = new QGroupBox(tr("3D FFT"));
    groupBox2->setLayout(gLayout2);


    vLayout1->addWidget(groupBox1);
    vLayout1->addWidget(groupBox2);
    vLayout1->addStretch();

}

void TTransformWidget::createConnections()
{
    connect(FFTButton,SIGNAL(clicked()),this,SLOT(performFFT()));
    connect(IFFTButton,SIGNAL(clicked()),this,SLOT(performIFFT()));

    connect(restoreToDefaultButton,SIGNAL(clicked()),this,SLOT(restoreToDefault()));
    connect(LaplaceCheckBox,SIGNAL(toggled(bool)),LaplaceWidthDoubleSpinBox,SLOT(setEnabled(bool)));

    connect(FFT3DButton,SIGNAL(clicked()),this,SLOT(perform3DFFT()));
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


void TTransformWidget::perform3DFFT()
{
   if(!isAncestorDefined()) return;
   if(ancestor()->FID_2D->FID.isEmpty()) return;

   bool ok=true;
   TFFT3D *fft3d=new TFFT3D;
   fft3d->FFT3D_setLengths(n1SpinBox->value(),n2SpinBox->value());

   ok=fft3d->process(ancestor()->FID_2D);

   if(!ok)
   {
       QMessageBox::warning(this,"3D FFT",fft3d->errorMessage());
       delete fft3d;
       return;
   }

   ancestor()->processOperations->processElements.append(fft3d);
   ancestor()->updateProcessSettings();

   ancestor()->plotters->update();


}
