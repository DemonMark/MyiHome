#ifndef SHELLY_H
#define SHELLY_H
#include <QWidget>
#include <QUdpSocket>
#include <QPushButton>
#include <QTimer>
#include <QDebug>
#include <QMouseEvent>
#include "mainwindow.h"
#include "mqtt_client.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

class shelly : public QPushButton
{
    Q_OBJECT

    typedef void (shelly::*SIGNALEMITTER)(bool);
    std::array<SIGNALEMITTER, 4> SIGNALPOINTERS{&shelly::INPUT_0, &shelly::INPUT_1, &shelly::INPUT_2, &shelly::INPUT_3};

public:
    explicit shelly(QWidget *parent = nullptr);

    MainWindow *sp_mw;
    QUdpSocket *shellsock;
    QLabel *label_cd = nullptr,
           *mute_ind = nullptr;
    QSpinBox *mute_counter_val = nullptr;

    int counter;

    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);

private:

signals:

    void SW(bool ON);
    void Relay(bool ON);
    void TIMER();
    void INPUT_0(bool ON);
    void INPUT_1(bool ON);
    void INPUT_2(bool ON);
    void INPUT_3(bool ON);
    void RSSI(uint);
    void SHELLY_CLICKED();

public slots:

     void answer();
     void open(int state);
     void msg_process(const QByteArray message);
};

#endif // SHELLY_H
