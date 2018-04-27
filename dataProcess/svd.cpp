#include "svd.h"
#include <math.h>

#include <QDebug>
#include <QVector>


#ifdef __APPLE__
#include <Accelerate/Accelerate.h>
typedef __CLPK_doublereal doublereal;
typedef __CLPK_integer integer;
typedef __CLPK_doublecomplex cdouble;
#else
#include "cblas.h"
#include "lapacke.h"
typedef double doublereal;
typedef int integer;
typedef lapack_complex_double cdouble;
#endif

KSVD::KSVD()
{
    setAxisDomain(fidDomain::ToggleDomain);
}

KSVD::~KSVD()
{
    ;
}

QStringList KSVD::processInformation()
{
    if(innerProduct()) {return QStringList() << "innerproduct";}
    else {return QStringList() << "process=covariance";}
}

QString KSVD::command()
{
    if(innerProduct()) {return "innerproduct";}
    else {return "covariance";}
}

bool KSVD::process(TFID_2D *fid_2d)
{
    bool r;
    r= svdProcess(fid_2d);
    return r;
}

bool KSVD::svdProcess(TFID_2D *fid_2d)
{
    int fidsize = fid_2d->FID.size();
    int fidal = fid_2d->al();
    int p;// q, r;
    char jobu = 'A', jobvt = 'A';
    double alpha = 1.0, beta = 0.0;
    integer m=fidsize, n=fidal, lda=fidsize, ldu=fidsize;
    integer ldvt=fidal, lwork=3*fidsize+fidal, info=0;
    doublereal *s, *rwork, avr, avi;
    cdouble *a;
    cdouble *u, *result, *result1;
    cdouble *vt, *tempvt, *work;

    s = new doublereal[fidal];
    rwork = new doublereal[5*fidsize];
    a = new cdouble[fidsize*fidal];
    u = new cdouble[fidsize*fidsize];
    vt = new cdouble[fidal*fidal];
    work = new cdouble[3*fidsize+fidal];

    //------------make matrix a (FID)----------
    for(int k=0; k<fidsize; k++){
        for(int i=0; i<fidal; i++){
#ifdef __APPLE__
            a[i*fidsize+k].r = fid_2d->FID.at(k)->real->sig.at(i);
            if(imagKeep()){a[i*fidsize+k].i = fid_2d->FID.at(k)->imag->sig.at(i);}
            else          {a[i*fidsize+k].i = 0.0;}
#else
            __real__(a[i*fidsize+k]) = fid_2d->FID.at(k)->real->sig.at(i);
            if(imagKeep()){__imag__(a[i*fidsize+k]) = fid_2d->FID.at(k)->imag->sig.at(i);}
            else          {__imag__(a[i*fidsize+k]) = 0.0;}
#endif
        }
    }

    //------------calculate new A = a - <a> (if covariance)-----------
    if(!innerProduct())
    {
        //qDebug() << "average calculating.....";
        for(int i=0; i<fidal; i++)
        {
            avr = 0.0; avi = 0.0;
            for(int k=0; k<fidsize; k++)
            {
#ifdef __APPLE__
                avr += a[i*fidsize+k].r;
                avi += a[i*fidsize+k].i;
#else
                avr += __real__ a[i*fidsize+k];
                avi += __imag__ a[i*fidsize+k];
#endif
            }
            avr = avr / fidsize;
            avi = avi / fidsize;
            for(int k=0; k<fidsize; k++)
            {
#ifdef __APPLE__
                a[i*fidsize+k].r -= avr;
                a[i*fidsize+k].i -= avi;
#else
                __real__ a[i*fidsize+k] -= avr;
                __imag__ a[i*fidsize+k] -= avi;
#endif
            }
        }
    }

    //-----singular value decomposition of general complex matrix------
    //qDebug() << "SVD calculating.....";
    zgesvd_(&jobu, &jobvt, &m, &n, a, &lda, s, u, &ldu, vt, &ldvt, work, &lwork, rwork, &info);

    delete a;
    delete u;
    delete work;
    delete rwork;

    //s is until fidsize
    for(int i=fidsize;i<fidal;i++){s[i] = 0.0;}

    //copy vt
    tempvt = new cdouble[fidal*fidal];
    for(int i=0; i<fidal*fidal; i++){
#ifdef __APPLE__
        tempvt[i].r = vt[i].r;
        tempvt[i].i = vt[i].i;
#else
        __real__ tempvt[i] = __real__ vt[i];
        __imag__ tempvt[i] = __imag__ vt[i];
#endif
    }

    //----------------------calculate Spectrum----------------------
    result = new cdouble[fidal*fidal];
    result1 = new cdouble[fidal*fidal];

    //----------straight forward calculation S(ss)*Vt (right side)----------
    //qDebug() << "right side calculating.....";
    for(int j=0; j<fidal; j++){
            for(int i=0; i<fidal; i++){
                    p = j*n+i;
#ifdef __APPLE__
                    result1[p].r = s[i] * tempvt[p].r;
                    result1[p].i = s[i] * tempvt[p].i;
#else
                    __real__ result1[p] = s[i] * __real__ tempvt[p];
                    __imag__ result1[p] = s[i] * __imag__ tempvt[p];
#endif
            }

        }
    delete s;
    //cblas_zgemm(CblasColMajor, CblasNoTrans, CblasNoTrans, fidal,
    //            fidal, fidal, &alpha, ss, fidal, tempvt, fidal, &beta, result1, fidal);
    //--------------------------------------------------

    for(int i=0; i<fidal*fidal; i++)
    {
#ifdef __APPLE__
        result[i].r = 0.0;
        result[i].i = 0.0;
#else
        __real__ result[i] = 0.0;
        __imag__ result[i] = 0.0;
#endif
    }

    //----------calculate V(Vtt)*{S(ss)*Vt}-------------
    //qDebug() << "left side calculating.....";
    cblas_zgemm(CblasColMajor, CblasConjTrans, CblasNoTrans, fidal,
                fidal, fidal, &alpha, vt, fidal, result1, fidal, &beta, result, fidal);

    delete tempvt;
    delete vt;
    delete result1;

    //---------------------------------------------------
    //qDebug() << "plotter updating.....";
    for(int i=0;i<fidal-fidsize;i++)
    {
        fid_2d->FID.append(new TFID(fidal));
        fid_2d->FID.last()->setEmpty(false);
        fid_2d->FID.last()->domain=fid_2d->FID.first()->domain;
        fid_2d->FID.last()->setXAxisLabel(fid_2d->FID.first()->xAxisLabel());
        fid_2d->FID.last()->setXAxisUnitSymbol(fid_2d->FID.first()->xAxisUnitSymbol());
        fid_2d->FID.last()->setDW(fid_2d->FID.first()->dw());
        fid_2d->FID.last()->setDx(fid_2d->FID.first()->dx());
        fid_2d->FID.last()->plotMetricPrefix.setPrefix(fid_2d->FID.first()->plotMetricPrefix.prefix());
        fid_2d->FID.last()->setXInitialValue(fid_2d->FID.first()->xInitialValue());
    }

    for(int i=0; i<fidal; i++)
    {
        for(int j=0; j<fidal; j++)
        {
#ifdef __APPLE__
            fid_2d->FID[i]->real->sig[j] = result[j*fidal+i].r / sqrt(double(fidsize));
            fid_2d->FID[i]->imag->sig[j] = result[j*fidal+i].i / sqrt(double(fidsize));
#else
            fid_2d->FID[i]->real->sig[j] = __real__ result[j*fidal+i] / sqrt(double(fidsize));
            fid_2d->FID[i]->imag->sig[j] = __imag__ result[j*fidal+i] / sqrt(double(fidsize));
#endif
        }
        fid_2d->FID[i]->updateAbs();
    }

    delete result;
    return true;
}
