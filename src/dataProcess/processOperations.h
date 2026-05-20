#ifndef TPROCESSOPERATIONS_H
#define TPROCESSOPERATIONS_H

#include <QObject>
#include <QSettings>
#include "processElement.h"


class TProcessOperations : public QObject
{
    Q_OBJECT
public:
    explicit TProcessOperations(QObject *parent = nullptr);
    ~TProcessOperations();

    QList<TProcessElement*> processElements;
    bool saveToFile(QString filename);
    bool loadFromFile(QString filename);
    bool applyTo(TFID_2D *fid_2d);

    QString errorMessage() {return FErrorMessage;}
    void setErrorMessage(QString qs) {FErrorMessage=qs;}

    int majorVersion() {return FMajorVersion;}
    void setMajorVersion(int k) {FMajorVersion=k;}
    int minorVersion() {return FMinorVersion;}
    void setMinorVersion(int k) {FMinorVersion=k;}
    int patchVersion() {return FPatchVersion;}
    void setPatchVersion(int k) {FPatchVersion=k;}

signals:

public slots:


private:

    int FMajorVersion;
    int FMinorVersion;
    int FPatchVersion;
    QString FErrorMessage;

};

#endif // TPROCESSOPERATIONS_H
