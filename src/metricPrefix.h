#ifndef TMETRICPREFIX_H
#define TMETRICPREFIX_H

#include <QString>
#include <QStringList>

class TMetricPrefix
{
public:
    enum prefixes {Yotta,Zetta,Exa,Peta,Tera,Giga,Mega,Kilo,Hecto,Deca,
                   None,
                   Deci,Centi,Milli,Micro,Nano,Pico,Femto,Atto,Zepto,Yocto};

    TMetricPrefix();
    prefixes prefix() {return FPrefix;}
   // prefixes indexOf(QString s);
    void setPrefix(prefixes p) {FPrefix=p;}
    void setPrefix(QString name);
    static double Decimal(prefixes p);
    static QString Symbol(prefixes p);
    static QString Name(prefixes p);
    static QStringList prefixList();

private:
    prefixes FPrefix;
    int prefixIndex(QString name);
};

#endif // TMETRICPREFIX_H
