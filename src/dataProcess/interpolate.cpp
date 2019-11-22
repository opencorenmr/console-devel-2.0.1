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

    //qDebug() << delta;
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

    //qDebug() << "tate:" << fid_2d->FID.size();
    //qDebug() << "yoko:" << fid_2d->FID.at(0)->real->sig.size();
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

            double r = sqrt(x*x + y*y) - 0.5;
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
            //if(b==0){b+=1;}

            double p1r=0, p2r=0, p3r=0, p4r=0;
            double p1i=0, p2i=0, p3i=0, p4i=0;

            if(a==nRow){
                p1r = helpFID2D->FID[0]->real->sig[b];
                p1i = helpFID2D->FID[0]->imag->sig[b];
                p2r = helpFID2D->FID[a-1]->real->sig[b];
                p2i = helpFID2D->FID[a-1]->imag->sig[b];
                p3r = helpFID2D->FID[0]->real->sig[b-1];
                p3i = helpFID2D->FID[0]->imag->sig[b-1];
                p4r = helpFID2D->FID[a-1]->real->sig[b-1];
                p4i = helpFID2D->FID[a-1]->imag->sig[b-1];
            } else if (b==0) {
                p1r=0; p2r=0; p3r=0; p4r=0;
                p1i=0; p2i=0; p3i=0; p4i=0;

                for(int j=0; j<helpFID2D->FID.size(); j++){
                    p1r += helpFID2D->FID[j]->real->sig.at(0);
                    p1i += helpFID2D->FID[j]->imag->sig.at(0);
                }
                p1r /= helpFID2D->FID.size();
                p1i /= helpFID2D->FID.size();

                p2r = p1r;
                p3r = p1r;
                p4r = p1r;
                p2i = p1i;
                p3i = p1i;
                p4i = p1i;
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

            // linear interpolation
            double re1=0.0, re2=0.0, im1=0.0, im2=0.0;

            re1 = p3r + (p1r - p3r) * (r - b - 1);
            im1 = p3i + (p1i - p3i) * (r - b - 1);
            re2 = p4r + (p2r - p4r) * (r - b - 1);
            im2 = p4i + (p2i - p4i) * (r - b - 1);

            fid_2d->FID[k]->real->sig[i] = re2 + (re1 - re2)*(phi - (a-1)*delta)/delta;
            fid_2d->FID[k]->imag->sig[i] = im2 + (im1 - im2)*(phi - (a-1)*delta)/delta;

            //qDebug() << x << y << phi << r;

        }

        fid_2d->FID[k]->updateAbs();
        for(int i=0; i<fid_2d->FID.size(); i++){
            fid_2d->FID[i]->setEmpty(false);
        }

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
