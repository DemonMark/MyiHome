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
#include "mqtt_client.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //Set the app style sheet
    QFile style_sheet_file("./style_sheet.qss");
    if(style_sheet_file.open(QFile::ReadOnly)){
        QString style_sheet = QLatin1String(style_sheet_file.readAll());
        qDebug() << "styl wczytany";
        a.setStyleSheet(style_sheet);
    }

    MyUDP client;
    mqtt_client mosquitto_client;
    MainWindow w;
    w.show();
    w.readscheduler();
    QObject::connect(&client, SIGNAL(changes()), &w, SLOT(receiving()));
    QObject::connect(&client, SIGNAL(hex_comming()), &w, SLOT(btnFinderfromHEX()));
    //QObject::connect(&client, SIGNAL(changes()), &pirek, SLOT(naruszeniestrefy()));
    QObject::connect(&w,SIGNAL(simulating(bool)), &client, SLOT(simulation(bool)));
    QObject::connect(&client, SIGNAL(all_off_()), &w, SLOT(LOff()));
    QObject::connect(&client, SIGNAL(gate()), &w, SLOT(wyjezdzam()));
    QObject::connect(&w,SIGNAL(UDP_ReadytoSend(QString)), &client, SLOT(WYSUDP(QString)));
    QObject::connect(&mosquitto_client, SIGNAL(msg(QString)), &w, SLOT(mqtt_processor(QString)));
    //QObject::connect(&pirek, SIGNAL(violation_name(QString)), &mosquitto_client, SLOT(publish(QString)));

  return a.exec();
}
