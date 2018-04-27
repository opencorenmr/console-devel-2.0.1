#include <QtWidgets>
#include <cmath>

#include "plotter.h"
#include "metricPrefix.h"
#include "math.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TPlotWindow::closeEvent(QCloseEvent *event)
{
    emit deleteRequest(this);
        event->accept();
}
//------------------------------------------------------------------------------
TFIDPlotters::TFIDPlotters(QWidget *parent): QWidget(parent)
{
    FFID2DSetted=false;


    FIDPlotters.append(new FIDPlotter);
    FIDPlotters.last()->setIDNumber(0);
    FIDPlotters.last()->IDLabel->setText("Plotter #"+QString::number(FIDPlotters.last()->IDNumber()));



    plotSplitters.append(new QSplitter);
    plotSplitters[plotSplitters.size()-1]->setHandleWidth(0);

    plotSplitters.append(new QSplitter);
    plotSplitters[plotSplitters.size()-1]->setHandleWidth(0);
    plotSplitters[plotSplitters.size()-2]->addWidget(plotSplitters[plotSplitters.size()-1]);


    plotSplitters[1]->addWidget(FIDPlotters[0]);
    connect(FIDPlotters[0],SIGNAL(splitRequest(FIDPlotter*,TFIDPlotters::PlotSplitMode)),
            this,SLOT(splitPlot(FIDPlotter*,TFIDPlotters::PlotSplitMode)));
    FN=1;

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->addWidget(plotSplitters[0]);
    setLayout(mainLayout);
}

void TFIDPlotters::setDevicePixelRatio(int r)
{
    FDevicePixelRatio=r;
    for(int k=0; k<FIDPlotters.size();k++)
        FIDPlotters[k]->setDevicePixelRatio(r);
}


void TFIDPlotters::setFFTEnabled(bool b)
{
  FisFFTEnabled=b;
  for(int k=0; k<FIDPlotters.size(); k++)
  {
      FIDPlotters[k]->FFTCheckBox->setEnabled(b);
      //if(!b) FIDPlotters[k]->FFTCheckBox->setVisible(false);
  }
}
//------------------------------------------------------------------------------
void TFIDPlotters::setFID2D(TFID_2D *f2)
{
//    qDebug() << QString(Q_FUNC_INFO);
    FID_2D=f2; FFID2DSetted=true;
//    qDebug() << QString(Q_FUNC_INFO)<<":" << FIDPlotters.size();

    for(int k=0; k<FIDPlotters.size();k++) FIDPlotters[k]->setFID2D(f2);
}
//------------------------------------------------------------------------------
void TFIDPlotters::update()
{
     for(int k=0; k<FIDPlotters.size();k++) FIDPlotters[k]->update();
}
//------------------------------------------------------------------------------
TFIDPlotters::~TFIDPlotters()
{
    while(!FIDPlotters.isEmpty()) FIDPlotters.removeLast();
    while(!plotSplitters.isEmpty()) plotSplitters.removeLast();
    while(!plotWindows.isEmpty())
    {
        plotWindows.last()->close();

       // qDebug() << QString(Q_FUNC_INFO) << "!";

        plotWindows.removeLast();
    }
}
//------------------------------------------------------------------------------
void TFIDPlotters::setBackgroundColor0(QColor col)
{
    FBackgroundColor0=col;
    for(int k=0; k<FIDPlotters.size();k++) FIDPlotters[k]->plotter->setBackgroundColor0(col);
}
//------------------------------------------------------------------------------
void TFIDPlotters::setBackgroundColor1(QColor col)
{
    FBackgroundColor1=col;
    for(int k=0; k<FIDPlotters.size();k++) FIDPlotters[k]->plotter->setBackgroundColor1(col);
}
//------------------------------------------------------------------------------
void TFIDPlotters::addAndCopyPlotter(FIDPlotter *fp)
{
    FIDPlotters.append(new FIDPlotter);
    FIDPlotters.last()->setIDNumber(FIDPlotters.size()-1);
    FIDPlotters.last()->IDLabel->setText("Plotter #"+QString::number(FIDPlotters.last()->IDNumber()));
    FIDPlotters.last()->FFTCheckBox->setEnabled(isFFTEnabled());

    int k=FIDPlotters.size()-1;
    FIDPlotters[k]->setFID2D(FID_2D);
    FIDPlotters[k]->setDevicePixelRatio(devicePixelRatio());
    FIDPlotters[k]->FIDSelectSpinBox
            ->setMaximum(fp->FIDSelectSpinBox->maximum());

    FIDPlotters[k]->FIDSelectSpinBox->setValue(fp->FIDSelectSpinBox->value());
    FIDPlotters[k]->plotter->xini=fp->plotter->xini;
    FIDPlotters[k]->plotter->xfin=fp->plotter->xfin;
    FIDPlotters[k]->plotter->setBackgroundColor0(backgroundColor0());
    FIDPlotters[k]->plotter->setBackgroundColor1(backgroundColor1());

    FIDPlotters[k]->plotterDetails->xFinSpinBox->setMaximum(FID_2D->al()-1);
    FIDPlotters[k]->plotterDetails->xIniSpinBox->setMaximum(FID_2D->al()-2);

    if(fp->fid2d->FID.size()>0)
    {
      FIDPlotters[k]->plotter->setFID(FID_2D->FID[FIDPlotters[k]->FIDSelectSpinBox->value()-1]);
      FIDPlotters[k]->plotter->adjustScale();
      FIDPlotters[k]->plotter->refresh();
    }

    connect(FIDPlotters[k],SIGNAL(splitRequest(FIDPlotter*,TFIDPlotters::PlotSplitMode)),
            this,SLOT(splitPlot(FIDPlotter*, TFIDPlotters::PlotSplitMode)));

}
//------------------------------------------------------------------------------
void TFIDPlotters::splitPlot(FIDPlotter *fp, TFIDPlotters::PlotSplitMode splitMode)
{


    QSplitter *parentSplitter = (QSplitter*)fp->parentWidget();

    if(splitMode==NewWindow)
    {

       plotWindows.append(new QWidget);
       plotWindows.last()->setAttribute(Qt::WA_DeleteOnClose);
       plotWindows.last()->setWindowFlags ( Qt::CustomizeWindowHint |
                                             Qt::WindowTitleHint |
                                             Qt::WindowMinMaxButtonsHint);
       plotWindows.last()->setWindowTitle("Opencore NMR Process");
       plotWindows.last()->show();
       //QWidget *newWindow = new QWidget;
       //newWindow->show();

       QVBoxLayout *mainLayout = new QVBoxLayout;
       mainLayout->setMargin(0);
       mainLayout->setSpacing(0);

       plotSplitters.append(new QSplitter);
       plotSplitters[plotSplitters.size()-1]->setHandleWidth(0);
       mainLayout->addWidget(plotSplitters[plotSplitters.size()-1]);

       plotSplitters.append(new QSplitter);
       plotSplitters[plotSplitters.size()-1]->setHandleWidth(0);
       plotSplitters[plotSplitters.size()-2]->addWidget(plotSplitters[plotSplitters.size()-1]);

       addAndCopyPlotter(fp);
       plotSplitters[plotSplitters.size()-1]->addWidget(FIDPlotters[FIDPlotters.size()-1]);

       plotWindows.last()->setLayout(mainLayout);

    //   emit splitPlotUpdated(FIDPlotters.size()-1);
       emit numberOfPlottersUpdated(FIDPlotters.size());

       return;
    }

    if(splitMode==RemoveSplit) // remove
    {
//       qDebug() << "parent:"
//                << QString::fromUtf8(parentSplitter->metaObject()->className())
//                << "grandParent: "
//                << QString::fromUtf8(parentSplitter->parentWidget()->metaObject()->className())
//                << "grandGrandParent: "
//                << QString::fromUtf8(parentSplitter->parentWidget()->parentWidget()->metaObject()->className());

        if(0!=strcmp(parentSplitter->parentWidget()->metaObject()->className(),"QSplitter"))
        {
            return;
        }

        QSplitter *grandParentSplitter = (QSplitter*) (parentSplitter->parentWidget());

        int sIndex=plotSplitters.indexOf(parentSplitter);
        int fIndex=FIDPlotters.indexOf(fp);

        if(fIndex==0) return; // we do not remove the 1st plotter

        if(fIndex>-1 && FIDPlotters.size()>1)
        {
           FIDPlotters[fIndex]->setParent(NULL);
           //FIDPlotters[fIndex]->deleteLater();
           FIDPlotters.removeAt(fIndex);
           //qDebug() << "removed view!";
        }
        if(parentSplitter->count()==0)
        {
           // qDebug()<<"!";
//            int sIndex=plotSplitters.indexOf(parentSplitter);
            plotSplitters[sIndex]->setParent(NULL);
            //plotSplitters[sIndex]->deleteLater();
            plotSplitters.removeAt(sIndex);

            //qDebug() << "removed splitter!";
        }

//        qDebug() << "grandParent splitter count "<< grandParentSplitter->count();
        if(grandParentSplitter->count()==0)
        {
//        qDebug()<<QString::fromUtf8(grandParentSplitter->parentWidget()->metaObject()->className());
            if(QString::fromUtf8(grandParentSplitter->parentWidget()->metaObject()->className())=="QWidget")
              grandParentSplitter->parentWidget()->close();
            else
            {
              //qDebug()<<QString::fromUtf8(grandParentSplitter->parentWidget()->parentWidget()->metaObject()->className());
              //  grandParentSplitter->close();

            }
        }
        else  // grandParentSplitter->count() should be 1
        {
            //uncleSplitter
           QSplitter *grandGrandParentSplitter = (QSplitter*) (grandParentSplitter->parent());
           QSplitter *uncleSplitter = (QSplitter*) (grandParentSplitter->children().at(0));
           uncleSplitter->setParent(grandGrandParentSplitter);

           int gIndex=plotSplitters.indexOf(grandParentSplitter);
           plotSplitters[gIndex]->setParent(NULL);
           plotSplitters.removeAt(gIndex);


        }


        for(int k=0; k<FIDPlotters.size(); k++)
        {
//            qDebug() << "4" << k;
            FIDPlotters[k]->setIDNumber(k);
            FIDPlotters[k]->IDLabel->setText("Plotter #"+QString::number(FIDPlotters.at(k)->IDNumber()));

        }

       // emit splitPlotUpdated(FIDPlotters.size()-1);
        emit numberOfPlottersUpdated(FIDPlotters.size());

        return;
    }

    Qt::Orientation ori=Qt::Horizontal;
    if(splitMode==Horizontal) ori=Qt::Horizontal;
    else if(splitMode==Vertical) ori=Qt::Vertical;

    parentSplitter->setOrientation(ori);

    plotSplitters.append(new QSplitter);
    plotSplitters[plotSplitters.size()-1]->setHandleWidth(0);

    parentSplitter->addWidget(plotSplitters[plotSplitters.size()-1]);
    fp->setParent(plotSplitters[plotSplitters.size()-1]);

    plotSplitters.append(new QSplitter);
    plotSplitters[plotSplitters.size()-1]->setHandleWidth(0);
    parentSplitter->addWidget(plotSplitters[plotSplitters.size()-1]);

    parentSplitter->setCollapsible(0,false);
    parentSplitter->setCollapsible(1,false);

    addAndCopyPlotter(fp);
    plotSplitters[plotSplitters.size()-1]->addWidget(FIDPlotters[FIDPlotters.size()-1]);

   // emit splitPlotUpdated(FIDPlotters.size()-1);
    emit numberOfPlottersUpdated(FIDPlotters.size());

}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
plotterDetailsWidget::plotterDetailsWidget(QWidget *parent): QWidget(parent)
{
    hide();
    setFixedWidth(120);

    detailsComboBox = new QComboBox;
    detailsComboBox->clear();
    detailsComboBox->addItems(QStringList()<<"Cursor"<<"Plot range");

    realLineEdit=new QLineEdit; realLineEdit->setReadOnly(true);
    imagLineEdit=new QLineEdit; imagLineEdit->setReadOnly(true);
    absLineEdit=new QLineEdit; absLineEdit->setReadOnly(true);
    phaseLineEdit=new QLineEdit; phaseLineEdit->setReadOnly(true);
    xPosLineEdit=new QLineEdit; xPosLineEdit->setReadOnly(false);
    xValLineEdit=new QLineEdit; xValLineEdit->setReadOnly(true);

    deltaInfoTextEdit=new QPlainTextEdit; deltaInfoTextEdit->clear();

    xIniSpinBox=new QSpinBox; xIniSpinBox->setMinimum(0);xIniSpinBox->setMaximum(0);
    xFinSpinBox=new QSpinBox; xFinSpinBox->setMinimum(0);xFinSpinBox->setMaximum(0);
    xIniValLineEdit=new QLineEdit; xIniValLineEdit->setReadOnly(true);
    xFinValLineEdit=new QLineEdit; xFinValLineEdit->setReadOnly(true);
    xIniUnitLabel=new QLabel;
    xFinUnitLabel=new QLabel;

    //xPosSpinBox=new QSpinBox; xPosSpinBox->setMinimum(0);

    QStackedLayout *stackedLayout = new QStackedLayout;


    QWidget *widget1 = new QWidget;
    QVBoxLayout *vLayout1 = new QVBoxLayout(widget1);
    //vLayout1->setSpacing(0);
    vLayout1->setMargin(0);
    vLayout1->addWidget(new QLabel(tr("Position")));
    vLayout1->addWidget(xPosLineEdit);
    //vLayout1->addWidget(xPosSpinBox);
    vLayout1->addWidget(new QLabel(tr("X value")));
    vLayout1->addWidget(xValLineEdit);
    vLayout1->addWidget(new QLabel(tr("In-phase")));
    vLayout1->addWidget(realLineEdit);
    vLayout1->addWidget(new QLabel(tr("Quadrature")));
    vLayout1->addWidget(imagLineEdit);
    vLayout1->addWidget(new QLabel(tr("Abs. value")));
    vLayout1->addWidget(absLineEdit);
    vLayout1->addWidget(new QLabel(tr("Phase")));
    vLayout1->addWidget(phaseLineEdit);
    vLayout1->addWidget(deltaInfoTextEdit);
    //vLayout1->addStretch();

    QWidget *widget2 = new QWidget;
    QVBoxLayout *vLayout2 = new QVBoxLayout(widget2);
    vLayout2->setMargin(0);
    vLayout2->addWidget(new QLabel("Initial X"));
    vLayout2->addWidget(xIniSpinBox);
      QHBoxLayout *hLayout1=new QHBoxLayout;
      hLayout1->addWidget(xIniValLineEdit); hLayout1->addWidget(xIniUnitLabel);
    vLayout2->addLayout(hLayout1);
    vLayout2->addWidget(new QLabel("Final X"));
    vLayout2->addWidget(xFinSpinBox);
      QHBoxLayout *hLayout2=new QHBoxLayout;
      hLayout2->addWidget(xFinValLineEdit); hLayout2->addWidget(xFinUnitLabel);
    vLayout2->addLayout(hLayout2);
    vLayout2->addStretch();
//    vLayout2->addWidget(new QLabel("soon."));

    stackedLayout->addWidget(widget1);
    stackedLayout->addWidget(widget2);

    connect(detailsComboBox,SIGNAL(currentIndexChanged(int)),stackedLayout,SLOT(setCurrentIndex(int)));
    detailsComboBox->setCurrentIndex(0);

    connect(xIniSpinBox,SIGNAL(valueChanged(int)),this,SLOT(onXIniSpinBoxValueChanged()));
    connect(xFinSpinBox,SIGNAL(valueChanged(int)),this,SLOT(onXFinSpinBoxValueChanged()));

    connect(xPosLineEdit,SIGNAL(returnPressed()),this,SLOT(onXPosLineEditReturnPressed()));
    //connect(xIniValLineEdit,SIGNAL(returnPressed()),this,SLOT(onXIniValLineEditReturnPressed()));

    QVBoxLayout *mainLayout = new QVBoxLayout;
//    mainLayout->setMargin(0);
//    mainLayout->setSpacing(0);
    mainLayout->addWidget(detailsComboBox);
    mainLayout->addLayout(stackedLayout);
   // mainLayout->addWidget(widget1);
    setLayout(mainLayout);


}
//------------------------------------------------------------------------------
void plotterDetailsWidget::onXPosLineEditReturnPressed()
{
    bool ok;

    int xi = xPosLineEdit->text().toInt(&ok);
    if(!ok)
    {
        xPosLineEdit->setText(previousXPos);
        return;
    }
    else
    {
        emit xCursorPositionUpdateRequest(xi);
    }

}
//------------------------------------------------------------------------------
void plotterDetailsWidget::onXIniValLineEditReturnPressed()
{

  bool ok;

  int xi = xIniValLineEdit->text().toDouble(&ok);
  if(!ok)
  {
      QMessageBox::warning(this,"error","Invalid Number.");
      return;
  }

  if(xi<0)
  {
      QMessageBox::warning(this,"error","Out of Range.");
      return;
  }

 // qDebug() << xi;

}
//------------------------------------------------------------------------------
void plotterDetailsWidget::onXFinValLineEditReturnPressed()
{


}

//------------------------------------------------------------------------------
void plotterDetailsWidget::onXIniSpinBoxValueChanged()
{
    int xi=xIniSpinBox->value();
    xFinSpinBox->setMinimum(xi);
    emit xPlotRangeUpdateRequest(xi,xFinSpinBox->value());
    //qDebug()<<QString(Q_FUNC_INFO);
}
//------------------------------------------------------------------------------
void plotterDetailsWidget::onXFinSpinBoxValueChanged()
{
    int xf=xFinSpinBox->value();
    xIniSpinBox->setMaximum(xf);
    emit xPlotRangeUpdateRequest(xIniSpinBox->value(),xf);

}
//------------------------------------------------------------------------------
void plotterDetailsWidget::setCursorInfo(QStringList sl)
{
    if(sl.size()<6) return;
    xPosLineEdit->setText(sl.at(0)); // xPosSpinBox->setValue(sl.at(0).toInt());
    xValLineEdit->setText(sl.at(1));
    realLineEdit->setText(sl.at(2));
    imagLineEdit->setText(sl.at(3));
    absLineEdit->setText(sl.at(4));
    phaseLineEdit->setText(sl.at(5));

    previousXPos=sl.at(0);

//    qDebug() << FPreviousCursorInfo << FCurrentCursorInfo;
}
//------------------------------------------------------------------------------
void plotterDetailsWidget::setPlotRangeInfo(QStringList sl)
{
    if(sl.size()<6) return;
    disconnect(xIniSpinBox,SIGNAL(valueChanged(int)),this,SLOT(onXIniSpinBoxValueChanged()));
    disconnect(xFinSpinBox,SIGNAL(valueChanged(int)),this,SLOT(onXFinSpinBoxValueChanged()));
    xIniSpinBox->setValue(sl.at(0).toInt());
    xIniValLineEdit->setText(sl.at(1));
    xIniUnitLabel->setText(sl.at(2));
    xFinSpinBox->setValue(sl.at(3).toInt());
    xFinValLineEdit->setText(sl.at(4));
    xFinUnitLabel->setText(sl.at(5));
    connect(xIniSpinBox,SIGNAL(valueChanged(int)),this,SLOT(onXIniSpinBoxValueChanged()));
    connect(xFinSpinBox,SIGNAL(valueChanged(int)),this,SLOT(onXFinSpinBoxValueChanged()));
}
//------------------------------------------------------------------------------
FIDPlotter::FIDPlotter(QWidget *parent): QWidget(parent)
//FIDPlotter::FIDPlotter(QWidget *parent): Plotter(parent)
{
    plotter = new Plotter;
    fid2d = new TFID_2D;
    Ffid2dSetted = false;

    setPath("~/"); // path for export data/image

    createActions();
    createToolBar();

    QWidget *widget1 = new QWidget;
    QHBoxLayout *hLayout = new QHBoxLayout(widget1);
    hLayout->setMargin(0);
    hLayout->setSpacing(0);

    hLayout->addWidget(toolBar);
    hLayout->addStretch();

    vcxLabel = new QLabel;
    hLayout->addWidget(vcxLabel);
    connect(plotter,SIGNAL(yRealSignal(QString)),vcxLabel,SLOT(setText(QString)));

    QWidget *widget2 = new QWidget;
    QHBoxLayout *hLayout2 = new QHBoxLayout(widget2);
    hLayout2->setMargin(0);
    hLayout2->setSpacing(0);
    hLayout2->addWidget(toolBar2);
    hLayout2->addWidget(plotterDetails); plotterDetails->hide();
    hLayout2->addWidget(plotter);
    //hLayout2->addWidget(processPanelWidget); processPanelWidget->hide();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(widget1);
    mainLayout->addWidget(widget2);
    setLayout(mainLayout);

    connect(plotter,SIGNAL(rubberBandActionRequest(bool)),rubberBandAction,SLOT(setChecked(bool)));
    connect(FIDSelectSpinBox,SIGNAL(valueChanged(int)),this,SLOT(onFIDSelectSpinBoxChanged(int)));

    connect(plotter,SIGNAL(cursorInfo(QStringList)),plotterDetails,SLOT(setCursorInfo(QStringList)));
    connect(plotter,SIGNAL(deltaInfo(QString)),plotterDetails->deltaInfoTextEdit,SLOT(setPlainText(QString)));
    connect(plotter,SIGNAL(plotRangeInfo(QStringList)),plotterDetails,SLOT(setPlotRangeInfo(QStringList)));
    connect(plotterDetails,SIGNAL(xPlotRangeUpdateRequest(int,int)),plotter,SLOT(updatePlotRange(int,int)));
    connect(plotterDetails,SIGNAL(xCursorPositionUpdateRequest(int)),plotter,SLOT(updateXCursorPosition(int)));

    connect(penWidthSpinBox,SIGNAL(valueChanged(int)),plotter,SLOT(setPenWidth(int)));

    connect(FFTCheckBox,SIGNAL(toggled(bool)),this,SLOT(update()));
    connect(processCheckBox,SIGNAL(toggled(bool)),this,SLOT(update()));
   // connect(phase0SpinBox,SIGNAL(valueChanged(int)),this,SLOT(update()));

}
//------------------------------------------------------------------------------
void FIDPlotter::createActions()
{

    FFTCheckBox = new QCheckBox(tr("FFT")); FFTCheckBox->setChecked(false);
    processCheckBox = new QCheckBox(tr("Process")); processCheckBox->setChecked(false);
    formatComboBox = new QComboBox;
    formatComboBox->addItems(QStringList()<<tr("Cartesian")<<tr("Polar"));
    formatComboBox->setCurrentIndex(0);
    plotter->setPlotFormat(Plotter::CartesianPlot);
    connect(formatComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(setPlotFormat()));

    //testLabel = new QLabel;
    splitViewComboBox = new QComboBox;
    splitViewComboBox->addItems(QStringList()<<tr("Split")
                                << tr("Horizontal") << tr("Vertical") << tr("New Window") << tr("Close"));
    splitViewComboBox->view()->setBaseSize(10,20);
    //QSizePolicy sp = splitViewComboBox->view()->sizePolicy();
    //sp.setHorizontalPolicy(QSizePolicy::MinimumExpanding);
    splitViewComboBox->view()->setMinimumWidth(splitViewComboBox->sizeHint().width()+30);
    connect(splitViewComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(onSplitViewRequest(int)));

    FIDSelectSpinBox = new QSpinBox;
    FIDSelectSpinBox->setMaximum(1);
    FIDSelectSpinBox->setMinimum(1);
    FID2DLengthLabel = new QLabel;

    IDLabel = new QLabel;

    //exportAction = new QAction(QIcon(":/images/export.png"),tr("export"),this);
    //connect(exportAction,SIGNAL(triggered(bool)),this,SLOT(exportAsci()));

    penWidthSpinBox = new QSpinBox;
    penWidthSpinBox->setMinimum(1);
    penWidthSpinBox->setMaximum(10);

    scaleComboBox = new QComboBox;
    scaleComboBox->addItems(QStringList() << tr("auto") << tr("manual"));
    plotter->scaleSetting=Plotter::DataScale;
    connect(scaleComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(setPlotScaleOption()));

    vCursorAction = new QAction(QIcon(":/images/cursor1.png"),tr("vertical cursor"),this);
    vCursorAction->setCheckable(true);
    connect(vCursorAction,SIGNAL(triggered(bool)),this,SLOT(setVCursor(bool)));

    exportAsciiAction = new QAction(QIcon(":/images/export.png"),tr("export ascii data"),this);
    connect(exportAsciiAction,SIGNAL(triggered(bool)),this,SLOT(exportAscii()));

    exportImageAction = new QAction(QIcon(":/images/export2.png"),tr("export image"),this);
    connect(exportImageAction,SIGNAL(triggered(bool)),this,SLOT(exportImage()));

    rubberBandAction = new QAction(QIcon(":/images/rubberBand.png"),tr("rubber band"),this);
    rubberBandAction->setCheckable(true);
    connect(rubberBandAction,SIGNAL(triggered(bool)),this,SLOT(setRubberBand(bool)));

    realAction = new QAction(QIcon(":/images/real.png"),tr("show/hide real part"),this);
    realAction->setCheckable(true); realAction->setChecked(true);
    imagAction = new QAction(QIcon(":/images/imag.png"),tr("show/hide Imag part"),this);
    imagAction->setCheckable(true); imagAction->setChecked(true);
    absAction = new QAction(QIcon(":/images/abs.png"),tr("show/hide Abs part"),this);
    absAction->setCheckable(true); absAction->setChecked(false);
    setPlotItemOption();

    thickkerAction = new QAction(QIcon(":/images/thickker.png"),tr("thicker line"),this);
    thinnerAction = new QAction(QIcon(":/images/thinner.png"),tr("thinner line"),this);
    connect(thickkerAction,SIGNAL(triggered(bool)),this,SLOT(thickkerLine()));
    connect(thinnerAction,SIGNAL(triggered(bool)),this,SLOT(thinnerLine()));

    bitLineAction = new QAction(QIcon(":/images/bitLines.png"),tr("show/hide bit lines"),this);
    bitLineAction->setCheckable(true); bitLineAction->setChecked(true);


    connect(realAction,SIGNAL(toggled(bool)),this,SLOT(setPlotItemOption()));
    connect(imagAction,SIGNAL(toggled(bool)),this,SLOT(setPlotItemOption()));
    connect(absAction,SIGNAL(toggled(bool)),this,SLOT(setPlotItemOption()));
    connect(bitLineAction,SIGNAL(toggled(bool)),this,SLOT(setBitLineOption()));


    hExpandAction = new QAction(QIcon(":/images/hExpand.png"),tr("horizontal expand"),this);
    hShrinkAction = new QAction(QIcon(":/images/hShrink.png"),tr("horizontal shrink"),this);
    moveRightAction = new QAction(QIcon(":/images/right2.png"), tr("move right"),this);
    moveLeftAction = new QAction(QIcon(":/images/left2.png"),tr("move left"),this);

    hExpandAction->setAutoRepeat(true);
    hShrinkAction->setAutoRepeat(true);
    moveRightAction->setAutoRepeat(true);
    moveLeftAction->setAutoRepeat(true);
    connect(hExpandAction,SIGNAL(triggered(bool)),plotter,SLOT(hExpand()));
    connect(hShrinkAction,SIGNAL(triggered(bool)),plotter,SLOT(hShrink()));
    connect(moveRightAction,SIGNAL(triggered(bool)),plotter,SLOT(moveRight()));
    connect(moveLeftAction,SIGNAL(triggered(bool)),plotter,SLOT(moveLeft()));

    plotterDetails = new plotterDetailsWidget(this);
    //processPanelWidget = new TProcessPanelWidget(this);


}


//------------------------------------------------------------------------------
void FIDPlotter::createToolBar()
{
    toolBar = new QToolBar(this);
    toolBar->setOrientation(Qt::Horizontal);
    toolBar->setIconSize(QSize(20,20));
    toolBar->setFixedHeight(25);

    //toolBar->addAction(hExpandAction);
    //toolBar->addAction(hShrinkAction);
    //toolBar->addAction(moveLeftAction);
    //toolBar->addAction(moveRightAction);
    //toolBar->addSeparator();
    toolBar->addWidget(IDLabel);
    toolBar->addWidget(splitViewComboBox); splitViewComboBox->setFixedWidth(60);
    toolBar->addSeparator();
    toolBar->addWidget(new QLabel(tr("Scale")));
    toolBar->addWidget(scaleComboBox);scaleComboBox->setFixedWidth(80);
    toolBar->addSeparator();
    toolBar->addWidget(formatComboBox);
    toolBar->addSeparator();
    toolBar->addWidget(FIDSelectSpinBox);
    toolBar->addWidget(FID2DLengthLabel);
    toolBar->addSeparator();
    toolBar->addWidget(FFTCheckBox);
   // toolBar->addWidget(processCheckBox);
   // toolBar->addWidget(new QLabel(tr("phase")));
   // toolBar->addWidget(phase0SpinBox);
   // toolBar->addSeparator();
   // toolBar->addAction(exportAction);


    toolBar2 = new QToolBar(this);
    toolBar2->setOrientation(Qt::Vertical);
    toolBar2->setIconSize(QSize(20,20));
    toolBar2->setFixedWidth(25);
    toolBar2->addAction(rubberBandAction);rubberBandAction->setChecked(false);setRubberBand(false);
    toolBar2->addAction(vCursorAction); vCursorAction->setChecked(false);setVCursor(false);plotter->setVCursorEnabled(false);
    toolBar2->addAction(bitLineAction); bitLineAction->setChecked(false); setBitLineOption();
    toolBar2->addSeparator();
    //toolBar2->addAction(rubberBandAction); rubberBandAction->setChecked(false);setRubberBand(false);
    toolBar2->addAction(realAction);
    toolBar2->addAction(imagAction);
    toolBar2->addAction(absAction);
    toolBar2->addSeparator();
    toolBar2->addAction(thickkerAction);
    toolBar2->addAction(thinnerAction);
    //toolBar2->addWidget(penWidthSpinBox);
    toolBar2->addSeparator();
    toolBar2->addAction(exportAsciiAction);
    toolBar2->addAction(exportImageAction);
    toolBar2->addSeparator();

}
//------------------------------------------------------------------------------
void FIDPlotter::thickkerLine()
{
  if(penWidthSpinBox->value()>=penWidthSpinBox->maximum()) return;
  penWidthSpinBox->setValue(penWidthSpinBox->value()+1);
  plotter->refresh();

}

//------------------------------------------------------------------------------
void FIDPlotter::thinnerLine()
{
    if(penWidthSpinBox->value()<=1) return;
    penWidthSpinBox->setValue(penWidthSpinBox->value()-1);
    plotter->refresh();
}

//------------------------------------------------------------------------------
void FIDPlotter::update()
{
//qDebug() << QString(Q_FUNC_INFO) << "0";
    //if(processCheckBox->isChecked()) processPanelWidget->show(); else processPanelWidget->hide();

    if(!isFID2DSetted())
    {  //qDebug() << QString(Q_FUNC_INFO) << "!isFIDSetted";
        return;
    }
    if(fid2d->FID.size() < 1)
    {  //qDebug() << QString(Q_FUNC_INFO) <<"size<1 ";
        return;
    }
  //  qDebug() << QString(Q_FUNC_INFO) << "1";

    FIDSelectSpinBox->setMaximum(fid2d->FID.size());
   // if(fid2d->FID.size() < FIDSelectSpinBox->value()) FIDSelectSpinBox->setMaximum(fid2d->FID.size());

    FID2DLengthLabel->setText("/"+QString::number(fid2d->FID.size()));
    plotter->setFID(fid2d->FID[FIDSelectSpinBox->value()-1]);
    plotterDetails->xFinSpinBox->setMaximum(fid2d->FID[FIDSelectSpinBox->value()-1]->al()-1);
    plotterDetails->xIniSpinBox->setMaximum(fid2d->FID[FIDSelectSpinBox->value()-1]->al()-1);

    if(FFTCheckBox->isChecked())
    {
        plotter->localFID->setAL(fid2d->FID[FIDSelectSpinBox->value()-1]->al());
        plotter->localFID->setDW(fid2d->FID[FIDSelectSpinBox->value()-1]->dw());
        for(int k=0; k<plotter->localFID->al(); k++)
        {
            plotter->localFID->real->sig[k]=fid2d->FID[FIDSelectSpinBox->value()-1]->real->sig.at(k);
            plotter->localFID->imag->sig[k]=fid2d->FID[FIDSelectSpinBox->value()-1]->imag->sig.at(k);
        }
        plotter->localFID->updateAbs();

        fidDomain::process(plotter->localFID,fidDomain::TimeDomain);
        TFFT fft;
        fft.setAxisDomain(fidDomain::FrequencyDomain);
        fft.process(plotter->localFID);

        plotter->localFID->setEmpty(false);
        plotter->setFID(plotter->localFID);

    }
    else
    {
      //
      //
    }

    //plotter->localFID->rotate(phase0SpinBox->value());
    //qDebug() << QString(Q_FUNC_INFO) << phase0SpinBox->value();

    plotter->adjustScale();
    plotter->refresh();
}
//------------------------------------------------------------------------------
void FIDPlotter::onSplitViewRequest(int i)
{
    if(i==0) return;
    splitViewComboBox->setCurrentIndex(0);
    TFIDPlotters::PlotSplitMode sMode;

    switch(i){
      case 1: sMode=TFIDPlotters::Horizontal; break;
      case 2: sMode=TFIDPlotters::Vertical; break;
      case 3: sMode=TFIDPlotters::NewWindow; break;
      default: sMode=TFIDPlotters::RemoveSplit; break;
    }

    emit splitRequest(this,sMode);
}
//------------------------------------------------------------------------------
void FIDPlotter::onFIDSelectSpinBoxChanged(int i)
{
    if(!isFID2DSetted()) return;
    if(!plotter->fidSetted) return;

    plotter->setFID(fid2d->FID[i-1]);
    plotter->adjustScale();
    plotter->refresh();
}
//------------------------------------------------------------------------------
void FIDPlotter::exportAscii()
{
    if(!isFID2DSetted()) return;
    if(!plotter->fidSetted) return;

    QString fileName= QFileDialog::getSaveFileName(this,"Export Ascii Data", path() , "dat(*.dat)");

//    QString fileName = QFileDialog::getSaveFileName();
    if(fileName.isEmpty()) return;

    if(!plotter->fid->exportAscii(fileName,plotter->xini,plotter->xfin))
    {
        QMessageBox::warning(this,"error","failed to export ascii data.");
    }

    QFileInfo fInfo(fileName);
    setPath(fInfo.absolutePath());

}


void FIDPlotter::exportImage()
{
    if(!isFID2DSetted()) return;
    if(!plotter->fidSetted) return;

    QString fileName= QFileDialog::getSaveFileName(this, "Export Image", path(), "PNG(*.png)");
   // QString fileName = QFileDialog::getSaveFileName();
    if(fileName.isEmpty()) return;

    QFile file(fileName);
    file.open(QIODevice::WriteOnly);
    plotter->pixmap.save(&file, "PNG");

    QFileInfo fInfo(fileName);
    setPath(fInfo.absolutePath());

}

//------------------------------------------------------------------------------
void FIDPlotter::setVCursor(bool b)
{
    if(b) plotterDetails->show(); else plotterDetails->hide();
    if(!isFID2DSetted()) return;
    if(!plotter->fidSetted) return;

   //if(plotterDetails->detailsComboBox->currentIndex()==0) // cursor
   // {
        plotter->setVCursorEnabled(b);
   // }
}
//------------------------------------------------------------------------------
void FIDPlotter::setRubberBand(bool b)
{
//    rubberBandAction->setChecked(true);
    plotter->setRubberBandEnabled(b);
}
//------------------------------------------------------------------------------
void FIDPlotter::setDevicePixelRatio(int r)
{
    FDevicePixelRatio=r;
    plotter->setDevicePixedRatio(r);
}
//------------------------------------------------------------------------------
void FIDPlotter::setBitLineOption()
{
   plotter->setBitLineIsShown(bitLineAction->isChecked());
   plotter->refresh();
}
//------------------------------------------------------------------------------
void FIDPlotter::setPlotItemOption()
{
    if(realAction->isChecked()) plotter->plotItemOption << Plotter::plotReal;
                           else plotter->plotItemOption.remove(Plotter::plotReal);
    if(imagAction->isChecked()) plotter->plotItemOption << Plotter::plotImag;
                           else plotter->plotItemOption.remove(Plotter::plotImag);
    if(absAction->isChecked()) plotter->plotItemOption << Plotter::plotAbs;
                           else plotter->plotItemOption.remove(Plotter::plotAbs);

    plotter->refresh();
}
//------------------------------------------------------------------------------
void FIDPlotter::setPlotFormat()
{
  if(formatComboBox->currentIndex()==0)
      plotter->setPlotFormat(Plotter::CartesianPlot);
  else
      plotter->setPlotFormat(Plotter::PolarPlot);
  plotter->refresh();
}
//------------------------------------------------------------------------------
void FIDPlotter::setPlotScaleOption()
{
//    if(scaleComboBox->currentIndex()==0) // receiver scale
//      plotter->scaleSetting=Plotter::ReceiverScale;
    if (scaleComboBox->currentIndex()==0) // data scale
      plotter->scaleSetting=Plotter::DataScale;
    else // (scaleComboBox->currentIndex()==2)  // manual scale
      plotter->scaleSetting=Plotter::ManualScale;

    plotter->adjustScale();
    plotter->refresh();
}
//------------------------------------------------------------------------------
Plotter::Plotter(QWidget *parent) :
    QWidget(parent)
{

  localFID=new TFID(4);
  FVCursorXPosition=0;
  setBackgroundRole(QPalette::Dark);
  setAutoFillBackground(true);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  setFocusPolicy(Qt::StrongFocus);
  setRubberBandIsShown(false);
  setVCursorIsShown(false);

  setRubberBandEnabled(false);
  fidSetted=false;
  scaleSetting=ReceiverScale;
  setScale(1.0); // will be corrected later (Takeda)

  setPlotSettings(PlotSettings());

  setPlotFormat(CartesianPlot);

  setPenWidth(1);

  // default background gradation color
  setBackgroundColor0(QColor("midnightblue"));
  setBackgroundColor1(QColor("black"));

  setVOffset(0);
  vCenter=0.0;
  plotOffset=0.0;

  Margin=30;
  topMargin=0;
  bottomMargin=40;
  leftMargin=0;
  rightMargin=0;

  plotItemOption << plotReal << plotImag << plotAbs;

}
//------------------------------------------------------------------------------
Plotter::~Plotter()
{
   delete localFID;
}
//------------------------------------------------------------------------------
double Plotter::tic(double min, double max)
{
    double span=fabs(max-min);
    double unit=1.0;
    if(span<=0) return 0.0;
    else
    {
        while (span>=10.0) {span/=10.0; unit*=10.0;}
        while (span<=1.0) {span*=10.0; unit/=10.0;}
    }
    if(span>8.0) return 2.0*unit;
    else if(span>4.0) return unit;
    else if(span>2.0) return 0.5*unit;
    else return 0.25*unit;
}
//------------------------------------------------------------------------------
void Plotter::setPlotSettings(const PlotSettings &settings)
{
    zoomStack.clear();
    zoomStack.append(settings);
    curZoom=0;
//    zoomInButton->hide();
//    zoomOutButton->hide();
    refreshPixmap();
}
//------------------------------------------------------------------------------
void Plotter::zoomOut()
{
    if(curZoom>0)
    {
        --curZoom;
//        zoomOutButton->setEnabled(curZoom>0);
//        zoomInButton->setEnabled(true);
//        zoomInButton->show();
        refreshPixmap();
    }
}
//------------------------------------------------------------------------------
void Plotter::zoomIn()
{
    if(curZoom<zoomStack.count()-1)
    {
        ++curZoom;
//        zoomInButton->setEnabled(curZoom<zoomStack.count()-1);
//        zoomOutButton->setEnabled(true);
//        zoomOutButton->show();
        refreshPixmap();
    }
}
//------------------------------------------------------------------------------
void Plotter::setCurveData(int id, const QVector<QPointF> &data)
{
    curveMap[id]=data;
    setX();
    refreshPixmap();
}
//------------------------------------------------------------------------------
void Plotter::clearCurve(int id)
{
    curveMap.remove(id);
    setX();
    refreshPixmap();
}
//------------------------------------------------------------------------------
QSize Plotter::minimumSizeHint() const
{
    return QSize(6*Margin,4*Margin);
}
//------------------------------------------------------------------------------
QSize Plotter::sizeHint() const
{
    return QSize(12*Margin,8*Margin);
}
//------------------------------------------------------------------------------
void Plotter::paintEvent(QPaintEvent * /* event */)
{
    QStylePainter painter(this);
    painter.drawPixmap(0,0,pixmap);

    if(rubberBandIsShown())
    {
        painter.setPen(Qt::gray);
        painter.drawRect(rubberBandRect.normalized().adjusted(0,0,-1,-1));
    }

    if(hasFocus())
    {
        QStyleOptionFocusRect option;
        option.initFrom(this);
        option.backgroundColor=palette().dark().color();
        painter.drawPrimitive(QStyle::PE_FrameFocusRect,option);
    }

}
//------------------------------------------------------------------------------
void Plotter::resizeEvent(QResizeEvent * /* event */)
{
    /*
    int x = width() - (zoomInButton->width() + zoomOutButton->width() + 10);
    zoomInButton->move(x,5);
    zoomOutButton->move(x+zoomInButton->width()+5,5);

    realButton->move(5,5);
    imagButton->move(realButton->width()+5,5);
    absButton->move(realButton->width()+imagButton->width()+5,5);
*/

    refreshPixmap();
}
//------------------------------------------------------------------------------
void Plotter::mousePressEvent(QMouseEvent *event)
{
    if(!fidSetted) return;
    QRect rect(leftMargin,topMargin,width()-(leftMargin+rightMargin),height()-(topMargin+bottomMargin));
    if(!rect.contains(event->pos()))return;

    if(event->button()==Qt::LeftButton)
    {
        //qDebug() << "1";
        if(rubberBandEnabled())
        {
           // qDebug() << "2";
            setRubberBandIsShown(true);
            rubberBandRect.setTopLeft(event->pos());
            rubberBandRect.setBottomRight(event->pos());
            updateRubberBandRegion();
            setCursor(Qt::CrossCursor);
        }
        else
        {
            int plotWidth=width()-(leftMargin+rightMargin);
            int xPos=xini + (xfin-xini)*(event->pos().x()-leftMargin)/plotWidth;
            emit clickedXPosition(xPos);
            if(vCursorEnabled())
            {
                setVCursorIsShown(true);
                setVCursorXPosition(xPos);
                refreshPixmap();
            }

        }

    }
}
//------------------------------------------------------------------------------
void Plotter::mouseMoveEvent(QMouseEvent *event)
{
    if(rubberBandIsShown())
    {
        updateRubberBandRegion();
        rubberBandRect.setBottomRight(event->pos());
        updateRubberBandRegion();
    }
}
//------------------------------------------------------------------------------
void Plotter::mouseReleaseEvent(QMouseEvent *event)
{
    if((event->button()==Qt::LeftButton) && rubberBandIsShown())
    {
        setRubberBandIsShown(false);
        updateRubberBandRegion();
        unsetCursor();

        QRect rect=rubberBandRect.normalized();
        if(rect.width()<4 || rect.height()<4) return;
        rect.translate(-leftMargin,-topMargin);

        PlotSettings prevSettings=zoomStack[curZoom];
        PlotSettings settings;
        double dx=prevSettings.spanX() / (width()-(leftMargin+rightMargin));
        double dy=prevSettings.spanY() / (height()-(topMargin+bottomMargin));
        settings.minX=prevSettings.minX + dx*rect.left();
        settings.maxX=prevSettings.minX + dx*rect.right();

        if(scaleSetting==ManualScale)
        {
        settings.minY=prevSettings.maxY - dy*rect.bottom();
        settings.maxY=prevSettings.maxY - dy*rect.top();
      //  settings.adjust();
        }

      //  zoomStack.resize(curZoom+1);
        zoomStack.append(settings);


        int oxini=xini;
        int oxfin=xfin;
        int plotWidth=width()-(leftMargin+rightMargin);

        xini=oxini+(oxfin-oxini)*((double)rect.left()/plotWidth);
        xfin=oxini+(oxfin-oxini)*((double)rect.right()/plotWidth);


        zoomIn();
        setRubberBandEnabled(false);
        emit rubberBandActionRequest(false);

    }
}
//------------------------------------------------------------------------------
void Plotter::hExpand()
{
  if(!fidSetted) return;
//  int dx;
  int oxini,oxfin;
  oxini=xini;oxfin=xfin;
  xini=oxini+0.2*(oxfin-oxini);
  xfin=oxfin-0.2*(oxfin-oxini);
  if(xini>=xfin) {xini=oxini; xfin=oxfin;}
  refreshPixmap();
}
void Plotter::hShrink()
{
    if(!fidSetted) return;
  //  int dx;
    int oxini,oxfin;
    oxini=xini;oxfin=xfin;
    xini=oxini-0.2*(oxfin-oxini);
    xfin=oxfin+0.2*(oxfin-oxini);
    if(xini<0) {xini=0;}
    if(xfin>fid->al()-1) {xfin=fid->al()-1;}
    refreshPixmap();
}
void Plotter::moveRight()
{
    if(!fidSetted) return;
    int dx;
  //  int oxini,oxfin;
    dx=-1;
    if(xini+dx<0) {xfin=xfin-xini; xini=0;}
    else if(xfin+dx>fid->al()-1) {xini=fid->al()-1-(xfin-xini); xfin=fid->al()-1;}
    else xini+=dx;xfin+=dx;
    refreshPixmap();

}

void Plotter::moveLeft()
{
    if(!fidSetted) return;
    int dx;
    dx=1;
    if(xini+dx<0) {xfin=xfin-xini; xini=0;}
    else if(xfin+dx>fid->al()-1) {xini=fid->al()-1-(xfin-xini); xfin=fid->al()-1;}
    else xini+=dx;xfin+=dx;
    refreshPixmap();
}

//------------------------------------------------------------------------------
void Plotter::updateXCursorPosition(int xi)
{
    int nxi;
    if(xi<0) nxi=0; else nxi=xi;
    if(nxi>fid->al()-1) nxi=fid->al()-1;
    setVCursorXPosition(nxi);
    refreshPixmap();
}
//------------------------------------------------------------------------------
void Plotter::updatePlotRange(int i, int f)
{
    xini=i; xfin=f;
    refreshPixmap();
}

//------------------------------------------------------------------------------
void Plotter::keyPressEvent(QKeyEvent *event)
{
    if(!fidSetted) return;
    int dx;
    int oxini,oxfin;
    //if(curveMap.isEmpty()) {return;}

    switch(event->key())
    {
        //case Qt::Key_Plus:
        //    zoomIn();
        //    break;
        //case Qt::Key_Minus:
        //    zoomOut();
        //    break;

        case Qt::Key_Left:
          if(vCursorEnabled())
          {
              FVCursorXPosition--;
              if(vCursorXPosition()<xini) setVCursorXPosition(xini);
          }
          else
          {
            dx=10;
            if(xini+dx<0) {xfin=xfin-xini; xini=0;}
            else if(xfin+dx>fid->al()-1) {xini=fid->al()-1-(xfin-xini); xfin=fid->al()-1;}
            else xini+=dx;xfin+=dx;
//            zoomStack[curZoom].scroll(-1,0);
          }
            refreshPixmap();
            break;
        case Qt::Key_Right:
          if(vCursorEnabled())
          {
            FVCursorXPosition++;
            if(vCursorXPosition()>=xfin) setVCursorXPosition(xfin-1);
          }
          else
          {
            dx=-10;
            if(xini+dx<0) {xfin=xfin-xini; xini=0;}
            else if(xfin+dx>fid->al()-1) {xini=fid->al()-1-(xfin-xini); xfin=fid->al()-1;}
            else xini+=dx;xfin+=dx;
           // zoomStack[curZoom].scroll(+1,0);
          }
            refreshPixmap();
            break;
        case Qt::Key_Down:
            //zoomStack[curZoom].scroll(0,-1);
            //scale/=1.2;
            oxini=xini;oxfin=xfin;
            xini=oxini-0.2*(oxfin-oxini);
            xfin=oxfin+0.2*(oxfin-oxini);
            if(xini<0) {xini=0;}
            if(xfin>fid->al()-1) {xfin=fid->al()-1;}
            refreshPixmap();
            break;
        case Qt::Key_Up:
            //zoomStack[curZoom].scroll(0,+1);
            //scale*=1.2;
            oxini=xini;oxfin=xfin;
            xini=oxini+0.2*(oxfin-oxini);
            xfin=oxfin-0.2*(oxfin-oxini);
            if(xini>=xfin) {xini=oxini; xfin=oxfin;}
            refreshPixmap();
            break;
        default:
            QWidget::keyPressEvent(event);
    }


}
//------------------------------------------------------------------------------
void Plotter::wheelEvent(QWheelEvent *event)
{
    if(!fidSetted) return;

    if(event->orientation()==Qt::Horizontal)
    {

#if defined(__APPLE__)
        double dx=-(event->delta());
#elif defined(_WIN32)
        double dx=-(event->delta()/20.0);
#elif defined(__linux__)
        double dx=-(event->delta()/20.0);
#endif


      if(xini+dx<0) {xfin=xfin-xini; xini=0;}
      else if(xfin+dx>fid->al()-1) {xini=fid->al()-1-(xfin-xini); xfin=fid->al()-1;}
      else
      {
          xini+=dx;xfin+=dx;
      }
      refreshPixmap();
    }
    else if(event->orientation()==Qt::Vertical && scaleSetting==ManualScale)
    {

#if defined(__APPLE__)
        double dy=pow(1.005,-1.0*event->delta());
#elif defined(_WIN32)
        double dy=pow(1.005,-1.0*event->delta()/20.0);
#elif defined(__linux__)
        double dy=pow(1.005,-1.0*event->delta()/20.0);
#endif

        setScale(scale()*dy);
        double scale2=scale()*(height()-(topMargin+bottomMargin))/2;
        if(scale2>1000*height()) setScale(scale()/dy);

        refreshPixmap();
    }
}
//------------------------------------------------------------------------------
void Plotter::updateRubberBandRegion()
{
    int offset = FDevicePixelRatio; // used for Retina displays on Mac
    QRect rect = rubberBandRect.normalized();

    update(rect.left(),rect.top()+1-offset,rect.width(),offset);
    update(rect.left()+1-offset,rect.top(),offset,rect.height());
    update(rect.left(),rect.bottom()+1-offset,rect.width(),offset);
    update(rect.right()+1-offset,rect.top(),offset,rect.height());
}
//------------------------------------------------------------------------------
void Plotter::refreshPixmap()
{
    if(!fidSetted) {return;}

    if(fid->isEmpty()) { return;}

    pixmap=QPixmap(size());
//    pixmap.fill(this,0,0); // obsolete and does not work for Qt5 2014 Jul Takeda
    pixmap.fill(QColor("aliceblue"));

    QPainter painter(&pixmap);
    painter.initFrom(this);

    drawFrame(&painter);
    drawXTicks(&painter);
    drawFID(&painter);

    if(bitLineIsShown()) drawBitLines(&painter);

    if(vCursorIsShown())
    {
        drawVCursor(&painter,vCursorXPosition());
     //   emit vCursorXSignal(QString::number(vCursorXValue));
        if(vCursorXPosition()>-1 && vCursorXPosition()<fid->al())
        {
            emit cursorInfo(QStringList()
                            << QString::number(vCursorXPosition())
                            << QString::number(fid->xValue(vCursorXPosition())) + " "
                                        +fid->xAxisUnitString()
                            << QString::number(fid->real->sig.at(vCursorXPosition()))
                            << QString::number(fid->imag->sig.at(vCursorXPosition()))
                            << QString::number(fid->abs->sig.at(vCursorXPosition()))
                            << QString::number(180/3.141592*
                                               atan2(fid->imag->sig.at(vCursorXPosition()),
                                                     fid->real->sig.at(vCursorXPosition())),'f',2)

                            );

            emit deltaInfo("Prev. x: \n"+ QString::number(previousVCursorXPosition()) + "\n"
                           + "Delta: \n" + QString::number(fid->xValue(vCursorXPosition())
                                                           -fid->xValue(previousVCursorXPosition()))
                                         + " " + fid->xAxisUnitString() + "\n"
                           + "In-phase area: \n" + QString::number(
                               fid->real->sum(vCursorXPosition(),previousVCursorXPosition())) + "\n"
                           + "In-phase sigma: \n" + QString::number(
                               fid->real->standardDeviation(vCursorXPosition(),previousVCursorXPosition())) + "\n"
                           + "Abs sigma: \n" + QString::number(
                               fid->abs->standardDeviation(vCursorXPosition(),previousVCursorXPosition()))

                           );

        }
      //  emit yRealSignal(QString::number(fid->real->sig.at(vCursorXValue)));
        //  TODO: check the index range!
    }
    emit plotRangeInfo(QStringList()
                       << QString::number(xini)
                       << QString::number(fid->xValue(xini))
                       << fid->xAxisUnitString()
                       << QString::number(xfin)
                       << QString::number(fid->xValue(xfin))
                       << fid->xAxisUnitString()
                       );

    drawYZeroLine(&painter);


    emit scaleSignal(QString::number(scale()));
    emit xIniSignal(xini);
    emit xFinSignal(xfin);

    update();

  //  qDebug() << QString(Q_FUNC_INFO);
}
//------------------------------------------------------------------------------
void Plotter::drawFrame(QPainter *painter)
{
    QRect rect(leftMargin,topMargin,width()-(leftMargin+rightMargin),height()-(topMargin+bottomMargin));
    if(!rect.isValid()) return;
    QPen light=palette().light().color();
    light.setColor(Qt::black);
    painter->setPen(light);

    QLinearGradient lg(QPointF(0,0),QPointF(3000,1000));
    lg.setColorAt(0,backgroundColor0());
    lg.setColorAt(1,backgroundColor1());
//    lg.setColorAt(1,QColor("powderblue"));
    QBrush brush(lg);

    painter->fillRect(rect.adjusted(0,0,-1,-1),brush);
    painter->drawRect(rect.adjusted(0,0,-1,-1));

}
//------------------------------------------------------------------------------
void Plotter::drawXTicks(QPainter *painter)
{
    if(!fidSetted) return;
  //  if(plotFormat()==PolarPlot) return;
    if(xfin-xini==0) return;
    QRect rect(leftMargin,topMargin,width()-(leftMargin+rightMargin),height()-(topMargin+bottomMargin));
    if(!rect.isValid()) return;
    QPen light=palette().light().color();
    light.setColor(Qt::black);

    double x1,x2,tick,p;
    QString xLabel=fid->xAxisUnitString();

//qDebug () << QString(Q_FUNC_INFO) << ": " << fid->xAxisUnitString() << fid->xAxisUnitSymbol();

    if(fid->xAxisLabel()!="") xLabel =  fid->xAxisLabel() + " / " + xLabel;

    x1=fid->xValue(xini);
    x2=fid->xValue(xfin);

    if(x2-x1==0) return;
    tick=tic(x1,x2);
    if(x1>x2) tick*=-1.0;

 //qDebug() << QString(Q_FUNC_INFO) << xini << xfin
 //         << x1 << x2 << "tick: " << tick;

    double xIniVal = fid->xInitialValue()/TMetricPrefix::Decimal(fid->plotMetricPrefix.prefix());

    xIniVal = ((int)(xIniVal/tick))*tick;

    p = xIniVal + ((int)((x1-xIniVal)/tick) + 1)*tick;

    // qDebug() << QString(Q_FUNC_INFO) << "xIniVal: " << xIniVal <<"p: " << p << "(p-x1)*(p-x2)" << (p-x1)*(p-x2);

      painter->setPen(light);
      while((p-x1)*(p-x2)<=0) // while p is between x1 and x2, irrespective of +-.
      {
        int x = rect.left() + (rect.width()-1)*(p-x1)/(x2-x1);
        painter->drawLine(x,rect.bottom(),x,rect.bottom()+5);
        painter->drawText(x-50,rect.bottom()+5,100,15, Qt::AlignHCenter | Qt::AlignTop, QString::number(p));
        p+=tick;
      } //

      p = xIniVal + ((int)((x1-xIniVal)/tick) + 1)*tick;
      p-=tick; // We examine the other direction.
      while((p-x1)*(p-x2)<=0) // while p is between x1 and x2, irrespective of +-.
      {
        int x = rect.left() + (rect.width()-1)*(p-x1)/(x2-x1);
        painter->drawLine(x,rect.bottom(),x,rect.bottom()+5);
        painter->drawText(x-50,rect.bottom()+5,100,15, Qt::AlignHCenter | Qt::AlignTop, QString::number(p));
        p-=tick;
      } //
    painter->drawText(rect.width()/2-150,rect.bottom()+20,300,15, Qt::AlignHCenter | Qt::AlignTop, xLabel);
}
//------------------------------------------------------------------------------
void Plotter::drawYZeroLine(QPainter *painter)
{
    if(!fidSetted) return;
    if(plotFormat()==PolarPlot) return;

    QRect rect(leftMargin,topMargin,width()-(leftMargin+rightMargin),height()-(topMargin+bottomMargin));
    if(!rect.isValid()) return;
    vCenter=topMargin+(height()-topMargin-bottomMargin)/2;
    vCenter+=vOffset();

//    QPen pen=palette().light().color();
//    pen.setColor(Qt::magenta);
//    painter->setPen(pen);
    painter->setPen(QPen(QColor("gray"), 4.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    painter->drawLine(rect.right()-15,vCenter,rect.right(),vCenter);

    return;
}
//------------------------------------------------------------------------------
void Plotter::drawBitLines(QPainter *painter)
{
   if(!fidSetted) return;
   if(plotFormat()==PolarPlot) return;
   QRect rect(leftMargin,topMargin,width()-(leftMargin+rightMargin),height()-(topMargin+bottomMargin));
   if(!rect.isValid()) return;
   QPen light=palette().light().color();
   light.setColor(Qt::gray);

   vCenter=topMargin+(height()-topMargin-bottomMargin)/2;
   double scale2=scale()*(height()-(topMargin+bottomMargin))/2;

   vCenter+=vOffset();

   painter->setPen(light);
   painter->drawLine(rect.left(),vCenter,rect.right(),vCenter);

   double q=(height()-topMargin-bottomMargin)/scale2;
   int qq=trunc(log2(q))-3;
   int y;

   int c=qq; if(c<1) c=1;
   y=vCenter-scale2*pow(2,c);
   while(y>0)
   {
      painter->drawLine(rect.left(),y,rect.right(),y);
      painter->drawText(rect.right()-50,round(y-16),50,14,
                        Qt::AlignHCenter | Qt::AlignTop, QString::number(c)+" bit");
      ++c;
      y=vCenter-scale2*pow(2,c);
 //     --qq;
   } // while


   c=qq; if(c<1) c=1;
   y=vCenter+scale2*pow(2,c);
   while(y<rect.height())
   {
       painter->drawLine(rect.left(),y,rect.right(),y);
       painter->drawText(rect.right()-50,round(y-14),50,14,
                         Qt::AlignHCenter | Qt::AlignTop, QString::number(c)+" bit");

       ++c;
       y=vCenter+scale2*pow(2,c);
   }

   return;
}
//------------------------------------------------------------------------------
void Plotter::drawGrid(QPainter *painter)
{
    if(!fidSetted) return;

    QRect rect(leftMargin,topMargin,width()-(leftMargin+rightMargin),height()-(topMargin+bottomMargin));
    if(!rect.isValid()) return;

    PlotSettings settings = zoomStack[curZoom];
    QPen quiteDark=palette().dark().color().light();
    QPen light=palette().light().color();
    light.setColor(Qt::gray);

    for(int i=0; i<=settings.numXTicks; ++i)
    {
        int x = rect.left() + (i*(rect.width()-1)/settings.numXTicks);
        double label = settings.minX + (i * settings.spanX()/settings.numXTicks);
//        painter->setPen(quiteDark);
        painter->setPen(light);
        painter->drawLine(x,rect.top(),x,rect.bottom());
        painter->setPen(light);
        painter->drawLine(x,rect.bottom(),x,rect.bottom()+5);
        painter->drawText(x-50,rect.bottom()+5,100,15, Qt::AlignHCenter | Qt::AlignTop, QString::number(label));
    }
    for(int j=0; j<=settings.numYTicks; ++j)
    {
        int y = rect.bottom() - (j*(rect.height()-1)/settings.numYTicks);
        double label = settings.minY + (j*settings.spanY()/settings.numYTicks);
//        painter->setPen(quiteDark);
        painter->setPen(light);
        painter->drawLine(rect.left(),y,rect.right(),y);
        painter->setPen(light);
        painter->drawLine(rect.left()-5,y,rect.left(),y);
        painter->drawText(rect.left()-Margin,y-10,Margin-5,20, Qt::AlignRight | Qt::AlignVCenter, QString::number(label));
    }
    painter->drawRect(rect.adjusted(0,0,-1,-1));
}
//------------------------------------------------------------------------------
void Plotter::setX()
{
    QMapIterator<int,QVector<QPointF> > i(curveMap);
    if(i.hasNext())
    {
        i.next();
        const QVector<QPointF> &data=i.value();
        minimumXValue=data[0].x();
        maximumXValue=data[0].x();
        for(int j=0; j<data.count(); ++j)
        {
            if(data[j].x() < minimumXValue) minimumXValue=data[j].x();
            if(data[j].x() > maximumXValue) maximumXValue=data[j].x();
           // if(data[j].y() < minimumYValue) minimumYValue=data[j].y();
           // if(data[j].y() > maximumYValue) maximumYValue=data[j].y();
        }

    }
    while(i.hasNext())
    {
        i.next();
        const QVector<QPointF> &data2=i.value();
        for(int j=0; j<data2.count(); ++j)
        {
            if(data2[j].x() < minimumXValue) minimumXValue=data2[j].x();
            if(data2[j].x() > maximumXValue) maximumXValue=data2[j].x();
           // if(data[j].y() < minimumYValue) minimumYValue=data[j].y();
           // if(data[j].y() > maximumYValue) maximumYValue=data[j].y();
        }

    }

}
//------------------------------------------------------------------------------
void Plotter::drawCurves(QPainter *painter)
{
    static const QColor colorForIDs[6]=
      {Qt::red, Qt::darkGreen, Qt::blue, Qt::cyan, Qt::magenta, Qt::yellow};
    PlotSettings settings=zoomStack[curZoom];
    QRect rect(Margin,Margin,width()-2*Margin,height()-2*Margin);
    if(!rect.isValid()) return;

    painter->setClipRect(rect.adjusted(+1,+1,-1,-1));

    QMapIterator<int,QVector<QPointF> > i(curveMap);
    while(i.hasNext())
    {
        i.next();
        int id=i.key();
        const QVector<QPointF> &data=i.value();
        QPolygonF polyline(data.count());

        for(int j=0; j<data.count(); ++j)
        {
            double dx=data[j].x() - settings.minX;
            double dy=data[j].y() - settings.minY;
            double x=rect.left() + (dx*(rect.width()-1)/settings.spanX());
            double y=rect.bottom() - (dy*(rect.height()-1)/settings.spanY());
            polyline[j]=QPointF(x,y);
        }

   //     painter->setPen(colorForIDs[uint(id) % 6]);
        painter->setPen(QPen(colorForIDs[uint(id) % 6], 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter->setRenderHint(QPainter::Antialiasing,true);
        painter->drawPolyline(polyline);
    }
}
//------------------------------------------------------------------------------
void Plotter::drawVCursor(QPainter *painter, int x)
{
   if(!fidSetted) return;
//   if(plotFormat()==PolarPlot) return;
   if(x<xini || x>xfin) return;
   int plotWidth=width()-(leftMargin+rightMargin);
   int p;
   if(xfin-xini>0) p=leftMargin + round(plotWidth*(x-xini)/(xfin-xini));
   else p=leftMargin+round(plotWidth*0.5);

   if(plotFormat()==CartesianPlot)
   {
       painter->setPen(QPen(QColor("gray"), 1.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
       painter->drawLine(p,topMargin,p,height()-bottomMargin);
   }

   painter->setPen(QPen(QColor(tr("aqua")), 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
   painter->drawPie(p-10,height()-bottomMargin-10,20,20,70*16,40*16);

}
//------------------------------------------------------------------------------
void Plotter::drawHalfFID(THalfFID *hFID, QPainter *painter, QColor color)
{
    painter->setPen(QPen(color, penWidth(), Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));


    vCenter=topMargin+(height()-topMargin-bottomMargin)/2;
    double scale2=scale()*(height()-(topMargin+bottomMargin))/2;

    double x1=leftMargin;
    double y1=round(vCenter-scale2*(hFID->sig.at(xini) - plotOffset)) + vOffset();

    double x2,y2;

//    QPolygonF polyline;
//    polyline << QPointF(round(x),round(y));

    if(xfin==xini)
    {
        x2=leftMargin+width();
        painter->drawLine(round(x1),round(y1),round(x2),round(y1));
        return;
    }

    if((width()-(leftMargin+rightMargin))>(xfin-xini+1))
    {
       int k=xini+1;
       while(k<=xfin)
       {
           x2=leftMargin + ((double)(k-xini))/((double)(xfin-xini))*(width()-(leftMargin+rightMargin));
           y2=(vCenter-scale2*(hFID->sig.at(k) - plotOffset )) + vOffset();
           painter->drawLine(round(x1),round(y1),round(x2),round(y2));
          // qDebug()<<QString(Q_FUNC_INFO)<< hFID->sig.at(k);
           x1=x2;y1=y2;
           k++;
       }
    }
    else
    {
       for(double q=1; q<=width()-(leftMargin+rightMargin); q+=((double)(width()-(leftMargin+rightMargin)))/(xfin-xini+1))
       {
           int p=round(xini+(xfin-xini+1)*q/(width()-(leftMargin+rightMargin)));
           if(p>xfin) p=xfin;
           y2=round(vCenter-scale2*(hFID->sig.at(p) - plotOffset)) + vOffset();
           x2=q+leftMargin;
           painter->drawLine(round(x1),round(y1),round(x2),round(y2));
         //  qDebug()<<QString(Q_FUNC_INFO)<< hFID->sig.at(p);
           x1=x2;y1=y2;
       }

    }

}
//------------------------------------------------------------------------------
void Plotter::drawPolarFID(QPainter *painter, QColor color)
{    

    int margin=5;
    int radius;
    if(width()>=height()-bottomMargin) radius=(height()-bottomMargin)/2-margin; else radius=width()/2-margin;
    int xCenter=width()/2;
    int yCenter=(height()-bottomMargin)/2;

    QRect rect(xCenter-radius, yCenter-radius, 2*radius,2*radius);
    if(!rect.isValid()) return;
    painter->setPen(QPen(QColor("white"), 1.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawEllipse(rect);
    painter->drawLine(rect.left(),rect.center().y(),rect.right(),rect.center().y());
    painter->drawLine(rect.center().x(),rect.top(),rect.center().x(),rect.bottom());


//    vCenter=topMargin+(height()-topMargin-bottomMargin)/2;
    double scale2=scale()*radius;

    int x1 = xCenter + round(scale2*(fid->real->sig.at(xini)));
    int y1 = yCenter - round(scale2*(fid->imag->sig.at(xini)));
    int x2, y2;

    painter->setPen(QPen(color, penWidth(), Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    int k=xini+1;
    while(k<=xfin)
    {
        x2=xCenter+round(scale2*(fid->real->sig.at(k)));
        y2=yCenter-round(scale2*(fid->imag->sig.at(k)));
        painter->drawLine(x1,y1,x2,y2);
        x1=x2;y1=y2;
        k++;
    }

    if(vCursorIsShown())
    {
        if(vCursorXPosition()>-1 && vCursorXPosition()<fid->al())
        {
            painter->setPen(QPen(QColor(tr("aqua")), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            x1=xCenter+round(scale2*(fid->real->sig.at(vCursorXPosition())));
            y1=yCenter-round(scale2*(fid->imag->sig.at(vCursorXPosition())));
           // QRectF rectF(x1-10,y1+10,x1+10,y1-10);
            painter->drawPie(x1-20,y1-20,40,40,70*16,40*16);

        }
    }
}
//------------------------------------------------------------------------------
void Plotter::drawFID(QPainter *painter)
{

    if(!fidSetted) return;
    if (xfin-xini<0) {return;}

    QRect rect(leftMargin,topMargin,width()-(leftMargin+rightMargin),height()-(topMargin+bottomMargin));
    if(!rect.isValid()) return;

    painter->setClipRect(rect.adjusted(+1,+1,-1,-1));

    if(fid->domain==TFID::FrequencyDomain) setVOffset(rect.height()*0.4); else setVOffset(0);

    if(plotFormat()==CartesianPlot)
    {
      if(plotItemOption.contains(plotImag)) drawHalfFID(fid->imag, painter, QColor("lime"));
      if(plotItemOption.contains(plotReal)) drawHalfFID(fid->real, painter, QColor("deeppink"));
      if(plotItemOption.contains(plotAbs)) drawHalfFID(fid->abs, painter, QColor("yellow"));
    }
    else // PolarPlot
    {
       drawPolarFID(painter, QColor("darkorange"));
    }

}
//------------------------------------------------------------------------------
void Plotter::setFID(TFID *f)
{
    fid=f;
    fidSetted=true;
//    refreshPixmap();
}
//------------------------------------------------------------------------------
void Plotter::adjustScale()
{
    if(!fidSetted) {return;}
    if(scaleSetting==ManualScale) {return;}

    if(fid->abs->absMax()==0) {setScale(1.0); return;}

    if(scaleSetting==DataScale)
    {
        setScale(1/(fid->abs->absMax()));
//        qDebug() << "scale: " << scale();
    }
    else if (scaleSetting==ReceiverScale)
    {
        if(fid->actualNA==0) setScale(1.0/16384*512);
        else setScale(1.0/(16384*fid->actualNA*512));
    }
}
//------------------------------------------------------------------------------
void Plotter::refresh()
{
   if(!fidSetted) {return;}
   refreshPixmap();
}
//------------------------------------------------------------------------------
PlotSettings::PlotSettings()
{
    minX=0.0;
    maxX=10.0;
    numXTicks=4;
    minY=0.0;
    maxY=10.0;
    numYTicks=4;
}
//------------------------------------------------------------------------------
void PlotSettings::xScroll(int dx)
{
    minX += dx;
    maxX += dx;
}
//------------------------------------------------------------------------------
void PlotSettings::yScroll(int dy)
{
    minY += dy;
    maxY += dy;
}
//------------------------------------------------------------------------------
void PlotSettings::scroll(int dx, int dy)
{
    minX += dx;
    maxX += dx;
    minY += dy;
    maxY += dy;
}
//------------------------------------------------------------------------------
void PlotSettings::adjust()
{
    adjustAxis(minX,maxX,numXTicks);
    adjustAxis(minY,maxY,numYTicks);
}
//------------------------------------------------------------------------------
void PlotSettings::adjustAxis(double &min, double &max, int &numTicks)
{
    const int MinTicks = 4;
//    const int MinTicks = 2;
    double grossStep = (max-min)/MinTicks;

    double step = pow(10.0, floor(log10(grossStep)));

    if(5*step < grossStep) step *= 5;
    else if (2*step < grossStep) step *=2;


    numTicks=int(ceil(max/step) - floor(min/step));
    if(numTicks<MinTicks) numTicks=MinTicks;

 //   numTicks=MinTicks;

    min=floor(min/step)*step;
    max=ceil(max/step)*step;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
