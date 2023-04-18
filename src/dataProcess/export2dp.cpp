#include "export2dp.h"
#include <math.h>

#include <QDebug>
#include <QString>
#include <QFile>

KExport2DP::KExport2DP()
{
    setAxisDomain(fidDomain::ToggleDomain);
}

KExport2DP::~KExport2DP()
{
    ;
}

bool KExport2DP::process(TFID_2D *fid_2d, QString fn, int sf1, int ef1,
                         int line, double contL, double contH)
{
    double max=0.0, min=0.0, base=0.0;
    int f1, f2;

    QFile file(fn);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
         fid_2d->errorMessage=QString(Q_FUNC_INFO)+ ": Failed to open " + fn;
         return false;
    }
    file.close();

    if(QFile::exists(fn+".tmp")) QFile::remove(fn+".tmp");

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
         setErrorMessage(QString(Q_FUNC_INFO)+ ": Failed to open " + fn);
         return false;
    }

    bool get1=getMaxMinParameter(fid_2d, &max, &min, &base, &f1, &f2);
    if(get1==false) return false;

    double befMax = max;
    int i, j;
    bool multiplied = false;

    if(befMax<10e4){
        for(i=0;i<fid_2d->FID.size();i++){
            for(j=0;j<fid_2d->defaultAl();j++){
                fid_2d->FID[i]->real->sig[j] = fid_2d->FID[i]->real->sig[j] * 10e4;
            }
        }
        multiplied = true;
    }

    contL = contL / 100;
    contH = contH / 100;

    //--------write parameter for contour plot-----------
    QTextStream out(&file);
    out << "[Points]" << "\n";
    out << "t1 = " << QString::number(fid_2d->FID.size()) << "\n";
    out << "t2 = " << QString::number(fid_2d->defaultAl()) << "\n";

    out << "[Area]" << "\n";
    out << "F1 = " << QString::number(sf1) << "\n";
    out << "to = " << QString::number(ef1) << "\n";
    out << "F2 = " << QString::number(sf1) << "\n";
    out << "to = " << QString::number(ef1) << "\n";

    bool get2=getMaxMinParameter(fid_2d, &max, &min, &base, &f1, &f2);

    if(get2==false) return false;

    out << "[Max and Min]" << "\n";
    out << "Max = " << QString::number(max,'E',14) <<"\n";
    out << "Min =" << QString::number(min,'E',14) <<"\n";
    out << "Base= " << QString::number(base,'E',14) <<"\n";
    out << "F2  = " << QString::number(f2) <<"\n";
    out << "F1  = " << QString::number(f1) <<"\n";

    out << "[Contours]" << "\n";
    out << "Linear" << "\n";
    out << " + : " << QString::number(line) << "\n";
    out << " - : 0" << "\n";
    out << "High= " << QString::number(contH) << "\n";
    out << "Low = " << QString::number(contL) << "\n";

    out << "[t1/f1]" << "\n";
    out << "Width   = " << QString::number(1.0e6 / fid_2d->dw()) <<"\n";
    out << "Larmour = " << QString::number(fid_2d->sf1()) <<"\n";
    out << "Point   = " << QString::number(fid_2d->defaultAl()) << "\n";
    out << "Todat   = " << QString::number(fid_2d->defaultAl()) << "\n";
    out << "True    = " << QString::number(fid_2d->defaultAl()) << "\n";
    out << "Reference value = 0.00000" << "\n";
    out << "Reference mark  = 0.00000" << "\n";
    out << "Reference point = 1" << "\n";
    out << "kHz" << "\n";
    out << "5" << "\n";

    out << "[t2/f2]" << "\n";
    out << "Width   = " << QString::number(1.0e6 / fid_2d->dw()) <<"\n";
    out << "Larmour = " << QString::number(fid_2d->sf1()) <<"\n";
    out << "Point   = " << QString::number(fid_2d->defaultAl()) << "\n";
    out << "Todat   = " << QString::number(fid_2d->defaultAl()) << "\n";
    out << "True    = " << QString::number(fid_2d->defaultAl()) << "\n";
    out << "Reference value = 0.00000" << "\n";
    out << "Reference mark  = 0.00000" << "\n";
    out << "Reference point = 1" << "\n";
    out << "kHz" << "\n";
    out << "5" << "\n";

    out << "[Options]" << "\n";
    out << "No Diag" << "\n";
    out << "Same F1F2" << "\n";
    out << "No Mirror" << "\n";
    out << "BlackLine" << "\n";
    out << "Norm_Line" << "\n";
    if(multiplied){
        out << "# multiplied 100000";
    }

    file.close();

    return true;
}

bool KExport2DP::getMaxMinParameter(TFID_2D *fid_2d, double *max, double *min,
                                    double *base, int *f1, int *f2)
{
    int i, j, f1s=0, f2s=0;
    double mx = -1.0e15;
    double mn =  1.0e15;
    double bs = 0.0;
    double data;

    for(i=0; i<fid_2d->FID.size(); i++)
    {
        for(j=0; j<fid_2d->defaultAl(); j++)
        {
            data = fid_2d->FID.at(i)->real->sig.at(j);
            if(data < mn) mn = data;
            if(data > mx)
            {
                mx = data;
                f1s = i;
                f2s = j;
            }
            if(i-1<j)
            {
                bs += data;
            }
        }
    }
    bs = bs / double(fid_2d->defaultAl() * fid_2d->defaultAl());

    *max = mx;
    *min = mn;
    *base = bs;
    *f1 = f1s;
    *f2 = f2s;

    return true;
}

