#ifndef EXPORT2DP_H
#define EXPORT2DP_H

#include "processElement.h"
#include "fidDomain.h"
#include <QString>

class KExport2DP: public TProcessElement
{

public:
    using TProcessElement::process;
    KExport2DP();
    ~KExport2DP();

    int axisDomain() {return FAxisDomain;}
    void setAxisDomain(int axisDomain) {FAxisDomain=axisDomain;}

    bool process(TFID_2D *fid_2d, QString fn, int sf1, int ef1,
                 int line, double contL, double contH);

private:
    bool getMaxMinParameter(TFID_2D *fid_2d, double *max, double *min,
                            double *base, int *f1, int *f2);
    int FAxisDomain;
};

#endif // EXPORT2DP_H
