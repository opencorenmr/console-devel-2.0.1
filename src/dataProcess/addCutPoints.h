#ifndef TADDCUTPOINTS_H
#define TADDCUTPOINTS_H

#include "processElement.h"


class TAddCutPoints : public TProcessElement
{
public:
    TAddCutPoints();
    ~TAddCutPoints();
    enum THeadTail {Head,Tail,Both,headTailUndefined};
    enum TOperation {Cut, Leave, ZeroFill, Extrapolate, Add, operationUndefined};

    bool process(TFID *fid);
    bool process(TFID_2D *fid_2d, int k);
    bool process(TFID_2D *fid_2d);



    QStringList processInformation();
    QString command();

    int headTail() {return FHeadTail;}
    void setHeadTail(int ht) {FHeadTail=ht;}
    int operation() {return FOperation;}
    void setOperation(int op) {FOperation=op;}
    int headPoints() {return FHeadPoints;}
    int tailPoints() {return FTailPoints;}
    int averagePoint() {return FAveragePoints;}


public slots:
    void setHeadPoints(int p) {FHeadPoints=p;}
    void setTailPoints(int p) {FTailPoints=p;}
    void setAveragePoints(int p) {FAveragePoints=p;}

private:
    int FHeadTail;
    int FOperation;
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
    bool extrapolateHeadTail(TFID *fid);


};


#endif // TADDCUTPOINTS_H
