#include "shelly.h"

QByteArray plugsocket;
QByteArray psData;
QDateTime mute_timer;

shelly::shelly(QWidget *parent) : QPushButton(parent)
{
    sp_mw = MainWindow::getMainWinPtr();

    shellsock = new QUdpSocket(this);
    QTimer *schonline = new QTimer (this);

    connect(schonline, SIGNAL(timeout()), this, SLOT(answer()));
    schonline->start(10000);

    connect(this, &shelly::toggled, [=](bool checked){
        open(checked);
    });

    connect(this, &shelly::Relay, [=](bool ON){
        if(ON){
            this->setIcon(this->property("on").value<QIcon>());
        }else if(this->property("mute")==0){
            this->setIcon(this->property("off").value<QIcon>());
        }
    });
}

void shelly::answer(){

    plugsocket[0]=0x53;
    plugsocket[1]=0x04; //brak zmiany stanu przekaznika
    psData.clear();
    psData.append(plugsocket);
    shellsock->writeDatagram(psData,QHostAddress(this->accessibleName()),4210);
    QTimer::singleShot(9000, [=](){
        QLabel *rsi_label = sp_mw->findChild<QLabel*>("rsi_shelly_" + this->objectName().split("_")[1]);
        if(rsi_label!=nullptr){rsi_label->setText("--");}
    });

}

void shelly::mousePressEvent(QMouseEvent *ev)
{
    mute_timer = QDateTime::currentDateTime();
    emit SHELLY_CLICKED();
    if(this->isCheckable()){
        this->setChecked(!this->isChecked());
    }else{
        open(this->accessibleDescription().toInt());
    }
    if(this->property("MQTT").toBool()){
        QMqttClient *shelly_c = sp_mw->findChild<QMqttClient*>("FAAC");
        if(shelly_c){
            qDebug() << shelly_c->objectName();
            shelly_c->publish(QMqttTopicName(this->property("mqtt_topic").toString()), this->property("mqtt_pyload").toByteArray());
        }
    }
}

void shelly::mouseReleaseEvent(QMouseEvent *ev)
{
    if(mute_timer.secsTo(QDateTime::currentDateTime()) >=2){
        emit TIMER();
    }
}

void shelly::open(int state){

    plugsocket[1]=state;
    psData.clear();
    psData.append(plugsocket);
    shellsock->writeDatagram(psData,QHostAddress(this->accessibleName()),4210);
    qDebug() << plugsocket.toHex();
}
