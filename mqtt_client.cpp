#include "mqtt_client.h"

mqtt_client::mqtt_client(QObject *parent) :
    QObject(parent)
{
    subscriber = new QMQTT::Client(QHostAddress::LocalHost, 1883);
    subscriber->connectToHost();

    QObject::connect(subscriber, &QMQTT::Client::connected, [this](){
        subscriber->subscribe("RPi4");
    });

    QObject::connect(subscriber, &QMQTT::Client::received, [=](const QMQTT::Message& message){
            qDebug() << "MESSAGE FROM LAMBDA: " << QString::fromUtf8(message.payload());
            emit msg(QString::fromUtf8(message.payload()));
    });

}

mqtt_client::~mqtt_client()
{}
