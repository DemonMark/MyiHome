#include "shelly.h"

QByteArray plugsocket;
QByteArray psData;
QString csname;

shelly::shelly(QWidget *parent) : QPushButton(parent)
{
    shellsock = new QUdpSocket(this);
    QTimer *auto_open = new QTimer(this);
    QTimer *schonline = new QTimer (this);

    connect(schonline, SIGNAL(timeout()), this, SLOT(answer()));
    schonline->start(10000);

    connect(this, &shelly::toggled, [=](bool checked){
        if(checked){
            connect(auto_open, SIGNAL(timeout()), this, SLOT(open()));
            auto_open->start(20000);
        }else{
            auto_open->stop();
            disconnect(auto_open, SIGNAL(timeout()), this, SLOT(open()));
            csname.clear();
        }
    });
}

void shelly::answer(){
    if(this->autoExclusive()==false){ //dodano w celu eliminacji podwójnego zgłoszenia w przypadku kilku ikon dla jednego Shelly (przypadek brama wjazdowa)
        plugsocket[0]=0x53;
        plugsocket[1]=0x04; //brak zmiany stanu przekaznika
        psData.clear();
        psData.append(plugsocket);
        shellsock->writeDatagram(psData,QHostAddress(this->accessibleName()),4210);
        QTimer::singleShot(10000, this, SLOT(offline()));
    }
}

void shelly::mousePressEvent(QMouseEvent *ev){
    open();
    if(this->isCheckable()){
        csname = this->objectName();
    }
}

void shelly::offline(){
    this->setIcon(QIcon(this->statusTip()));
}

void shelly::open(){
    bool ok;
    plugsocket[1]=this->accessibleDescription().toInt(&ok);
    psData.clear();
    psData.append(plugsocket);
    shellsock->writeDatagram(psData,QHostAddress(this->accessibleName()),4210);
    qDebug() << plugsocket.toHex();
}
