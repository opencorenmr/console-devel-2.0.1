#ifndef TAXISFORMATWIDGET_H
#define TAXISFORMATWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QSpinBox>
#include <QDebug>
#include <QComboBox>
#include <QStringList>
#include <QSettings>
#include "../fid.h"
#include "axisStyle.h"
#include "processBase.h"

class TAxisFormatWidget : public TProcessBase
{
    Q_OBJECT
public:
    explicit TAxisFormatWidget();
    ~TAxisFormatWidget() {;}
   // TFID_2D *FID_2D;
   // void setFID2D(TFID_2D *f) {FID_2D=f; FfidSetted=true;}
   // bool fidSetted() {return FfidSetted;}

    TAxisStyle *axisStyle;

    QComboBox *domainComboBox, *unitComboBox;
    QLineEdit *axisLabelLineEdit;
    QSpinBox *setReferenceSpinBox;
    QPushButton *setReferencePushButton;
    QLineEdit *referenceValueLineEdit;
    QPushButton *applyPushButton, *cancelPushButton;

    void createWidgets();
    void createPanel();
    void createConnections();

signals:
    //void requestClickSet();
    //void cancelClickSet();
   // void displayRequest();  // display
  //  void performRequest(); // display & ini file

public slots:
    void init();
    void revert();
    void refresh();
    void apply();
    void setReferencePoint(int rp) {setReferenceSpinBox->setValue(rp);}
    void setDomain();
    void setUnit();
    void setFrequencyUnit();
    void setTimeUnit();
    void setOtherUnit();
    void createSettings(QSettings *settings, QString section);
    void readSettings(QSettings *settings, QString section);
    void updateXAxis();

private slots:
    void onSetReferencePushButtonToggled();

private:
   // bool FfidSetted;
    QStringList timeStringList;
};

#endif // TAXISFORMATWIDGET_H
