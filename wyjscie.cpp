#include "wyjscie.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    timerId= startTimer(10);                //czas timera 10ms

    /******************************ZEGAR************************/
    QTimer *timer =new QTimer(this);
    connect (timer,SIGNAL(timeout()),this,SLOT (showTime()));

    timer->start();
    QDateTime dateTime=QDateTime::currentDateTime();

    QString datetimetext= dateTime.toString("dd/MM/yyyy");
    ui->datatime->setText(datetimetext);

}
