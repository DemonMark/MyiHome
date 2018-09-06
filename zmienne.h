#ifndef ZMIENNE_H
#define ZMIENNE_H

#include <QString>

class Singleton {

public:
    QString ciapek;

private:
    Singleton();
    Singleton (const Singleton&);


    friend Singleton& Zmienne();
};



Singleton& Zmienne();
#endif // ZMIENNE_H
