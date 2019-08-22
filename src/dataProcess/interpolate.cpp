#include "interpolate.h"
#include "math.h"
#include "float.h"

KInterpolate::KInterpolate()
{

}

bool KInterpolate::processInterpolate(TFID_2D *fid_2d, double delta)
{
    int nRow=fid_2d->FID.size();  //qDebug() << QString(Q_FUNC_INFO) << "nRow: " <<nRow;
    int nCol=fid_2d->FID.at(0)->al();  //qDebug() << QString(Q_FUNC_INFO) << "nCol: " <<nCol;

    //
    //  We introduce a new TFID_2D class,
    //  and copy the data
    //
    TFID_2D *helpFID2D = new TFID_2D;
    helpFID2D->FID.clear();
    for(int k=0; k<nRow; k++)
    {
        helpFID2D->FID.append(new TFID(nCol));
        for(int i=0; i<nCol; i++)
        {
            helpFID2D->FID[k]->real->sig[i] = fid_2d->FID.at(k)->real->sig.at(i);
            helpFID2D->FID[k]->imag->sig[i] = fid_2d->FID.at(k)->imag->sig.at(i);
        }
        helpFID2D->FID.last()->updateAbs();
    }

    //
    // We clear the content of fid_2d, and make n x n matrix
    //
    fid_2d->FID.clear();
    while(fid_2d->FID.size() < nCol){
        fid_2d->FID.append(new TFID(nCol));
    }

    qDebug() << "tate:" << fid_2d->FID.size();
    qDebug() << "yoko:" << fid_2d->FID.at(0)->real->sig.size();
/*
    for(int k=0; k<nRow; k++)
    {
        for(int i=0; i<nCol; i++)
        {
            fid_2d->FID[k]->real->sig[i] = 10.0;
            fid_2d->FID[k]->imag->sig[i] = 5.0;
        }
        fid_2d->FID.last()->updateAbs();
        fid_2d->FID.last()->setEmpty(false);
    }
*/

    // interpolation
    // find near 4 points

    int n = nCol;

    for(int k=0; k<n; k++)
    {
        for(int i=0; i<n; i++)
        {
            double x = double(i-n/2);
            double y = double(n/2-1-k);

            double r = sqrt(x*x + y*y);
            double phi=0;
            if((fabs(x) <= DBL_EPSILON * fmax(1,x))){
                if(fabs(y)<= DBL_EPSILON * fmax(1,y)){
                    phi = 0.0;
                } else if(y>0){
                    phi = 90.0;
                } else if(y<0){
                    phi = 270.0;
                }
            } else if (fabs(y)<= DBL_EPSILON * fmax(1,y)){
                if(fabs(x)<= DBL_EPSILON * fmax(1,x)){
                    phi = 0.0;
                } else if(x>0){
                    phi = 0.0;
                } else if(x<0){
                    phi = 180.0;
                }
            } else if(x>0 && y>0){
                phi = atan(y/x) / M_PI * 180.0;
            } else if(x<0 && y>0){
                phi = atan(y/x) / M_PI * 180.0 + 180.0;
            } else if(x<0 && y<0){
                phi = atan(y/x) / M_PI * 180.0 + 180.0;
            } else if(x>0 && y<0){
                phi = atan(y/x) / M_PI * 180.0 + 360.0;
            }

            int a, b;


            a = int(ceil(phi/delta));
            b = int(ceil(r));

            if(a==0){a+=1;}
            if(b==0){b+=1;}

            double p1r, p2r, p3r, p4r;
            double p1i, p2i, p3i, p4i;

            if(a==nRow){
                p1r = helpFID2D->FID[0]->real->sig[b];
                p1i = helpFID2D->FID[0]->imag->sig[b];
                p2r = helpFID2D->FID[a-1]->real->sig[b];
                p2i = helpFID2D->FID[a-1]->imag->sig[b];
                p3r = helpFID2D->FID[0]->real->sig[b-1];
                p3i = helpFID2D->FID[0]->imag->sig[b-1];
                p4r = helpFID2D->FID[a-1]->real->sig[b-1];
                p4i = helpFID2D->FID[a-1]->imag->sig[b-1];
            } else {
                p1r = helpFID2D->FID[a]->real->sig[b];
                p1i = helpFID2D->FID[a]->imag->sig[b];
                p2r = helpFID2D->FID[a-1]->real->sig[b];
                p2i = helpFID2D->FID[a-1]->imag->sig[b];
                p3r = helpFID2D->FID[a]->real->sig[b-1];
                p3i = helpFID2D->FID[a]->imag->sig[b-1];
                p4r = helpFID2D->FID[a-1]->real->sig[b-1];
                p4i = helpFID2D->FID[a-1]->imag->sig[b-1];
            }

            fid_2d->FID[k]->real->sig[i] = (p1r+p2r+p3r+p4r)/4;
            fid_2d->FID[k]->imag->sig[i] = (p1i+p2i+p3i+p4i)/4;

        }

        fid_2d->FID[k]->updateAbs();
        fid_2d->FID.last()->setEmpty(false);

//        qDebug()<< QString(Q_FUNC_INFO)<<fid_2d->FID.last()->isEmpty();
    }


    // We set the current FID to be the first one.
    //fid_2d->setCurrentFID(0);

    delete helpFID2D;
    return true;
}

QStringList KInterpolate::processInformation()
{
    return QStringList() << "process=interpolate";
}

QString KInterpolate::command()
{
    return "interpolate";
}
