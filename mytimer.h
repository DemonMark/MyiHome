#ifndef MYTIMER_H
#define MYTIMER_H

#include <QTimer>
#include <QObject>

class MyTimer : public QObject
{
    Q_OBJECT

public:
    //explicit MyTimer(QObject *parent = 0);
    explicit MyTimer(QObject *target, const char *slot);
    void TimerStart();

public slots:

};

#endif // MYTIMER_H
