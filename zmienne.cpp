#include "zmienne.h"

Singleton::Singleton()
{

};

Singleton& Zmienne() {

    static Singleton zmienne;
    return zmienne;
};
