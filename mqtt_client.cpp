#include "mqtt_client.h"


mqtt_client::mqtt_client(QString name, QString topic, QObject *parent) :
    QMqttClient(parent)
{
    this->setObjectName(name);
    this->setHostname("LocalHost");
    this->setPort(1883);
    this->connectToHost();

    connect(this, &QMqttClient::connected, [=](){
        this->subscribe(QMqttTopicFilter(topic));
    });
    connect(this, &QMqttClient::messageReceived, [=](const QByteArray message){
        qDebug() << "MESSAGE FROM LAMBDA: " << message;
        emit msg(message);
    });
}

void mqtt_client::sub(const QString topic)
{
    connect(this, &QMqttClient::connected, [=](){
        qDebug() << "SHELLY CONNECTED TO MQTT TO " << topic;
        this->subscribe(QMqttTopicFilter(topic));
    });
}

mqtt_client::~mqtt_client()
{}
