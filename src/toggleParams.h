#ifndef TTOGGLEPARAMS_H
#define TTOGGLEPARAMS_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QToolButton>
#include <QLineEdit>

class TToggleParams : public QWidget
{
    Q_OBJECT
public:
    explicit TToggleParams(QWidget *parent = nullptr);

public slots:

signals:


private:
    void createWidgets();
    void createPanel();

};

#endif // TTOGGLEPARAMS_H
