#ifndef MYUDP_H
#define MYUDP_H

#include <QUdpSocket>
#include <QObject>
#include <QTimer>

class MyUDP : public QObject
{
  Q_OBJECT
  public:
        explicit MyUDP(QObject *parent = 0);
        void WYSUDP(QString addr);
        QUdpSocket *socket;
        void zerujWyj();

private:
        QTimer *timer_LOff;
        QTimer *timer_obecnosc;
        QTimer *action;
        QTimer *ping;

  signals:
        void changes();
        void all_off_();
        void gate();

  public slots:
        void readyRead();
        void lightsOff();
        void obecnosc_none();
        void simulation(bool on);
        void simulation_holder();
        void random_off();
        void ping_slot();
};
#endif // MYUDP_H
