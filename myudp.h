#ifndef MYUDP_H
#define MYUDP_H
#include "mainwindow.h"
#include <QUdpSocket>
#include <QObject>
#include <QTimer>
#include "mydbs.h"

class MyUDP : public QObject
{
  Q_OBJECT
  public:
        explicit MyUDP(QObject *parent = 0);

        QUdpSocket *socket;

private:
        QTimer *timer_LOff;
        QTimer *timer_obecnosc;
        QTimer *action;
        QTimer *ping;

  signals:
        void changes();
        void all_off_();
        void gate();
        void hex_comming();

  public slots:
        void readyRead();
        void simulation(bool on);
        void random_off();
        void WYSUDP(QString addr);
};
#endif // MYUDP_H
