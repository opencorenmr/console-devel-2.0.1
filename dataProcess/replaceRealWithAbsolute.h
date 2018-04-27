#ifndef TREPLACEREALWITHABSOLUTE_H
#define TREPLACEREALWITHABSOLUTE_H

#include "processElement.h"

class TReplaceRealWithAbsolute : public TProcessElement
{
public:
//    using TProcessElement::process;
    TReplaceRealWithAbsolute();
    bool process(TFID *fid);
    bool process(TFID_2D *fid_2d, int k);
    bool process(TFID_2D *fid_2d);

    QStringList processInformation() {return QStringList() << "process=replace real with absolute";}
    QString command() {return "replace real with abs";}
}
;

#endif // TREPLACEREALWITHABSOLUTE_H
