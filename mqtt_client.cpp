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

    connect(this, &QMqttClient::messageReceived, [=](const QByteArray &message){
        qDebug() << "MESSAGE FROM LAMBDA: " << message;
        emit msg(QString::fromUtf8(message));
    });
}

void mqtt_client::publish(QString pMessage, QString pTopic)
{
    const QString msg = "{'ID':'1'}";
    const QString topic = "FAAC_CONTROL";
    //iHome_mqtt->publish(QMqttTopicName(topic),msg.toUtf8(), 0, false);
    //quint32 mm = iHome_mqtt->publish(topic, msg.toUtf8(), 0, false);
    //qDebug() << mm;
}

mqtt_client::~mqtt_client()
{}
