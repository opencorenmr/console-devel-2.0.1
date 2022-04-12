#ifndef S3DREARRANGEMENTWIDGET_H
#define S3DREARRANGEMENTWIDGET_H

#include <QPushButton>

#include "processBase.h"
#include "fidDomain.h"

class S3DrearrangementWidget : public TProcessBase
{
    Q_OBJECT
public:
    explicit S3DrearrangementWidget();
    ~S3DrearrangementWidget(){;}

    QSpinBox *size1SpinBox,*size2SpinBox;
    QPushButton *setCenterattheOriginPushButton;
    QPushButton *xyzRotationPushButton;

public slots:
    void performSetCenterattheOrigin();
    void performxyzRotation();

private:
    void createWidgets();
    void createPanel();
    void CreateConnections();
};

#endif // 3DREARRANGEMENTWIDGET_H
