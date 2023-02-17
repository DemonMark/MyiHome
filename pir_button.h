#ifndef PIR_BUTTON_H
#define PIR_BUTTON_H

#include <QPushButton>
#include <QMouseEvent>
#include <QDebug>
#include <QTimer>
#include <QDateTime>
#include "mainwindow.h"
#include "mydbs.h"

class pir_button : public QPushButton
{
    Q_OBJECT
public:
    explicit pir_button(QWidget *parent = 0);
    void mousePressEvent(QMouseEvent *ev);
    void db_active();
    QTimer *present;
    QLabel *label_countdown;
    MainWindow *p_mw;
    int c_time;

signals:

    void klik();
    void violation_name(QString vname);

public slots:

    void naruszeniestrefy(int &time);
    void setdial();
};

#endif // PIR_BUTTON_H
