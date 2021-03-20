#include "mytimer.h"
#include "myudp.h"

MyTimer::MyTimer(QObject *target, const char *slot, QString t_name)
{
    QTimer *timer_name = new QTimer;
    timer_name->setObjectName(t_name);
    timer_name->setParent(target);
    QObject::connect(timer_name, SIGNAL(timeout()), target, slot);
}
void MyTimer::TimerStart()
{

   //tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("Form", "Tab 1", 0,
}
