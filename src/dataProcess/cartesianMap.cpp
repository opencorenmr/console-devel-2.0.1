#include "cartesianMap.h"
//#include "fft.h"
#include <QVector3D>
#include <float.h>
#include <QRegularExpression>

TCartesianMap3D::TCartesianMap3D()
{
  setProcessType(TProcessType::CartesianMap3D);
  origPolarAngles.clear();
  clearIndices();
}


TCartesianMap3D::~TCartesianMap3D()
{
  origPolarAngles.clear();
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
        sl2=sl1.at(k).trimmed().split(QRegularExpression("\\s+"));
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

bool TCartesianMap3D::findPointsABC(TPolarAngle p)
{
  int aIndex=-1,bIndex=-1,cIndex=-1;
  QList<int> closerIndices;
  int numberofFocusedPoint = 9; //>3
  QVector3D op,oa,ob,oc;
  if(origPolarAngles.size()<numberofFocusedPoint)
  {
    errorQ=true;
    setErrorMessage("Number of points is insufficient.");
    return false;
  }

  clearIndices();
  op=TPolarAngle::vector3D(p);
  QVector3D ok,ol;
  double coskop,coslop;
  for(int k=0; k<origPolarAngles.size(); k++)
  {
      closerIndices.append(k);
      ok=TPolarAngle::vector3D(origPolarAngles.at(k));
      coskop = QVector3D::dotProduct(op,ok);

      int l=closerIndices.size()-1;
      while(l)
      {
          l-=1;
          ol=TPolarAngle::vector3D(origPolarAngles.at(closerIndices.at(l)));
          coslop = QVector3D::dotProduct(op,ol);
          if(coslop<coskop)
          {
              closerIndices.swapItemsAt(l,l+1);
          }
          else
          {
              l=0;
          }
      }
      if(closerIndices.size()>numberofFocusedPoint)
      {
          closerIndices.removeLast();
      }
  }

  QList<double> dotList;
  dotList.clear();
  for(int i=0;i<closerIndices.size();i++){
      dotList.append(QVector3D::dotProduct(op,TPolarAngle::vector3D(origPolarAngles.at(closerIndices.at(i)))));
      if(i>0){
          if(dotList.at(i-1)<dotList.at(i)){
              qDebug() << "something wrong with closer points list";
          }
      }
  }

//  qDebug() << closerIndices;

  aIndex=closerIndices.at(0);
  if(aIndex<0)
  {
      errorQ=true;
      setErrorMessage("Failed to find point A.");
      return false;
  }

  oa=TPolarAngle::vector3D(origPolarAngles.at(aIndex));
//  pa=oa-op;

  double dotab,dotbc,dotca,dotap,dotbp,dotcp,u,v,w,angleSum,angleSumMin=3*PI;
  dotap=QVector3D::dotProduct(oa,op);
  for(int ib=1; ib<numberofFocusedPoint-1; ib++)
  {
      ob=TPolarAngle::vector3D(origPolarAngles.at(closerIndices.at(ib)));
      dotab=QVector3D::dotProduct(oa,ob);
      dotbp=QVector3D::dotProduct(ob,op);
      for(int ic=ib+1; ic<numberofFocusedPoint; ic++)
      {
          oc=TPolarAngle::vector3D(origPolarAngles.at(closerIndices.at(ic)));
          dotbc=QVector3D::dotProduct(ob,oc);
          dotca=QVector3D::dotProduct(oc,oa);
          dotcp=QVector3D::dotProduct(oc,op);
          u=(1-dotbc*dotbc)*dotap + (dotbc*dotca-dotab)*dotbp + (dotab*dotbc-dotca)*dotcp;
          v=(dotbc*dotca-dotab)*dotap + (1-dotca*dotca)*dotbp + (dotca*dotab-dotbc)*dotcp;
          w=(dotab*dotbc-dotca)*dotap + (dotca*dotab-dotbc)*dotbp + (1-dotab*dotab)*dotcp;
          if(u<0||v<0||w<0){continue;}

          angleSum = acos(dotab)+acos(dotbc)+acos(dotca);
          if(angleSum<angleSumMin)
          {
              angleSumMin=angleSum;
              bIndex=closerIndices.at(ib);
              cIndex=closerIndices.at(ic);
          }
      }
  }
  if(bIndex<0||cIndex<0)
  {
      errorQ=true;
      setErrorMessage("Failed to find point B and C.");
      return false;
  }

//  ob=TPolarAngle::vector3D(origPolarAngles.at(bIndex));
//  pb=ob-op;
//  oc=TPolarAngle::vector3D(origPolarAngles.at(cIndex));
//  pc=oc-op;

  FPointAIndex=aIndex;
  FPointBIndex=bIndex;
  FPointCIndex=cIndex;

  return true;
}

void TCartesianMap3D::calcWeight(TPolarAngle p,double rop){
    if(rop == 0){
        ceiledr = 0;
        FWeightA = 0;
        FWeightB = 0;
        FWeightC = 0;
        FWeightQ = 0;
        return;
    }

    QVector3D op,OA,OB,OC;
    op = TPolarAngle::vector3D(p);
    OA = TPolarAngle::vector3D(origPolarAngles.at(FPointAIndex));
    OB = TPolarAngle::vector3D(origPolarAngles.at(FPointBIndex));
    OC = TPolarAngle::vector3D(origPolarAngles.at(FPointCIndex));

    if(interpolateMode==vector){
        double dota,dotb,dotc,dotpa,dotpb,dotpc;
        dota = QVector3D::dotProduct(OB,OC);
        dotb = QVector3D::dotProduct(OC,OA);
        dotc = QVector3D::dotProduct(OA,OB);
        dotpa = QVector3D::dotProduct(op,OA);
        dotpb = QVector3D::dotProduct(op,OB);
        dotpc = QVector3D::dotProduct(op,OC);

        double un0,vn0,wn0,kd0;
        un0 = (1-dota*dota)*dotpa + (dota*dotb-dotc)*dotpb + (dotc*dota-dotb)*dotpc;
        vn0 = (dota*dotb-dotc)*dotpa + (1-dotb*dotb)*dotpb + (dotb*dotc-dota)*dotpc;
        wn0 = (dotc*dota-dotb)*dotpa + (dotb*dotc-dota)*dotpb + (1-dotc*dotc)*dotpc;
        kd0 = un0+vn0+wn0;

        if(kd0!=0)
        {
//            qDebug() << (un+vn+wn)/kd;
//            if(un<0||vn<0||wn<0){
//                qDebug() << "could not get surrounding 3 points";
//                qDebug() << un/kd << vn/kd << wn/kd;
//            }

            QVector3D oq;
            oq = (un0*OA + vn0*OB + wn0*OC)/kd0;
            ceiledr = (int) ceil(rop/oq.length());

    //        QVector3D sub = OP-(un*OA + vn*OB + wn*OC)/kn;
    //        if(sub.length()>1e-4){
    //            qDebug() << "Mismatch of coefficients";
    //            qDebug() << sub.length() << OP << (un*OA + vn*OB + wn*OC)/kn;
    //        }

            double QQ2Length,PQLength;
            FWeightA = un0/kd0;
            FWeightB = vn0/kd0;
            FWeightC = wn0/kd0;
            QQ2Length = oq.length();
            PQLength = ceiledr*oq.length()-rop;
            FWeightQ = 1 - PQLength/QQ2Length;

//            if (abs(FWeightQ-0.5) > 0.5){
//                qDebug() << rop << ceil(rop) << cr;
//                qDebug() << OA << OB << OC;
//                qDebug() << un0 << vn0 << wn0 << kd0;
//                qDebug() << QQ2Length << PQLength << FWeightQ;
//                qDebug() << (un0*OA + vn0*OB + wn0*OC)/kd0;
//                qDebug() << "";
//            }
        }
        else
        {
            FWeightA=0; FWeightB=0; FWeightC=0; FWeightQ=0; ceiledr=0;
        }
    }else if(interpolateMode==dInverse){
        ceiledr = (int) ceil(rop);
        FWeightQ = rop-floor(rop);

        QVector3D pa,pb,pc;
        pa=OA-op;
        pb=OB-op;
        pc=OC-op;

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
    }
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

    for(int k=1; k<origPolarAngles.size(); k++)
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
    switch (interpolateMode){
    case vector:
        interpolate();
        break;
    case dInverse:
        interpolate();
        break;
    case gridSinc:
        gridding();
        break;
    case gridGaus:
        gridding();
        break;
    default:
        interpolate();
        break;
    }
}

void TCartesianMap3D::interpolate()
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
    // We make a (numberofPointsonCubeSide x numberofPointsonCubeSide) x numberofPointsonCubeSide matrix
    //
    int arrayLength=numberofPointsonCubeSide*numberofPointsonCubeSide;
    for(int k=0; k<arrayLength; k++)
    {
        helpFID2D->FID.append(new TFID(numberofPointsonCubeSide));
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
    FLength1D=numberofPointsonCubeSide;
//    while(!thetaTable.isEmpty()) thetaTable.removeLast();
//    while(!phiTable.isEmpty()) phiTable.removeLast();
    while(!rTable.isEmpty()) rTable.removeLast();
    while(!polarAngleTable.isEmpty()) polarAngleTable.removeLast();

    for(int x=0; x<length1D(); x++)
    {
        emit tableCount(x);

        rTable.append(QList<QList<double> >());
//        thetaTable.append(QList<QList<double> >());
//        phiTable.append(QList<QList<double> >());
        polarAngleTable.append(QList<QList<TPolarAngle> >());
        for(int y=0; y<length1D(); y++)
        {
        //    cartesianMapTable[x].append(QList<QPoint>());
            rTable[x].append(QList<double>());
//            thetaTable[x].append(QList<double>());
//            phiTable[x].append(QList<double>());
            polarAngleTable[x].append(QList<TPolarAngle>());
            for(int z=0; z<length1D(); z++)
            {
          //      cartesianMapTable[x][y].append(QPoint(x+z*length1D(),y));
                double h=double(length1D()) / 2.0;
                double z2=double(z)-h;
                double x2=double(x)-h;
                double y2=double(y)-h;
                polarAngleTable[x][y].append(TPolarAngle(x2,y2,z2));
                rTable[x][y].append(ratioofDistanceBetweenPoints*sqrt(x2*x2+y2*y2+z2*z2));
//                thetaTable[x][y].append(polarAngleTable[x][y][z].theta());
//                phiTable[x][y].append(polarAngleTable[x][y][z].phi());

            } //
        } //
    } //

    emit genTableComplete();

    //
    // Interpolation!
    //
    for(int z=0; z<numberofPointsonCubeSide; z++)
    {
      emit calcCount(z);
      emit info("Processing ... ("
              + QString::number(z) + "/"
              + QString::number(numberofPointsonCubeSide) + ")"
              );
      if(stopped) return;

    for(int y=0; y<numberofPointsonCubeSide; y++)
    {
    for(int x=0; x<numberofPointsonCubeSide; x++)
    {
      if((int) ceil(rTable.at(x).at(y).at(z)) > nCol-1) // Outside the sphere -> zero
      {
          helpFID2D->FID[x+z*numberofPointsonCubeSide]->real->sig[y]=0.0;
          helpFID2D->FID[x+z*numberofPointsonCubeSide]->imag->sig[y]=0.0;
          emit info("Data zero was set at ("
                + QString::number(x) + ","
                + QString::number(y) + ","
                + QString::number(z) + ")"
                );

   //       qDebug() << "TCartesianMap3D::process: Data zero was set at (" << x <<"," << y << "," << z << ").";
      }
      else if(rTable.at(x).at(y).at(z)<DBL_EPSILON) // Origin
      {
          helpFID2D->FID[x+z*numberofPointsonCubeSide]->real->sig[y]=rOrigin;
          helpFID2D->FID[x+z*numberofPointsonCubeSide]->imag->sig[y]=iOrigin;
          emit info("Origin found at ("
                + QString::number(x) + ","
                + QString::number(y) + ","
                + QString::number(z) + ")"
                );
          // qDebug() << "TCartesianMap3D::process: Origin found at (" << x <<"," << y << "," << z << ").";
      }
      else
      {
          double d1,d2,e1,e2;
          double r=rTable.at(x).at(y).at(z);
          int rr=round(r);
          int fr=floor(r);
          int cr=ceil(r);
          checkParallel(polarAngleTable.at(x).at(y).at(z));
          if(parallelIndex()>-1) // Parallel data found -> no need to "laterally" interpolate
          {
              emit info("Parallel axis found at ("
                    + QString::number(x) + ","
                    + QString::number(y) + ","
                    + QString::number(z) + ")"
                    );
            // qDebug() << "Parallel axis found at (" << x <<"," << y << "," << z << ").";
            if(fabs(rr-r)<DBL_EPSILON)
            {
              helpFID2D->FID[x+z*numberofPointsonCubeSide]->real->sig[y] = FID_2D->FID.at(parallelIndex())->real->sig.at(rr);
              helpFID2D->FID[x+z*numberofPointsonCubeSide]->imag->sig[y] = FID_2D->FID.at(parallelIndex())->imag->sig.at(rr);
            }
            else
            {
              d1=FID_2D->FID.at(parallelIndex())->real->sig.at(fr);
              d2=FID_2D->FID.at(parallelIndex())->real->sig.at(cr);
              e1=FID_2D->FID.at(parallelIndex())->imag->sig.at(fr);
              e2=FID_2D->FID.at(parallelIndex())->imag->sig.at(cr);
              helpFID2D->FID[x+z*numberofPointsonCubeSide]->real->sig[y] = (cr-r)*d1 + (r-fr)*d2;
              helpFID2D->FID[x+z*numberofPointsonCubeSide]->imag->sig[y] = (cr-r)*e1 + (r-fr)*e2;
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

//             qDebug() << x << y << z << ":" << FPointAIndex << FPointBIndex << FPointCIndex;

            calcWeight(polarAngleTable.at(x).at(y).at(z),rTable.at(x).at(y).at(z));

            // qDebug() << FWeightA << FWeightB << FWeightC;

            double da,db,dc,ea,eb,ec;
//            if(fabs(rr-r)<DBL_EPSILON)
//            {
//                da=FID_2D->FID.at(FPointAIndex)->real->sig.at(rr);
//                db=FID_2D->FID.at(FPointBIndex)->real->sig.at(rr);
//                dc=FID_2D->FID.at(FPointCIndex)->real->sig.at(rr);
//                helpFID2D->FID[x+z*nCol]->real->sig[y] = FWeightA*da + FWeightB*db + FWeightC*dc;
//                ea=FID_2D->FID.at(FPointAIndex)->imag->sig.at(rr);
//                eb=FID_2D->FID.at(FPointBIndex)->imag->sig.at(rr);
//                ec=FID_2D->FID.at(FPointCIndex)->imag->sig.at(rr);
//                helpFID2D->FID[x+z*nCol]->imag->sig[y] = FWeightA*ea + FWeightB*eb + FWeightC*ec;
//            }
//            else // We need to "radially" interpolate!
//            {
              if(ceiledr>nCol-1){
                  helpFID2D->FID[x+z*numberofPointsonCubeSide]->real->sig[y] = 0.0;
                  helpFID2D->FID[x+z*numberofPointsonCubeSide]->imag->sig[y] = 0.0;
//                  qDebug() << rTable.at(x).at(y).at(z) << ceiledr;
              }else{
              da=FID_2D->FID.at(FPointAIndex)->real->sig.at(ceiledr-1);
              db=FID_2D->FID.at(FPointBIndex)->real->sig.at(ceiledr-1);
              dc=FID_2D->FID.at(FPointCIndex)->real->sig.at(ceiledr-1);
              d1=FWeightA*da + FWeightB*db + FWeightC*dc;
              da=FID_2D->FID.at(FPointAIndex)->real->sig.at(ceiledr);
              db=FID_2D->FID.at(FPointBIndex)->real->sig.at(ceiledr);
              dc=FID_2D->FID.at(FPointCIndex)->real->sig.at(ceiledr);
              d2=FWeightA*da + FWeightB*db + FWeightC*dc;

              ea=FID_2D->FID.at(FPointAIndex)->imag->sig.at(ceiledr-1);
              eb=FID_2D->FID.at(FPointBIndex)->imag->sig.at(ceiledr-1);
              ec=FID_2D->FID.at(FPointCIndex)->imag->sig.at(ceiledr-1);
              e1=FWeightA*ea + FWeightB*eb + FWeightC*ec;
              ea=FID_2D->FID.at(FPointAIndex)->imag->sig.at(ceiledr);
              eb=FID_2D->FID.at(FPointBIndex)->imag->sig.at(ceiledr);
              ec=FID_2D->FID.at(FPointCIndex)->imag->sig.at(ceiledr);
              e2=FWeightA*ea + FWeightB*eb + FWeightC*ec;
              helpFID2D->FID[x+z*numberofPointsonCubeSide]->real->sig[y] = (1-FWeightQ)*d1 + FWeightQ*d2;
              helpFID2D->FID[x+z*numberofPointsonCubeSide]->imag->sig[y] = (1-FWeightQ)*e1 + FWeightQ*e2;
              }
//            } // else
          }



      }

    }
    }
    }


    emit calcComplete();


    //
    // We clear the content of FID_2D,
    // make a (numberofPointsonCubeSide x numberofPointsonCubeSide) x numberofPointsonCubeSide matrix,
    // and copy data from helpFID2D
    //
    FID_2D->FID.clear();
    for(int k=0; k<arrayLength; k++)
    {
        emit copyCount(k);
        FID_2D->FID.append(new TFID(numberofPointsonCubeSide));
        for(int j=0; j<numberofPointsonCubeSide; j++)
        {
           FID_2D->FID[k]->real->sig[j]=helpFID2D->FID.at(k)->real->sig.at(j);
           FID_2D->FID[k]->imag->sig[j]=helpFID2D->FID.at(k)->imag->sig.at(j);
        } // j
        // We update the absolute values
        FID_2D->FID[k]->updateAbs();
        FID_2D->FID[k]->setEmpty(false);
    }

    FID_2D->setCurrentFID(0);

    delete helpFID2D;

    emit copyComplete();

    mutex.lock();
    condition.wait(&mutex); // We let the thread sleep.
    mutex.unlock();

  } // forever
}

void TCartesianMap3D::gridding()
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
        // We make a (numberofPointsonCubeSide x numberofPointsonCubeSide) x numberofPointsonCubeSide matrix
        //
        int arrayLength=numberofPointsonCubeSide*numberofPointsonCubeSide;
        for(int k=0; k<arrayLength; k++)
        {
            helpFID2D->FID.append(new TFID(numberofPointsonCubeSide));
            helpFID2D->FID.last()->setEmpty(false);
        }

////        double x,y,z,xB,yB,zB,dx,dy,dz,tmpd;
//        double density,tmp;
//        double weight,n_1;
////        QVector3D vec,vecB;
//        for(int row=0;row<nRow;row++){
//            emit tableCount(row*nCol/nRow);
////            vec = TPolarAngle::vector3D(origPolarAngles[row]);
//            for(int col=0;col<nCol;col++){
////                x = col * vec.x();
////                y = col * vec.y();
////                z = col * vec.z();
////                density = 0.0;
////                for(int rowB=0;rowB<nRow;rowB++){
////                    vecB = TPolarAngle::vector3D(origPolarAngles[rowB]);
//////                    for(int colB=0; colB<fmin(nCol,numberofPointsonCubeSide*ratioofDistanceBetweenPoints); colB++){  //debug
////                    for(int colB=0;colB<nCol;colB++){  //release
////                        xB = colB * vecB.x();
////                        yB = colB * vecB.y();
////                        zB = colB * vecB.z();
////                        dx = x - xB;
////                        dy = y - yB;
////                        dz = z - zB;
////                        tmpd = weightFunction(PI*dx/ratioofDistanceBetweenPoints)*weightFunction(PI*dy/ratioofDistanceBetweenPoints)*weightFunction(PI*dz/ratioofDistanceBetweenPoints);
////                        density += tmpd;
////                    }
////                }
//                n_1 = sqrt((3.0*nRow/PI-1.0)/12.0);
//                if(0<col&&col<n_1){
//                    weight = (12.0*col*col+1.0)/3.0/nRow;
//                }else if(n_1<=col){
//                    weight = 8.0*n_1*(col-n_1)/nRow+1.0/PI;
//                }else{
//                    weight = 1.0/6.0/nRow;
//                }
//                density = 1.0/weight;
//                if(abs(density)<DBL_EPSILON){
//                    FID_2D->FID[row]->real->sig[col] = DBL_MAX;
//                    FID_2D->FID[row]->imag->sig[col] = DBL_MAX;
//                    qDebug() << "zero density point is found." << row << col;
//                }else{
//                    tmp = FID_2D->FID.at(row)->real->sig.at(col);
//                    FID_2D->FID[row]->real->sig[col] = tmp / density;
//                    tmp = FID_2D->FID.at(row)->imag->sig.at(col);
//                    FID_2D->FID[row]->imag->sig[col] = tmp / density;
//                }
////                qDebug() << density;
//            }
//        }

        emit genTableComplete();

        double (TCartesianMap3D::*kernel)(double);
        switch (interpolateMode){
        case gridSinc:
            kernel = &TCartesianMap3D::sinc;
            break;
        case gridGaus:
            kernel = &TCartesianMap3D::gaussian;
            break;
        default:
            kernel = &TCartesianMap3D::gaussian;
        }

        //
        // Gridding!
        //
        double xd,yd,zd,xB2,yB2,zB2,valr,vali,sigr,sigi,tmpr,tmpi;
        QVector3D vecB2;
        for(int z=0; z<numberofPointsonCubeSide; z++)
        {
            emit calcCount(z);
            emit info("Processing ... ("
                    + QString::number(z) + "/"
                    + QString::number(numberofPointsonCubeSide) + ")"
                    );
            if(stopped) return;
            zd = (z-numberofPointsonCubeSide/2.0) * ratioofDistanceBetweenPoints;
            for(int y=0; y<numberofPointsonCubeSide; y++)
            {
                yd = (y-numberofPointsonCubeSide/2.0) * ratioofDistanceBetweenPoints;
                for(int x=0; x<numberofPointsonCubeSide; x++)
                {
                    xd = (x-numberofPointsonCubeSide/2.0) * ratioofDistanceBetweenPoints;
                    valr = 0.0;
                    vali = 0.0;
                    for(int rowB=0; rowB<nRow; rowB++)
                    {
                        vecB2 = TPolarAngle::vector3D(origPolarAngles[rowB]);
//                        for(int colB=0; colB<fmin(nCol,numberofPointsonCubeSide*ratioofDistanceBetweenPoints); colB++)  //debug
                        for(int colB=0; colB<nCol; colB++)  //release
                        {
                            xB2 = colB * vecB2.x();
                            yB2 = colB * vecB2.y();
                            zB2 = colB * vecB2.z();
                            sigr = FID_2D->FID.at(rowB)->real->sig.at(colB);
                            sigi = FID_2D->FID.at(rowB)->imag->sig.at(colB);
                            tmpr = sigr*(this->*kernel)((xd-xB2)/ratioofDistanceBetweenPoints)*(this->*kernel)((yd-yB2)/ratioofDistanceBetweenPoints)*(this->*kernel)((zd-zB2)/ratioofDistanceBetweenPoints);
                            tmpi = sigi*(this->*kernel)((xd-xB2)/ratioofDistanceBetweenPoints)*(this->*kernel)((yd-yB2)/ratioofDistanceBetweenPoints)*(this->*kernel)((zd-zB2)/ratioofDistanceBetweenPoints);
                            valr += tmpr;
                            vali += tmpi;
                        }
                    }
                    helpFID2D->FID[x+z*numberofPointsonCubeSide]->real->sig[y] = valr;
                    helpFID2D->FID[x+z*numberofPointsonCubeSide]->imag->sig[y] = vali;
                }
            }
        }

        emit calcComplete();

//        TFFT3D *fft3d=new TFFT3D;
//        fft3d->FFT3D_setLengths(numberofPointsonCubeSide,numberofPointsonCubeSide);
//        fft3d->process(helpFID2D);

        //
        // We clear the content of FID_2D,
        // make a (numberofPointsonCubeSide x numberofPointsonCubeSide) x numberofPointsonCubeSide matrix,
        // and copy data from helpFID2D
        //
        FID_2D->FID.clear();
        for(int k=0; k<arrayLength; k++)
        {
            emit copyCount(k);
            FID_2D->FID.append(new TFID(numberofPointsonCubeSide));
            for(int j=0; j<numberofPointsonCubeSide; j++)
            {
               FID_2D->FID[k]->real->sig[j]=helpFID2D->FID.at(k)->real->sig.at(j);
               FID_2D->FID[k]->imag->sig[j]=helpFID2D->FID.at(k)->imag->sig.at(j);
            } // j
            // We update the absolute values
            FID_2D->FID[k]->updateAbs();
            FID_2D->FID[k]->setEmpty(false);
        }

        FID_2D->setCurrentFID(0);

        delete helpFID2D;

        emit copyComplete();

        mutex.lock();
        condition.wait(&mutex); // We let the thread sleep.
        mutex.unlock();

        } // forever
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

double TCartesianMap3D::weightFunction(double x)
{
    double fx;
    fx = abs(griddingKernel(x));
    return fx;
}

double TCartesianMap3D::griddingKernel(double x)
{
    double fx;
    fx = gaussian(x);
    return fx;
}

double TCartesianMap3D::sinc(double x)
{
    double fx;
    double xs = PI*x;
    if(abs(xs)<DBL_EPSILON){
        fx = 1.0;
    }else{
        fx = sin(xs)/xs;
    }
    return fx;
}

double TCartesianMap3D::hamming(double x)
{
    double fx;
    const double a=0.54;
    const double w=3;
    if(abs(x)<PI*w/2){
        fx = a + (1-a)*cos(2*x/w);
    }else{
        fx = 0;
    }
    return fx;
}

double TCartesianMap3D::unitBox(double x)
{
    double fx;
    if(abs(x)<=0.5){
        fx = 1.0;
    }else{
        fx = 0;
    }
    return fx;
}

double TCartesianMap3D::gaussian(double x)
{
    double fx;
    double sigma = 0.33;
    fx = exp(-x*x/2.0/sigma/sigma);
    return fx;
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

