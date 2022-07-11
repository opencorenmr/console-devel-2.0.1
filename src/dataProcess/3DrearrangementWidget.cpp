#include "3DrearrangementWidget.h"
#include "3Drearrangement.h"
#include "processPanelWidget.h"

#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <QGroupBox>

S3DrearrangementWidget::S3DrearrangementWidget()
{
    createWidgets();
    createPanel();
    CreateConnections();
}

void S3DrearrangementWidget::createWidgets()
{
    size1SpinBox = new QSpinBox;
        size1SpinBox->setMinimum(1);
        size1SpinBox->setMaximum(65536);
        size1SpinBox->setValue(1);
    size2SpinBox = new QSpinBox;
        size2SpinBox->setMinimum(1);
        size2SpinBox->setMaximum(65536);
        size2SpinBox->setValue(1);
    d1DisplacementSpinBox = new QSpinBox;
        d1DisplacementSpinBox->setMinimum(0);
        d1DisplacementSpinBox->setMaximum(65536);
        d1DisplacementSpinBox->setValue(0);
    d2DisplacementSpinBox = new QSpinBox;
        d2DisplacementSpinBox->setMinimum(0);
        d2DisplacementSpinBox->setMaximum(65536);
        d2DisplacementSpinBox->setValue(0);
    abscissaDisplacementSpinBox = new QSpinBox;
        abscissaDisplacementSpinBox->setMinimum(0);
        abscissaDisplacementSpinBox->setMaximum(65536);
        abscissaDisplacementSpinBox->setValue(0);

    translationPushButton = new QPushButton(tr("Translation"));
    xyzRotationPushButton = new QPushButton(tr("xyz rotation"));
}

void S3DrearrangementWidget::createPanel()
{
    QGroupBox *translationGroupBox = new QGroupBox(tr("Translation"));
    QGridLayout *translationLayout = new QGridLayout;
    translationLayout->addWidget(new QLabel(tr("Displacement (1st dim.)")),0,0,1,1);
    translationLayout->addWidget(d1DisplacementSpinBox,0,1,1,1);
    translationLayout->addWidget(new QLabel(tr("Displacement (2nd dim.)")),1,0,1,1);
    translationLayout->addWidget(d2DisplacementSpinBox,1,1,1,1);
    translationLayout->addWidget(new QLabel(tr("Displacement (Abscissa)")),2,0,1,1);
    translationLayout->addWidget(abscissaDisplacementSpinBox,2,1,1,1);
    translationLayout->addWidget(translationPushButton,3,0,1,2);
    translationGroupBox->setLayout(translationLayout);

    QGroupBox *xyzRotationGroupBox = new QGroupBox(tr("Rotation"));
    QGridLayout *xyzRotationLayout = new QGridLayout;
    xyzRotationLayout->addWidget(xyzRotationPushButton,0,0,1,1);
    xyzRotationGroupBox->setLayout(xyzRotationLayout);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(new QLabel(tr("Size (1st dimension)")),0,0,1,1);
    layout->addWidget(size1SpinBox,0,1,1,1);
    layout->addWidget(new QLabel(tr("Size (2nd dimension)")),1,0,1,1);
    layout->addWidget(size2SpinBox,1,1,1,1);
    layout->addWidget(translationGroupBox,2,0,1,2);
    layout->addWidget(xyzRotationGroupBox,3,0,1,2);

    setLayout(layout);
}

void S3DrearrangementWidget::CreateConnections()
{
    connect(translationPushButton,SIGNAL(clicked()),this,SLOT(performTranslation()));
    connect(xyzRotationPushButton,SIGNAL(clicked()),this,SLOT(performxyzRotation()));
}

void S3DrearrangementWidget::performTranslation()
{
    if(!isAncestorDefined()) return;
    if(ancestor()->FID_2D->FID.isEmpty()) return;

    bool ok;
    int size1=size1SpinBox->value();
    int size2=size2SpinBox->value();
    STranslation *translation = new STranslation;
    translation->d1Displacement = d1DisplacementSpinBox->value();
    translation->d2Displacement = d2DisplacementSpinBox->value();
    translation->abscissaDisplacement = abscissaDisplacementSpinBox->value();
    ok=translation->process(ancestor()->FID_2D,size1,size2);
    if(!ok)
    {
        QMessageBox::warning(this,"error",translation->errorMessage());
        delete translation;
        return;
    }

    ancestor()->plotters->update();
}

void S3DrearrangementWidget::performxyzRotation()
{
    if(!isAncestorDefined()) return;
    if(ancestor()->FID_2D->FID.isEmpty()) return;

    bool ok;
    int size1=size1SpinBox->value();
    int size2=size2SpinBox->value();
    SxyzRotation *xyzrotation = new SxyzRotation;
    ok=xyzrotation->process(ancestor()->FID_2D,size1,size2);
    if(!ok)
    {
        QMessageBox::warning(this,"error",xyzrotation->errorMessage());
        delete xyzrotation;
        return;
    }

    ancestor()->plotters->update();
}
