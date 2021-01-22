#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <QObject>
#include "qmqtt.h"

class mqtt_client : public QObject
{
    Q_OBJECT
public:
    explicit mqtt_client(QObject *parent = 0);
    virtual ~mqtt_client();

    QMQTT::Client *subscriber;

signals:

    void msg(QString msg);

};

#endif // MQTT_CLIENT_H
