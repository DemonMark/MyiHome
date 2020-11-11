#include "myudp.h"
#include <QtCore/QCoreApplication>
#include <QApplication>
#include <QtCore/QDateTime>
#include <QDateTime>
#include <QDateTimeEdit>
#include <QString>
#include <QDebug>
#include "mainwindow.h"
#include <QTabWidget>
#include "ui_mainwindow.h"
#include "zdarzenie.h"
#include "mytimer.h"
#include "mydbs.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MyUDP client;
    pir_button pirek;
    MainWindow w;
    w.show();
    w.readscheduler();
    client.zerujWyj();
    QObject::connect(&client, SIGNAL(changes()), &w, SLOT(receiving()));
    QObject::connect(&client, SIGNAL(changes()), &pirek, SLOT(naruszeniestrefy()));
    QObject::connect(&w,SIGNAL(simulating(bool)), &client, SLOT(simulation(bool)));
    QObject::connect(&w,SIGNAL(all_off()), &client, SLOT(lightsOff()));
    QObject::connect(&client, SIGNAL(all_off_()), &w, SLOT(LOff()));
    QObject::connect(&client, SIGNAL(gate()), &w, SLOT(wyjezdzam()));
  return a.exec();
}
