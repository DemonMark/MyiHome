#ifndef ZDARZENIE_H
#define ZDARZENIE_H

#include <QObject>
#include <QtCore>


class Zdarzenie : public QObject
{

     Q_OBJECT
    public:
         Zdarzenie();
         QTimer *timer;

public slots:
         void MySlot();


};

#endif // ZDARZENIE_H
