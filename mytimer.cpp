#include "mytimer.h"
#include "myudp.h"
#include <QDebug>
extern unsigned char maskawysl[10];
extern int c7;

MyTimer::MyTimer(QObject *parent):
    QObject (parent)
{

    // create a timer
    timerConf = new QTimer(this);

    // setup signal and slot
    connect(timerConf, SIGNAL(timeout()), this, SLOT(MyTimerSlot1())) ;
    timerConf->start(5000);
    qDebug() << "TIMER STWORZONY" << timerConf->timerId();
    qDebug() << timerConf->isActive();

}
void MyTimer::TimerStart()
{
    //timerConf = new QTimer(this);
    //connect(timerConf, SIGNAL(timeout()),this, SLOT(MyTimerSlot()));
    // msec
    //timerConf->start(5000);
    qDebug() <<"*******POSZLO*************";

   // tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("Form", "Tab 1", 0,
}

void MyTimer::MyTimerSlot1()
{
        qDebug() << "TIMER ZAKONCZONY";
        timerConf->stop();

}
