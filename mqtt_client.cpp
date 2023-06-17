#include "mqtt_client.h"

mqtt_client::mqtt_client(QObject *parent) :
    QObject(parent)
{
    //subscriber = new QMQTT::Client(QHostAddress::LocalHost, 1883);
    //subscriber->connectToHost();
    iHome_mqtt = new QMqttClient(this);
    iHome_mqtt->setHostname("LocalHost");
    iHome_mqtt->setPort(1883);
    iHome_mqtt->connectToHost();

    connect(iHome_mqtt, &QMqttClient::connected, [=](){
        iHome_mqtt->subscribe(QMqttTopicFilter("RPi4"));
    });

    connect(iHome_mqtt, &QMqttClient::messageReceived, [=](const QByteArray &message){
            //qDebug() << "MESSAGE FROM LAMBDA: " << QString::fromUtf8(message);
            emit msg(QString::fromUtf8(message));
    });

}

void mqtt_client::publish(QString vname)
{
    //const QMQTT::Message msg(1,"alarm", vname.toUtf8());
    //subscriber->publish(msg);
}

mqtt_client::~mqtt_client()
{}
