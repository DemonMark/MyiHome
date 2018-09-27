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
        void zerujWyj();

private:
        QTimer *timer_LOff;
        QTimer *timer_obecnosc;

  signals:
        void changes();

  public slots:
        void readyRead();
        void lightsOff();
        void obecnosc_none();
};
#endif // MYUDP_H
