#include "inifile.h"
#include <QFile>
#include <QTextStream>

TIniFile::TIniFile(QString fn)
{

  iniFileName=fn;
  QFile f(iniFileName);
  fileExists = false;
  fileExists = f.exists();

}


QStringList TIniFile::readSections()
{
  QStringList sl;
  sl.clear();
  Sections.clear();

  if(!fileExists) return sl;


  QFile file(iniFileName);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return sl;

  QTextStream in(&file);
  while (!in.atEnd()) {
    QString line = in.readLine();
    QString trimmedLine = line.trimmed();
    if (trimmedLine.startsWith("[") && trimmedLine.endsWith("]")) {
      trimmedLine.chop(1);
      trimmedLine.remove(0,1);
      trimmedLine=trimmedLine.trimmed();
      if (trimmedLine.length()>0) sl.append(trimmedLine);
    }
  }

  file.close();

  Sections=sl;
  return sl;
}
