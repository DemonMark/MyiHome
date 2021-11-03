#include "pir_button.h"
#include "ui_mainwindow.h"

extern QList<QDial*> dpList;
extern bool locked;

pir_button::pir_button(QWidget *parent) :
    QPushButton(parent)
{
    present = new QTimer;
    connect(present, &QTimer::timeout, [=](){
        c_time--;
        label_countdown->setText(QDateTime::fromTime_t(c_time).toUTC().toString("mm:ss"));
        if(c_time==0){
            label_countdown->setStyleSheet("QLabel { background-color: transparent;"
                                           "font: 75 7pt 'Piboto Condensed';"
                                           "color: rgb(76, 76, 76);}");
            this->setChecked(false);
            present->stop();
        }
    });

    connect(this, SIGNAL(klik()), this, SLOT(setdial()));
}

void pir_button::mousePressEvent(QMouseEvent *ev){

    emit klik();

}

void pir_button::naruszeniestrefy(int &time){

    p_mw = MainWindow::getMainWinPtr();
    label_countdown = p_mw->findChild<QLabel*>("label_" + this->objectName());
    label_countdown->setStyleSheet("QLabel { background-color: transparent;"
                                   "font: 75 7pt 'Piboto Condensed';"
                                   "color: rgb(0, 0, 255);}");
    c_time=time/1000;
    present->start(1000);
}

void pir_button::setdial(){

    p_mw = MainWindow::getMainWinPtr();
    if(p_mw!=nullptr){
        QDial *pir_dial = p_mw->findChild<QDial*>("dial_" + this->objectName());
        foreach(QDial *dial, dpList){
            if(dial->objectName()!=pir_dial->objectName()){
                dial->setVisible(false);
            }else{
                if(pir_dial->isVisible()){
                    pir_dial->setVisible(false);
                }else{
                    pir_dial->setVisible(true);
                }
                p_mw->show_item(pir_dial->isVisible());
            }
        }
    }
}
