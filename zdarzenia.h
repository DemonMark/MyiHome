#ifndef ZDARZENIE_H
#define ZDARZENIE_H
#include <QObject>


class Zdarzenie : public QObject
{
    Q_OBJECT

public:
    explicit Zdarzenie(QObject  *parent = 0);
    int op;
    void WYKONAJ();

private:

};

#endif // ZDARZENIE_H
