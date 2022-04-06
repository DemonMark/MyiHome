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
    void mouseReleaseEvent(QMouseEvent *ev);

    int temp_counter;

    MainWindow *tp_mw;

    QTimer *tmp_counter;

signals:

public slots:
};

#endif // ICON_TEMP_H
