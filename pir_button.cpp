#include "pir_button.h"
#include "ui_mainwindow.h"

extern bool locked;
#define baza "/media/HDD2/Moje projekty/MyiHome/scene.db"

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

    bool state;
    QString code_PIRname, pir_name;


    p_mw = MainWindow::getMainWinPtr();
    if(p_mw!=nullptr){
        QDial *pir_dial = p_mw->findChild<QDial*>("pir_dial");

        //pir_dial->setParent(this->parentWidget());
        //pir_dial->setGeometry(130,32,90,100);
        pir_dial->setProperty("pir_name", this->objectName());
        code_PIRname = pir_dial->property("pir_name").toString();

        if(pir_dial->isVisible()==true && pir_dial->property("old_pir_name").toString()==code_PIRname){
            state = false;
        }else{
            state = true;
            pir_dial->setProperty("old_pir_name", code_PIRname);
        }

        mydbs read_value(baza);
        QSqlQuery *qry = read_value.query();
        qry->prepare("SELECT timer_time, nazwa FROM PIR WHERE code_name='"+code_PIRname+"'");
        if(qry->exec()){
            qry->next();
            pir_dial->setValue((qry->value("timer_time").toInt())/1000);
            pir_name = qry->value("nazwa").toString();
        }
        p_mw->show_item(state, this->parentWidget(), pir_name);
        delete qry;
    }
}
