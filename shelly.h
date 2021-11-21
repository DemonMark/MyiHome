#ifndef SHELLY_H
#define SHELLY_H
#include <QWidget>
#include <QUdpSocket>
#include <QPushButton>
#include <QTimer>
#include <QDebug>
#include <QMouseEvent>
#include "mainwindow.h"

class shelly : public QPushButton
{
    Q_OBJECT
public:
    explicit shelly(QWidget *parent = nullptr);

    MainWindow *sp_mw;
    QUdpSocket *shellsock;
    QTimer *mute_counter;
    QLabel *label_cd = nullptr,
           *mute_ind = nullptr;
    QSpinBox *mute_counter_val = nullptr;

    int counter;

    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);

private:

signals:

public slots:

     void answer();
     void offline();
     void open();
};

#endif // SHELLY_H
