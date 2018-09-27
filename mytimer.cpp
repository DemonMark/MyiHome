#include "mytimer.h"
#include "myudp.h"
extern QList<QTimer*> scheduledtimers;
extern int gn;

MyTimer::MyTimer(QObject *target, const char *slot)
{
    QTimer *timer_name = new QTimer;
    QObject::connect(timer_name, SIGNAL(timeout()), target, slot);
    scheduledtimers.insert(gn, timer_name);
    //qDebug() << timer_name->timerId() << timer_name << "GN:" << gn;
}
void MyTimer::TimerStart()
{

   //tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("Form", "Tab 1", 0,
}
