#ifndef ICON_TEMP_H
#define ICON_TEMP_H

#include <QPushButton>
#include <QMouseEvent>
#include "mainwindow.h"
#include "mydbs.h"

class icon_temp : public QLabel
{
    Q_OBJECT
public:
    explicit icon_temp(QWidget *parent = nullptr);

    void mousePressEvent(QMouseEvent *ev);

    MainWindow *tp_mw;

signals:

public slots:
};

#endif // ICON_TEMP_H
