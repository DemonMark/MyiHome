#include "pir_button.h"
#include "ui_mainwindow.h"
extern QString time_text;
extern QList<QPushButton*> pirList;
extern QList<QDial*> dpList;
extern unsigned char temp[20];
extern unsigned char hexx[9];
extern QString ips;
QList<QTimer*> ptimers;
extern int spimy; // tylko do celów sprawdzenia ruchu w nocy - do wykasowania po analizie

pir_button::pir_button(QWidget *parent) :
    QPushButton(parent)
{
    QTimer *present = new QTimer;
    connect(present, SIGNAL(timeout()), this, SLOT(absent()));
    ptimers.append(present);
    connect(this, SIGNAL(klik()), this, SLOT(setdial()));
}

void pir_button::mousePressEvent(QMouseEvent *ev){

    emit klik();
    //MainWindow *mw = MainWindow::getMainWinPtr();

}

void pir_button::naruszeniestrefy(){
    if(ips=="192.168.1.103"){
        for(int i=1; i<=pirList.length(); i++){
            if(hexx[i] & temp[0]){
                pirList.at(i-1)->setChecked(true);
                ptimers.at(i)->setSingleShot(true);
                ptimers.at(i)->start(300000);
                if (spimy==1){
                    qDebug() << time_text << pirList.at(i-1)->objectName();
                }
            }
        }
    }
}

void pir_button::setdial(){

    for(int i=0; i<=(pirList.length()-1); i++){
        if(pirList.at(i)==QObject::sender()){
            if(dpList.at(i)->isVisible()){
                dpList.at(i)->setVisible(false);
            }else{
                dpList.at(i)->setVisible(true);

            }
        }else{
            dpList.at(i)->setVisible(false);
        }
    }
}

void pir_button::absent()
{
    for(int i=1; i<=pirList.length();i++){
        if(ptimers.at(i)==QObject::sender()){
            pirList.at(i-1)->setChecked(false);
        }
    }
}
