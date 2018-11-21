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
#include "myudp.h"
#include "zdarzenie.h"
#include "mytimer.h"


extern int q,ram,brakram;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MyUDP client;
    MainWindow w;
    w.show();
    w.readscheduler();
    client.zerujWyj();
    QObject::connect(&client, SIGNAL(changes()), &w, SLOT(receiving()));
  return a.exec();
}
