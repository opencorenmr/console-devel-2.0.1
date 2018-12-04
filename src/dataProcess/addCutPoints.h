#ifndef TADDCUTPOINTS_H
#define TADDCUTPOINTS_H

#include "processElement.h"

class TAddCutPoints : public TProcessElement
{
public:
    enum THeadTail {Head,Tail,Both,headTailUndefined};
    enum TOperation {Cut, Leave, ZeroFill, Extrapolate, Add, operationUndefined};
    TAddCutPoints();
    ~TAddCutPoints();

    bool process(TFID *fid);
    bool process(TFID_2D *fid_2d, int k);
    bool process(TFID_2D *fid_2d);



    QStringList processInformation();
    QString command();

    THeadTail headTail() {return FHeadTail;}
    void setHeadTail(THeadTail ht) {FHeadTail=ht;}
    TOperation operation() {return FOperation;}
    void setOperation(TOperation op) {FOperation=op;}
    int headPoints() {return FHeadPoints;}
    int tailPoints() {return FTailPoints;}
    int averagePoint() {return FAveragePoints;}


public slots:
    void setHeadPoints(int p) {FHeadPoints=p;}
    void setTailPoints(int p) {FTailPoints=p;}
    void setAveragePoints(int p) {FAveragePoints=p;}

private:
    THeadTail FHeadTail;
    TOperation FOperation;
    int FHeadPoints;
    int FTailPoints;
    int FAveragePoints;

    bool cutHead(TFID *fid);
    bool leaveHead(TFID *fid);
    bool leaveTail(TFID *fid);
    bool leaveMiddle(TFID *fid);
    bool cutTail(TFID *fid);
    bool cutHeadTail(TFID *fid);
    bool zeroFillHead(TFID *fid);
    bool zeroFillTail(TFID *fid);
    bool zeroFillHeadTail(TFID *fid);
    bool extrapolateHead(TFID *fid);
    bool extrapolateTail(TFID *fid);


};


#endif // TADDCUTPOINTS_H
