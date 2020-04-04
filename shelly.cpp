#include "shelly.h"

QByteArray plugsocket;
QByteArray psData;
extern bool jestem;

shelly::shelly(QWidget *parent) : QPushButton(parent)
{
    shellsock = new QUdpSocket(this);

    QTimer *schonline = new QTimer (this);
    connect(schonline, SIGNAL(timeout()), this, SLOT(answer()));
    schonline->start(10000);
}

void shelly::answer(){

    plugsocket[0]=0x53;
    plugsocket[1]=0x02; //bram zmiany stanu przekaznika
    psData.clear();
    psData.append(plugsocket);
    shellsock->writeDatagram(psData,QHostAddress("192.168.1.105"),4210);
    QTimer::singleShot(9990, this, SLOT(offline()));

}

void shelly::mousePressEvent(QMouseEvent *ev){

    if(plugsocket[1]&0x01){
        plugsocket[1]=0x00;
    }else{
        plugsocket[1]=0x01;
    }
    psData.clear();
    psData.append(plugsocket);
    shellsock->writeDatagram(psData,QHostAddress("192.168.1.105"),4210);

}

void shelly::offline(){
    jestem=false;
}
