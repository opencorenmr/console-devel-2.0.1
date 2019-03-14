#include "transpose.h"

TTranspose::TTranspose()
{

}

bool TTranspose::process(TFID_2D *fid_2d)
{
    int nRow=fid_2d->FID.size();  //qDebug() << QString(Q_FUNC_INFO) << "nRow: " <<nRow;
    int nCol=fid_2d->FID.at(0)->al();  //qDebug() << QString(Q_FUNC_INFO) << "nCol: " <<nCol;

    //
    //  We introduce a new TFID_2D class
    //  and create nRow x nCol ---> nCol x nRow transposed data
    //
    TFID_2D *helpFID2D = new TFID_2D;
    helpFID2D->FID.clear();
    for(int k=0; k<nCol; k++)
    {
        helpFID2D->FID.append(new TFID(nRow));
        for(int i=0; i<nRow; i++)
        {
            helpFID2D->FID[k]->real->sig[i] = fid_2d->FID.at(i)->real->sig.at(k);
            helpFID2D->FID[k]->imag->sig[i] = fid_2d->FID.at(i)->imag->sig.at(k);
        }
        helpFID2D->FID.last()->updateAbs();
    }


    //
    // We clear the content of fid_2d, and copy the transposed data
    //
    fid_2d->FID.clear();
    for(int k=0; k<nCol; k++)
    {
        fid_2d->FID.append(new TFID(nRow));
        for(int i=0; i<nRow; i++)
        {
          fid_2d->FID[k]->real->sig[i]=helpFID2D->FID.at(k)->real->sig.at(i);
          fid_2d->FID[k]->imag->sig[i]=helpFID2D->FID.at(k)->imag->sig.at(i);
        }
        fid_2d->FID[k]->updateAbs();
        fid_2d->FID.last()->setEmpty(false);

//        qDebug()<< QString(Q_FUNC_INFO)<<fid_2d->FID.last()->isEmpty();
    }

    // We set the current FID to be the first one.
    fid_2d->setCurrentFID(0);

    delete helpFID2D;
    return true;
}

QStringList TTranspose::processInformation()
{
    return QStringList() << "process=transpose";
}

QString TTranspose::command()
{
    return "transpose";
}
