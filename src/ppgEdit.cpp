#include "ppgEdit.h"

#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QGroupBox>

#include <QDebug>

#include "variableedit.h"
#include "intToHex.h"
#include "digitalfilter.h"
#include "rcvrcom.h"

TppgEdit::TppgEdit(QWidget *parent) :
    QWidget(parent)
{
    ppg=new TpulseProgram(3);  // 3 is the number of channels
    receiverCommands.clear();

#if defined(__APPLE__)
  editorFont.setFamily("Monaco");
  editorFont.setPointSize(12);
#elif defined(_WIN32)
  editorFont.setFamily("Consolas");
  editorFont.setPointSize(10);
#endif
  editorFont.setFixedPitch(true);

  expSettings = new TExpSettings;
  expSettings->setPPG(ppg);
  expSettings->setTitle(tr("Off-line experimental settings"));
  expSettings->dataSaveButton->hide();
  expSettings->jobLoadButton->hide();//setEnabled(true);
  expSettings->jobSaveButton->hide();
  expSettings->jobQueueButton->hide();

    //QLinearGradient lg(QPointF(0,0),QPointF(300,300));
    //lg.setColorAt(1,QColor("silver"));
    //lg.setColorAt(0,QColor("mistyrose"));
    //QBrush brush(lg);
    //expSettings->setAutoFillBackground(true);
    //QPalette pal(palette());
    //pal.setBrush(QPalette::Window,brush);
    //expSettings->setPalette(pal);


  loadJobButton = new QPushButton("Load");
  saveJobButton = new QPushButton("Save");
  queueJobButton = new QPushButton("Queue");

  ppgFilenameLabel = new QLabel;
  compilerReport = new QPlainTextEdit;
    compilerReport->document()->setMaximumBlockCount(1000);
  ppgOpenButton = new QPushButton("Open");
  ppgSaveButton = new QPushButton("Save");
    ppgSaveButton->setEnabled(false);
  ppgSaveAsButton = new QPushButton("Save As");

  showExpSettingsButton = new QPushButton("Settings");
    showExpSettingsButton->setEnabled(false);

  ppgTransferButton = new QPushButton(tr("Transfer"));
    ppgTransferButton->setEnabled(false);

    ppgCompileButton = new QPushButton(tr("Compile"));
    ppgCompileButton->setEnabled(false);
  ppgPartListWidget = new QListWidget;
  ppgPartTextEdit = new CodeEditor;
    ppgPartTextEdit->setFont(editorFont);
    highlighter = new Highlighter(ppgPartTextEdit->document());

  ppgCompilerMainSplitter = new QSplitter(Qt::Horizontal);
    ppgCompilerMainSplitter->setHandleWidth(1);
    ppgCompilerMainSplitter->setChildrenCollapsible(false);
  ppgCompilerRightSplitter = new QSplitter(Qt::Vertical);

  QVBoxLayout *ppgCompilerButtonLayout = new QVBoxLayout;
    ppgCompilerButtonLayout->addWidget(ppgOpenButton);
    ppgCompilerButtonLayout->addWidget(ppgSaveButton);
    ppgCompilerButtonLayout->addWidget(ppgSaveAsButton);
    ppgCompilerButtonLayout->addWidget(ppgCompileButton);
//    ppgCompilerButtonLayout->addWidget(ppgTransferButton);

  QGroupBox *ppgGroupBox = new QGroupBox(tr("Pulse program"));
  ppgGroupBox->setLayout(ppgCompilerButtonLayout);


  QVBoxLayout *jobLayout = new QVBoxLayout;
  jobLayout->addWidget(loadJobButton);
  jobLayout->addWidget(saveJobButton); saveJobButton->setEnabled(false);
  jobLayout->addWidget(queueJobButton); queueJobButton->setEnabled(false);
  QGroupBox *jobGroupBox = new QGroupBox(tr("Job"));
  jobGroupBox->setLayout(jobLayout);

  QVBoxLayout *expLayout = new QVBoxLayout;
  expLayout->addWidget(showExpSettingsButton);
  expLayout->addWidget(ppgTransferButton);
  QGroupBox *expGroupBox = new QGroupBox(tr("Experiment"));
  expGroupBox->setLayout(expLayout);


  QWidget *widget0 = new QWidget;
  widget0->setFixedWidth(100);
  layout0 =new QVBoxLayout(widget0);
    layout0->setMargin(0);
    layout0->setSpacing(4);
    layout0->addWidget(ppgGroupBox);
    layout0->addWidget(expGroupBox);
    layout0->addWidget(jobGroupBox);
    layout0->addWidget(ppgPartListWidget);


  QWidget *widget1 = new QWidget;
  layout1 =new QVBoxLayout(widget1);
    layout1->setMargin(0);
    layout1->setSpacing(4);


    stackedLayout = new QStackedLayout;
    stackedLayout->addWidget(ppgPartTextEdit);
    stackedLayout->addWidget(expSettings);
    //stackedLayout->addWidget(splitter3);


//    layout1->addLayout(ppgCompilerButtonLayout);
    layout1->addWidget(ppgFilenameLabel);
    layout1->addLayout(stackedLayout);

  ppgCompilerRightSplitter->addWidget(widget1);
  ppgCompilerRightSplitter->addWidget(compilerReport);
  ppgCompilerRightSplitter->setStretchFactor(0,1);
  ppgCompilerRightSplitter->setHandleWidth(1);
  ppgCompilerRightSplitter->setChildrenCollapsible(false);

//  ppgCompilerMainSplitter->addWidget(ppgPartListWidget);
  ppgCompilerMainSplitter->addWidget(widget0);
  ppgCompilerMainSplitter->addWidget(ppgCompilerRightSplitter);
  ppgCompilerMainSplitter->setStretchFactor(0,1);

  QHBoxLayout *ppgEditLayout = new QHBoxLayout;
  ppgEditLayout->setMargin(0);
  ppgEditLayout->addWidget(ppgCompilerMainSplitter,0);
  setLayout(ppgEditLayout);

  connect(ppgOpenButton,SIGNAL(clicked()),this,SLOT(ppgRead()));
  connect(ppgSaveButton,SIGNAL(clicked()),this,SLOT(ppgSave()));
  connect(ppgSaveAsButton,SIGNAL(clicked()),this,SLOT(ppgSaveAs()));
  connect(ppgCompileButton,SIGNAL(clicked()), this, SLOT(ppgCompile()));
  connect(ppgPartListWidget,SIGNAL(currentRowChanged(int)), this, SLOT(displayPPGPart(int)));

  connect(ppgTransferButton,SIGNAL(clicked()),this,SLOT(transferClicked()));
  connect(showExpSettingsButton,SIGNAL(clicked()),this,SLOT(showExpSettings()));

  connect(expSettings,SIGNAL(ppgCompiled()), this, SLOT(onPPGCompiled()));
  connect(loadJobButton,SIGNAL(clicked()),expSettings,SLOT(onJobLoadButtonClicked()));
  connect(saveJobButton,SIGNAL(clicked()),expSettings,SLOT(onJobSaveButtonClicked()));
  connect(queueJobButton,SIGNAL(clicked()),expSettings,SLOT(onJobQueueButtonClicked()));

}
//----------------------------------------------------------------------------
TppgEdit::~TppgEdit()
{
   // if(!expSettings->isHidden()) expSettings->close();
}
//----------------------------------------------------------------------------
void TppgEdit::showEditor()
{
    stackedLayout->setCurrentIndex(0);
}
//----------------------------------------------------------------------------
void TppgEdit::showExpSettings()
{
    stackedLayout->setCurrentIndex(1);
    ppgPartListWidget->setCurrentRow(1);
}
//----------------------------------------------------------------------------
void TppgEdit::transferClicked()
{
   if(!ppg->compiled) return;


   expSettings->onJobSaveButtonClicked();
   if(!expSettings->ok) return;

   emit viewRequest();
   emit toTerminal(expSettings->jobFileName);

}
//----------------------------------------------------------------------------
void TppgEdit::ppgReadFromFile(QString fileName)
{
    if (fileName.isEmpty()) return;
    if(!QFile::exists(fileName)) return;

      compilerReport->clear();
      ppgPartListWidget->clear();

      ppg->setppgFileName(fileName);
      if(!(ppg->loadPPG()))
      {
         compilerReport->insertPlainText(ppg->errorMessage);
         compilerReport->moveCursor(QTextCursor::End);
         return;
      }

      ppgPartListWidget->clear();
      ppgPartListWidget->addItem(tr("Source"));
      ppgPartListWidget->setCurrentRow(0);
      ppgFilenameLabel->setText(ppg->sourceFileName());

      ppgSaveButton->setEnabled(false);
      showExpSettingsButton->setEnabled(false);

}

void TppgEdit::ppgRead()
{
    QString path;

    if (QDir(ppg->ppgFilePath()).exists())
    {
        path=ppg->ppgFilePath()+'/';
    //    qDebug()<<"dir exists.";
    }
    else
    {
        path="~/";
        // qDebug()<<"dir does not exist.";
    }

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open a pulse program"),
                                                    path,
                                                    tr("pulse program source (*.ppg)"));

    ppgReadFromFile(fileName);

    /*
    if (fileName.isEmpty()) return;

      compilerReport->clear();
      ppgPartListWidget->clear();

      ppg->setppgFileName(fileName);
      if(!(ppg->loadPPG()))
      {
         compilerReport->insertPlainText(ppg->errorMessage);
         compilerReport->moveCursor(QTextCursor::End);
         return;
      }

      ppgPartListWidget->clear();
      ppgPartListWidget->addItem(tr("Source"));
      ppgPartListWidget->setCurrentRow(0);
      ppgFilenameLabel->setText(ppg->sourceFileName());

      ppgSaveButton->setEnabled(false);
      showExpSettingsButton->setEnabled(false);
      */

}
//----------------------------------------------------------------------------
void TppgEdit::ppgSaveAs()
{
    if(ppgPartListWidget->currentRow()>0) return;

    QString fileName = QFileDialog::getSaveFileName();
    if(fileName.isNull()) return;

    ppg->sourcePPG=ppgPartTextEdit->toPlainText().split('\n');
    ppg->setppgFileName(fileName);
    if(!ppg->savePPG()) {
        compilerReport->insertPlainText(ppg->errorMessage);
        compilerReport->moveCursor(QTextCursor::End);
    }
    ppgPartListWidget->clear();
    ppgPartListWidget->addItem(tr("Source"));
    ppgPartListWidget->setCurrentRow(0);
    ppgFilenameLabel->setText(ppg->sourceFileName());

    ppgSaveButton->setEnabled(false);
    showExpSettingsButton->setEnabled(false);

}
//----------------------------------------------------------------------------
void TppgEdit::ppgSave()
{
    if(ppgPartListWidget->currentRow()>0) return;
    ppg->sourcePPG=ppgPartTextEdit->toPlainText().split('\n');
    if(!ppg->savePPG()) {
        compilerReport->insertPlainText(ppg->errorMessage);
        compilerReport->moveCursor(QTextCursor::End);
    }
    ppgSaveButton->setEnabled(false);
}
//----------------------------------------------------------------------------
void TppgEdit::onPPGCompiled()
{
    if(ppg->warning)
    {
      compilerReport->insertPlainText(tr("Warninig(s):\n"));
      compilerReport->insertPlainText(ppg->warningMessage);
    }
    compilerReport->insertPlainText("Compilation successful.\n");
    compilerReport->moveCursor(QTextCursor::End);

    while (ppgPartListWidget->count()>0) delete ppgPartListWidget->takeItem(ppgPartListWidget->count()-1);
    ppgPartListWidget->addItem(tr("Source"));
    ppgPartListWidget->addItem(tr("Settings"));

    ppgPartListWidget->addItem(tr("Compiled program"));
    ppgPartListWidget->addItem(tr("Receiver commands"));
    receiverCommands=ppg->receiverInfo.receiverCommands();

    ppgPartListWidget->addItem(tr("Gate definition file"));
    ppgPartListWidget->addItem(tr("Preamble info"));

    ppgPartListWidget->setCurrentRow(1);

    saveJobButton->setEnabled(true);
    queueJobButton->setEnabled(true);
    showExpSettingsButton->setEnabled(true);

}
//----------------------------------------------------------------------------
void TppgEdit::ppgCompile()
{

    ppgSave();

    compilerReport->clear();
    receiverCommands.clear();
    updatedCommands.clear();

    ppgPartListWidget->setCurrentRow(0);
    while (ppgPartListWidget->count()>1) delete ppgPartListWidget->takeItem(ppgPartListWidget->count()-1);

    if(!ppg->compilePPG())
    {
        compilerReport->insertPlainText(ppg->errorMessage);
        compilerReport->moveCursor(QTextCursor::End);
    }
    else
    {
        onPPGCompiled();

        // expSettings->showWindow();
        expSettings->setFreqList();
        expSettings->updateVariableTable();
        expSettings->arrayWidget->initialSetup(ppg);
        expSettings->acquisitionWidget->setOrigNA(ppg->variables.at(ppg->variableIndex("NA"))->numeric().toInt());
        expSettings->acquisitionWidget->setOrigAL(ppg->variables.at(ppg->variableIndex("AL"))->numeric().toInt());
        expSettings->jobSaveButton->setEnabled(true);
        expSettings->setModified(true);         //20160306 KT

    }
}
//----------------------------------------------------------------------------
void TppgEdit::displayPPGPart(int j)
{
    if (j<0) return;

    ppgPartTextEdit->setReadOnly(true);
    ppgCompileButton->setEnabled(false);
    disconnect(ppgPartTextEdit,SIGNAL(textChanged()),this,SLOT(setppgSaveButtonEnabled()));

   // if(j==1) // variables
   // {
   //     stackedLayout->setCurrentIndex(1);
   //     return;
   // }

    if(j==1) // exp. settings!
    {
      stackedLayout->setCurrentIndex(1);
      ppgFilenameLabel->setText(tr("Off-line experimental settings"));
      ppgCompileButton->setEnabled(false);
      ppgSaveAsButton->setEnabled(false);
    }
    else
    {
      stackedLayout->setCurrentIndex(0);
      if(j==0)
      {
        ppgPartTextEdit->setReadOnly(false);
        ppgPartTextEdit->setPlainText(ppg->sourcePPG.join("\n"));
        ppgCompileButton->setEnabled(true);
        ppgSaveAsButton->setEnabled(true);
        ppgFilenameLabel->setText(ppg->sourceFileName());

        connect(ppgPartTextEdit,SIGNAL(textChanged()),this,SLOT(setppgSaveButtonEnabled()));

      }
      else if(j==2) // compiled ppg
      {
        ppgPartTextEdit->setPlainText(ppg->toFPGA.join("\n"));
        ppgFilenameLabel->setText(tr(""));
        ppgCompileButton->setEnabled(false);
        ppgSaveAsButton->setEnabled(false);
      }
      else if(j==3) // receiver commands
      {
        ppgPartTextEdit->setPlainText(receiverCommands.join("\n"));
       // qDebug() << receiverCommands;
        ppgFilenameLabel->setText(tr(""));
        ppgCompileButton->setEnabled(false);
        ppgSaveAsButton->setEnabled(false);
      }
      else if(j==4) // gate definition file
      {
        QFile file(ppg->gateFileName());
        file.open(QFile::ReadOnly | QFile::Text);
        QTextStream ReadFile(&file);
        ppgPartTextEdit->setPlainText(ReadFile.readAll());
        ppgFilenameLabel->setText(ppg->gateFileName());
        ppgCompileButton->setEnabled(false);
        ppgSaveAsButton->setEnabled(false);
      }
      else if(j==5) // preamble info
      {
        ppgPartTextEdit->setPlainText(ppg->preambleInfo.join("\n"));
        ppgFilenameLabel->setText(tr(""));
        ppgCompileButton->setEnabled(false);
        ppgSaveAsButton->setEnabled(false);
      }
      else
      {
         return;
      }

    } // else

}

