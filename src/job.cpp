
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

#include "job.h"


TJob::TJob()
{
    Ferror=false;
}

TFavoriteJobWidget::TFavoriteJobWidget(QWidget *parent) :
    QWidget(parent)
{
    this->hide();
    this->setWindowTitle(tr("Favorite Jobs"));
    this->setWindowFlags(Qt::Dialog);
    this->setFixedWidth(400);
    this->setFixedHeight(300);

    lastPath=QDir::homePath();
    //running=false;

    createWidgets();
    createPanel();
}


TJobQueueWidget::TJobQueueWidget(QWidget *parent) :
    QWidget(parent)
{
    this->hide();
    this->setWindowTitle(tr("Job Queue"));
    this->setWindowFlags(Qt::Dialog);
    this->setFixedWidth(400);
    this->setFixedHeight(300);

    lastPath=QDir::homePath();
    running=false;

    createWidgets();
    createPanel();
}

void TFavoriteJobWidget::createWidgets()
{
//    currentJobLabel = new QLabel(tr("Current job"));
//    queuedJobLabel = new QLabel(tr("Job queue"));
//    currentJobLineEdit = new QLineEdit;
//    currentJobLineEdit->setReadOnly(true);
    jobListWidget = new QListWidget;
    addJobPushButton = new QPushButton(tr("Add"));
    removeJobPushButton = new QPushButton(tr("Remove"));
    loadJobPushButton = new QPushButton(tr("Load"));
    upButton = new QToolButton(); upButton->setIcon(QIcon(":/images/up1.png"));
    downButton = new QToolButton(); downButton->setIcon(QIcon(":/images/down1.png"));

    connect(addJobPushButton,SIGNAL(clicked()),this,SLOT(onAddJobPushButtonClicked()));
    connect(removeJobPushButton,SIGNAL(clicked()),this,SLOT(onRemoveJobPushButtonClicked()));
    connect(upButton,SIGNAL(clicked()),this,SLOT(onUpButtonClicked()));
    connect(downButton,SIGNAL(clicked()),this,SLOT(onDownButtonClicked()));
    connect(loadJobPushButton,SIGNAL(clicked()),this,SLOT(onLoadJobPushButtonClicked()));
    connect(jobListWidget,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(onLoadJobPushButtonClicked()));

}

void TFavoriteJobWidget::createPanel()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    QHBoxLayout *hLayout0 = new QHBoxLayout;
      QVBoxLayout *vLayout0 = new QVBoxLayout;
      vLayout0->addWidget(upButton);
      vLayout0->addWidget(downButton);
      vLayout0->addStretch();
      hLayout0->addWidget(jobListWidget);
      hLayout0->addLayout(vLayout0);
    mainLayout->addLayout(hLayout0);
    QHBoxLayout *hLayout1 = new QHBoxLayout;
      hLayout1->addWidget(addJobPushButton);
      hLayout1->addWidget(removeJobPushButton);
      hLayout1->addStretch();
      hLayout1->addWidget(loadJobPushButton);
    mainLayout->addLayout(hLayout1);
    setLayout(mainLayout);
}


void TFavoriteJobWidget::addJob(QString jobFilename)
{
    jobListWidget->addItem(jobFilename);
}

void TFavoriteJobWidget::onAddJobPushButtonClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open job"),
                                                    lastPath,
                                                    tr("job file (*.nmrjob)"));

    if (fileName.isEmpty()) return;

    QFileInfo fInfo(fileName);
    lastPath=fInfo.path();

    for(int k=0; k<jobListWidget->count(); k++)
    {
        if(jobListWidget->item(k)->text()==fileName) return; // already registered.
    }

    jobListWidget->addItem(fileName);
//    qDebug() << "Job added.";

}

void TFavoriteJobWidget::onRemoveJobPushButtonClicked()
{
    int row=jobListWidget->currentRow();
    if(row<0) return;

    delete jobListWidget->takeItem(row);
}


void TFavoriteJobWidget::onUpButtonClicked()
{

    if(jobListWidget->count()<=1) return;
    int row=jobListWidget->currentRow();
    if(row<1) return;
    QString qs = jobListWidget->currentItem()->text();
    delete jobListWidget->takeItem(row);
    jobListWidget->insertItem(row-1,qs);
    jobListWidget->setCurrentRow(row-1);

}

void TFavoriteJobWidget::onDownButtonClicked()
{

    if(jobListWidget->count()<=1) return;
    int row=jobListWidget->currentRow();
    if(row==jobListWidget->count()-1 || row<0) return;
    QString qs=jobListWidget->currentItem()->text();
    delete jobListWidget->takeItem(row);
    jobListWidget->insertItem(row+1,qs);
    jobListWidget->setCurrentRow(row+1);
}

void TFavoriteJobWidget::onLoadJobPushButtonClicked()
{
    if(!loadJobPushButton->isEnabled()) return;
    // if the list is double-clicked when the load button is disabled, we do nothing.

    //qDebug() << "Job started.";
    if(jobListWidget->count()<1) return;

    int row=jobListWidget->currentRow();
    if(row<0) return;

    QString currentJobFile=jobListWidget->item(row)->text();
    emit loadJobRequest(currentJobFile);

}




void TJobQueueWidget::createWidgets()
{
    currentJobLabel = new QLabel(tr("Current job"));
    queuedJobLabel = new QLabel(tr("Job queue"));
    currentJobLineEdit = new QLineEdit;
    currentJobLineEdit->setReadOnly(true);
    jobListWidget = new QListWidget;
    addJobPushButton = new QPushButton(tr("Add"));
    removeJobPushButton = new QPushButton(tr("Remove"));
    runJobPushButton = new QPushButton(QIcon(":/images/run.png"),tr("Run"));
    upButton = new QToolButton(); upButton->setIcon(QIcon(":/images/up1.png"));
    downButton = new QToolButton(); downButton->setIcon(QIcon(":/images/down1.png"));

    connect(addJobPushButton,SIGNAL(clicked()),this,SLOT(onAddJobPushButtonClicked()));
    connect(removeJobPushButton,SIGNAL(clicked()),this,SLOT(onRemoveJobPushButtonClicked()));
    connect(upButton,SIGNAL(clicked()),this,SLOT(onUpButtonClicked()));
    connect(downButton,SIGNAL(clicked()),this,SLOT(onDownButtonClicked()));
    connect(runJobPushButton,SIGNAL(clicked()),this,SLOT(onRunJobPushButtonClicked()));
}

void TJobQueueWidget::createPanel()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    //mainLayout->setMargin(0);
   // mainLayout->setSpacing(0);

    mainLayout->addWidget(currentJobLabel);
    mainLayout->addWidget(currentJobLineEdit);
    mainLayout->addWidget(queuedJobLabel);
    QHBoxLayout *hLayout0 = new QHBoxLayout;
      QVBoxLayout *vLayout0 = new QVBoxLayout;
      vLayout0->addWidget(upButton);
      vLayout0->addWidget(downButton);
      vLayout0->addStretch();
      hLayout0->addWidget(jobListWidget);
      hLayout0->addLayout(vLayout0);
    mainLayout->addLayout(hLayout0);
    QHBoxLayout *hLayout1 = new QHBoxLayout;
      hLayout1->addWidget(addJobPushButton);
      hLayout1->addWidget(removeJobPushButton);
      hLayout1->addStretch();
      hLayout1->addWidget(runJobPushButton);
    mainLayout->addLayout(hLayout1);
    setLayout(mainLayout);
}

void TJobQueueWidget::addJob(QString jobFilename)
{
    jobListWidget->addItem(jobFilename);
}

void TJobQueueWidget::onAddJobPushButtonClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open job"),
                                                    lastPath,
                                                    tr("job file (*.nmrjob)"));

    if (fileName.isEmpty()) return;

    QFileInfo fInfo(fileName);
    lastPath=fInfo.path();

    jobListWidget->addItem(fileName);
//    qDebug() << "Job added.";

}

void TJobQueueWidget::onRemoveJobPushButtonClicked()
{
//    qDebug() << "Job removed.";

    QMutexLocker locker(&mutex);

    int row=jobListWidget->currentRow();
    if(row<0) return;

    delete jobListWidget->takeItem(row);
}

void TJobQueueWidget::onRunJobPushButtonClicked()
{
    //qDebug() << "Job started.";
    if(jobListWidget->count()<1) return;

     QMutexLocker locker(&mutex);

     running=true;
     runJobPushButton->setEnabled(false);

     QString currentJobFile=jobListWidget->item(0)->text();
     currentJobLineEdit->setText(currentJobFile);
     delete jobListWidget->takeItem(0);
     emit runJobRequest(currentJobFile);

}

void TJobQueueWidget::onJobFinished()
{
    //qDebug() << "Job finished.";

    QMutexLocker locker(&mutex);

   // if(!running) return;
    if(jobListWidget->count()<1)
    {
        currentJobLineEdit->clear();
        running=false;
        runJobPushButton->setEnabled(true);
        return;
    }
    running=true;
    QString currentJobFile=jobListWidget->item(0)->text();
    currentJobLineEdit->setText(currentJobFile);
    delete jobListWidget->takeItem(0);
    emit runJobRequest(currentJobFile);

}

void TJobQueueWidget::onJobAborted()
{
  //  qDebug() << "Job aborted.";
    QMutexLocker locker(&mutex);
    if(!running) return; // job was not running...

    running=false;
    runJobPushButton->setEnabled(true);
    jobListWidget->insertItem(0,currentJobLineEdit->text());
    currentJobLineEdit->clear();
}

void TJobQueueWidget::onUpButtonClicked()
{
    QMutexLocker locker(&mutex);

    if(jobListWidget->count()<=1) return;
    int row=jobListWidget->currentRow();
    if(row<1) return;
    QString qs = jobListWidget->currentItem()->text();
    delete jobListWidget->takeItem(row);
    jobListWidget->insertItem(row-1,qs);
    jobListWidget->setCurrentRow(row-1);

}

void TJobQueueWidget::onDownButtonClicked()
{
    QMutexLocker locker(&mutex);

    if(jobListWidget->count()<=1) return;
    int row=jobListWidget->currentRow();
    if(row==jobListWidget->count()-1 || row<0) return;
    QString qs=jobListWidget->currentItem()->text();
    delete jobListWidget->takeItem(row);
    jobListWidget->insertItem(row+1,qs);
    jobListWidget->setCurrentRow(row+1);
}
