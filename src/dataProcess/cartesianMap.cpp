#include "cartesianMap.h"
#include <QVector3D>
#include <float.h>


TCartesianMap3D::TCartesianMap3D()
{
  origPolarAngles.clear();
  clearIndices();
}


TCartesianMap3D::~TCartesianMap3D()
{
  origPolarAngles.clear();
  while(!cartesianMapTable.isEmpty()) cartesianMapTable.removeLast();
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

void TCartesianMap3D::generateTables()
{
    while(!cartesianMapTable.isEmpty()) cartesianMapTable.removeLast();
    while(!thetaTable.isEmpty()) thetaTable.removeLast();
    while(!phiTable.isEmpty()) phiTable.removeLast();
    while(!rTable.isEmpty()) rTable.removeLast();
    while(!polarAngleTable.isEmpty()) polarAngleTable.removeLast();

    for(int x=0; x<length1D(); x++)
    {
        cartesianMapTable.append(QList<QList<QPoint> >());
        rTable.append(QList<QList<double> >());
        thetaTable.append(QList<QList<double> >());
        phiTable.append(QList<QList<double> >());
        polarAngleTable.append(QList<QList<TPolarAngle> >());
        for(int y=0; y<length1D(); y++)
        {
            cartesianMapTable[x].append(QList<QPoint>());
            rTable[x].append(QList<double>());
            thetaTable[x].append(QList<double>());
            phiTable[x].append(QList<double>());
            polarAngleTable[x].append(QList<TPolarAngle>());
            for(int z=0; z<length1D(); z++)
            {
                cartesianMapTable[x][y].append(QPoint(x+z,y));
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

  double rpa=TPolarAngle::vector3D(origPolarAngles.at(aIndex)).length();
  double rpb=TPolarAngle::vector3D(origPolarAngles.at(bIndex)).length();
  double rpc=TPolarAngle::vector3D(origPolarAngles.at(cIndex)).length();

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


bool TCartesianMap3D::process(TFID_2D *fid_2d)
{
    int nRow=fid_2d->FID.size();  //qDebug() << QString(Q_FUNC_INFO) << "nRow: " <<nRow;
    int nCol=fid_2d->FID.at(0)->al();  //qDebug() << QString(Q_FUNC_INFO) << "nCol: " <<nCol;

    // We introduce a new TFID_2D class, named "helpFID2D", and copy data
    // In addition, we calculate the average value (rOrigin, iOrigin) at the origin, which will be used below.
    TFID_2D *helpFID2D = new TFID_2D;
    helpFID2D->FID.clear();
    double rOrigin=0, iOrigin=0;
    for(int k=0; k<nRow; k++)
    {
        helpFID2D->FID.append(new TFID(nCol));
        for(int i=0; i<nRow; i++)
        {
            helpFID2D->FID[k]->real->sig[i] = fid_2d->FID.at(k)->real->sig.at(i);
            helpFID2D->FID[k]->imag->sig[i] = fid_2d->FID.at(k)->imag->sig.at(i);
        }
        helpFID2D->FID.last()->updateAbs();
        rOrigin += fid_2d->FID.at(k)->real->sig.at(0);
        iOrigin += fid_2d->FID.at(k)->imag->sig.at(0);
    }
    rOrigin /= nRow;
    iOrigin /= nRow;


    //
    // We clear the content of fid_2d, and then make a (nCol x nCol) x nCol square matrix
    //
    fid_2d->FID.clear();
    int arrayLength=nCol*nCol;
    for(int k=0; k<arrayLength; k++)
    {
        fid_2d->FID.append(new TFID(nCol));
    }

    // We make tables
    setLength1D(nCol);

    //
    // Interpolation!
    //
    for(int x=0; x<nCol; x++)
    {
    for(int y=0; y<nCol; y++)
    {
    for(int z=0; z<nCol; z++)
    {
      if((int) ceil(rTable.at(x).at(y).at(z)) > fid_2d->al()-1) // Outside the sphere -> zero
      {
          fid_2d->FID[x+z]->real->sig[y]=0.0;
          fid_2d->FID[x+z]->imag->sig[y]=0.0;
          qDebug() << "TCartesianMap3D::process: Data zero was set at (" << x <<"," << y << "," << z << ").";
      }
      else if(rTable.at(x).at(y).at(z)==0) // Origin
      {
          fid_2d->FID[x+z]->real->sig[y]=rOrigin;
          fid_2d->FID[x+z]->imag->sig[y]=iOrigin;
          qDebug() << "TCartesianMap3D::process: Origin found at (" << x <<"," << y << "," << z << ").";
      }
      else
      {
          double d1,d2,e1,e2;
          double r=rTable.at(x).at(y).at(z);
          checkParallel(polarAngleTable.at(x).at(y).at(z));
          if(parallelIndex()>-1) // Parallel data found -> no need to "laterally" interpolate
          {
            d1=helpFID2D->FID.at(parallelIndex())->real->sig.at((int) floor(r));
            d2=helpFID2D->FID.at(parallelIndex())->real->sig.at((int) ceil(r));
            e1=helpFID2D->FID.at(parallelIndex())->imag->sig.at((int) floor(r));
            e2=helpFID2D->FID.at(parallelIndex())->imag->sig.at((int) ceil(r));


          }
          else // Both lateral and radial interpolation
          {
               // We try to find 3 directions from origPolarAngles
            if(!findPointsABC(polarAngleTable.at(x).at(y).at(z)))
            {
                return false;
            }
            double da,db,dc,ea,eb,ec;
            da=helpFID2D->FID.at(FPointAIndex)->real->sig.at((int) floor(r));
            db=helpFID2D->FID.at(FPointBIndex)->real->sig.at((int) floor(r));
            dc=helpFID2D->FID.at(FPointCIndex)->real->sig.at((int) floor(r));
            d1=FWeightA*da + FWeightB*db + FWeightC*dc;
            da=helpFID2D->FID.at(FPointAIndex)->real->sig.at((int) ceil(r));
            db=helpFID2D->FID.at(FPointBIndex)->real->sig.at((int) ceil(r));
            dc=helpFID2D->FID.at(FPointCIndex)->real->sig.at((int) ceil(r));
            d2=FWeightA*da + FWeightB*db + FWeightC*dc;

            ea=helpFID2D->FID.at(FPointAIndex)->imag->sig.at((int) floor(r));
            eb=helpFID2D->FID.at(FPointBIndex)->imag->sig.at((int) floor(r));
            ec=helpFID2D->FID.at(FPointCIndex)->imag->sig.at((int) floor(r));
            e1=FWeightA*ea + FWeightB*eb + FWeightC*ec;
            ea=helpFID2D->FID.at(FPointAIndex)->imag->sig.at((int) ceil(r));
            eb=helpFID2D->FID.at(FPointBIndex)->imag->sig.at((int) ceil(r));
            ec=helpFID2D->FID.at(FPointCIndex)->imag->sig.at((int) ceil(r));
            e2=FWeightA*ea + FWeightB*eb + FWeightC*ec;

          }
          // We need to "radially" interpolate!
          fid_2d->FID[x+z]->real->sig[y] = (ceil(r)-r)*d1 + (r-floor(r))*d2;
          fid_2d->FID[x+z]->imag->sig[y] = (ceil(r)-r)*e1 + (r-floor(r))*e2;


      }





    }
    }
    }

    // We update the absolute values
    for(int k=0; k<fid_2d->FID.size(); k++) fid_2d->FID[k]->updateAbs();

    delete helpFID2D;
    return true;
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

    if(tt<0) tt=-1*tt;

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


    if(fabs(regionalTheta(pa1.theta())-regionalTheta(pa2.theta()))
            && fabs(regionalPhi(pa1.phi())-regionalPhi(pa2.phi())))
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

