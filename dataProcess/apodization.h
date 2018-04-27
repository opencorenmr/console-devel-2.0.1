#ifndef TAPODIZATION_H
#define TAPODIZATION_H

#include "processElement.h"

enum ApodizationType {Gaussian,Exponential,Sinc};

class TApodization : public TProcessElement
{
public:

    TApodization();
    ~TApodization();

    int apodizationType() {return FApodizationType;}
    void setApodizationType(int at) {FApodizationType=at;}
  //  double timeConstant() {return FTimeConstant;}
  //  void setTimeConstant(double tc) {FTimeConstant=tc;}
    double width() {return FWidth;}
    void setWidth(double d) {FWidth=d;}
    bool inverse() {return FInverse;}
    void setInverse(bool b) {FInverse=b;}

   // ApplyMode applyMode;

    bool process(TFID *fid);
    bool process(TFID_2D *fid_2d, int k);
    bool process(TFID_2D *fid_2d);

    QStringList processInformation();
    QString command();

public slots:

private:
    int FApodizationType;
  //  double FTimeConstant;
    double FWidth;
    bool FInverse;

};

#endif // TAPODIZATION_H
