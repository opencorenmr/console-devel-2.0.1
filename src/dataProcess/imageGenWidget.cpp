#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QDateTime>
#include <QDate>
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QFile>
#include "imageGenWidget.h"
#include "processPanelWidget.h"
#include "../plotter.h"

TImageGenWidget::TImageGenWidget()
{
  FBoundingBox1=0;
  FBoundingBox2=0;
  FBoundingBox3=600;
  FBoundingBox4=400;
  FLeftMargin=10;
  FRightMargin=10;
  FTopMargin=0;
  FBottomMargin=60;

  createWidgets();
  createPanel();
  createConnections();
}



void TImageGenWidget::createWidgets()
{
    plotterIDSpinBox = new QSpinBox;
    plotterIDSpinBox->setMinimum(0);
    plotterIDSpinBox->setMaximum(0);

    boundingBox1LineEdit = new QLineEdit();
    boundingBox2LineEdit = new QLineEdit();
    boundingBox3LineEdit = new QLineEdit();
    boundingBox4LineEdit = new QLineEdit();

    boundingBox1LineEdit->setAlignment(Qt::AlignRight);
    boundingBox2LineEdit->setAlignment(Qt::AlignRight);
    boundingBox3LineEdit->setAlignment(Qt::AlignRight);
    boundingBox4LineEdit->setAlignment(Qt::AlignRight);
    boundingBox1LineEdit->setText(QString::number(boundingBox1()));
    boundingBox2LineEdit->setText(QString::number(boundingBox2()));
    boundingBox3LineEdit->setText(QString::number(boundingBox3()));
    boundingBox4LineEdit->setText(QString::number(boundingBox4()));

    epsPlainTextEdit = new QPlainTextEdit;
    createEPSPushButton = new QPushButton(tr("Create EPS"));
   // saveEPSPushButton = new QPushButton(tr("Save EPS"));

}

void TImageGenWidget::createPanel()
{
    QVBoxLayout *vLayout1 = new QVBoxLayout(this);
    QHBoxLayout *hLayout1 = new QHBoxLayout();
      QComboBox *comboBox1 = new QComboBox();
      comboBox1->addItems(QStringList()<<"eps");
      hLayout1->addWidget(new QLabel(tr("Create")));
      hLayout1->addWidget(comboBox1);
      hLayout1->addStretch();

    QHBoxLayout *hLayout2 = new QHBoxLayout();
      hLayout2->addWidget(new QLabel("Bounding box:"));
      hLayout2->addWidget(boundingBox1LineEdit);
      hLayout2->addWidget(boundingBox2LineEdit);
      hLayout2->addWidget(boundingBox3LineEdit);
      hLayout2->addWidget(boundingBox4LineEdit);

    QGridLayout *gLayout1 = new QGridLayout();
      gLayout1->addWidget(new QLabel(tr("Adopt plot options of plotter ID #")),1,0,1,1);
      gLayout1->addWidget(plotterIDSpinBox,1,1,1,1);
    QHBoxLayout *hLayout3 = new QHBoxLayout();
      hLayout3->addWidget(createEPSPushButton);
      //hLayout3->addWidget(saveEPSPushButton);

    vLayout1->addLayout(hLayout1);
    vLayout1->addLayout(hLayout2);
    vLayout1->addLayout(gLayout1);
    vLayout1->addLayout(hLayout3);
    vLayout1->addWidget(epsPlainTextEdit);
    vLayout1->addStretch();
}
void TImageGenWidget::createConnections()
{
    connect(createEPSPushButton,SIGNAL(clicked()),this,SLOT(createEPS()));
   // connect(saveEPSPushButton,SIGNAL(clicked()),this,SLOT(saveEPS()));
}

void TImageGenWidget::saveEPS()
{
    if(!isAncestorDefined()) {return;}
    if(ancestor()->FID_2D->FID.isEmpty()) {return;}

    QString path="~/";
    QString dataFilePath=ancestor()->processFileWidget->dataFilePath();
    if(QDir(dataFilePath).exists()) path=dataFilePath+'/';
    QString fileName=QFileDialog::getSaveFileName(this,tr("Save"),path,tr("*.eps"));
    if(fileName.isEmpty()) {return;}

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;

    QTextStream out(&file);
    out << epsPlainTextEdit->toPlainText().toUtf8();
    file.close();



}

bool TImageGenWidget::setParams()
{
    bool ok;
    int i;

    // ---  Bounding Box 1  ---
    i=boundingBox1LineEdit->text().toInt(&ok);
    if(!ok)
    {
        errorMessage="Invalid expression (BoundingBox): " + boundingBox1LineEdit->text();
        return false;
    }
    if(i<0)
    {
        errorMessage="Parameter in BoundingBox cannot be negative:" + QString::number(i);
        return false;
    }

    setBoundingBox1(i);


    // ---  Bounding Box 2  ---
    i=boundingBox2LineEdit->text().toInt(&ok);
    if(!ok)
    {
        errorMessage="Invalid expression (BoundingBox): " + boundingBox2LineEdit->text();
        return false;
    }
    if(i<0)
    {
        errorMessage="Parameter in BoundingBox cannot be negative:" + QString::number(i);
        return false;
    }

    setBoundingBox2(i);

    // ---  Bounding Box 3  ---
    i=boundingBox3LineEdit->text().toInt(&ok);
    if(!ok)
    {
        errorMessage="Invalid expression (BoundingBox): " + boundingBox3LineEdit->text();
        return false;
    }
    if(i<0)
    {
        errorMessage="Parameter in BoundingBox cannot be negative:" + QString::number(i);
        return false;
    }
    if(i<=boundingBox1())
    {
        errorMessage="The 3rd parameter" + QString::number(i)
                + " of the BoundingBox must be larger than the 1st parameter "
                + QString::number(boundingBox1());
        return false;
    }
    setBoundingBox3(i);

    // ---  Bounding Box 4  ---
    i=boundingBox4LineEdit->text().toInt(&ok);
    if(!ok)
    {
        errorMessage="Invalid expression (BoundingBox): " + boundingBox4LineEdit->text();
        return false;
    }
    if(i<0)
    {
        errorMessage="Parameter in BoundingBox cannot be negative:" + QString::number(i);
        return false;
    }
    if(i<=boundingBox2())
    {
        errorMessage="The 4th parameter" + QString::number(i)
                + " of the BoundingBox must be larger than the 2st parameter "
                + QString::number(boundingBox2());
        return false;
    }
    setBoundingBox4(i);

    return true;
}

bool TImageGenWidget::EPSCartesian(TFID *fid)
{

  return true;
}

void TImageGenWidget::createEPS()
{
  if(!isAncestorDefined()) return;
  if(ancestor()->FID_2D->FID.isEmpty()) return;

  epsPlainTextEdit->clear();
  int currentPlotter=plotterIDSpinBox->value();
  int xi=ancestor()->plotters->FIDPlotters[currentPlotter]->plotter->xini;
  int xf=ancestor()->plotters->FIDPlotters[currentPlotter]->plotter->xfin;

  setXRangeIni(xi);
  setXRangeFin(xf);
  setXPlotIni(xi);
  setXPlotFin(xf);

  double os=ancestor()->plotters->FIDPlotters[currentPlotter]->plotterDetails->vOffsetSpinBox->value();
  setOffset(os);

  if(!setParams())
  {
    epsPlainTextEdit->appendPlainText(errorMessage);
    return;
  }

  if(ancestor()->plotters->FIDPlotters[currentPlotter]->plotter->plotFormat()==Plotter::PolarPlot)
  {
      // Polar
      epsPlainTextEdit->clear();
      epsPlainTextEdit->appendPlainText("Polar plot will be available soon...");
      return;
  }


  QDateTime dayTime=QDateTime::currentDateTime();

  epsPlainTextEdit->appendPlainText("%!PS-Adobe-3.0 EPSF-3.0");
  epsPlainTextEdit->appendPlainText("%%BoundingBox: " + QString::number(boundingBox1())
                                    + " " + QString::number(boundingBox2())
                                    + " " + QString::number(boundingBox3())
                                    + " " + QString::number(boundingBox4()));
  epsPlainTextEdit->appendPlainText("%%Title:");
  epsPlainTextEdit->appendPlainText("%%Creator: Opencore NMR Process");

  epsPlainTextEdit->appendPlainText("%%Creation Date: " // + today.toString("ddd MMMM d yyyy")
                                    + dayTime.toString("hh:mm ddd MMMM d yyyy"));
  epsPlainTextEdit->appendPlainText("%%Copyright:");
  epsPlainTextEdit->appendPlainText("%%EndComments");

  epsPlainTextEdit->appendPlainText("%%BeginProlog");
  epsPlainTextEdit->appendPlainText("/l {lineto} bind def");
  epsPlainTextEdit->appendPlainText("/m {moveto} bind def");
  epsPlainTextEdit->appendPlainText("0.5 setlinewidth 1 setlinecap 1 setlinejoin");
  epsPlainTextEdit->appendPlainText("%%EndProlog");


  int lineWidth=ancestor()->plotters->FIDPlotters[currentPlotter]->penWidthSpinBox->value();
  epsPlainTextEdit->appendPlainText(QString::number(lineWidth*0.5) + " setlinewidth");

  float aScale=ancestor()->plotters->FIDPlotters[currentPlotter]->plotter->scale();
  setScale(plotHeight()*(1-offset())*aScale);

 /*
  if(ancestor()->plotters->FIDPlotters[currentPlotter]->plotter->plotFormat()==Plotter::CartesianPlot)
  {
      // Cartesian
  }
  else if(ancestor()->plotters->FIDPlotters[currentPlotter]->plotter->plotFormat()==Plotter::PolarPlot)
  {
      // Polar
  }
  */


  if(ancestor()->plotters->FIDPlotters[currentPlotter]->plotter
                         ->plotItemOption.contains(Plotter::plotReal))
  {
    if(!drawEPSHalfFID(ancestor()->FID_2D->FID[ancestor()->FID_2D->currentFID()]->real))
    {
        //error
      return;
    }
    else
    {
        epsPlainTextEdit->appendPlainText("newpath");
        epsPlainTextEdit->appendPlainText("1 0 0 setrgbcolor");
        epsPlainTextEdit->appendPlainText(eps0.join("\n"));
        epsPlainTextEdit->appendPlainText("stroke");

    }
  }

  if(ancestor()->plotters->FIDPlotters[currentPlotter]->plotter
                         ->plotItemOption.contains(Plotter::plotImag))
  {
    if(!drawEPSHalfFID(ancestor()->FID_2D->FID[ancestor()->FID_2D->currentFID()]->imag))
    {
        //TODO error
       return;
    }
    else
    {
        epsPlainTextEdit->appendPlainText("newpath");
        epsPlainTextEdit->appendPlainText("0 1 0 setrgbcolor");
        epsPlainTextEdit->appendPlainText(eps0.join("\n"));
        epsPlainTextEdit->appendPlainText("stroke");
    }
  }

  if(ancestor()->plotters->FIDPlotters[currentPlotter]->plotter
                         ->plotItemOption.contains(Plotter::plotAbs))
  {
    if(!drawEPSHalfFID(ancestor()->FID_2D->FID[ancestor()->FID_2D->currentFID()]->abs))
    {
        // TODO error
      return;
    }
    else
    {
        epsPlainTextEdit->appendPlainText("newpath");
        epsPlainTextEdit->appendPlainText("0 0 1 setrgbcolor");
        epsPlainTextEdit->appendPlainText(eps0.join("\n"));
        epsPlainTextEdit->appendPlainText("stroke");
    }
  }

  if(!drawEPSXAxis(ancestor()->FID_2D->FID[ancestor()->FID_2D->currentFID()]))
  {
      //TODO: error handling
      return;
  }
  else
  {
      epsPlainTextEdit->appendPlainText("0.5 setlinewidth 1 setlinecap 1 setlinejoin");
      epsPlainTextEdit->appendPlainText("newpath");
      epsPlainTextEdit->appendPlainText("0 0 0 setrgbcolor");
      epsPlainTextEdit->appendPlainText(eps0.join("\n"));
      epsPlainTextEdit->appendPlainText("stroke");
  }



  epsPlainTextEdit->appendPlainText("%%EOF");

  saveEPS();

}

bool TImageGenWidget::drawEPSXAxis(TFID *fid)
{
  eps0.clear();

  double x1=fid->xValue(xRangeIni());
  double x2=fid->xValue(xRangeFin());
  double tick=Plotter::tic(x1,x2);
  if(x1>x2) tick *= -1;

  double xIniVal = fid->xInitialValue()/TMetricPrefix::Decimal(fid->plotPrefix());
  xIniVal=static_cast<int>(xIniVal/tick)*tick;

  eps0.append("/Helvetica findfont 18 scalefont setfont");

  eps0.append(QString::number(leftMargin()) + " " + QString::number(bottomMargin()) + " m");
  eps0.append(QString::number(boxWidth()-rightMargin()) + " " + QString::number(bottomMargin()) + " l");

  double p;
  p=xIniVal+(static_cast<int>((x1-xIniVal)/tick) + 1)*tick;
  while((p-x1)*(p-x2)<=0)
  {
    int x=leftMargin() + plotWidth()*(p-x1)/(x2-x1);
    eps0.append(QString::number(x) + " " + QString::number(bottomMargin()) + " m");
    eps0.append(QString::number(x) + " " + QString::number(bottomMargin()-5) + " l");
    eps0.append("(" + QString::number(p) + ") dup stringwidth pop 2 div neg" );
    eps0.append(QString::number(x) + " add " + QString::number(bottomMargin()-20) + " moveto show");

    p+=tick;
  }

  p=xIniVal+(static_cast<int>((x1-xIniVal)/tick) + 1)*tick;
  p-=tick; // We examine the other direction.
  while((p-x1)*(p-x2)<=0) // while p is between x1 and x2, irrespective of +-.
  {
      int x=leftMargin() + plotWidth()*(p-x1)/(x2-x1);
      eps0.append(QString::number(x) + " " + QString::number(bottomMargin()) + " m");
      eps0.append(QString::number(x) + " " + QString::number(bottomMargin()-5) + " l");
      eps0.append("(" + QString::number(p) + ") dup stringwidth pop 2 div neg" );
      eps0.append(QString::number(x) + " add " + QString::number(bottomMargin()-20) + " moveto show");
      p-=tick;
  }

  QString xAxisLabel=ancestor()->axisFormatWidget->axisStyle->label();
  if(!xAxisLabel.isEmpty()) {xAxisLabel.append(" / ");}
  xAxisLabel += ancestor()->axisFormatWidget->axisStyle->unitString();

  eps0.append("/Helvetica findfont 20 scalefont setfont");
  eps0.append("("+xAxisLabel+") dup stringwidth pop 2 div neg");
  eps0.append(QString::number(leftMargin()+(plotWidth())/2) + " add "
            + QString::number(bottomMargin()-50) + " moveto show");


  return true;
}

bool TImageGenWidget::drawEPSHalfFID(THalfFID *hf)
{
  if(!isAncestorDefined()) return false;
  if(ancestor()->FID_2D->FID.isEmpty()) return false;

  if(xRangeIni()<0) return false;
  if(xRangeFin()<=xRangeIni()) return false;
  if(xPlotIni()<0) return false;
  if(xPlotFin()>hf->al()-1) return false;

  eps0.clear();

  QString qs1;


  float x=1.0*leftMargin();
  float dx=1.0*plotWidth()/(xPlotFin()-xPlotIni());

  int xc=xRangeIni();
  while(xc<xPlotIni()) {xc++;}
  // move to the 1st point
  qs1 = QString::number(leftMargin()) + " "
        + QString::number(bottomMargin()+offset()*plotHeight()+(hf->sig.at(xc))*scale()) + " m"; // moveto
  eps0.append(qs1);
  xc++;
  x += dx;

  while(xc<=xPlotFin())
  {  
    qs1 = QString::number(x) + " " + QString::number(bottomMargin() + offset()*plotHeight() + (hf->sig.at(xc))*scale()) + " l";
    eps0.append(qs1);

    xc++;
    x += dx;
  }

  return true;
}
