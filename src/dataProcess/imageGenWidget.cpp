#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QDateTime>
#include <QDate>
#include "imageGenWidget.h"
#include "processPanelWidget.h"


TImageGenWidget::TImageGenWidget()
{
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
    boundingBox1LineEdit->setText("0");
    boundingBox2LineEdit->setText("0");
    boundingBox3LineEdit->setText("200");
    boundingBox4LineEdit->setText("100");

    epsPlainTextEdit = new QPlainTextEdit;
    createEPSPushButton = new QPushButton(tr("Create EPS"));
    saveEPSPushButton = new QPushButton(tr("Save EPS"));

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
      hLayout3->addWidget(saveEPSPushButton);

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
}



void TImageGenWidget::createEPS()
{
  int currentPlotter=plotterIDSpinBox->value();
  int xi=ancestor()->plotters->FIDPlotters[currentPlotter]->plotter->xini;
  int xf=ancestor()->plotters->FIDPlotters[currentPlotter]->plotter->xfin;

  QDateTime dayTime=QDateTime::currentDateTime();
//  QDate today=QDate::currentDate();


  epsPlainTextEdit->clear();
  epsPlainTextEdit->appendPlainText("%!PS-Adobe-3.0 EPSF-3.0");
  epsPlainTextEdit->appendPlainText("%%BoundingBox: " + boundingBox1LineEdit->text()
                                    + " " + boundingBox2LineEdit->text()
                                    + " " + boundingBox3LineEdit->text()
                                    + " " + boundingBox4LineEdit->text());
  epsPlainTextEdit->appendPlainText("%%Title:");
  epsPlainTextEdit->appendPlainText("%%Creator: Opencore NMR Process");

  epsPlainTextEdit->appendPlainText("%%Creation Date: " // + today.toString("ddd MMMM d yyyy")
                                    + dayTime.toString("hh:mm ddd MMMM d yyyy"));
  epsPlainTextEdit->appendPlainText("%%Copyright:");
  epsPlainTextEdit->appendPlainText("%%EndComments");

  epsPlainTextEdit->appendPlainText("%%BeginProlog");
  epsPlainTextEdit->appendPlainText("/l {lineto} bind def");
  epsPlainTextEdit->appendPlainText("/m {moveto} bind def");
  epsPlainTextEdit->appendPlainText("1 setlinewidth 1 setlinecap 1 setlinejoin");
  epsPlainTextEdit->appendPlainText("%%EndProlog");

//
// ...
//

  epsPlainTextEdit->appendPlainText("%%EOF");

}
