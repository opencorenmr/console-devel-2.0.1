#include "fid.h"
#include "math.h"

#include <QDataStream>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QString>
#include <QSettings>

#include <QDebug>




bool TFID_2D::setPlotData(int k, QVector<QPointF> *re, QVector<QPointF> *im, QVector<QPointF> *abs)
{

    re->clear(); im->clear(); abs->clear();

    for(int i=0; i<FID.at(k)->al(); i++)
    {
        re->append(QPointF(i,FID.at(k)->real->sig.at(i)));
        im->append(QPointF(i,FID.at(k)->imag->sig.at(i)));
        abs->append(QPointF(i,FID.at(k)->abs->sig.at(i)));

    }

    return true;
}

bool TFID_2D::ReadsmdFile(QString fn)
{
    QFileInfo fi;
    fi.setFile(fn);
    QString base = fi.completeBaseName();
    QString path = fi.absolutePath()+'/';
    QString smd = path+base+".smd";

    // qDebug()<< "1";

    QMutexLocker locker(&mutex);

    QFile file(fn);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
         errorMessage=QString(Q_FUNC_INFO)+ ": Failed to open " + fn;
         return false;
    }

    QString source; source.clear();
    QTextStream in(&file);

    bool pointDefined=false;
    bool dwDefined=false;
    bool sf1Defined=false;

    bool ok;

    parameters.clear();

    while (!in.atEnd())
    {
        source=in.readLine();
        source.remove(QChar(' '), Qt::CaseInsensitive);
        if(source.startsWith('#')) break;
        parameters.append(source);

        if(source.startsWith("point=",Qt::CaseInsensitive))
        {
            pointDefined=true;
            int i=source.mid(6).toInt(&ok);
            if(!ok) {errorMessage="Invalid expression for point="; file.close(); return false;}
            setDefaultAl(i);
        }
        if(source.startsWith("dw=",Qt::CaseInsensitive))
        {
            dwDefined=true;
            double d=source.mid(3).toDouble(&ok);
            if(!ok) {errorMessage="Invalid expression for dw="; file.close(); return false;}
            setDW(d);
        }
        if(source.startsWith("sf1=",Qt::CaseInsensitive))
        {
            sf1Defined=true;
            double s=source.mid(4).toDouble(&ok);
            if(!ok) {errorMessage="Invalid expression for sf1="; file.close(); return false;}
            setSF1(s);
        }

    }

    if(!pointDefined)
    {
        errorMessage="point=xxx is missing.";  file.close(); return false;
    }
    if(!dwDefined)
    {
        errorMessage="dw=xxx is missing.";  file.close(); return false;
    }
    if(!sf1Defined)
    {
        errorMessage="sf1=xxx is missing.";  file.close(); return false;
    }

//    qDebug() << defaultAl() << dw() << sf1() ;

    FID.clear();
    FID.append(new TFID(defaultAl()));
    FID.last()->setDW(dw());
    FID.last()->setSF1(sf1());

    QString line;
    QStringList sl;
    for(int k=0; k<defaultAl(); k++)
    {
        if(in.atEnd())
        {
            errorMessage="Data is shorter than " + QString::number(defaultAl());
            file.close(); return false;
        }

        line = in.readLine();
        line=line.trimmed();
        line.replace("\t"," ");
        ok=line.contains("  ");
        while(ok)
        {
            line.replace("  "," ");
            ok=line.contains("  ");
        }

        sl=line.split(' ');
        if(sl.size()<1)
        {
            errorMessage = "Data is empty at " + QString::number(k);
            file.close(); return false;
        }

        if(sl.size()==1)
        {
            FID[0]->real->sig[k]=sl.at(0).toDouble();
            FID[0]->imag->sig[k]=0.0;
        }
        else
        {
            FID[0]->real->sig[k]=sl.at(0).toDouble();
            FID[0]->imag->sig[k]=sl.at(1).toDouble();
        }

    }


    file.close();

    FID.last()->updateAbs();
    FID.last()->setEmpty(false);

    comments.clear();

    setCurrentFID(0);

    return true;

}

bool TFID_2D::ReadopaFile(QString fn)
// still incomplete... (6 Jul 2020 KT)
{
    QMutexLocker locker(&mutex);

    QFile file(fn);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
         errorMessage=QString(Q_FUNC_INFO)+ ": Failed to open " + fn;
         return false;
    }

    QString source; source.clear();
    QTextStream in(&file);

    bool ok;
    QString line;
    QStringList sl;

    FID.append(new TFID(defaultAl()));
    FID[FID.size()-1]->setSF1(sf1());
    FID[FID.size()-1]->setDW(dw());

    int k=0;

    while(k<defaultAl())
    {
//    for(int k=0; k<al(); k++)
//    {
        if(in.atEnd())
        {
            errorMessage="Data is shorter than " + QString::number(defaultAl());
            file.close(); return false;
        }

        line = in.readLine();
        line=line.trimmed();
        line.replace("\t"," ");
        ok=line.contains("  ");
        while(ok)
        {
            line.replace("  "," ");
            ok=line.contains("  ");
        }

        sl=line.split(' ');
        if(sl.size()<1)
        {
            errorMessage = "Data is empty at " + QString::number(k);
            file.close(); return false;
        }

        if(sl.size()==1)
        {
            FID.last()->real->sig[k]=sl.at(0).toDouble();
            FID.last()->imag->sig[k]=0.0;
        }
        else
        {
            FID.last()->real->sig[k]=sl.at(0).toDouble();
            FID.last()->imag->sig[k]=sl.at(1).toDouble();
        }

        k++;
    }

    FID.last()->updateAbs();
    FID.last()->setCustomXAxis(isXAxisCustom());
    FID.last()->setXInitialValue(xInitialValue());
    FID.last()->setDx(dx());
    FID.last()->setXAxisLabel(xAxisLabel());
    FID.last()->setXAxisUnitSymbol(xAxisUnitSymbol());
    FID.last()->setPrefix(prefix());
    FID.last()->setPlotPrefix(plotPrefix());
    FID.last()->setDomain(TFID::TimeDomain);

    FID.last()->setEmpty(false);




    file.close();
    setCurrentFID(0);
    return true;
}

bool TFID_2D::ReadopaFiles(QString fn)
{
    QFileInfo fi;
    fi.setFile(fn);
    QString base = fi.completeBaseName();
    QString path = fi.absolutePath()+'/';
    QString opp = path+base+".opp";
    QString opa = path+base+".opa";

    if(!ReadoppFile(opp)) return false;
    if(!ReadopaFile(opa)) return false;

    return true;
}

bool TFID_2D::ReadopFiles(QString fn)
{
    QFileInfo fi;
    fi.setFile(fn);
    QString base = fi.completeBaseName();
    QString path = fi.absolutePath()+'/';
    QString opp = path+base+".opp";
    QString opd = path+base+".opd";

    if(!ReadoppFile(opp)) return false;
    if(!ReadopdFile(opd)) return false;

    return true;
}


bool TFID_2D::Readsm2Files(QString fn)
{
    QFileInfo fi;
    fi.setFile(fn);
    QString base = fi.completeBaseName();
    QString path = fi.absolutePath()+'/';
    QString sm2p = path+base+".sm2p";
    QString sm2d = path+base+".sm2d";

    if(!Readsm2pFile(sm2p)) return false;
    if(!Readsm2dFile(sm2d)) return false;

    return true;
}

bool TFID_2D::WriteopFiles(QString fn, QIODevice::OpenModeFlag flag)
{
    QFileInfo fi;
    fi.setFile(fn);
    QString base = fi.completeBaseName();
    QString path = fi.absolutePath()+'/';
    QString opp = path+base+".opp";
    QString opd = path+base+".opd";

    if(!WriteoppFile(opp)) return false;
    if(!WriteopdFile(opd,flag)) return false;

    return true;
}

bool TFID_2D::Writesm2Files(QString fn, QIODevice::OpenModeFlag flag)
{
    QFileInfo fi;
    fi.setFile(fn);
    QString base = fi.completeBaseName();
    QString path = fi.absolutePath()+'/';
    QString sm2p = path+base+".sm2p";
    QString sm2d = path+base+".sm2d";

    if(!Writesm2pFile(sm2p)) return false;
    if(!Writesm2dFile(sm2d,flag)) return false;

    return true;
}

bool TFID_2D::ReadoppFile(QString fn)
{
    QMutexLocker locker(&mutex);
    QFile file(fn);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
         errorMessage=QString(Q_FUNC_INFO)+ ": Failed to open " + fn;
         return false;
    }

    QString source; source.clear();
    QTextStream in(&file);

    bool pointDefined=false;
    bool dwDefined=false;
    bool sf1Defined=false;

    parameters.clear();

    bool ok;

    while (!in.atEnd())
    {
        source=in.readLine();
        parameters.append(source);
        source.remove(QChar(' '), Qt::CaseInsensitive);
        if(source.startsWith('#')) break;
        if(source.startsWith("point=",Qt::CaseInsensitive))
        {
            pointDefined=true;
            FAL=source.mid(6).toInt(&ok);
            if(!ok) {errorMessage="Invalid expression for point="; file.close(); return false;}
        }
        if(source.startsWith("dw=",Qt::CaseInsensitive))
        {
            dwDefined=true;
            FDW=source.mid(3).toDouble(&ok);
            if(!ok) {errorMessage="Invalid expression for dw="; file.close(); return false;}
        }
        if(source.startsWith("sf1=",Qt::CaseInsensitive))
        {
            sf1Defined=true;
            FSF1=source.mid(4).toDouble(&ok);
            if(!ok) {errorMessage="Invalid expression for sf1="; file.close(); return false;}
        }

    }

    comments.clear();
//    while(!file.atEnd()) comments << in.readLine();
    while(!in.atEnd()) comments << in.readLine();


    file.close();

    if(!pointDefined)
    {
        errorMessage="point=xxx is missing."; file.close(); return false;
    }
    if(!dwDefined)
    {
        errorMessage="dw=xxx is missing."; file.close(); return false;
    }
    if(!sf1Defined)
    {
        errorMessage="sf1=xxx is missing."; file.close(); return false;
    }

    QSettings settings(fn,QSettings::IniFormat);
    settings.beginGroup("XAxis");
      setCustomXAxis(settings.value("isCustom",false).toBool());
      setXInitialValue(settings.value("xInitialValue",0).toDouble());
      setDx(settings.value("xIncrement",0).toDouble());
      setXAxisLabel(settings.value("xAxisLabel","").toString());
      setXAxisUnitSymbol(settings.value("xAxisUnitSymbol","").toString());
      setPrefix(settings.value("metricPrefix","None").toString());
      setPlotPrefix(settings.value("plotMetricPrefix","None").toString());
      //qDebug() << metricPrefix.prefix() << plotMetricPrefix.prefix();
    settings.endGroup();

/*
        settings.beginGroup("XAxis");
          settings.setValue("isCustom",FID.at(0)->isXAxisCustom());
          settings.setValue("xInitialValue",FID.at(0)->xInitialValue());
          settings.setValue("xIncrement",FID.at(0)->dx());
          settings.setValue("xAxisLabel",FID.at(0)->xAxisLabel());
          settings.setValue("xAxisUnitSymbol",FID.at(0)->xAxisUnitSymbol());
          settings.setValue("metricPrefix",TMetricPrefix::Name(FID.at(0)->metricPrefix.prefix()));
          settings.setValue("plotMetricPrefix",TMetricPrefix::Name(FID.at(0)->plotMetricPrefix.prefix()));
        settings.endGroup();
 */
    return true;
}


bool TFID_2D::Readsm2pFile(QString fn)
{
    QMutexLocker locker(&mutex);
    QFile file(fn);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
         errorMessage=QString(Q_FUNC_INFO)+ ": Failed to open " + fn;
         return false;
    }

    QString source; source.clear();
    QTextStream in(&file);

    bool pointDefined=false;
    bool dwDefined=false;
    bool sf1Defined=false;

    parameters.clear();

    bool ok;

    while (!in.atEnd())
    {
        source=in.readLine();
        parameters.append(source);
        source.remove(QChar(' '), Qt::CaseInsensitive);
        if(source.startsWith('#')) break;
        if(source.startsWith("point=",Qt::CaseInsensitive))
        {
            pointDefined=true;
            FAL=source.mid(6).toInt(&ok);
            if(!ok) {errorMessage="Invalid expression for point="; file.close(); return false;}
        }
        if(source.startsWith("dw=",Qt::CaseInsensitive))
        {
            dwDefined=true;
            FDW=source.mid(3).toDouble(&ok);
            if(!ok) {errorMessage="Invalid expression for dw="; file.close(); return false;}
        }
        if(source.startsWith("sf1=",Qt::CaseInsensitive))
        {
            sf1Defined=true;
            FSF1=source.mid(4).toDouble(&ok);
            if(!ok) {errorMessage="Invalid expression for sf1="; file.close(); return false;}
        }

    }

    comments.clear();
//    while(!file.atEnd()) comments << in.readLine();
    while(!in.atEnd()) comments << in.readLine();


    file.close();

    setXAxisLabel("");
    setXAxisUnitSymbol("sec");
//qDebug () << QString(Q_FUNC_INFO) << ": " << xAxisUnitSymbol();
    setPrefix("micro");
    setPlotPrefix("milli");


    if(!pointDefined)
    {
        errorMessage="point=xxx is missing."; file.close(); return false;
    }
    if(!dwDefined)
    {
        errorMessage="dw=xxx is missing."; file.close(); return false;
    }
    if(!sf1Defined)
    {
        errorMessage="sf1=xxx is missing."; file.close(); return false;
    }

    // qDebug() << FAL << FDW << FSF1 << "\n";


    return true;
}

bool TFID_2D::WriteoppFile(QString fn)
{
    QMutexLocker locker(&mutex);

    QFile file(fn);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
         errorMessage=QString(Q_FUNC_INFO)+ ": Failed to open " + fn;
         return false;
    }
    file.close();

        QSettings settings(fn+".tmp", QSettings::IniFormat);
        settings.beginGroup("Log");
    //      settings.setValue("NA",FID.at(0)->na());
          settings.setValue("actualNA",FID.at(0)->actualNA);
        settings.endGroup();

        settings.beginGroup("XAxis");
          settings.setValue("isCustom",FID.at(0)->isXAxisCustom());
          settings.setValue("xInitialValue",FID.at(0)->xInitialValue());
          settings.setValue("xIncrement",FID.at(0)->dx());
          settings.setValue("xAxisLabel",FID.at(0)->xAxisLabel());
          settings.setValue("xAxisUnitSymbol",FID.at(0)->xAxisUnitSymbol());
          settings.setValue("metricPrefix",TMetricPrefix::Name(FID.at(0)->prefix()));
          settings.setValue("plotMetricPrefix",TMetricPrefix::Name(FID.at(0)->plotPrefix()));
        settings.endGroup();
        settings.sync();

    QFile f1(fn+".tmp");
    f1.open(QIODevice::ReadOnly | QIODevice::Text);
    QStringList qsl;
    QTextStream in(&f1);

    while(!in.atEnd()) qsl.append(in.readLine());

    f1.close();

    if(QFile::exists(fn+".tmp")) QFile::remove(fn+".tmp");

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
         errorMessage=QString(Q_FUNC_INFO)+ ": Failed to open " + fn;
         return false;
    }

    QTextStream out(&file);
    out << "point=" << QString::number(defaultAl()) << "\n";
    out << "dw=" << QString::number(dw(),'g',12) << "\n";
    out << "sf1=" << QString::number(sf1(),'g',12) << "\n";
    out << "#\n";

    for(int i=0; i<comments.size(); i++) out << comments.at(i) << "\n";

    for(int k=0; k< qsl.size(); k++) out << qsl.at(k) << "\n";

    file.close();
    return true;
}


bool TFID_2D::Writesm2pFile(QString fn)
{
    QMutexLocker locker(&mutex);

    QFile file(fn);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
         errorMessage=QString(Q_FUNC_INFO)+ ": Failed to open " + fn;
         return false;
    }

    QTextStream out(&file);
    out << "point=" << QString::number(defaultAl()) << "\n";
    out << "dw=" << QString::number(dw(),'g',12) << "\n";
    out << "sf1=" << QString::number(sf1(),'g',12) << "\n";
    out << "#\n";
    for(int i=0; i<comments.size(); i++) out << comments.at(i);

    file.close();
    return true;
}

bool TFID_2D::WriteopaFile(QString fn, QIODevice::OpenModeFlag flag)
{
    QMutexLocker locker(&mutex);

    QFile file(fn);
    if (!file.open(flag | QIODevice::Text))
    {
         errorMessage=QString(Q_FUNC_INFO)+ ": Failed to open " + fn;
         return false;
    }

    QTextStream out(&file);
//    out << "point=" << QString::number(al()) << "\n";
//    out << "dw=" << QString::number(dw(),'g',12) << "\n";
//    out << "sf1=" << QString::number(sf1(),'g',12) << "\n";
//    out << "#\n";
//    for(int i=0; i<comments.size(); i++) out << comments.at(i);

    for(int i=0; i<FID.size(); i++)
    {
        for(int k=0; k<FID.at(i)->al(); k++)
        {
           out << QString::number(FID.at(i)->real->sig.at(k),'g',12)
               << " "
               << QString::number(FID.at(i)->imag->sig.at(k),'g',12)
               << "\n";
        } // k
        out << "\n";
    } // i


    file.close();
    return true;

}

bool TFID_2D::WriteopaFile(QStringList fnList, QIODevice::OpenModeFlag flag)
{
    int currentPosition=0;
    int count=0;
    bool ok;

    QMutexLocker locker(&mutex);
    while(count<FID.size())
    {
      ok=FID[count]->WriteopaFile(fnList.at(currentPosition),flag);
      if(!ok)
      {
          errorMessage=QString(Q_FUNC_INFO)+ ": Failed to save data.";
          return false;
      }

      currentPosition++;
      if(currentPosition>fnList.size()-1) currentPosition=0;
      count++;
    }

    return true;
}

// JEOL Format
bool TFID_2D::ReadjdfFile(QString fn)
{
    QMutexLocker locker(&mutex);

    QFile file(fn);
    if (!file.open(QIODevice::ReadOnly))
    {
         errorMessage=QString(Q_FUNC_INFO)+ ": Failed to open " + fn;
         return false;
    }

    QFileInfo fInfo;
    fInfo.setFile(file);

    QDataStream in(&file);
    in.setFloatingPointPrecision(QDataStream::DoublePrecision);
    in.setByteOrder(QDataStream::LittleEndian);

    in.skipRawData(1712);
    unsigned int po;
    in >> po;
    setDefaultAl(po);
    // qDebug() << al();

    // 1968 - 1716 = 252
    in.skipRawData(252);
    double d;
    in >> d;
    setSF1(d/1e6); // Hz -> MHz
    // qDebug() << QString(Q_FUNC_INFO) << sf1();

    // 2608 - 1976 = 632
    in.skipRawData(632);
    in >> d;
    setDW(1e6*d/defaultAl());
    // qDebug() << QString(Q_FUNC_INFO) << dw();


    //16384 - 2616 = 13768
    in.skipRawData(13768);
    FID.clear();

    for(int k=0; k<1; k++)
    {
        FID.append(new TFID(defaultAl()));
        FID[FID.size()-1]->setSF1(sf1());
        FID[FID.size()-1]->setDW(dw());
        for(int m=0; m<defaultAl(); m++) in >> FID[k]->imag->sig[m];
        for(int m=0; m<defaultAl(); m++) in >> FID[k]->real->sig[m];
        FID[k]->updateAbs();
        FID[k]->setCustomXAxis(isXAxisCustom());
        FID[k]->setXInitialValue(xInitialValue());
        FID[k]->setDx(dx());
        FID[k]->setXAxisLabel(xAxisLabel());
        FID[k]->setXAxisUnitSymbol(xAxisUnitSymbol());
        FID[k]->setPrefix(prefix());
        FID[k]->setPlotPrefix(plotPrefix());

        FID[k]->setDomain(TFID::TimeDomain);

        FID[k]->setEmpty(false);
    } // k

    setCurrentFID(0);

    file.close();

    return true;
}

bool TFID_2D::ReadopdFile(QString fn)
{
    QMutexLocker locker(&mutex);

    QFile file(fn);
    if (!file.open(QIODevice::ReadOnly))
    {
         errorMessage=QString(Q_FUNC_INFO)+ ": Failed to open " + fn;
         return false;
    }

    QFileInfo fInfo;
    fInfo.setFile(file);
    int nByte=fInfo.size();
    int n=nByte/(2*FAL*sizeof(double));

    QDataStream in(&file);
    in.setFloatingPointPrecision(QDataStream::DoublePrecision);
    in.setByteOrder(QDataStream::LittleEndian);

    FID.clear();

    for(int k=0; k<n; k++)
    {
        FID.append(new TFID(FAL));
        FID[FID.size()-1]->setSF1(sf1());
        FID[FID.size()-1]->setDW(FDW);
        for(int m=0; m<FAL; m++) in >> FID[k]->real->sig[m] >> FID[k]->imag->sig[m];
        FID[k]->updateAbs();
        FID[k]->setCustomXAxis(isXAxisCustom());
        FID[k]->setXInitialValue(xInitialValue());
        FID[k]->setDx(dx());
        FID[k]->setXAxisLabel(xAxisLabel());
        FID[k]->setXAxisUnitSymbol(xAxisUnitSymbol());
        FID[k]->setPrefix(prefix());
        FID[k]->setPlotPrefix(plotPrefix());

        FID[k]->setDomain(TFID::TimeDomain);

        FID[k]->setEmpty(false);
    } // k

    file.close();
    setCurrentFID(0);
    return true;
}


bool TFID_2D::Readsm2dFile(QString fn)
{
    QMutexLocker locker(&mutex);

    QFile file(fn);
    if (!file.open(QIODevice::ReadOnly))
    {
         errorMessage=QString(Q_FUNC_INFO)+ ": Failed to open " + fn;
         return false;
    }

    QFileInfo fInfo;
    fInfo.setFile(file);
    int nByte=fInfo.size();
    int n=nByte/(2*FAL*sizeof(float));

    QDataStream in(&file);
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);
    in.setByteOrder(QDataStream::LittleEndian);

    FID.clear();

    for(int k=0; k<n; k++)
    {
        FID.append(new TFID(FAL));
        FID.at(FID.size()-1)->setDW(FDW);
        FID[FID.size()-1]->setSF1(sf1());

        for(int m=0; m<FAL; m++) in >> FID[k]->real->sig[m] >> FID[k]->imag->sig[m];
        FID[k]->updateAbs();
        FID[k]->setEmpty(false);


        FID[k]->setXAxisLabel("");
        FID[k]->setXAxisUnitSymbol("sec");
        FID[k]->setPrefix("micro");
        FID[k]->setPlotPrefix("milli");

        FID[k]->setDomain(TFID::TimeDomain);


    } // k

    file.close();
    setCurrentFID(0);
    return true;
}

bool TFID_2D::Writesm2dFile(QString fn, QIODevice::OpenModeFlag flag)
{
    QMutexLocker locker(&mutex);

    QFile file(fn);
    if (!file.open(flag))
    {
         errorMessage=QString(Q_FUNC_INFO)+ ": Failed to open " + fn;
         return false;
    }

    QDataStream out(&file);
    out.setFloatingPointPrecision(QDataStream::SinglePrecision);
    out.setByteOrder(QDataStream::LittleEndian);

    for(int k=0; k<FID.size(); k++)
    {
        for(int m=0; m<FID.at(k)->al(); m++) out << (float) FID.at(k)->real->sig.at(m)
                                                 << (float) FID.at(k)->imag->sig.at(m);
    }

    file.close();
    return true;

}


bool TFID_2D::Writesm2dFile(QStringList fnList, QIODevice::OpenModeFlag flag)
{
    int currentPosition=0;
    int count=0;
    bool ok;

    QMutexLocker locker(&mutex);
    while(count<FID.size())
    {
      ok=FID[count]->Writesm2dFile(fnList.at(currentPosition),flag);
      if(!ok)
      {
          errorMessage=QString(Q_FUNC_INFO)+ ": Failed to save data.";
          return false;
      }

      currentPosition++;
      if(currentPosition>fnList.size()-1) currentPosition=0;
      count++;
    }

    return true;
}


bool TFID_2D::WriteopdFile(QStringList fnList, QIODevice::OpenModeFlag flag)
{
    int currentPosition=0;
    int count=0;
    bool ok;

    QMutexLocker locker(&mutex);
    while(count<FID.size())
    {
      ok=FID[count]->WriteopdFile(fnList.at(currentPosition),flag);
      if(!ok)
      {
          errorMessage=QString(Q_FUNC_INFO)+ ": Failed to save data.";
          return false;
      }

      currentPosition++;
      if(currentPosition>fnList.size()-1) currentPosition=0;
      count++;
    }

    return true;
}




bool TFID_2D::WriteopdFile(QString fn, QIODevice::OpenModeFlag flag)
{
    QMutexLocker locker(&mutex);

    QFile file(fn);
    if (!file.open(flag))
    {
         errorMessage=QString(Q_FUNC_INFO)+ ": Failed to open " + fn;
         return false;
    }

    QDataStream out(&file);
    out.setFloatingPointPrecision(QDataStream::DoublePrecision);
    out.setByteOrder(QDataStream::LittleEndian);

    for(int k=0; k<FID.size(); k++)
    {
        for(int m=0; m<FID.at(k)->al(); m++) out << FID.at(k)->real->sig.at(m) << FID.at(k)->imag->sig.at(m);
    }

    file.close();
    return true;

}


TFID::TFID(int al)
{
    FisEmpty=true;

    real=new THalfFID(al);
    imag=new THalfFID(al);
    abs=new THalfFID(al);
    FAL=al;
    FDW=1;
    FxInitialValue=0; // no metric prefix!
    Fdx=FDW*TMetricPrefix::Decimal(TMetricPrefix::Micro); // no metric prefix!
    FXAxisLabel="";

    FPhase0=0;
    setDomain(TimeDomain);
    setXUnit(TFIDXUnit::Second);
    setPrefix(TMetricPrefix::Micro);
    setPlotPrefix(TMetricPrefix::Milli);
    setCustomXAxis(false);

    // NonUniformSampling(Kobayashi)
    NUS = false;
    indirectTime = 0.0;
    nusTimes.clear();
    // end
}

QString TFID::xAxisUnitString()
{
    /*
    QString qs;
    switch(xunit)
    {
      case Second: qs="sec"; break;
      case Hz: qs="Hz"; break;
      case Tesla: qs="T"; break;
      case QuantumNumber: qs="Quantum Number"; break;
      case ppm: qs="ppm"; break;
      default: qs=""; break;
    }
    */

    return TMetricPrefix::Symbol(plotPrefix())
            +xAxisUnitSymbol();
}

double TFID::xValue(int k)
{
    int m=k;
    if(m<0) m=0; else if(m>al()-1) m=al()-1;

  //qDebug()<<QString(Q_FUNC_INFO) << "dx()" << dx();

    double d0=xInitialValue()+m*dx();
    d0/=TMetricPrefix::Decimal(plotPrefix());

    return d0;
}

int TFID::xIndex(double x)
{
    int k=0;
    double d=x;
    d*=TMetricPrefix::Decimal(plotPrefix());
    d-=xInitialValue();
    d/=dx();
    k=static_cast<int>(round(d));
    if(k<0) {k=0;}
    if(k>al()-1) {k=al()-1;}
    return k;
}

bool TFID::exportAscii(QString fn, int xini, int xfin)
{
    QFileInfo fi;
    fi.setFile(fn);
    QString base = fi.completeBaseName();
    QString path = fi.absolutePath()+'/';
    QString fileName = path+base+".dat";

    QMutexLocker locker(&mutex);

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
         errorMessage=QString(Q_FUNC_INFO)+ ": Failed to open " + fileName;
         return false;
    }

    QTextStream out(&file);
    for(int k=xini; k<=xfin; k++)
    out << QString::number(xValue(k),'g',12) << " "
        << QString::number(real->sig.at(k),'g',12) << " "
        << QString::number(imag->sig.at(k),'g',12) << " "
        << QString::number(abs->sig.at(k),'g',12)
        << "\n";

    file.close();
    return true;


}

bool TFID::exportAscii(QString fn)
{
    QFileInfo fi;
    fi.setFile(fn);
    QString base = fi.completeBaseName();
    QString path = fi.absolutePath()+'/';
    QString fileName = path+base+".dat";

    QMutexLocker locker(&mutex);

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
         errorMessage=QString(Q_FUNC_INFO)+ ": Failed to open " + fileName;
         return false;
    }

    QTextStream out(&file);
    for(int k=0; k<al(); k++)
    out << QString::number(xValue(k),'g',12) << " "
        << QString::number(real->sig.at(k),'g',12) << " "
        << QString::number(imag->sig.at(k),'g',12) << " "
        << QString::number(abs->sig.at(k),'g',12)
        << "\n";

    file.close();
    return true;

}

bool TFID::WriteopFiles(QString fn, QIODevice::OpenModeFlag flag)
{
    QFileInfo fi;
    fi.setFile(fn);
    QString base = fi.completeBaseName();
    QString path = fi.absolutePath()+'/';
    QString opp = path+base+".opp";
    QString opd = path+base+".opd";


    if(!WriteoppFile(opp)) return false;
    if(!WriteopdFile(opd,flag)) return false;

    return true;
}


bool TFID::Writesm2Files(QString fn, QIODevice::OpenModeFlag flag)
{
    QFileInfo fi;
    fi.setFile(fn);
    QString base = fi.completeBaseName();
    QString path = fi.absolutePath()+'/';
    QString sm2p = path+base+".sm2p";
    QString sm2d = path+base+".sm2d";


    if(!Writesm2pFile(sm2p)) return false;
    if(!Writesm2dFile(sm2d,flag)) return false;

    return true;
}


bool TFID::WriteoppFile(QString fn)
{
    QMutexLocker locker(&mutex);

    QFile file(fn);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
         errorMessage=QString(Q_FUNC_INFO)+ ": Failed to open " + fn;
         return false;
    }

    QTextStream out(&file);
    out << "point=" << QString::number(al()) << "\n";
    out << "dw=" << QString::number(dw(),'g',12) << "\n";
    out << "sf1=" << QString::number(sf1(),'g',12) << "\n";
    out << "#\n";

    for(int i=0; i<comment().size(); i++) out << comment().at(i);

    file.close();
    return true;
}


bool TFID::Writesm2pFile(QString fn)
{
    QMutexLocker locker(&mutex);

    QFile file(fn);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
         errorMessage=QString(Q_FUNC_INFO)+ ": Failed to open " + fn;
         return false;
    }

    QTextStream out(&file);
    out << "point=" << QString::number(al()) << "\n";
    out << "dw=" << QString::number(dw(),'g',12) << "\n";
    out << "sf1=" << QString::number(sf1(),'g',12) << "\n";
    out << "#\n";

    for(int i=0; i<comment().size(); i++) out << comment().at(i);

    file.close();
    return true;
}


bool TFID::WriteopaFile(QString fn, QIODevice::OpenModeFlag flag)
{
    QMutexLocker locker(&mutex);

    QFile file(fn);
    if (!file.open(flag | QIODevice::Text))
    {
         errorMessage=QString(Q_FUNC_INFO)+ ": Failed to open " + fn;
         return false;
    }

    QTextStream out(&file);

        for(int k=0; k<al(); k++)
        {
           out << QString::number(real->sig.at(k),'g',12)
               << " "
               << QString::number(imag->sig.at(k),'g',12)
               << "\n";
        } // k
        out << "\n";


    file.close();
    return true;

}

bool TFID::WriteopdFile(QString fn, QIODevice::OpenModeFlag flag)
{
    QMutexLocker locker(&mutex);

    QFile file(fn);
    if (!file.open(flag))
    {
         errorMessage=QString(Q_FUNC_INFO)+ ": Failed to open " + fn;
         return false;
    }

    QDataStream out(&file);
    out.setFloatingPointPrecision(QDataStream::DoublePrecision);
    out.setByteOrder(QDataStream::LittleEndian);

    for(int m=0; m<al(); m++) out << real->sig.at(m) << imag->sig.at(m);

    file.close();
    return true;

}


bool TFID::Writesm2dFile(QString fn, QIODevice::OpenModeFlag flag)
{
    QMutexLocker locker(&mutex);

    QFile file(fn);
    if (!file.open(flag))
    {
         errorMessage=QString(Q_FUNC_INFO)+ ": Failed to open " + fn;
         return false;
    }

    QDataStream out(&file);
    out.setFloatingPointPrecision(QDataStream::SinglePrecision);
    out.setByteOrder(QDataStream::LittleEndian);

    for(int m=0; m<al(); m++) out << (float) real->sig.at(m)
                                  << (float) imag->sig.at(m);

    file.close();
    return true;

}


bool TFID::WritesmdFile(QString fn)
{
    QMutexLocker locker(&mutex);

    QFileInfo fi;
    fi.setFile(fn);
    QString base = fi.completeBaseName();
    QString path = fi.absolutePath()+'/';
    QString smd = path+base+".smd";

    QFile file(smd);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
         errorMessage=QString(Q_FUNC_INFO)+ ": Failed to open " + smd;
         return false;
    }

    QTextStream out(&file);
    out << "point=" << QString::number(al()) << "\n";
    out << "dw=" << QString::number(dw(),'g',12) << "\n";
    out << "sf1=" << QString::number(sf1(),'g',12) << "\n";
    out << "#" << "\n";

    for(int i=0; i<al(); i++) out << QString::number(real->sig.at(i),'g',12)
                                  << " "
                                  << QString::number(imag->sig.at(i),'g',12)
                                  << "\n";

    file.close();
    return true;


}

bool TFID::ReadsmdFile(QString fn)
{
    QMutexLocker locker(&mutex);

    QFile file(fn);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
         errorMessage=QString(Q_FUNC_INFO)+ ": Failed to open " + fn;
         return false;
    }

    QString source; source.clear();
    QTextStream in(&file);

    bool pointDefined=false;
    bool dwDefined=false;
    bool sf1Defined=false;

    bool ok;

    while (!in.atEnd())
    {
        source=in.readLine();
        source.remove(QChar(' '), Qt::CaseInsensitive);
        if(source.startsWith('#')) break;
        if(source.startsWith("point=",Qt::CaseInsensitive))
        {
            pointDefined=true;
            int i=source.mid(6).toInt(&ok);
            if(!ok) {errorMessage="Invalid expression for point="; file.close(); return false;}
            setAL(i);
        }
        if(source.startsWith("dw=",Qt::CaseInsensitive))
        {
            dwDefined=true;
            double d=source.mid(3).toDouble(&ok);
            if(!ok) {errorMessage="Invalid expression for dw="; file.close(); return false;}
            setDW(d);
        }
        if(source.startsWith("sf1=",Qt::CaseInsensitive))
        {
            sf1Defined=true;
            double s=source.mid(4).toDouble(&ok);
            if(!ok) {errorMessage="Invalid expression for sf1="; file.close(); return false;}
            setSF1(s);
        }

    }

    if(!pointDefined)
    {
        errorMessage="point=xxx is missing.";  file.close(); return false;
    }
    if(!dwDefined)
    {
        errorMessage="dw=xxx is missing.";  file.close(); return false;
    }
    if(!sf1Defined)
    {
        errorMessage="sf1=xxx is missing.";  file.close(); return false;
    }

    QString line;
    QStringList sl;
    for(int k=0; k<al(); k++)
    {
        if(in.atEnd())
        {
            errorMessage="Data is shorter than " + QString::number(al());
            file.close(); return false;
        }

        line = in.readLine();
        line=line.trimmed();
        line.replace("\t"," ");
        ok=line.contains("  ");
        while(ok)
        {
            line.replace("  "," ");
            ok=line.contains("  ");
        }

        sl=line.split(' ');
        if(sl.size()<1)
        {
            errorMessage = "Data is empty at " + QString::number(k);
            file.close(); return false;
        }

        if(sl.size()==1)
        {
//            real->sig[k]=sl.at(0).toFloat();
            real->sig[k]=sl.at(0).toDouble();
            imag->sig[k]=0.0;
        }
        else
        {
//            real->sig[k]=sl.at(0).toFloat();
//            imag->sig[k]=sl.at(1).toFloat();
            real->sig[k]=sl.at(0).toDouble();
            imag->sig[k]=sl.at(1).toDouble();
        }

    }


    file.close();

    updateAbs();
    return true;

}

void TFID::setDW(double dw)
{
    FDW=dw;
    Fdx=FDW*TMetricPrefix::Decimal(TMetricPrefix::Micro);
}

void TFID::setSF1(double sf1)
{
    FSF1=sf1;
}

void TFID::setAL(int al)
{
    FAL=al;
    real->sig.resize(FAL);
    imag->sig.resize(FAL);
    abs->sig.resize(FAL);
}

double TFID::phase(int k)
{
    if(k<0 || k>FAL-1) return 0;
    else return atan2(imag->sig.at(k),real->sig.at(k));
}

void TFID::phaseReverse()
{
    for(int k=0; k<al(); k++)
    {
        imag->sig[k]=-imag->sig[k];
    }
}

void TFID::rotate(double angle)
{
    // qDebug() << QString(Q_FUNC_INFO) << angle;
  double phi=3.14159265358979*angle/180.0;
  double c=cos(phi);
  double s=sin(phi);
  double xr,xi;
  for(int k=0; k<FAL; k++){
      xr=real->sig.at(k); xi=imag->sig.at(k);
      real->sig[k]=c*xr-s*xi;
      imag->sig[k]=c*xi+s*xr;
  }
  FPhase0 += angle;
}

void TFID::fft()
{
    double theta;
    double xr,xi,wr,wi;
    int i,j,k,mh,irev,m,n,nh;

    // We get such n that 2^n=AL
    n=1;
    while(n<al()) n = n << 1;  // qDebug() << "n: " << n;

    // We zero-fill, if necessary
    if(al()<n)
    {
      k=al();
      setAL(n);
      for(int m=k; m<al(); m++)
      {
          real->sig[m]=0.0;
          imag->sig[m]=0.0;
          abs->sig[m]=0.0;
      }
      //warningQ=true;
      //warningMessage = "Automatic zero-fill to "
      //                 + QString::number(fid->al())
      //                 + " point has been performed.";
    }

//    theta=TWOPI/n;
    theta=6.28318530717958647692528676656/n;
    // ----  FFT main ----

    i=0;
    for(j=1; j<n-1; j++)
    {
      k = n >> 1;
      i = i ^ k;   // i xor k
      while(k>i)
      {
        k = k >> 1;
        i = i ^ k;
      }
      if(j<i)
      {
        xr = real->sig.at(j);
        xi = imag->sig.at(j);
        real->sig[j] = real->sig.at(i);
        imag->sig[j] = imag->sig.at(i);
        real->sig[i] = xr;
        imag->sig[i] = xi;
      }
    } // j

    mh=1;
    m = mh << 1;
    while(m<=n)
    {
      irev=0;
      i=0;
      while(i<n)
      {
        wr = cos(theta * irev);
        wi = sin(theta * irev);

        k = n >> 2;
        irev = irev ^ k;   // irev xor k
        while(k>irev)
        {
          k = k >> 1;
          irev = irev ^ k;
        }
        for(j=i; j<mh+i; j++)
        {
          k = j + mh;
          xr = real->sig.at(j) - real->sig.at(k);
          xi = imag->sig.at(j) - imag->sig.at(k);

          real->sig[j] += real->sig.at(k);
          imag->sig[j] += imag->sig.at(k);

          real->sig[k] = wr*xr - wi*xi;
          imag->sig[k] = wr*xi + wi*xr;
        }  // j

        i=i+m;

      } // while(i<n)

      mh = m;
      m = mh << 1;
    } // while(m<=n)

    // Data arrangement for NMR (left: positive freq., right: negative freq.)
    nh = n/2;
    for(k=0; k<nh; k++)
    {
      xr = real->sig.at(k);
      xi = imag->sig.at(k);
      real->sig[k] = real->sig.at(nh+k);
      imag->sig[k] = imag->sig.at(nh+k);
      real->sig[nh+k] = xr;
      imag->sig[nh+k] = xi;
    }

    for(k=0; k<al(); k++)
    {
        real->sig[k] = real->sig[k]/al();
        imag->sig[k] = imag->sig[k]/al();
    }
    updateAbs();
/*
    if(xunit==TFID::Second)
    {
        xunit=TFID::Hz;
        Fdx = -1.0/(dw()*TMetricPrefix::Decimal(TMetricPrefix::Micro)*al());
        FxInitialValue= 0.5/(dw()*TMetricPrefix::Decimal(TMetricPrefix::Micro));

    }
    else if(xunit==TFID::Hz)
    {
        xunit=TFID::Second;
        Fdx=dw()*TMetricPrefix::Decimal(TMetricPrefix::Micro);
        FxInitialValue=0;

    }
*/
    if(domain()==TFID::TimeDomain) {setDomain(TFID::FrequencyDomain);}
    else if(domain()==TFID::FrequencyDomain) {setDomain(TFID::TimeDomain);}

    return;
}

void TFID::updateAbs()
{
   double re,im;
   for(int k=0; k<FAL; k++)
   {
       re=real->sig.at(k); im=imag->sig.at(k);
       abs->sig[k]=sqrt(re*re+im*im);
   }
}

void TFID::swapDomain()
{
    if(domain()==TimeDomain)
      {setDomain(FrequencyDomain);}
    else  // domain should be Frequency
      {setDomain(TimeDomain);}
}



double THalfFID::max(int ini, int fin)
{
    double m;
    int mini,mfin;
    if(ini<0) {mini=0;} else {mini=ini;}
    if(fin>sig.size()-1) {mfin=sig.size()-1;} else {mfin=fin;}

    m=sig.at(mini);
    for(int k=mini; k<=mfin; k++)
    {
        if(m<sig.at(k)) m=sig.at(k);
    }
    return m;
}


double THalfFID::max()
{
    return max(0,sig.size()-1);
}

int THalfFID::maxAt(int ini, int fin)
{
    int p;

    double m;
    int mini,mfin;
    if(ini<0) {mini=0;} else {mini=ini;}
    if(fin>sig.size()-1) {mfin=sig.size()-1;} else {mfin=fin;}

    p=mini;
    m=sig.at(mini);
    for(int k=mini; k<=mfin; k++)
    {
        if(m<sig.at(k)) {m=sig.at(k); p=k;}
    }

    return p;
}

int THalfFID::maxAt()
{
    return maxAt(0,sig.size()-1);
}

double THalfFID::min(int ini, int fin)
{
    double m;
    int mini,mfin;
    if(ini<0) {mini=0;} else {mini=ini;}
    if(fin>sig.size()-1) {mfin=sig.size()-1;} else {mfin=fin;}

    m=sig.at(mini);
    for(int k=mini; k<=mfin; k++)
    {
        if(m>sig.at(k)) m=sig.at(k);
    }
    return m;
}

double THalfFID::min()
{
    return min(0,sig.size()-1);
}

double THalfFID::absMax(int ini, int fin)
{
    double m;
    int mini,mfin;
    if(ini<0) {mini=0;} else {mini=ini;}
    if(fin>sig.size()-1) {mfin=sig.size()-1;} else {mfin=fin;}

    m=fabs(sig.at(mini));
    for(int k=mini; k<=mfin; k++)
    {
        if(m<fabs(sig.at(k))) m=fabs(sig.at(k));
    }
    return m;
}

double THalfFID::absMax()
{
    return absMax(0,sig.size()-1);
}


double THalfFID::sum()
{
  double result=0.0;
  if (!sig.isEmpty())
  {
      for(int k=0; k<sig.size(); ++k) result += sig.at(k);
  }
  return result;
}

double THalfFID::sum(int ini, int fin)
{
  double result=0.0;
  int mini,mfin;
  if(ini>fin)
  {
      mini=fin;
      mfin=ini;
  }
  else
  {
      mini=ini;
      mfin=fin;
  }

  if(mini<0) {mini=0;}
  if(mfin>sig.size()-1) {mfin=sig.size()-1;}

  if (!sig.isEmpty())
  {
      for(int k=mini; k<=mfin; ++k) result += sig.at(k);
  }
  return result;
}

double THalfFID::average()
{
    double result=0.0;
    if(!sig.empty()) result=sum()/sig.size();
    return result;
}

double THalfFID::average(int ini,int fin)
{
    double result=0.0;
    int mini,mfin;
    if(ini>fin)
    {
        mini=fin;
        mfin=ini;
    }
    else
    {
        mini=ini;
        mfin=fin;
    }

    if(mini<0) {mini=0;}
    if(mfin>sig.size()-1) {mfin=sig.size()-1;}

    result=sum(mini,mfin)/(mfin-mini+1);
    return result;
}

double THalfFID::standardDeviation(int ini, int fin)
{
    double av=0,sigma=0;
    int mini,mfin;
    if(ini>fin)
    {
        mini=fin;
        mfin=ini;
    }
    else
    {
        mini=ini;
        mfin=fin;
    }

    if(mini<0 || mfin>sig.size()-1) sigma=0.0;
    else
    {
        av=average(mini,mfin);
        for(int k=mini; k<=mfin; k++) sigma+=(sig.at(k)-av)*(sig.at(k)-av);
        sigma=sigma/(mfin-mini+1);
        sigma=sqrt(sigma);
    }

  return sigma;
}

double THalfFID::standardDeviation()
{
    return standardDeviation(0,sig.size()-1);
}
