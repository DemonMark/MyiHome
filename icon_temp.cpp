#include "icon_temp.h"

#define baza "/media/HDD2/Moje projekty/MyiHome/scene.db"

icon_temp::icon_temp(QWidget *parent) :
    QLabel(parent)
{

}

void icon_temp::mousePressEvent(QMouseEvent *ev)
{
    bool status = this->property("status").toBool();
    bool active = this->property("ON").toBool();
    tp_mw = MainWindow::getMainWinPtr();
    QPushButton *p_th_master = tp_mw->findChild<QPushButton*>("th_master");
    QLabel *p_th_priority = tp_mw->findChild<QLabel*>("th_priority");
    p_th_master->setParent(tp_mw);
    p_th_master->setGeometry(this->x()-12,this->y()+3,15,35);
    p_th_master->setProperty("th", this->objectName());
    p_th_master->setChecked(status);
    p_th_master->setVisible(!p_th_master->isVisible());
    p_th_priority->setParent(tp_mw);
    p_th_priority->setGeometry(this->x()-58,this->y()+3,35,35);
    p_th_priority->setVisible(!p_th_priority->isVisible());
/*
    if(status==true){
        p_th_master->setStyleSheet("QPushButton {background-image: url(:/HDD1/admin/iHome/28-02-2018/media/thermo_"+QVariant(active).toString()+".png);"
                                                                                                                                                "background-repeat: none;"
                                                                                                                                                "background-color: transparent;"
                                                                                                                                                "border: none;}");
    }else{

    }*/
}
