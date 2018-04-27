#include "intToHex.h"


QString intToHex(int i, int digits)
{
    QString s=QString::number(i, 16);

    if(s.length()>digits) s=s.right(digits);
    while(s.length()<digits) s='0'+s;

    return s.right(digits).toUpper();


}

QString myHex(quint64 n, int digits)
{
  QString h="0123456789ABCDEF";
  QString s="";
  int position;
  for(int i=1; i<=digits; ++i) {
    position = n & 0x0F;
    s.prepend(h.at(position));
    if (i!=digits) n = n >> 4;
  } // i
  return s;
}

