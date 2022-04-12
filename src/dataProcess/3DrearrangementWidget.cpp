#include "3DrearrangementWidget.h"
#include "3Drearrangement.h"
#include "processPanelWidget.h"

#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>

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

    setCenterattheOriginPushButton = new QPushButton(tr("Set center data at the origin"));
    xyzRotationPushButton = new QPushButton(tr("xyz rotation"));
}

void S3DrearrangementWidget::createPanel()
{
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(new QLabel(tr("Size (1st dimension)")),0,0,1,1);
    layout->addWidget(size1SpinBox,0,1,1,1);
    layout->addWidget(new QLabel(tr("Size (2st dimension)")),1,0,1,1);
    layout->addWidget(size2SpinBox,1,1,1,1);
    layout->addWidget(setCenterattheOriginPushButton,2,0,1,2);
    layout->addWidget(xyzRotationPushButton,3,0,1,2);

    setLayout(layout);
}

void S3DrearrangementWidget::CreateConnections()
{
    connect(setCenterattheOriginPushButton,SIGNAL(clicked()),this,SLOT(performSetCenterattheOrigin()));
    connect(xyzRotationPushButton,SIGNAL(clicked()),this,SLOT(performxyzRotation()));
}

void S3DrearrangementWidget::performSetCenterattheOrigin()
{
    if(!isAncestorDefined()) return;
    if(ancestor()->FID_2D->FID.isEmpty()) return;

    bool ok;
    int size1=size1SpinBox->value();
    int size2=size2SpinBox->value();
    SSetCenterattheOrigin *SCatO = new SSetCenterattheOrigin;
    ok=SCatO->process(ancestor()->FID_2D,size1,size2);
    if(!ok)
    {
        QMessageBox::warning(this,"error",SCatO->errorMessage());
        delete SCatO;
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
