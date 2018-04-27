#include "peakPick.h"
#include <math.h>

#include <QDir>
#include <QDataStream>
#include <QFileDialog>
#include <QFileInfo>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QFile>
#include <QMessageBox>

#include <QDebug>


KPeakPick::KPeakPick()
{
    setAxisDomain(fidDomain::ToggleDomain);
}

KPeakPick::~KPeakPick()
{
    ;
}

QStringList KPeakPick::processInformation() {return QStringList() << "process=peakPick";}
QString KPeakPick::command() {return "peakpick";}

bool KPeakPick::process(TFID_2D *fid_2d, double tgPeak, int tgFID)
{
    bool r;
    r= peakPickProcess(fid_2d, tgPeak, tgFID);
    return r;
}

bool KPeakPick::process2D(TFID_2D *fid_2d, double tgPeak, int tgSt, int tgEn)
{
    bool r;
    r= peakPickProcess2D(fid_2d, tgPeak, tgSt, tgEn);
    return r;
}

bool KPeakPick::peakPickProcess(TFID_2D *fid_2d, double tgPeak, int tgFID)
{
    QString path="~/";
      if(QDir(dataFilePath()).exists()) path=dataFilePath()+'/';
      QString fn=QFileDialog::getSaveFileName(0,tr("Save"),path,tr("*.txt"));
      if (fn.isEmpty()) return false;

    //----------------peak pick process--------------
    double va, tgt, vb, max, value;
    int i;
    QString ss;

    max = 0.0;

    for(i=0; i<fid_2d->FID.at(tgFID-1)->al(); i++)
    {
        value = fid_2d->FID.at(tgFID-1)->real->sig.at(i);
        if(max < value){max = value;}
    }

    for(i=1; i<fid_2d->FID.at(tgFID-1)->al()-1; i++)
    {
        va  = fid_2d->FID.at(tgFID-1)->real->sig.at(i-1);
        tgt = fid_2d->FID.at(tgFID-1)->real->sig.at(i);
        vb  = fid_2d->FID.at(tgFID-1)->real->sig.at(i+1);

        if(tgt > va && tgt > vb)
        {
            if(tgt > max * tgPeak / 100)
            {
                ss.append(QString::number(i) + " "
                          + QString::number(tgt) + "\n");
            }
        }
    }

    QFile file(fn);
    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::information(0, tr("Unable to open file"),file.errorString());
        return false;
    }

    QTextStream out(&file);
    foreach (QString item, ss)
    {
        out << item;
    }


    qDebug() << "process!";
    return true;
}

bool KPeakPick::peakPickProcess2D(TFID_2D *fid_2d, double tgPeak, int tgSt, int tgEn)
{
    QString path="~/";
      if(QDir(dataFilePath()).exists()) path=dataFilePath()+'/';
      QString fn=QFileDialog::getSaveFileName(0,tr("Save"),path,tr("*.txt"));
      if (fn.isEmpty()) return false;

    //----------------peak pick 2D process--------------
    double v[8];
    double tgt, max, aroundmax, value;
    int i, j, k;
    QString ss;

    max = 0.0;

    for(j=1; j<fid_2d->FID.size()-1; j++)
    {
        for(i=1; i<fid_2d->al()-1; i++)
        {
            value = fid_2d->FID.at(j)->real->sig.at(i);
            if(max < value){max = value;}
        }
    }

    for(j=tgSt+1; j<tgEn-1; j++)
    {
        for(i=tgSt+1; i<tgEn-1; i++)
        {
            v[0]  = fid_2d->FID.at(j-1)->real->sig.at(i-1);
            v[1] = fid_2d->FID.at(j-1)->real->sig.at(i);
            v[2] = fid_2d->FID.at(j-1)->real->sig.at(i+1);
            v[3] = fid_2d->FID.at(j)->real->sig.at(i-1);
            tgt = fid_2d->FID.at(j)->real->sig.at(i);
            v[4] = fid_2d->FID.at(j)->real->sig.at(i+1);
            v[5] = fid_2d->FID.at(j+1)->real->sig.at(i-1);
            v[6] = fid_2d->FID.at(j+1)->real->sig.at(i);
            v[7] = fid_2d->FID.at(j+1)->real->sig.at(i+1);

            aroundmax = 0.0;

            for(k=0; k<8; k++)
            {
                if(aroundmax < v[k]){aroundmax = v[k];}
            }

            if(tgt > aroundmax)
            {
                if(tgt > max * tgPeak / 100)
                {
                    ss.append(QString::number(j) + " "
                              + QString::number(i) + " "
                              + QString::number(tgt) + "\n");
                }
            }
        }
    }

    QFile file(fn);
    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::information(0, tr("Unable to open file"),file.errorString());
        return false;
    }

    QTextStream out(&file);
    foreach (QString item, ss)
    {
        out << item;
    }


    //qDebug() << "process!";

    return true;
}
