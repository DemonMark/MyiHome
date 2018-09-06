#ifndef MYTIMER_H
#define MYTIMER_H

#include <QTimer>
#include <QObject> //+

class MyTimer : public QObject
{
    Q_OBJECT

public:
    explicit MyTimer(QObject *parent = 0);
    QTimer *timerConf;
    void TimerStart();

public slots:
    void MyTimerSlot1();
};

#endif // MYTIMER_H
