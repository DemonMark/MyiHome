#include "zdarzenie.h"
#include "myudp.h"
#include <QUdpSocket>
extern int q,ram;

Zdarzenie:: Zdarzenie()
{
    timer = new QTimer (this);
}

void Zdarzenie:MySlot()

{
    MyUDP client;
    ram=1;
    client.WYSUDP();
}

