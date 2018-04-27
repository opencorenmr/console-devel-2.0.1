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

bool KExport2DP::process(TFID_2D *fid_2d, QString fn, int sf1, int ef1, int sf2, int ef2)
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
         errorMessage=QString(Q_FUNC_INFO)+ ": Failed to open " + fn;
         return false;
    }

    //--------write parameter for contour plot-----------
    QTextStream out(&file);
    out << "[Points]" << "\r\n";
    out << "t1 = " << QString::number(fid_2d->FID.size()) << "\r\n";
    out << "t2 = " << QString::number(fid_2d->al()) << "\r\n";

    out << "[Area]" << "\r\n";
    out << "F1 = " << QString::number(sf1) << "\r\n";
    out << "to = " << QString::number(ef1) << "\r\n";
    out << "F2 = " << QString::number(sf2) << "\r\n";
    out << "to = " << QString::number(ef2) << "\r\n";

    bool get=getMaxMinParameter(fid_2d, &max, &min, &base, &f1, &f2);
    if(get==false) return false;

    out << "[Max and Min]" << "\r\n";
    out << "Max = " << QString::number(max,'E',14) <<"\r\n";
    out << "Min =" << QString::number(min,'E',14) <<"\r\n";
    out << "Base= " << QString::number(base,'E',14) <<"\r\n";
    out << "F2  = " << QString::number(f2) <<"\r\n";
    out << "F1  = " << QString::number(f1) <<"\r\n";

    out << "[Contours]" << "\r\n";
    out << "Linear" << "\r\n";
    out << " + : 10" << "\r\n";
    out << " - : 0" << "\r\n";
    out << "High= 0.99" << "\r\n";
    out << "Low = 0.05" << "\r\n";

    out << "[t1/f1]" << "\r\n";
    out << "Width   = " << QString::number(1.0e6 / fid_2d->dw()) <<"\r\n";
    out << "Larmour = " << QString::number(fid_2d->sf1()) <<"\r\n";
    out << "Point   = " << QString::number(fid_2d->al()) << "\r\n";
    out << "Todat   = " << QString::number(fid_2d->al()) << "\r\n";
    out << "True    = " << QString::number(fid_2d->al()) << "\r\n";
    out << "Reference value = 0.00000" << "\r\n";
    out << "Reference mark  = 0.00000" << "\r\n";
    out << "Reference point = 1" << "\r\n";
    out << "kHz" << "\r\n";
    out << "5" << "\r\n";

    out << "[t2/f2]" << "\r\n";
    out << "Width   = " << QString::number(1.0e6 / fid_2d->dw()) <<"\r\n";
    out << "Larmour = " << QString::number(fid_2d->sf1()) <<"\r\n";
    out << "Point   = " << QString::number(fid_2d->al()) << "\r\n";
    out << "Todat   = " << QString::number(fid_2d->al()) << "\r\n";
    out << "True    = " << QString::number(fid_2d->al()) << "\r\n";
    out << "Reference value = 0.00000" << "\r\n";
    out << "Reference mark  = 0.00000" << "\r\n";
    out << "Reference point = 1" << "\r\n";
    out << "kHz" << "\r\n";
    out << "5" << "\r\n";

    out << "[Options]" << "\r\n";
    out << "No Diag" << "\r\n";
    out << "Same F1F2" << "\r\n";
    out << "No Mirror" << "\r\n";
    out << "BlackLine" << "\r\n";
    out << "Norm_Line" << "\r\n";

    file.close();

    return true;
}

bool KExport2DP::getMaxMinParameter(TFID_2D *fid_2d, double *max, double *min,
                                    double *base, int *f1, int *f2)
{
    int i, j, f1s, f2s;
    double mx = -1.0e15;
    double mn =  1.0e15;
    double bs = 0.0;
    double data;

    for(i=0; i<fid_2d->FID.size(); i++)
    {
        for(j=0; j<fid_2d->al(); j++)
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
    bs = bs / double(fid_2d->al() * fid_2d->al());

    *max = mx;
    *min = mn;
    *base = bs;
    *f1 = f1s;
    *f2 = f2s;

    return true;
}

