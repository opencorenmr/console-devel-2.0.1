#ifndef TRESHAPE_H
#define TRESHAPE_H

#include "processElement.h"

class TReshape : public TProcessElement
{
public:
    TReshape();
    ~TReshape();

    bool process(TFID_2D *fid_2d);
    QString command();

    int reshapeAL() {return FNewAL;}
    void setReshapeAL(int i) {FNewAL=i; return;}
    int reshapeNOfDivisions() {return FNOfDivisions;}
    void setReshapeNOfDivisions(int i) {FNOfDivisions=i; return;}
    TReshapeOption reshapeOption() {return FReshapeOption;}
    void setReshapeOption(TReshapeOption ro) {FReshapeOption=ro; return;}
    QString reshapeOptionStr();
    void setReshapeOptionStr(QString);

private:
    TReshapeOption FReshapeOption;
    int FNewAL;
    int FNOfDivisions;

};

#endif // TRESHAPE_H
