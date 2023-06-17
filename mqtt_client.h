#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <QObject>
#include <QMqttClient>

class mqtt_client : public QObject
{
    Q_OBJECT
public:
    explicit mqtt_client(QObject *parent = 0);
    virtual ~mqtt_client();

    QMqttClient *iHome_mqtt;

signals:

    void msg(QString msg);

public slots:

    void publish(QString vname);

};

#endif // MQTT_CLIENT_H
