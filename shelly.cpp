#include "shelly.h"

QByteArray plugsocket;
QByteArray psData;
int val = 3;

shelly::shelly(QWidget *parent) : QPushButton(parent)
{
    shellsock = new QUdpSocket(this);
    mute_counter = new QTimer(this);
    QTimer *auto_open = new QTimer(this);
    QTimer *schonline = new QTimer (this);
    qDebug() << &counter;
    connect(schonline, SIGNAL(timeout()), this, SLOT(answer()));
    schonline->start(10000);

    connect(auto_open, &QTimer::timeout, [=](){
        open(this->accessibleDescription().toInt());
    });

    connect(this, &shelly::toggled, [=](bool checked){
        if(checked){
            open(this->accessibleDescription().toInt());
            auto_open->start(20000);
        }else{
            open(0);
            auto_open->stop();
        }
    });

    connect(mute_counter, &QTimer::timeout, [=](){

        if(++counter==3){
            if(label_cd!=nullptr){
                this->setProperty("mute", 1);
                this->setIcon(this->property("mute_icon").value<QIcon>());
                mute_counter_val = sp_mw->findChild<QSpinBox*>("dzwonek_t");
                mute_ind->setPixmap(mute_ind->property("mute_on").value<QPixmap>());
                mute_ind->setVisible(true);
                val = mute_counter_val->value()*60;
            }
        }
        if(counter<3 && this->property("mute").toInt()==1){
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
        if(counter>=3 && this->property("mute").toInt()==1){
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
        QTimer::singleShot(10000, this, SLOT(offline()));
    }
}

void shelly::mousePressEvent(QMouseEvent *ev){

    sp_mw = MainWindow::getMainWinPtr();
    label_cd = sp_mw->findChild<QLabel*>("label_" + this->objectName());
    mute_ind = sp_mw->findChild<QLabel*>("label_mute");
    counter = 0;
    if(this->property("mute").toInt()==1){
        mute_counter->start(300);
    }else{
        mute_counter->start(1000);
    }
    if(this->isCheckable()){
        if(this->isChecked()){
            this->setChecked(false);
        }else{
            this->setChecked(true);
        }
    }
}
void shelly::mouseReleaseEvent(QMouseEvent *ev){

    if(this->property("mute").toInt()!=1 && !this->isCheckable()){
        mute_counter->stop();
        open(this->accessibleDescription().toInt());
    }else if(this->property("mute").toInt()==1 && label_cd!=nullptr){
        qDebug() << "jestem tu";
        mute_ind->setVisible(false);
    }
}

void shelly::offline(){
    if(!this->isChecked()){
        this->setIcon(this->property("offline").value<QIcon>());
    }
}

void shelly::open(int state){
    //bool ok;
    plugsocket[1]=state;
    psData.clear();
    psData.append(plugsocket);
    shellsock->writeDatagram(psData,QHostAddress(this->accessibleName()),4210);
    qDebug() << plugsocket.toHex();
}
