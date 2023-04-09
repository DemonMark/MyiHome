#include "shelly.h"

QByteArray plugsocket;
QByteArray psData;
int val = 3;

shelly::shelly(QWidget *parent) : QPushButton(parent)
{
    sp_mw = MainWindow::getMainWinPtr();
    shellsock = new QUdpSocket(this);
    mute_counter = new QTimer(this);
    QTimer *schonline = new QTimer (this);

    connect(schonline, SIGNAL(timeout()), this, SLOT(answer()));
    schonline->start(10000);

    connect(this, &shelly::toggled, [=](bool checked){
        if(checked){
            open(this->accessibleDescription().toInt());
        }else{
            open(0);
        }
    });
    connect(this, &shelly::Relay, [=](bool ON){
        if(ON){
            this->setIcon(this->property("on").value<QIcon>());
        }else{
            this->setIcon(this->property("off").value<QIcon>());
        }
    });

    connect(mute_counter, &QTimer::timeout, [=](){

        if(++counter==2){
            if(label_cd!=nullptr){
                this->setProperty("mute", 1);
                this->setIcon(this->property("mute_icon").value<QIcon>());
                mute_counter_val = sp_mw->findChild<QSpinBox*>("dzwonek_t");
                mute_ind->setPixmap(mute_ind->property("mute_on").value<QPixmap>());
                mute_ind->setVisible(true);
                val = mute_counter_val->value()*60;
            }
        }
        if(counter<2 && this->property("mute").toInt()==1){
            this->setProperty("mute", 0);
            this->setIcon(this->property("off").value<QIcon>());
            if(label_cd!=nullptr){
                label_cd->setVisible(false);
                mute_ind->setPixmap(mute_ind->property("mute_off").value<QPixmap>());
                mute_ind->setVisible(true);
                QTimer::singleShot(1000, [=](){
                    mute_ind->setVisible(false);
                });
            }
            mute_counter->stop();
        }
        if(counter>=2 && this->property("mute").toInt()==1){
            if(label_cd!=nullptr){
                label_cd->setText(QDateTime::fromTime_t(val-counter).toUTC().toString("mm:ss"));
                label_cd->setVisible(true);
            }
        }
        if(counter>=val && this->property("mute").toInt()==1){
            this->setProperty("mute", 0);
            label_cd->setVisible(false);
            mute_counter->stop();
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
        QTimer::singleShot(10000, [=](){
            QLabel *rsi_label = sp_mw->findChild<QLabel*>("rsi_shelly_" + this->objectName().split("_")[1]);
            if(rsi_label!=nullptr){rsi_label->setText("--");}
        });
    }
}

void shelly::mousePressEvent(QMouseEvent *ev){

    //sp_mw = MainWindow::getMainWinPtr();
    label_cd = sp_mw->findChild<QLabel*>("label_" + this->objectName());
    mute_ind = sp_mw->findChild<QLabel*>("label_mute");
    counter = 0;
    if(this->property("mute").toInt()==1){
        mute_counter->start(300);
    }else{
        mute_counter->start(1000);
    }
    if(this->isCheckable()){
        this->setChecked(!this->isChecked());
    }
}
void shelly::mouseReleaseEvent(QMouseEvent *ev){

    if(this->property("mute").toInt()!=1 && !this->isCheckable()){
        mute_counter->stop();
        open(this->accessibleDescription().toInt());
    }else if(this->property("mute").toInt()==1 && label_cd!=nullptr){
        mute_ind->setVisible(false);
    }
}

void shelly::open(int state){

    plugsocket[1]=state;
    psData.clear();
    psData.append(plugsocket);
    shellsock->writeDatagram(psData,QHostAddress(this->accessibleName()),4210);
    qDebug() << plugsocket.toHex();
}
