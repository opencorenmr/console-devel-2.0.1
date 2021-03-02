#include "cartesianMap.h"
#include <QVector3D>
#include <float.h>


TCartesianMap3D::TCartesianMap3D()
{
  setProcessType(TProcessType::CartesianMap3D);
  origPolarAngles.clear();
  clearIndices();
}


TCartesianMap3D::~TCartesianMap3D()
{
  origPolarAngles.clear();
//  while(!cartesianMapTable.isEmpty()) cartesianMapTable.removeLast();
  while(!thetaTable.isEmpty()) thetaTable.removeLast();
  while(!phiTable.isEmpty()) phiTable.removeLast();
  while(!rTable.isEmpty()) rTable.removeLast();
  while(!polarAngleTable.isEmpty()) polarAngleTable.removeLast();

}

void TCartesianMap3D::clearIndices()
{
    FPointAIndex=-1;
    FPointBIndex=-1;
    FPointCIndex=-1;
    FNorthWestIndex=-1;
    FNorthEastIndex=-1;
    FSouthWestIndex=-1;
    FSouthEastIndex=-1;
    FParallelIndex=-1;
}

void TCartesianMap3D::setLength1D(int al)
{
    FLength1D=al;
    generateTables();
}

QString TCartesianMap3D::origPolarAnglesStr()
{
    QString qs;

    for(int k=0; k<origPolarAngles.size(); k++)
    {
        qs.append(QString::number(origPolarAngles[k].theta()));
        qs.append(' ');
        qs.append(QString::number(origPolarAngles[k].phi()));
        qs.append('\n');
    }

    return qs;
}

bool TCartesianMap3D::setOrigPolarAngles(QString qs)
{
    bool ok=true;
    double th,ph;
    QStringList sl1,sl2;
    origPolarAngles.clear();

    sl1=qs.split('\n');
    for(int k=0; k<sl1.size(); k++)
    {
        sl2=sl1.at(k).trimmed().split(QRegExp("\\s+"));
        if(sl2.size()!=2)
        {
            setErrorMessage("Invalid polar-angle data.");
            return false;
        }
        else // 2 elements
        {
            th=sl2.at(0).toDouble(&ok);
            ph=sl2.at(1).toDouble(&ok);
            if(!ok)
            {
                setErrorMessage("Invalid polar-angle data: " + sl1.at(k));
                return false;
            }
            else
            {
                origPolarAngles.append(TPolarAngle(th,ph));
            }
        }
    }

    return ok;
}

void TCartesianMap3D::generateTables()
{
   // while(!cartesianMapTable.isEmpty()) cartesianMapTable.removeLast();
    while(!thetaTable.isEmpty()) thetaTable.removeLast();
    while(!phiTable.isEmpty()) phiTable.removeLast();
    while(!rTable.isEmpty()) rTable.removeLast();
    while(!polarAngleTable.isEmpty()) polarAngleTable.removeLast();

    for(int x=0; x<length1D(); x++)
    {
      //  cartesianMapTable.append(QList<QList<QPoint> >());
        rTable.append(QList<QList<double> >());
        thetaTable.append(QList<QList<double> >());
        phiTable.append(QList<QList<double> >());
        polarAngleTable.append(QList<QList<TPolarAngle> >());
        for(int y=0; y<length1D(); y++)
        {
        //    cartesianMapTable[x].append(QList<QPoint>());
            rTable[x].append(QList<double>());
            thetaTable[x].append(QList<double>());
            phiTable[x].append(QList<double>());
            polarAngleTable[x].append(QList<TPolarAngle>());
            for(int z=0; z<length1D(); z++)
            {
          //      cartesianMapTable[x][y].append(QPoint(x+z*length1D(),y));
                int h=length1D() / 2;
                int z2=z-h;
                int x2=x-h;
                int y2=y-h;
                polarAngleTable[x][y].append(TPolarAngle(x2,y2,z2));
                rTable[x][y].append(sqrt(x2*x2+y2*y2+z2*z2));
                thetaTable[x][y].append(polarAngleTable[x][y][z].theta());
                phiTable[x][y].append(polarAngleTable[x][y][z].phi());

            } //
        } //
    } //

}



bool TCartesianMap3D::findPointsABC(TPolarAngle p)
{
  int aIndex,bIndex,cIndex;
  QVector3D op,oa,ob,oc,pa,pb,pc,v1;
  if(origPolarAngles.size()<3)
  {
    errorQ=true;
    setErrorMessage("Number of points is insufficient.");
    return false;
  }

  clearIndices();
  op=TPolarAngle::vector3D(p);
  aIndex=closestPolarAngleIndex(p);
  if(aIndex<0)
  {
      errorQ=true;
      setErrorMessage("Failed to find point A.");
      return false;
  }
  oa=TPolarAngle::vector3D(origPolarAngles.at(aIndex));

  pa=oa-op;

  double b,bMax;
  if(aIndex==0) bIndex=1; else bIndex=0;
  ob=TPolarAngle::vector3D(origPolarAngles.at(bIndex));
  pb=ob-op;
  v1=QVector3D::crossProduct(pa,pb);
  b=fabs(v1.length())/pb.lengthSquared();
  bMax=b;

  int bi=bIndex+1;
  for(int k=bi; k<origPolarAngles.size(); k++)
  {
    if(k!=aIndex)
    {
        ob=TPolarAngle::vector3D(origPolarAngles.at(k));
        pb=ob-op;
        v1=QVector3D::crossProduct(pa,pb);
        b=fabs(v1.length())/pb.lengthSquared();
        if(b>bMax)
        {
            bIndex=k;
            bMax=b;
        }
    } // if
  } //k

  double c,cMax;
  cIndex=0;
  while((cIndex==aIndex) || (cIndex==bIndex)) {cIndex++;}
  if(cIndex>origPolarAngles.size()-1)
  {
      errorQ=true;
      setErrorMessage("cIndex exceeded the number of points.");
      return false;
  }
  oc=TPolarAngle::vector3D(origPolarAngles.at(cIndex));
  pc=oc-op;
  v1=-1*(pa+pb);
  c=QVector3D::dotProduct(v1,pc)/pc.lengthSquared();
  cMax=c;

  int ci=cIndex+1;
  for(int k=ci; k<origPolarAngles.size(); k++)
  {
    if((k!=aIndex) && (k!=bIndex))
    {
        oc=TPolarAngle::vector3D(origPolarAngles.at(k));
        pc=oc-op;
        v1=-1*(pa+pb);
        c=QVector3D::dotProduct(v1,pc)/pc.lengthSquared();
        if(c>cMax)
        {
            cIndex=k;
            cMax=c;
        }
    } // if
  } // k

  FPointAIndex=aIndex;
  FPointBIndex=bIndex;
  FPointCIndex=cIndex;

//  double rpa=TPolarAngle::vector3D(origPolarAngles.at(aIndex)).length();
//  double rpb=TPolarAngle::vector3D(origPolarAngles.at(bIndex)).length();
//  double rpc=TPolarAngle::vector3D(origPolarAngles.at(cIndex)).length();
  double rpa=pa.length();
  double rpb=pb.length();
  double rpc=pc.length();

  double rps= rpa*rpb + rpb*rpc + rpc*rpa;
  if(rps>0)
  {
    FWeightA = rpb*rpc/rps;
    FWeightB = rpc*rpa/rps;
    FWeightC = rpa*rpb/rps;
  }
  else
  {
    FWeightA=0; FWeightB=0; FWeightC=0;
  }
  return true;
}

void TCartesianMap3D::checkParallel(TPolarAngle pa)
{
    FParallelIndex=-1;

    relativeOrientation.clear();
    for(int k=0; k<origPolarAngles.size(); k++)
    {
      int ori=TPolarAngle::orientation(pa,origPolarAngles[k]);
      relativeOrientation.append(ori);
      if(ori==Parallel) { FParallelIndex=k; return;}
    }
}



int TCartesianMap3D::closestPolarAngleIndex(TPolarAngle polarAngle)
{
    int index=-1;
    if(origPolarAngles.size()<1) return index;

    double dotP;
    double maxDotP;
    QVector3D v1,v2;
    v1=TPolarAngle::vector3D(polarAngle);
    v2=TPolarAngle::vector3D(origPolarAngles.at(0));

    dotP=QVector3D::dotProduct(v1,v2);
    maxDotP=dotP;
    index=0;

    for(int k=0; k<origPolarAngles.size(); k++)
    {
        v2=TPolarAngle::vector3D(origPolarAngles.at(k));
        dotP=QVector3D::dotProduct(v1,v2);
        if(dotP>maxDotP)
        {
            index=k;
            maxDotP=dotP;
        }
    }

    return index;
}



void TCartesianMap3D::run()
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
    //
    // We make a (nCol x nCol) x nCol matrix
    //
    int arrayLength=nCol*nCol;
    for(int k=0; k<arrayLength; k++)
    {
        helpFID2D->FID.append(new TFID(nCol));
        helpFID2D->FID.last()->setEmpty(false);
    }

    // We calculate the average value (rOrigin, iOrigin) at the origin, which will be used below.
    double rOrigin=0, iOrigin=0;
    for(int k=0; k<nRow; k++)
    {
        rOrigin += FID_2D->FID.at(k)->real->sig.at(0);
        iOrigin += FID_2D->FID.at(k)->imag->sig.at(0);
    }
    rOrigin /= nRow;
    iOrigin /= nRow;

    // We make tables
    FLength1D=nCol;
    while(!thetaTable.isEmpty()) thetaTable.removeLast();
    while(!phiTable.isEmpty()) phiTable.removeLast();
    while(!rTable.isEmpty()) rTable.removeLast();
    while(!polarAngleTable.isEmpty()) polarAngleTable.removeLast();

    for(int x=0; x<length1D(); x++)
    {
        emit tableCount(x);

        rTable.append(QList<QList<double> >());
        thetaTable.append(QList<QList<double> >());
        phiTable.append(QList<QList<double> >());
        polarAngleTable.append(QList<QList<TPolarAngle> >());
        for(int y=0; y<length1D(); y++)
        {
        //    cartesianMapTable[x].append(QList<QPoint>());
            rTable[x].append(QList<double>());
            thetaTable[x].append(QList<double>());
            phiTable[x].append(QList<double>());
            polarAngleTable[x].append(QList<TPolarAngle>());
            for(int z=0; z<length1D(); z++)
            {
          //      cartesianMapTable[x][y].append(QPoint(x+z*length1D(),y));
                int h=length1D() / 2;
                int z2=z-h;
                int x2=x-h;
                int y2=y-h;
                polarAngleTable[x][y].append(TPolarAngle(x2,y2,z2));
                rTable[x][y].append(sqrt(x2*x2+y2*y2+z2*z2));
                thetaTable[x][y].append(polarAngleTable[x][y][z].theta());
                phiTable[x][y].append(polarAngleTable[x][y][z].phi());

            } //
        } //
    } //

    emit genTableComplete();

    //
    // Interpolation!
    //
    for(int x=0; x<nCol; x++)
    {
      emit calcCount(x);
      if(stopped) return;

    for(int y=0; y<nCol; y++)
    {
    for(int z=0; z<nCol; z++)
    {
      if((int) ceil(rTable.at(x).at(y).at(z)) > nCol) // Outside the sphere -> zero
      {
          helpFID2D->FID[x+z*nCol]->real->sig[y]=0.0;
          helpFID2D->FID[x+z*nCol]->imag->sig[y]=0.0;
          qDebug() << "TCartesianMap3D::process: Data zero was set at (" << x <<"," << y << "," << z << ").";
      }
      else if(rTable.at(x).at(y).at(z)==0) // Origin
      {
          helpFID2D->FID[x+z*nCol]->real->sig[y]=rOrigin;
          helpFID2D->FID[x+z*nCol]->imag->sig[y]=iOrigin;
          qDebug() << "TCartesianMap3D::process: Origin found at (" << x <<"," << y << "," << z << ").";
      }
      else
      {
          double d1,d2,e1,e2;
          double r=rTable.at(x).at(y).at(z);
          checkParallel(polarAngleTable.at(x).at(y).at(z));
          if(parallelIndex()>-1) // Parallel data found -> no need to "laterally" interpolate
          {
            qDebug() << "Parallel axis found at (" << x <<"," << y << "," << z << ").";
            if(fabs(round(r)-r)<DBL_EPSILON)
            {
              helpFID2D->FID[x+z*nCol]->real->sig[y] = FID_2D->FID.at(parallelIndex())->real->sig.at((int) r);
              helpFID2D->FID[x+z*nCol]->imag->sig[y] = FID_2D->FID.at(parallelIndex())->imag->sig.at((int) r);
            }
            else
            {
              d1=FID_2D->FID.at(parallelIndex())->real->sig.at((int) floor(r));
              d2=FID_2D->FID.at(parallelIndex())->real->sig.at((int) ceil(r));
              e1=FID_2D->FID.at(parallelIndex())->imag->sig.at((int) floor(r));
              e2=FID_2D->FID.at(parallelIndex())->imag->sig.at((int) ceil(r));
              helpFID2D->FID[x+z*nCol]->real->sig[y] = (ceil(r)-r)*d1 + (r-floor(r))*d2;
              helpFID2D->FID[x+z*nCol]->imag->sig[y] = (ceil(r)-r)*e1 + (r-floor(r))*e2;
            }
          }
          else // lateral interpolation
          {
               // We try to find 3 directions from origPolarAngles
            if(!findPointsABC(polarAngleTable.at(x).at(y).at(z)))
            {
                errorQ=true;
                return;
            }

            // qDebug() << x << y << z << ":" << FPointAIndex << FPointBIndex << FPointCIndex;

            // qDebug() << FWeightA << FWeightB << FWeightC;

            double da,db,dc,ea,eb,ec;
            if(fabs(round(r)-r)<DBL_EPSILON)
            {
                da=FID_2D->FID.at(FPointAIndex)->real->sig.at((int) r);
                db=FID_2D->FID.at(FPointBIndex)->real->sig.at((int) r);
                dc=FID_2D->FID.at(FPointCIndex)->real->sig.at((int) r);
                helpFID2D->FID[x+z*nCol]->real->sig[y] = FWeightA*da + FWeightB*db + FWeightC*dc;
                ea=FID_2D->FID.at(FPointAIndex)->imag->sig.at((int) r);
                eb=FID_2D->FID.at(FPointBIndex)->imag->sig.at((int) r);
                ec=FID_2D->FID.at(FPointCIndex)->imag->sig.at((int) r);
                helpFID2D->FID[x+z*nCol]->imag->sig[y] = FWeightA*ea + FWeightB*eb + FWeightC*ec;
            }
            else // We need to "radially" interpolate!
            {
              da=FID_2D->FID.at(FPointAIndex)->real->sig.at((int) floor(r));
              db=FID_2D->FID.at(FPointBIndex)->real->sig.at((int) floor(r));
              dc=FID_2D->FID.at(FPointCIndex)->real->sig.at((int) floor(r));
              d1=FWeightA*da + FWeightB*db + FWeightC*dc;
              da=FID_2D->FID.at(FPointAIndex)->real->sig.at((int) ceil(r));
              db=FID_2D->FID.at(FPointBIndex)->real->sig.at((int) ceil(r));
              dc=FID_2D->FID.at(FPointCIndex)->real->sig.at((int) ceil(r));
              d2=FWeightA*da + FWeightB*db + FWeightC*dc;

              ea=FID_2D->FID.at(FPointAIndex)->imag->sig.at((int) floor(r));
              eb=FID_2D->FID.at(FPointBIndex)->imag->sig.at((int) floor(r));
              ec=FID_2D->FID.at(FPointCIndex)->imag->sig.at((int) floor(r));
              e1=FWeightA*ea + FWeightB*eb + FWeightC*ec;
              ea=FID_2D->FID.at(FPointAIndex)->imag->sig.at((int) ceil(r));
              eb=FID_2D->FID.at(FPointBIndex)->imag->sig.at((int) ceil(r));
              ec=FID_2D->FID.at(FPointCIndex)->imag->sig.at((int) ceil(r));
              e2=FWeightA*ea + FWeightB*eb + FWeightC*ec;
              helpFID2D->FID[x+z*nCol]->real->sig[y] = (ceil(r)-r)*d1 + (r-floor(r))*d2;
              helpFID2D->FID[x+z*nCol]->imag->sig[y] = (ceil(r)-r)*e1 + (r-floor(r))*e2;
            } // else
          }



      }

    }
    }
    }


    emit calcComplete();


    //
    // We clear the content of FID_2D,
    // make a (nCol x nCol) x nCol matrix,
    // and copy data from helpFID2D
    //
    FID_2D->FID.clear();
    for(int k=0; k<arrayLength; k++)
    {
        emit copyCount(k);
        FID_2D->FID.append(new TFID(nCol));
        FID_2D->FID.last()->setEmpty(false);
        for(int j=0; j<nCol; j++)
        {
           FID_2D->FID[k]->real->sig[j]=helpFID2D->FID.at(k)->real->sig.at(j);
           FID_2D->FID[k]->imag->sig[j]=helpFID2D->FID.at(k)->imag->sig.at(j);
        } // j
        // We update the absolute values
        FID_2D->FID[k]->updateAbs();
    }

    FID_2D->setCurrentFID(0);
    emit copyComplete();

    delete helpFID2D;



    mutex.lock();
    condition.wait(&mutex); // We let the thread sleep.
    mutex.unlock();
  } // foever
}


bool TCartesianMap3D::process(TFID_2D *fid_2d)
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


QStringList TCartesianMap3D::processInformation()
{
    return QStringList() << "process=cartesianMap3D";
}

QString TCartesianMap3D::command()
{
    return "cartesianMap3D";
}


double TPolarAngle::regionalTheta(double t)
{
    double tt=t;

    if(tt<-PI)
    {
        while(tt<-PI) tt += 2*PI;
    }
    else if(tt>PI)
    {
        while(tt>PI) tt -= 2*PI;
    }

    return tt;

}

double TPolarAngle::regionalPhi(double p)
{
    double pp=p;
    if(pp<-PI)
    {
        while(pp<-PI) pp+=2*PI;
    }
    else if(pp>PI)
    {
        while(pp>PI) pp-=2*PI;
    }
    return pp;
}

void TPolarAngle::setTheta(double t)
{
    FTheta=regionalTheta(t);
}

void TPolarAngle::setPhi(double p)
{
    FPhi=regionalPhi(p);
}

int TPolarAngle::orientation(TPolarAngle pa1, TPolarAngle pa2)
{
    // orientation of pa2 looking from pa1
    enum TNorthSouth {North, South};
    TNorthSouth northSouth=North;
    enum TEastWest {East, West};
    TEastWest eastWest=East;


    if(fabs(regionalTheta(pa1.theta())-regionalTheta(pa2.theta())) < DBL_EPSILON
            && fabs(regionalPhi(pa1.phi())-regionalPhi(pa2.phi())) < DBL_EPSILON)
    {
        return Parallel;
    }

    // North or South
    if(regionalTheta(pa1.theta())>=regionalTheta(pa2.theta())) northSouth=North; else northSouth=South;

    // East or West
    double phiPrime=atan2(-sin(pa1.phi()),-cos(pa1.phi())); //

    if(pa1.phi()>0)
    {
        if((phiPrime<pa2.phi()) && (pa2.phi()<pa1.phi())) eastWest=West;
        else eastWest=East;
    }
    else // pa1.phi()<0
    {
        if((pa1.phi()<pa2.phi()) && (pa2.phi()<phiPrime)) eastWest=East;
        else eastWest=West;
    }


    if(northSouth==North) // North
    {
      if(eastWest==East) return NorthEast; else return NorthWest;
    }
    else // South
    {
      if(eastWest==East) return SouthEast; else return SouthWest;
    }

}

