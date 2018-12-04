#include "dataProcess.h"
#include <QtWidgets>
#include <QLayout>
#include <QFileDialog>
#include <QFileInfo>
#include <QDir>




TProcess::TProcess(QObject *parent) :
    QObject(parent)
{
}


TprocessTerminal::TprocessTerminal(QWidget *parent):
    QWidget(parent)
{
    FID_2D = new TFID_2D;

    tw = new TransformWindow();
    tw->setWindowTitle(tr("Transformation"));
    tw->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Dialog);
//    tw->setWindowFlags(Qt::WindowStaysOnTopHint);
   // tw->resize(400,400);
    tw->setFixedSize(200,400);
  //  tw->setWindowFlags(Qt::Dialog); // We disable the minimize

    fp = new FIDPlotter;
    //fp->splitViewComboBox->setEnabled(false);
    fp->plotter->setBackgroundColor0(QColor("white"));
    fp->plotter->setBackgroundColor1(QColor("white"));
    QGridLayout *processLayout = new QGridLayout;
    processLayout->setMargin(0);
    processLayout->setSpacing(0);

      QHBoxLayout *hLayout = new QHBoxLayout;
      FIDReadButton = new QPushButton(tr("Open"));
      FIDSaveButton = new QPushButton(tr("Save")); FIDSaveButton->setEnabled(false);
      transformButton = new QPushButton(QIcon(":/images/stop.png"),"Transform"); transformButton->setEnabled(false);


      dataFilePathLabel = new QLabel;
      //FID2DLengthLabel = new QLabel;

      hLayout->addWidget(FIDReadButton);
      hLayout->addWidget(FIDSaveButton);
      hLayout->addWidget(dataFilePathLabel);
      hLayout->addStretch();
      hLayout->addWidget(transformButton);
      //hLayout->addWidget(FIDSelectSpinBox);
      //hLayout->addWidget(FID2DLengthLabel);

    processLayout->addLayout(hLayout,1,0);
    processLayout->addWidget(fp);
    setLayout(processLayout);

    connect(FIDReadButton,SIGNAL(clicked()),this,SLOT(readFID()));
    connect(FIDSaveButton,SIGNAL(clicked()),this,SLOT(saveFID()));

//    connect(FIDSelectSpinBox,SIGNAL(valueChanged(int)),this,SLOT(plotUpdate(int)));
    connect(transformButton,SIGNAL(clicked()),this,SLOT(showTransformPanel()));

    connect(tw,SIGNAL(FFTRequest()),this,SLOT(fft()));
    connect(tw,SIGNAL(iFFTRequest()),this,SLOT(ifft()));

}

TprocessTerminal::~TprocessTerminal()
{
    //tw->hide();
    if(!tw->isHidden()) tw->close();
}


void TprocessTerminal::showTransformPanel()
{
    tw->show();
//    tw->setFocus();
//    tw->setWindowFlags(Qt::WindowStaysOnTopHint);  // We bring the window to front, then...
//    tw->setWindowFlags(Qt::Dialog);  //
}

void TprocessTerminal::saveFID()
{
    QString path="~/";
    if(QDir(dataFilePath).exists()) path=dataFilePath+'/';
//    QString fileName=QFileDialog::getSaveFileName(this,tr("Save data"),path,tr("*.sm2p or *.sm2d"));
    QString fileName=QFileDialog::getSaveFileName(this,tr("Save data"),path,tr("*.opp or *.opd"));
    if (fileName.isEmpty()) return;

//    if(!FID_2D->Writesm2Files(fileName))
    if(!FID_2D->WriteopFiles(fileName))
    {
        QMessageBox::warning(this,tr(""),
                           tr("<p>Failed to save data. "
                              ));
        return;
    }
}

void TprocessTerminal::readFID()
{

    QString path="~/";
    if(QDir(dataFilePath).exists()) path=dataFilePath+'/';
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open data"),
                                                    path,
                                                    tr("Opencore files (*.opp *.opd *.sm2p *.sm2d)"));
    if (fileName.isEmpty()) {return;}

    dataFilePath=QFileInfo(fileName).absolutePath();
    QString fileExt=QFileInfo(fileName).suffix();

    if(0==QString::compare(fileExt,"sm2p") || 0==QString::compare(fileExt,"sm2d"))
    {
      if(!FID_2D->Readsm2Files(fileName))
      {
        //qDebug()<<FID_2D->errorMessage;
        QMessageBox::warning(this,tr(""), FID_2D->errorMessage);
        return;
      }
    }
    else if(0==QString::compare(fileExt,"opp") || 0==QString::compare(fileExt,"opd"))
    {
      if(!FID_2D->ReadopFiles(fileName))
      {
                    //qDebug()<<FID_2D->errorMessage;
         QMessageBox::warning(this,tr(""), FID_2D->errorMessage);
         return;
      }
    }
    else
    {
        QMessageBox::warning(this,tr(""), "." + fileExt + " is not supported.");
        return;
    }

    dataFilePathLabel->setText(dataFilePath);

    fp->plotter->xini=0;
    fp->plotter->xfin=FID_2D->al()-1;
    fp->plotter->setScale(1/(FID_2D->FID[0]->abs->absMax()));

//    plotter->plotter->setFID(FID_2D->FID[0]);
    currentFIDIndex=0;



    fp->setFID2D(FID_2D);
    fp->FIDSelectSpinBox->setMinimum(1);
    fp->FIDSelectSpinBox->setMaximum(FID_2D->FID.size());

    PlotSettings pset;

    pset.minX=0;
    pset.maxX=FID_2D->al();
    pset.minY=-FID_2D->FID.at(currentFIDIndex)->abs->max();
    pset.maxY=FID_2D->FID.at(currentFIDIndex)->abs->max();

    fp->plotter->setPlotSettings(pset);

    fp->realAction->setChecked(true);
    fp->imagAction->setChecked(true);
    fp->absAction->setChecked(true);

    plotUpdate();

    transformButton->setEnabled(true);
    FIDSaveButton->setEnabled(true);
}

void TprocessTerminal::plotUpdate()
{
  if(FID_2D->FID.at(FID_2D->currentFID())->domain==TFID::TimeDomain)
  {
    fp->realAction->setChecked(true);
    fp->imagAction->setChecked(true);
    fp->absAction->setChecked(true);
  }
  else
  {
      fp->realAction->setChecked(true);
      fp->imagAction->setChecked(false);
      fp->absAction->setChecked(false);
  }
    fp->update();
}

void TprocessTerminal::ifft()
{
    TIFFT a;
    if(!a.process(FID_2D,fidDomain::ToggleDomain)) {qDebug()<<a.errorMessage; return;}

   // fp->realAction->setChecked(true);
   // fp->imagAction->setChecked(false);
   // fp->absAction->setChecked(false);

    plotUpdate();

}

void TprocessTerminal::fft()
{
    TFFT a;
    a.setAxisDomain(fidDomain::ToggleDomain);
    if(!a.process(FID_2D)) {qDebug()<<a.errorMessage; return;}

   // fp->realAction->setChecked(true);
   // fp->imagAction->setChecked(false);
   // fp->absAction->setChecked(false);

    plotUpdate();

}

