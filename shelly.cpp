#include "shelly.h"

QByteArray plugsocket;
QByteArray psData;
QDateTime mute_timer;

static mqtt_client *mqtt_shelly = nullptr;

shelly::shelly(QWidget *parent) : QPushButton(parent)
{
    if(!mqtt_shelly){
        mqtt_shelly = new mqtt_client("FAAC", "", this);
    }

    QTimer::singleShot(500, this, [&](){
        mqtt_shelly->sub(this->property("mqtt_topic_sub").toString());
        connect(mqtt_shelly, &mqtt_client::msg, this, &shelly::msg_process);
    });

    sp_mw = MainWindow::getMainWinPtr();

    shellsock = new QUdpSocket(this);
    QTimer *schonline = new QTimer (this);

    connect(schonline, SIGNAL(timeout()), this, SLOT(answer()));
    schonline->start(10000);

    connect(this, &shelly::toggled, [=](bool checked){
        open(checked);
    });

    connect(this, &shelly::Relay, [=](bool ON){
        QByteArray D = (QVariant(ON).toString()).toLocal8Bit();
        const char *V = D.data();
        this->setIcon(this->property(V).value<QIcon>());
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
            mqtt_shelly->publish(QMqttTopicName(this->property("mqtt_topic_pub").toString()), this->property("mqtt_pyload").toByteArray());
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

void shelly::msg_process(const QByteArray message)
{
    QJsonParseError parseError;
    QJsonDocument JS_message = QJsonDocument::fromJson(message, &parseError);

    if(parseError.error == QJsonParseError::NoError){
        QJsonObject JSO = JS_message.object();

        if(this->property("IP").toString() == JSO.value("IP").toString()){
            if(JSO.contains("INPUT") && (JSO.value("INPUT").toInt() != -1)){ //dziwne zachowaie SHELLY przy stracie sygnału WiFi generuje INPUT o wartości -1
                qDebug() << JSO << "INPUT " << JSO.value("STATUS").toInt();
                (this->*SIGNALPOINTERS[JSO.value("INPUT").toInt()])(JSO.value("STATUS").toInt());
            }
            if(JSO.contains("SIGNAL_STRENGHT")){
                QLabel *rsi_label = sp_mw->findChild<QLabel*>("rsi_shelly_" + JSO.value("IP").toString().mid(10,3));
                if(rsi_label!=nullptr){
                    rsi_label->setText(QString::number(JSO.value("SIGNAL_STRENGHT").toInt()) + "%");
                    //mqtt_shelly->publish(QMqttTopicName(this->property("mqtt_topic_pub").toString()), this->property("mqtt_presence").toByteArray());
                }
            }
            mqtt_shelly->publish(QMqttTopicName(this->property("mqtt_topic_pub").toString()), this->property("mqtt_presence").toByteArray());
        }
    }
}
