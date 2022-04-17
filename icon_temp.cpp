#include "icon_temp.h"

#define baza "/media/HDD2/Moje projekty/MyiHome/scene.db"

icon_temp::icon_temp(QWidget *parent) :
    QLabel(parent)
{
    tmp_counter = new QTimer(this);

    connect(tmp_counter, &QTimer::timeout, [=](){
        temp_counter++;
    });
}

void icon_temp::mousePressEvent(QMouseEvent *ev)
{
    temp_counter = 0;
    tmp_counter->start(500);
}

void icon_temp::mouseReleaseEvent(QMouseEvent *ev)
{
    tmp_counter->stop();
    if(temp_counter>2){
        this->setProperty("status", !this->property("status").toBool());
        int status = this->property("status").toInt();
        this->setPixmap(QPixmap("/media/HDD1/admin/iHome/28-02-2018/media/thermo_" + QString::number(status) + ".png"));
        mydbs read_value(baza);
        QSqlQuery *qry = read_value.query();
        qry->prepare("UPDATE temperature SET aktywna=? WHERE th='"+this->objectName()+"'");
        qry->addBindValue(status);
        qry->exec();
        delete qry;
    }
}
