#include "toggleParams.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QDebug>


TToggleParams::TToggleParams(QWidget *parent) : QWidget(parent)
{
    this->hide();
    this->setWindowTitle(tr("Toggle Parameters"));
    this->setWindowFlags(Qt::Dialog);
    this->setFixedWidth(400);
    this->setFixedHeight(300);

}


void TToggleParams::createWidgets()
{


}

void TToggleParams::createPanel()
{


}
