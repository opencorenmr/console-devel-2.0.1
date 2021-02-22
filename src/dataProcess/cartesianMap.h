#ifndef TCARTESIANMAP3D_H
#define TCARTESIANMAP3D_H

#include <QVector3D>
#include <math.h>
#include "processElement.h"
#define PI 3.14159265358979

enum TRelativeOrientation {NorthEast,NorthWest,SouthEast,SouthWest,Parallel};

class TPolarAngle
{
public:
    TPolarAngle() {setTheta(0); setPhi(0);}
    TPolarAngle(double t, double p)
    {
        double x,y,z;
        z=cos(t);
        x=sin(t)*cos(p);
        y=sin(t)*sin(p);
        double r=sqrt(x*x+y*y+z*z);
        if(r>0)
        {
            setTheta(acos(z/r));
            setPhi(atan2(y,x));
        }
        else
        {
            setTheta(0);
            setPhi(0);
        }

    }

    TPolarAngle(double x, double y, double z)
    {
      double r=sqrt(x*x+y*y+z*z);
      if(r>0)
      {
          setTheta(acos(z/r));
          setPhi(atan2(y,x));
      }
      else
      {
          setTheta(0);
          setPhi(0);
      }
    }
    ~TPolarAngle() {}

    void setThetaPhi(double t, double p)
    {
        double x,y,z;
        z=cos(t);
        x=sin(t)*cos(p);
        y=sin(t)*sin(p);
        double r=sqrt(x*x+y*y+z*z);
        if(r>0)
        {
            setTheta(acos(z/r));
            setPhi(atan2(y,x));
        }
        else
        {
            setTheta(0);
            setPhi(0);
        }

    }

    double theta() {return FTheta;}
    double phi() {return FPhi;}

    static double regionalTheta(double t);
    static double regionalPhi(double p);

    static QVector3D vector3D(TPolarAngle p)
    {
        return QVector3D(sin(p.theta())*cos(p.phi()),
                         sin(p.theta())*sin(p.phi()),
                         cos(p.theta()));
    }

    void setTheta(double t);
    void setPhi(double p);

    static int orientation(TPolarAngle pa1, TPolarAngle pa2);
    // orientation of pa2 looking from pa1

private:
    double FTheta;
    double FPhi;

};


class TCubeShell
{

public:



};

class TCartesianMap3D : public TProcessElement
{
public:
    TCartesianMap3D();
    ~TCartesianMap3D();

    QString cartesianMap3DPolarAnglesStr() {return origPolarAnglesStr();}
    bool setCartesianMap3DPolarAnglesStr(QString qs) {return setOrigPolarAngles(qs);}

    bool process(TFID_2D *fid_2d);

    QStringList processInformation();
    QString command();

    int length1D() {return FLength1D;}
    void setLength1D(int al);
    void generateCartesianMapTable();
 //   QList<QList<QList<QPoint> > > cartesianMapTable;
    QList<QList<QList<double> > > thetaTable;
    QList<QList<QList<double> > > phiTable;
    QList<QList<QList<double> > > rTable;
    QList<QList<QList<TPolarAngle> > > polarAngleTable;
    void generateTables();

    void checkParallel(TPolarAngle pa);
    bool findPointsABC(TPolarAngle p);

    QList<TPolarAngle> origPolarAngles;
    QString origPolarAnglesStr();
    bool setOrigPolarAngles(QString qs);
    // TODO: setOrigPolarAngles and/or readOrigPolarAngles
    int closestPolarAngleIndex(TPolarAngle polarAngle);


    int parallelIndex() {return FParallelIndex;}
    QList<int> relativeOrientation;

    int northWestIndex() {return FNorthWestIndex;}
    int northEastIndex() {return FNorthEastIndex;}
    int southWestIndex() {return FSouthWestIndex;}
    int southEastIndex() {return FSouthEastIndex;}

private:


    void clearIndices();

    int FLength1D;
    int FPointAIndex;
    int FPointBIndex;
    int FPointCIndex;

    double FWeightA;
    double FWeightB;
    double FWeightC;

    int FNorthWestIndex;
    double FNorthWestDotProduct;
    int FNorthEastIndex;
    double FNorthEastDotProduct;
    int FSouthWestIndex;
    double FSouthWestDotProduct;
    int FSouthEastIndex;
    double FSouthEastDotProduct;

    int FParallelIndex;
};

#endif // TCARTESIANMAP3D_H
