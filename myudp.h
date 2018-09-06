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
        void WYSUDP();
        QUdpSocket *socket;
        void custom_timer(int duration, QObject *target, const char *slot); //+
        void zerujWyj();

private:
        QTimer *timer_LOff;
        QTimer *timer_obecnosc;
        QTimer *timer_lazienka;

  signals:

  public slots:
        void readyRead();
        void lightsOff();
        void obecnosc_none();
        void Slott(); //+
};
#endif // MYUDP_H
