#include "cartesianMapWeight.h"

SCartesianMapWeight3D::SCartesianMapWeight3D()
{
    setProcessType(TProcessType::CartesianMap3D);
    origPolarAngles.clear();
    clearIndices();
}

SCartesianMapWeight3D::~SCartesianMapWeight3D()
{
    origPolarAngles.clear();
    while(!rTable.isEmpty()) rTable.removeLast();
    while(!polarAngleTable.isEmpty()) polarAngleTable.removeLast();
}

void SCartesianMapWeight3D::run()
{
    forever
    {
        if(stopped) return;

        emit calcCount(0);

        int nRow=FID_2D->FID.size();  //qDebug() << QString(Q_FUNC_INFO) << "nRow: " <<nRow;
        int nCol=FID_2D->FID.at(0)->al();  //qDebug() << QString(Q_FUNC_INFO) << "nCol: " <<nCol;

        if(nRow!=origPolarAngles.size())
        {
            errorQ=true;
            setErrorMessage("Size of data does not match that of the polar angle list.");
            return;
        }
        // We introduce a new TFID_2D class, named "helpFID2D", and copy data
        TFID_2D *helpFID2D = new TFID_2D;
        helpFID2D->FID.clear();

        for(int k=0; k<nRow; k++)
        {
            helpFID2D->FID.append(new TFID(nCol));
            helpFID2D->FID.last()->setEmpty(false);
            for(int l=0;l<nCol;l++){
                helpFID2D->FID[k]->real->sig[l] = 1.0;
                helpFID2D->FID[k]->imag->sig[l] = 0.0;
            }
        }

        double (SCartesianMapWeight3D::*kernel)(double);
        switch (interpolateMode){
        case gridSinc:
            kernel = &SCartesianMapWeight3D::sinc;
            break;
        case gridGaus:
            kernel = &SCartesianMapWeight3D::gaussian;
            break;
        default:
            kernel = &SCartesianMapWeight3D::gaussian;
        }

        //
        // Calculating!
        //

        QVector3D vec,vecB;
        double x,y,z,v,density,xB,yB,zB,vB;
        for(int i=0;i<iteration;i++){
            if(stopped) return;
            emit calcCount(i);
            for(int row=0;row<nRow;row++){
                vec = TPolarAngle::vector3D(origPolarAngles[row]);
                for(int col=0;col<nCol;col++){
                    x = col * vec.x();
                    y = col * vec.y();
                    z = col * vec.z();
                    v = helpFID2D->FID[row]->real->sig[col];
                    density = 0.0;
                    for(int rowB=0;rowB<nRow;rowB++){
                        vecB = TPolarAngle::vector3D(origPolarAngles[rowB]);
                        for(int colB=0;colB<nCol;colB++){
                            xB = colB * vecB.x();
                            yB = colB * vecB.y();
                            zB = colB * vecB.z();
                            vB = helpFID2D->FID[rowB]->real->sig[colB];
                            density += vB*(this->*kernel)((x-xB)/ratioofDistanceBetweenPoints)*(this->*kernel)((y-yB)/ratioofDistanceBetweenPoints)*(this->*kernel)((z-zB)/ratioofDistanceBetweenPoints);
                        }
                    }
                    if(abs(density)<__DBL_EPSILON__){
                        FID_2D->FID[row]->real->sig[col] = __DBL_MAX__;
                        FID_2D->FID[row]->imag->sig[col] = 0.0;
                        qDebug() << "zero density point is found." << row << col << i;
                    }else{
                        FID_2D->FID[row]->real->sig[col] = v / density;
                        FID_2D->FID[row]->imag->sig[col] = 0.0;
                    }
                }
            }
            for(int row=0;row<nRow;row++){
                for(int col=0;col<nCol;col++){
                    helpFID2D->FID[row]->real->sig[col] = FID_2D->FID[row]->real->sig[col];
                    helpFID2D->FID[row]->imag->sig[col] = FID_2D->FID[row]->imag->sig[col];
                }
            }
        }

        emit calcComplete();

        for(int row=0;row<nRow;row++){
            FID_2D->FID[row]->updateAbs();
            FID_2D->FID[row]->setEmpty(false);
        }
        FID_2D->setCurrentFID(0);

        delete helpFID2D;

        mutex.lock();
        condition.wait(&mutex); // We let the thread sleep.
        mutex.unlock();
    }
}

bool SCartesianMapWeight3D::process(TFID_2D *fid_2d)
{
    FID_2D=fid_2d;
    errorQ=false;

    QMutexLocker locker(&mutex);
    stopped=false;
    wasCanceled=false;
    if(!isRunning())
    {
      start(HighPriority);
    }
    else
    {
      condition.wakeOne();
    }

    return !errorQ;
}

QStringList SCartesianMapWeight3D::processInformation()
{
    return QStringList() << "process=cartesianMapWeight3D";
}

QString SCartesianMapWeight3D::command()
{
    return "cartesianMapWeight3D";
}

void SCartesianMapWeight3D::getIteration(int i)
{
    iteration = i;
}
