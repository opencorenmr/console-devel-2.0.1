#ifndef INIFILE_H
#define INIFILE_H

#include <QSettings>
#include <QString>
#include <QStringList>

class TIniFile : public QSettings
{
public:
  TIniFile(QString fn); // constructor
//  TIniFile(TIniFile&);  // copy constructor
//  ~TIniFile();          // destructor
  QStringList readSections();

  bool fileExists;

protected:
  QString iniFileName;
  QStringList Sections;



//signals:
    
//public slots:
    
};

#endif // INIFILE_H
