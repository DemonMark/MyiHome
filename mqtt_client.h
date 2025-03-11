#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <QObject>
#include <QMqttClient>
#include <QStringLiteral>

class mqtt_client : public QMqttClient
{
    Q_OBJECT
public:
    explicit mqtt_client(QString name, QString topic, QObject *parent = nullptr);
    virtual ~mqtt_client();

    void sub(const QString topic);

signals:

    void msg(const QByteArray);

public slots:

};

#endif // MQTT_CLIENT_H
