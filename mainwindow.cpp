#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "myudp.h"
#include "zdarzenie.h"
#include "pir_button.h"
#include "shelly.h"
#include "mydbs.h"
#include <QtGui>
#include <QLCDNumber>
#include <QUdpSocket>
#include <qabstractbutton.h>
#include <QTimer>
#include "QDebug"
#include <QSlider>
#include <QDateTime>
#include "mytimer.h"
#include <QtCore/QCoreApplication>
#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QStyle>
#include <QPixmap>
#include <QList>
#include <QIcon>
#include <QtWebKit/QtWebKit>
#include "bcm2835.h"

#define GEAR1 RPI_V2_GPIO_P1_16
#define GEAR2 RPI_V2_GPIO_P1_18
#define PIN_TH RPI_V2_GPIO_P1_07
#define DHT_MAXCOUNT 35000 //liczba dobrana do szybkości RPi2 - dotyczy max czasu odczytu powyżej TIMEOUT
#define DHT_BITS 41
#define DC '\xb0' //znak stopnia
QList<QLabel*> c_e;
QList<QPushButton*> bList; //lista przycisków oświetlenia
QList<QPushButton*> sbList; //lista przycisków scen
QList<pir_button*> pirList; //lista czujek
QList<QDial*> dList; //lista regulatorów temperatury
QList<QDial*> dpList; //lista regulatorów czujek pir
QList<QLabel*> lList; //lista etykiet temperatur
QList<QLabel*> ldList; //lista opisów regulatorów
QList<shelly*> shList; //Lista modułów wifi Shelly
int bPos[48]={31,28,33,32,29,12,1,0,34,30,15,17,8,11,5,10,4,14,7,26,26,26,26,26,19,26,26,26,26,26,26,26,9,16,6,18,2,3,24,13,25,23,27,26,26,26,26,26}; //pozycja przycisku na liście
QList<QList<int> > outmasks; //lista masek dla harmonogramu czujek
QList<QList<int> > scheduledcs; //lista zmiennych "c" harmonogramów
QList<QList<int> > scheduledbtns; //lista przycisków harmonogramów
QList<int> scheduledtime; //lista znaczników dla włączania harmonogramu wg czasu wieczornego lub innego
QList<QString> sunsetTime; //lista czasów zachodu słońca w momencie dodawania zdarzenia harmonogramu
QList<QString> sunriseTime; //lista czasów wschodu słońca w momencie dodawania zdarzenia harmonogramu
QList<int> tspinBox;
QList<int> gnpos, gn2pos;
QList<QString> startat;
QList<QString> stopat;
QList<QString> outnames;
QList<QString> pirnames;
QString time_text;
extern QString ips;
extern QString rssi[5];
extern int c[49];
QList<int> t({0,0,0,0,0,0,0});
int num=0;
extern unsigned char shell[1];
extern unsigned char temp[20];
extern unsigned char temperatura[63];
extern unsigned char hexx[9];
extern QList<unsigned char> scheduledhexxpir;
extern QList<QList<unsigned char> > scheduledhexxout;
extern QList<int> bgi;
extern QList<QTimer*> scheduledtimers;
unsigned char maskawysl[10];
extern int odliczG;
int a=0;
int gn=-1;
int gn2=0;
int qu;
int flaga=0,flaga_1=0,flaga_2=0,flaga_4=0,flaga_5=0,flaga_6=0,flaga_7=0,flaga_8=0,flaga_9=0,flaga_10=0;
int dzien;  //jeśli = 1 czujki nie włączają świateł
int spimy;  //jeśli = 1 odliczanie 10min od detekcji do wyłączenia światła(wyjść)
int obecnosc=0; //jeśli = 1 wyłączenie świateł(wyjść) o ustalonej godzinie nie odpala się
int otwarta=0; //status bramy garażowej
extern int scheduledaction;
QDate dateTime;
QString dateStamptext;
float humidity;
int humidityholder=1;
bool humiditywatcher=false;
bool persistent;
int count_up=0;
int count_down=0;
extern bool simulating_on;
bool scene_active;
bool scene_driving;
bool jestem = true;
bool shelly_on;
bool write_off=false;
extern QByteArray plugsocket;

//MainWindow * MainWindow::pMainWindow = nullptr; //dostep do MainWindow

//inicjalizacja pinów GPIO Raspberry
void initGPIO()
{
    bcm2835_gpio_fsel(GEAR1, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(GEAR2, BCM2835_GPIO_FSEL_OUTP);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    bcm2835_init();
    initGPIO();

    ui->setupUi(this);
    timerId = startTimer(500);
     /******************************ZEGAR************************/
    QTimer *timer = new QTimer(this);
    connect (timer,SIGNAL(timeout()),this,SLOT (showTime()));
    timer->start();
    bList = ui->tab_5->findChildren<QPushButton*>(QRegExp ("pushButton*")) + ui->tab_2->findChildren<QPushButton*>(QRegExp ("pushButton*"));
    //bList = MainWindow::findChildren<QPushButton*>();
    sbList = MainWindow::findChildren<QPushButton*>(QRegExp ("button_scene*"));
    dList = ui->tab_5->findChildren<QDial*>(QRegExp ("dial_temp_*")) + ui->tab_2->findChildren<QDial*>(QRegExp ("dial_temp_*"));
    dpList = ui->tab_5->findChildren<QDial*>(QRegExp ("dial_pir_*"));
    lList = ui->tab_5->findChildren<QLabel*>(QRegExp ("label_temp_*")) + ui->tab_2->findChildren<QLabel*>(QRegExp ("label_temp_*"));
    ldList = ui->tab_5->findChildren<QLabel*>(QRegExp ("label_dsc_*")) + ui->tab_2->findChildren<QLabel*>(QRegExp ("label_dsc_*"));
    pirList = ui->tab_5->findChildren<pir_button*>();
    shList = ui->tab_5->findChildren<shelly*>(QRegExp ("shelly*"));

    int tmp = (MainWindow::findChildren<QLabel*>(QRegExp ("con_err_*")).count());
    for(int lf=1; lf<=tmp; lf++){
        QString ln=QString::number(lf);
        c_e.append(MainWindow::findChildren<QLabel*>("con_err_"+ln));
    }

    foreach(QPushButton *btn, bList)
    {
        //qDebug() << btn->objectName();
        connect(btn, SIGNAL(clicked()), this, SLOT(ClickedbtnFinder()));
    }

    foreach (QPushButton *sbtn, sbList) {
        connect(sbtn, SIGNAL(toggled(bool)) , this, SLOT(ClickedscenebtnFinder(bool)));
    }

    foreach(QDial* dL, dList){
        connect(dL, SIGNAL(valueChanged(int)), this, SLOT(writescheduler()));
        connect(lList.at(num), SIGNAL(mouse_clicked()), this, SLOT(ClickedlabelFinder()));
        dL->setVisible(false);
        ldList.at(num)->setVisible(false);
        num++;
    }

    foreach(QDial* dpL, dpList){
        dpL->setVisible(false);
        connect(dpL, SIGNAL(valueChanged(int)), this, SLOT(settimers(int)));
    }
    //sprawa do opanowania - dostep do UI z subklasy
    foreach (pir_button* ere, pirList) {
        connect(ere, SIGNAL(klik()), this, SLOT(showui()));
    }

    //***timer styku bramy***//
    timer_bramaStykOff = new QTimer(this);
    connect(timer_bramaStykOff, SIGNAL(timeout()), this, SLOT(stykOff()));

    //PARAMETRY POCZĄTKOWE//

    ui->label_32->setVisible(false);
    ui->label_36->setVisible(false);
    ui->label_47->setVisible(false);
    ui->label_48->setVisible(false);
    ui->dial_12->setValue(1);

    webView = new QWebView(this);
    webView->setVisible(false);
    webView->load(QUrl("https://calendar.zoznam.sk/sunset-pl.php"));
    QObject::connect(webView,SIGNAL(loadFinished(bool)), this, SLOT(readTimeFromWWW()));

    dateTime = QDate::currentDate();
    dateStamptext = dateTime.toString("dd/MM/yyyy");

    movie_pompa_1 = new QMovie("/media/HDD1/admin/iHome/28-02-2018/media/pompa_on.gif");
    movie_pompa_2 = new QMovie("/media/HDD1/admin/iHome/28-02-2018/media/pompa_on.gif");
    movie_cyrkulacja = new QMovie("/media/HDD1/admin/iHome/28-02-2018/media/circulation_on.gif");
    movie_wentylacja = new QMovie("/media/HDD1/admin/iHome/28-02-2018/media/wentylator.gif");
    movie_countdown = new QMovie("/media/HDD1/admin/iHome/28-02-2018/media/countdown.gif");
    ui->label_19->setMovie(movie_countdown);
    ui->label_wentylator->setMovie(movie_wentylacja);
    ui->label_wentylator_2->setMovie(movie_wentylacja);
    movie_wentylacja->start();

    connect(ui->copyButton_8, SIGNAL(toggled(bool)), ui->pushButton_8,SLOT(setChecked(bool))); //wsparcie dla zdublowanego przycisku
    connect(movie_cyrkulacja, SIGNAL(frameChanged(int)), this, SLOT(cyrkulacja_rotation()));
}

MainWindow::~MainWindow()
{
    delete ui;
    killTimer(timerId);
}
//dostep do MainWindow z innych klas
/*MainWindow *MainWindow::getMainWinPtr()
{
    return pMainWindow;
}*/

void MainWindow::showTime(){
    QTime time=QTime::currentTime();
    time_text=time.toString("hh:mm:ss");
    ui->digitalclock->setText(time_text);

//*****************WYŁĄCZANIE WSZYSTKICH WYJŚĆ*****************//
     if (time_text==ui->timeEdit_3->text() && obecnosc==0){
         spimy=1;
         odliczG=1;
         //LOff();
         emit all_off();
         //ui->button_scene_4->setChecked(true);
     }
     else if (time_text==ui->timeEdit_3->text() && obecnosc==1){
         spimy=1;       
         //ui->button_scene_4->setChecked(true);
     }
//****************DZIEŃ LUB NOC*********************************//

     if (time_text>=ui->label_25->text() && time_text<=ui->label_26->text()){
         dzien=1;
         spimy=0;
         //ui->button_scene_4->setChecked(false);
     }
     else {
         dzien=0;
     }

    dateTime=QDate::currentDate();
    QString datetimetext= dateTime.toString("dd/MM/yyyy");
    ui->datatime->setText(datetimetext);

//***********ZMIANA DNIA**************//
    if(datetimetext!=dateStamptext){
        webView->load(QUrl("https://calendar.zoznam.sk/sunset-pl.php"));
        QObject::connect(webView,SIGNAL(loadFinished(bool)), this, SLOT(readTimeFromWWW()));
        dateStamptext = dateTime.toString("dd/MM/yyyy");
    }

//***************WYŚWIETLANIE TIMERA ORAZ AKTYWACJA ODLICZANIA PO NARUSZENIU STREFY W CZASIE NOCNYM****************//
        ui->label_11->setText(QDateTime::fromTime_t(odliczG).toUTC().toString("mm:ss")); //zamiana ms na minuty
        if(spimy==1 && obecnosc==1){
            movie_countdown->start();
            ui->label_19->setStyleSheet("background-image:none");
        }
//***************zarządzanie temperaturą cwu*******************//
        double temp_cwu = ui->label_21->text().split(DC)[0].toDouble();
        switch(obecnosc){
        case 1:
            if(temp_cwu < (ui->cwu_ob->value())){
                ui->cwu->setChecked(true);
            }else {
                ui->cwu->setChecked(false);
            }
            break;
        case 0:
            if(temp_cwu < (ui->cwu_nob->value())){
                ui->cwu->setChecked(true);
            }else {
                ui->cwu->setChecked(false);
            }
        }
}

/************************************************************** ODBIERANIE *********************************************************************/
void MainWindow::timerEvent(QTimerEvent *event){
    //sprawdzanie czy istnieją swiatła do włączenia z harmonogramu
    int i1=0;
    int i2=0;
    foreach(QString start, startat){
        int i=0;
        int j=gnpos.at(i1);
        if(time_text==start){
            foreach(unsigned char r, scheduledhexxout[j]){
                maskawysl[outmasks.value(j)[i]]|=r;
                MyUDP client;
                if(outmasks.value(j)[i]<5){
                    client.WYSUDP("192.168.1.101");
                }else{
                    client.WYSUDP("192.168.1.104");
                }
                c[scheduledcs.value(j)[i]]=1;
                bList.at(scheduledbtns.value(j)[i])->setChecked(true);
                i++;
            }
            qDebug() << "START";
        }
        i1++;
    }
    //sprawdzanie czy istnieją swiatła do wyłączenia z harmonogramu
    foreach(QString stop, stopat){
        int i=0;
        int j=gnpos.at(i2);
        if(time_text==stop){
            foreach(unsigned char r, scheduledhexxout[j]){
                maskawysl[outmasks.value(j)[i]]&=~r;
                MyUDP client;
                if(outmasks.value(j)[i]<5){
                    client.WYSUDP("192.168.1.101");
                }else{
                    client.WYSUDP("192.168.1.104");
                }
                c[scheduledcs.value(j)[i]]=1;
                bList.at(scheduledbtns.value(j)[i])->setChecked(false);
                i++;
            }
            qDebug() << "STOP";
        }
        i2++;
    }
    //nadzorowanie wilgotnosci w lazience
    if(persistent && !humiditywatcher && spimy==0){
        humiditywatcher=true;
        ui->dial_12->setValue(2);
        movie_wentylacja->setSpeed(200);
    }
    if(!persistent && humiditywatcher){
        ui->dial_12->setValue(humidityholder);
        humiditywatcher=false;
        movie_wentylacja->setSpeed(100);
    }
    //*************GEOLOKALIZACJA**************
   QFile comming("/home/pi/GDrive/comming_home.csv");
   if(comming.exists()){
        if(!jestem){
            qDebug() << "EXIST";
            comming.remove();
            QPoint point(0, 413.0);
            ui->pushButton_16->click();
            QMouseEvent *me = new QMouseEvent(QEvent::MouseButtonPress, point, Qt::LeftButton, Qt::LeftButton,  Qt::NoModifier);
            QCoreApplication::sendEvent(ui->_helly25_2,me);
            jestem=true; //flaga wprowadzona ze wzgledu na sprawdzanie obecnosci pliku 2x/s a odswiezanie GDrive co 1st
        }
   }else{
       //qDebug() << "N_EXIST";
       jestem=false;
   }
}

void MainWindow::receiving(){

    /**********ZAMIANA RAMKI Z TEMPERATURA NA QSTRING+WYSWIETLENIE*************/
    QString b,t,b1,t1,b2,t2,b3,t3,b4,t4,b5,t5,b6,t6,b7,t7,b8,t8,b9,t9,b10,t10,b11,t11,b12,t12,b13,t13,b14,t14,subzero;
    //a.split(" ")[0].toInt();
    //****BIBLIOTEKA****//
    b.append(QString("%1").arg(temperatura[3]));
    t.append(QString("%1").arg(temperatura[4]));
    //****SALON****//
    b1.append(QString("%1").arg(temperatura[6]));
    t1.append(QString("%1").arg(temperatura[7]));
    //****SYPIALNIA****//
    b2.append(QString("%1").arg(temperatura[9]));
    t2.append(QString("%1").arg(temperatura[10]));
    //****KORYTARZ DOL****//
    b3.append(QString("%1").arg(temperatura[12]));
    t3.append(QString("%1").arg(temperatura[13]));
    //****MARYNARSKI****//
    b4.append(QString("%1").arg(temperatura[15]));
    t4.append(QString("%1").arg(temperatura[16]));
    //****LAZIENKA DOL****//
    b5.append(QString("%1").arg(temperatura[18]));
    t5.append(QString("%1").arg(temperatura[19]));
    //****KUCHNIA-JADALNIA****//
    b6.append(QString("%1").arg(temperatura[21]));
    t6.append(QString("%1").arg(temperatura[22]));
    //****WIATROLAP****//
    b7.append(QString("%1").arg(temperatura[24]));
    t7.append(QString("%1").arg(temperatura[25]));
    //****POKOJ MYSZKI****//
    b8.append(QString("%1").arg(temperatura[27]));
    t8.append(QString("%1").arg(temperatura[28]));
    //****LOFT****//
    b9.append(QString("%1").arg(temperatura[30]));
    t9.append(QString("%1").arg(temperatura[31]));
    //****ŁAZIENKA GÓRA****//
    b11.append(QString("%1").arg(temperatura[33]));
    t11.append(QString("%1").arg(temperatura[34]));
    //****ZASOBNIK C.W.U.*****//
    b14.append(QString("%1").arg(temperatura[36]));
    t14.append(QString("%1").arg(temperatura[37]));
    //****KOTŁOWNIA****//
    b12.append(QString("%1").arg(temperatura[39]));
    t12.append(QString("%1").arg(temperatura[40]));
    //****ZEWNATRZ****//
    b10.append(QString("%1").arg(temperatura[45]));
    t10.append(QString("%1").arg(temperatura[46]));
    subzero.append(QString("%1").arg(temperatura[47]));
    //****GARAŻ****//
    b13.append(QString("%1").arg(temperatura[42]));
    t13.append(QString("%1").arg(temperatura[43]));

    ui->label_temp_9->setText(b+"."+t + DC);
    ui->label_temp_2->setText(b1+"."+t1 + DC);
    ui->label_temp_6->setText(b2+"."+t2 + DC);
    ui->label_12->setText(b3+"."+t3 + DC);
    ui->label_temp_3->setText(b4+"."+t4 + DC);
    ui->label_temp_1->setText(b5+"."+t5 + DC);
    ui->label_temp_4->setText(b6+"."+t6 + DC);
    ui->label_temp_5->setText(b7+"."+t7 + DC);
    ui->label_temp_8->setText(b8+"."+t8 + DC);
    ui->label_temp_10->setText(b9+"."+t9 + DC);
    ui->label_temp_7->setText(b11+"."+t11 + DC);
    ui->label_15->setText(b12+"."+t12 + DC);
    ui->label_16->setText(b13+"."+t13 + DC);
    ui->label_21->setText(b14+"."+t14 + DC);
    //warunek do wyswietlenia temperatury ujemnej (zastosowano tylko do czujnika zewnętrznego)
    if(subzero=="1"){
        ui->label_31->setText("-"+b10+"."+t10 + DC);
    }else{
        ui->label_31->setText(b10+"."+t10 + DC);
    }
    //

    QPixmap temp_on("/media/HDD1/admin/iHome/28-02-2018/media/thermo_on_1.png");
    QPixmap temp_off("/media/HDD1/admin/iHome/28-02-2018/media/thermo_1.png");
    QPixmap pompa_off("/media/HDD1/admin/iHome/28-02-2018/media/pompa_off.png");
    QPixmap con_err_off("/media/HDD1/admin/iHome/28-02-2018/media/con_err_off.png");
    QPixmap con_err_on("/media/HDD1/admin/iHome/28-02-2018/media/con_err_on.png");

    //**HISTEREZA BIBLIOTEKA**//
    if (((temperatura[3]*10)+temperatura[4]-ui->dial_7->value()) >=ui->dial_temp_9->value()){
        ui->th_10->setPixmap(temp_off);
        flaga=1;
    }
    if (((temperatura[3]*10)+temperatura[4]+ui->dial_7->value()) <=ui->dial_temp_9->value()){
        ui->th_10->setPixmap(temp_on);
        flaga=0;
    }
    //**HISTEREZA SYPIALNIA**//
    if (((temperatura[9]*10)+temperatura[10]-ui->dial_7->value()) >=ui->dial_temp_6->value()){
        ui->th_7->setPixmap(temp_off);
        flaga_7=1;
    }
    if (((temperatura[9]*10)+temperatura[10]+ui->dial_7->value()) <=ui->dial_temp_6->value()){
        ui->th_7->setPixmap(temp_on);
        flaga_7=0;
    }
    //**HISTEREZA LOFT**//
    if(((temperatura[30]*10)+temperatura[31]-ui->dial_7->value())>=ui->dial_temp_10->value()){
        ui->th_11->setPixmap(temp_off);
        flaga_8=1;
    }
    if(((temperatura[30]*10)+temperatura[31]+ui->dial_7->value())<=ui->dial_temp_10->value()){
        ui->th_11->setPixmap(temp_on);
        flaga_8=0;
    }
    //**HISTEREZA ŁAZIENKA GÓRA**//
    if(((temperatura[33]*10)+temperatura[34]-ui->dial_7->value())>=ui->dial_temp_7->value()){
        ui->th_8->setPixmap(temp_off);
        flaga_9=1;
    }
    if(((temperatura[33]*10)+temperatura[34]+ui->dial_7->value())<=ui->dial_temp_7->value()){
        ui->th_8->setPixmap(temp_on);
        flaga_9=0;
    }
    //**HISTEREZA POKOJ MYSZKI**//
    if(((temperatura[27]*10)+temperatura[28]-ui->dial_7->value())>=ui->dial_temp_8->value()){
        ui->th_9->setPixmap(temp_off);
        flaga_10=1;
    }
    if(((temperatura[27]*10)+temperatura[28]+ui->dial_7->value())<=ui->dial_temp_8->value()){
        ui->th_9->setPixmap(temp_on);
        flaga_10=0;
    }
    //**HISTEREZA SALON**//
    if (((temperatura[6]*10)+temperatura[7]-ui->dial_7->value()) >=ui->dial_temp_2->value()){

        ui->th_2->setPixmap(temp_off);
        flaga_1=1;
    }
    if (((temperatura[6]*10)+temperatura[7]+ui->dial_7->value()) <=ui->dial_temp_2->value()){

        ui->th_2->setPixmap(temp_on);
        flaga_1=0;
    }
    //**KUCHNIA-JADALNIA**//
    if (((temperatura[21]*10)+temperatura[22]-ui->dial_7->value()) >=ui->dial_temp_4->value()){

        ui->th_5->setPixmap(temp_off);
        flaga_2=1;
    }
    if (((temperatura[21]*10)+temperatura[22]+ui->dial_7->value()) <=ui->dial_temp_4->value()){

        ui->th_5->setPixmap(temp_on);
        flaga_2=0;
    }
    //**MARYNARSKI**//
    if (((temperatura[15]*10)+temperatura[16]-ui->dial_7->value()) >=ui->dial_temp_3->value()){

        ui->th_3->setPixmap(temp_off);
        flaga_4=1;
    }
    if (((temperatura[15]*10)+temperatura[16]+ui->dial_7->value()) <=ui->dial_temp_3->value()){

        ui->th_3->setPixmap(temp_on);
        flaga_4=0;
    }
    //**LAZIENKA DOL**//
    if (((temperatura[18]*10)+temperatura[19]-ui->dial_7->value()) >=ui->dial_temp_1->value()){

        ui->th_1->setPixmap(temp_off);
        flaga_5=1;
    }
    if (((temperatura[18]*10)+temperatura[19]+ui->dial_7->value()) <=ui->dial_temp_1->value()){

        ui->th_1->setPixmap(temp_on);
        flaga_5=0;
    }
    //**WIATROLAP**//
    if (((temperatura[24]*10)+temperatura[25]-ui->dial_7->value()) >=ui->dial_temp_5->value()){

        ui->th_6->setPixmap(temp_off);
        flaga_6=1;
    }
    if (((temperatura[24]*10)+temperatura[25]+ui->dial_7->value()) <=ui->dial_temp_5->value()){

        ui->th_6->setPixmap(temp_on);
        flaga_6=0;
    }

    qu++;
    if (qu==15){
        getHumidity();
        barometer();
        if(ui->pushButton_34->isChecked()){
            if(flaga==0){
                maskawysl[2]|=0x08;
                MyUDP client;
                client.WYSUDP("192.168.1.101");
            }
            if(flaga==1){
                maskawysl[2]&=~0x08;
                MyUDP client;
                client.WYSUDP("192.168.1.101");
            }
            if (flaga_7==0){
                maskawysl[2]|=0x10;
                MyUDP client;
                client.WYSUDP("192.168.1.101");
            }
            if (flaga_7==1){
                maskawysl[2]&=~0x10;
                MyUDP client;
                client.WYSUDP("192.168.1.101");
            }
            if(flaga_8==0){
                maskawysl[2]|=0x40;
                MyUDP client;
                client.WYSUDP("192.168.1.101");
            }
            if(flaga_8==1){
                maskawysl[2]&=~0x40;
                MyUDP client;
                client.WYSUDP("192.168.1.101");
            }
            if(flaga_9==0){
                maskawysl[2]|=0x80;
                MyUDP client;
                client.WYSUDP("192.168.1.101");
            }
            if(flaga_9==1){
                maskawysl[2]&=~0x80;
                MyUDP client;
                client.WYSUDP("192.168.1.101");
            }
            if(flaga_10==0){
                maskawysl[2]|=0x20;
                MyUDP client;
                client.WYSUDP("192.168.1.101");
            }
            if(flaga_10==1){
                maskawysl[2]&=~0x20;
                MyUDP client;
                client.WYSUDP("192.168.1.101");
            }
            if (flaga_1==0){
                maskawysl[3]|=0x04;
                MyUDP client;
                client.WYSUDP("192.168.1.101");
            }
            if (flaga_1==1){
                maskawysl[3]&=~0x04;
                MyUDP client;
                client.WYSUDP("192.168.1.101");
            }
            if (flaga_2==0){
                maskawysl[3]|=0x02;
                MyUDP client;
                client.WYSUDP("192.168.1.101");
            }
            if (flaga_2==1){
                maskawysl[3]&=~0x02;
                MyUDP client;
                client.WYSUDP("192.168.1.101");
            }
            if (flaga_4==0){
                maskawysl[3]|=0x08;
                MyUDP client;
                client.WYSUDP("192.168.1.101");
            }
            if (flaga_4==1){
                maskawysl[3]&=~0x08;
                MyUDP client;
                client.WYSUDP("192.168.1.101");
            }
            if (flaga_5==0){
                maskawysl[3]|=0x10;
                MyUDP client;
                client.WYSUDP("192.168.1.101");
            }
            if (flaga_5==1){
                maskawysl[3]&=~0x10;
                MyUDP client;
                client.WYSUDP("192.168.1.101");
            }
            if (flaga_6==0){
                maskawysl[3]|=0x20;
                MyUDP client;
                client.WYSUDP("192.168.1.101");
            }
            if (flaga_6==1){
                maskawysl[3]&=~0x20;
                MyUDP client;
                client.WYSUDP("192.168.1.101");
            }
            //*************flaga dla pomp podłogówki*******
            if (flaga_1==0 || flaga_2==0 || flaga_4==0 || flaga_5==0 || flaga_6==0){
                maskawysl[3]|=0x40;
                MyUDP client;
                client.WYSUDP("192.168.1.101");
                ui->label_pompa_1->setMovie(movie_pompa_1);
                movie_pompa_1->start();
            }
            if(flaga_1==1 && flaga_2==1 && flaga_4==1 && flaga_5==1 && flaga_6==1){
                maskawysl[3]&=~0x40;
                MyUDP client;
                client.WYSUDP("192.168.1.101");
                movie_pompa_1->stop();
                ui->label_pompa_1->setPixmap(pompa_off);
            }
            if (flaga==0 || flaga_7==0 || flaga_8==0 || flaga_9==0 || flaga_10==0){
                maskawysl[3]|=0x80;
                MyUDP client;
                client.WYSUDP("192.168.1.101");
                ui->label_pompa_2->setMovie(movie_pompa_2);
                movie_pompa_2->start();
            }
            if (flaga==1 && flaga_7==1 && flaga_8==1 && flaga_9==1 && flaga_10==1){
                maskawysl[3]&=~0x80;
                MyUDP client;
                client.WYSUDP("192.168.1.101");
                movie_pompa_2->stop();
                ui->label_pompa_2->setPixmap(pompa_off);
            }
            //********flaga dla pompy C.O.*************************
            if(flaga_1==0 || flaga_2==0 || flaga_4==0 || flaga_5==0 || flaga_6==0 || flaga==0 || flaga_7==0 || flaga_8==0 || flaga_9==0 || flaga_10==0){
                plugsocket[2]=0x01;
            } else{
                plugsocket[2]=0x00;
            }
        }
        qu=0;
    }
//****************sprawdzanie obecności czujników CT**********************************//
    uint16_t id[15]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x01,0x02,0x04,0x08,0x10,0x20,0x40}; //numer czujnika
    int tn[15]={48,48,48,48,48,48,48,48,49,49,49,49,49,49,49}; //numer bajtu kontrolnego stan czujnika
    for(int i=0; i<15;i++){
        if(temperatura[tn[i]]&id[i]){
            c_e.at(i)->setPixmap(con_err_off);
        }else{
            c_e.at(i)->setPixmap(con_err_on);
        }
    }
//*****************Shelly signal*************************************//

    ui->signal->setText(rssi[1]);
    ui->signal_2->setText(rssi[0]);
    ui->signal_3->setText(rssi[2]);
    ui->signal_4->setText(rssi[4]+DC); //temperatura układu Shelly2.5
    ui->signal_5->setText(rssi[3]);

    //****************zaznaczanie buttonów po wykryciu pakietu************************//
    if("192.168.1.100"==ips || simulating_on || "192.168.1.104"==ips){
           simulating_on=false;
           int m=0; //numeracja pozycji przycisku na liście

           for (int j=0; j<=5;j++){

               for (int i=1; i<=8;i++){

                   int y=bPos[m];
                   if(temp[j] & hexx[i]){  //szukanie aktywnego wejścia
                       //***wyjątki dla wejść z krańcówek bramy
                       if(temp[3] & 0x01) {
                           if (otwarta==0){
                               QPixmap bBrama("/media/HDD1/admin/iHome/28-02-2018/media/gdoorup.png");
                               QIcon biBrama(bBrama);
                               bList.at(13)->setIcon(biBrama);
                               bList.at(13)->setIconSize(bBrama.rect().size());
                               otwarta=1;
                           }
                           if (otwarta==2){
                               QPixmap bBrama("/media/HDD1/admin/iHome/28-02-2018/media/gdoordown.png");
                               QIcon biBrama(bBrama);
                               bList.at(13)->setIcon(biBrama);
                               bList.at(13)->setIconSize(bBrama.rect().size());
                               otwarta=1;
                           }
                       }
                       if(temp[3] & 0x02) {
                           QPixmap bBrama("/media/HDD1/admin/iHome/28-02-2018/media/gdooropen.png");
                           QIcon biBrama(bBrama);
                           bList.at(13)->setIcon(biBrama);
                           bList.at(13)->setIconSize(bBrama.rect().size());
                           otwarta=2;
                           QTime GOT=QTime::currentTime();
                           ui->label_otime->setText(GOT.toString());
                       }
                       //***koniec wyjątków
                       if (j!=3){
                           //włączanie/wyłączanie przycisku dla aktywnego wejścia
                           if(bList.at(y)->isChecked() == true){

                               bList.at(y)->setChecked(false);
                           }
                           else {
                               bList.at(y)->setChecked(true);
                           }
                           temp[j]&=~(hexx[i]);
                       }
                   }
                   m++;
               }
           }


//**************zmiana statusu/ikony bramy na ZAMKNIĘTA po zamknięciu***********
    if(otwarta == 1){

        if(temp[3] & 0x01){
            qDebug() << "NADAL W RUCHU";
        }
        else{
            QPixmap bBrama("/media/HDD1/admin/iHome/28-02-2018/media/gdoorclose.png");
            QIcon biBrama(bBrama);
            bList.at(13)->setIcon(biBrama);
            bList.at(13)->setIconSize(bBrama.rect().size());
            otwarta=0;
            QTime GOT=QTime::currentTime();
            ui->label_ctime->setText(GOT.toString());
        }
    }
}

//*********************WYŚWIETLANIE AKTYWNOŚCI CZUJEK PIR***********************//
    if("192.168.1.103"==ips){
        if(temp[1]&0x01){
            ui->pushButton_24->setChecked(true);
        }
//******************ZAZNACZANIE BUTTONA Z HARMONOGRAMU*************************//
        if(scheduledaction==1){
            foreach(int bgis, bgi){
                foreach(int btns, scheduledbtns[bgis]){
                    bList.at(btns)->setChecked(true);
                }
            }
            scheduledaction=0;
            bgi.clear();
        }
    }
}

void MainWindow::on_pushButton_16_pressed(){

    timer_bramaStykOff->start(500);
}

void MainWindow::ClickedscenebtnFinder(bool checked)
{
    if(checked){
        qDebug() << QObject::sender()->objectName();

        //qDebug() << sbList.indexOf(QPushButton::sender());
        //qDebug() << QPushButton.toolTip();
        QString tt = ui->button_scene_2->toolTip();
        scene_active=true;
        movie_countdown->start();
        ui->label_19->setStyleSheet("background-image:none");
        mydbs baza;
        QSqlQuery* qry = new QSqlQuery(baza.mydb);
        qry->prepare("select * from scenes where description ='"+tt+"'");
        if(qry->exec()){
            while(qry->next()){
            odliczG = (qry->value(2).toInt())*60;
            }
        }
    }else{
        scene_active=false;
        movie_countdown->stop();
        odliczG=0;
    }
}

void MainWindow::on_button_scene_1_toggled(bool checked)
{
    if(checked){
        scene_active=true;
        scene_driving=true;
        movie_countdown->start();
        ui->label_19->setStyleSheet("background-image:none");
        odliczG=600;
    }else{
        scene_active=false;
        scene_driving=false;
        movie_countdown->stop();
        odliczG=0;
    }
}

void MainWindow::on_button_scene_3_clicked()
{
    odliczG=1;
    emit all_off();
    MyUDP client;
    maskawysl[2]|=0x08;
    client.WYSUDP("192.168.1.101");
    temp[2]=0x08;
    simulating_on=true;
    receiving();
    c[20]=1;
}

//*******************************WYJŚCIE Z DOMU********************//
void MainWindow::wyjscie(){

    MyUDP client;
    maskawysl[4]|=0x80;
    client.WYSUDP("192.168.1.101");
    ui->button_scene_1->setChecked(false);
    timer_bramaStykOff->start(500);
}

void MainWindow::stykOff(){

    //QByteArray tab;
    MyUDP client;
    maskawysl[4]&=~0x80;
    client.WYSUDP("192.168.1.101");
    timer_bramaStykOff->stop();
    ui->pushButton_16->setChecked(false);
    c[40]=0;
}

void MainWindow::on_pushButton_23_clicked()
{
    if((ui->listWidget->currentRow()>=0 && ui->listWidget_2->currentRow()>=0) || (ui->pushButton_31->isChecked() && ui->listWidget_2->currentRow()>=0)){
        int hexxpos;
        int om;
        gn++;
        QListWidgetItem *new_item = new QListWidgetItem;
        QList<unsigned char> ingridiens;
        QList<int> outmask;
        QList<int> scheduledc;
        QList<int> scheduledbtn;
        QString tempnames;
        QList<QListWidgetItem*> itemtextlist = ui->listWidget_2->selectedItems();
        QList<QListWidgetItem*> itempirlist = ui->listWidget->selectedItems();
        QModelIndexList rowsy = ui->listWidget_2->selectionModel()->selectedRows(0);
        foreach(QModelIndex rows, rowsy){
            if(rows.row()<=7){
                hexxpos=0;
                om=0;
            }
            if(rows.row()>7 && rows.row()<=15){
                hexxpos=8;
                om=1;
            }
            if(rows.row()>15 && rows.row()<=23){
                hexxpos=16;
                om=2;
            }
            if(rows.row()>23 && rows.row()<=31){
                hexxpos=24;
                om=3;
            }
            if(rows.row()>31){
                hexxpos=32;
                om=4;
            }
            if(rows.row()>39){
                hexxpos=40;
                om=5;
            }
            ingridiens.insert(a,hexx[(rows.row())-hexxpos+1]);
            outmask.insert(a,om);
            scheduledc.insert(a,(rows.row())+1);
            tempnames.append(itemtextlist.at(a)->text() + ", ");
            scheduledbtn.insert(a,bPos[rows.row()]);
            a++;
        }
        //scheduledhexxpir.insert(gn,hexx[(ui->listWidget->currentRow())+1]);
        scheduledhexxout.insert(gn,ingridiens);
        outmasks.insert(gn,outmask);
        scheduledcs.insert(gn,scheduledc);
        scheduledbtns.insert(gn,scheduledbtn);
        outnames.insert(gn,tempnames);
        gn2pos.insert(gn,gn2);
        a=0;
        if(ui->pushButton_29->isChecked()){
            scheduledhexxpir.insert(gn,hexx[(ui->listWidget->currentRow())+1]);
            scheduledtime.insert(gn,1);
            scheduledtimers.insert(gn,0);
            tspinBox.insert(gn, 0);
            new_item->setIcon(QIcon("/media/HDD1/admin/iHome/28-02-2018/media/timer_2_on.png"));
            new_item->setText(itempirlist.takeFirst()->text()+" -> "+tempnames);
            pirnames.insert(gn,ui->listWidget->currentItem()->text());
        }
        if(ui->pushButton_30->isChecked()){
            scheduledhexxpir.insert(gn,hexx[(ui->listWidget->currentRow())+1]);
            MyTimer(this, SLOT(offfff()));
            tspinBox.insert(gn,ui->listWidget->currentRow());
            scheduledtime.insert(gn,0);
            new_item->setIcon(QIcon("/media/HDD1/admin/iHome/28-02-2018/media/timer_on.png"));
            new_item->setText(itempirlist.takeFirst()->text()+" -> "+tempnames);
            pirnames.insert(gn,ui->listWidget->currentItem()->text());
        }
        if(ui->pushButton_31->isChecked()){
            startat.insert(gn2,ui->timeEdit->text());
            stopat.insert(gn2,ui->timeEdit_2->text());
            scheduledhexxpir.insert(gn,0);
            scheduledtime.insert(gn,2);
            scheduledtimers.insert(gn,0);
            tspinBox.insert(gn,0);
            gnpos.insert(gn2,gn);
            pirnames.insert(gn, "-");
            if(ui->trackButton->isChecked()){
                sunsetTime.insert(gn2,ui->label_26->text());
                sunriseTime.insert(gn2,ui->label_25->text());
                new_item->setIcon(QIcon("/media/HDD1/admin/iHome/28-02-2018/media/timer_tracking.png"));
            }else{
                sunsetTime.insert(gn2,"0");
                sunriseTime.insert(gn2,"0");
                new_item->setIcon(QIcon("/media/HDD1/admin/iHome/28-02-2018/media/timer_3_on.png"));
            }
            //new_item->setIcon(QIcon("/media/HDD1/admin/iHome/28-02-2018/media/timer_3_on.png"));
            new_item->setText(ui->timeEdit->text()+" - "+ui->timeEdit_2->text()+" -> "+tempnames);
            gn2++;
        }
        ui->listWidget_3->addItem(new_item);
        ui->listWidget->clearSelection();
        ui->listWidget_2->clearSelection();
    }
    writescheduler();
}
//usuwanie pozycji z harmonogramu//
void MainWindow::on_pushButton_27_clicked()
{
    int cr = ui->listWidget_3->currentRow();
    if(cr>=0){
        if(scheduledtime[cr]==2){
            gnpos.removeAt(gn2pos[cr]);
            startat.removeAt(gn2pos[cr]);
            stopat.removeAt(gn2pos[cr]);
            sunsetTime.removeAt(gn2pos[cr]);
            sunriseTime.removeAt(gn2pos[cr]);
            if(--gn2<0){
                gn2=0;
            }
        }
        scheduledhexxout.removeAt(cr);
        scheduledhexxpir.removeAt(cr);
        outmasks.removeAt(cr);
        scheduledcs.removeAt(cr);
        scheduledtime.removeAt(cr);
        scheduledtimers.removeAt(cr);
        scheduledbtns.removeAt(cr);
        tspinBox.removeAt(cr);
        outnames.removeAt(cr);
        pirnames.removeAt(cr);
        gn2pos.removeAt(cr);

        delete ui->listWidget_3->currentItem();
        writescheduler();
        gn--;
    }
}

void MainWindow::on_pushButton_28_clicked()
{
    odliczG=1;
    emit all_off();
}

void MainWindow::offfff(){
    for(int j=0; j<=(scheduledhexxpir.length())-1;j++){
        if(scheduledtime[j]==0){
            if(scheduledtimers[j]->isActive()){
                qDebug() << "Timer aktywny:" << j;
            }
            else{
                int i=0;
                foreach(unsigned char r, scheduledhexxout[j]){
                    maskawysl[outmasks.value(j)[i]]&=~r;
                    MyUDP client;
                    client.WYSUDP("192.168.1.101");
                    c[scheduledcs.value(j)[i]]=0;
                    bList.at(scheduledbtns.value(j)[i])->setChecked(false);
                    i++;
                }
                scheduledtimers.at(j)->stop();
            }
        }
    }
}

void MainWindow::on_pushButton_31_toggled(bool checked)
{
    if (checked){
        ui->listWidget->setDisabled(true);
    }else{
        ui->listWidget->setDisabled(false);
    }
}

void MainWindow::readTimeFromWWW(){
    qDebug() << "ZMIANA DATY";
    QWebElement wTime = webView->page()->mainFrame()->findFirstElement("h1");
    QString swTime = wTime.toPlainText();
    ui->label_25->setText(swTime.mid(51,5));
    ui->label_26->setText(swTime.mid(73,5));
    QObject::disconnect(webView,SIGNAL(loadFinished(bool)), this, SLOT(readTimeFromWWW()));
    sunTimeWatcher(sunsetTime, ui->label_26->text(), ui->listWidget_3, 0, startat);
    sunTimeWatcher(sunriseTime, ui->label_25->text(), ui->listWidget_3, 11, stopat);
}

//****************************WYSYŁANIE Z IKON PULPITU**********************//

void MainWindow::ClickedbtnFinder(){
    QString IP_holder="192.168.1.101";
    int m=0; //numeracja pozycji przycisku na liście
    for (int j=0; j<=5;j++){
        for (int i=1; i<=8;i++){
            int y=bPos[m];
            if(bList.at(y)==QObject::sender()){
                QByteArray tab;
                MyUDP client;
                //przesunięcie wartości m o "+1" ze względu na numerację wyjść c od "1" a buttonów od "0"
                if(bList.at(y)->isChecked()){
                    maskawysl[j]|=(hexx[i]);
                    client.WYSUDP(IP_holder);
                    c[m+1]=1;
                }
                if(bList.at(y)->isChecked()==false){
                    maskawysl[j]&=~(hexx[i]);
                    client.WYSUDP(IP_holder);
                    c[m+1]=0;
                }
            }
            //obsługa modułu 6-CH
            if(++m>39){
                IP_holder="192.168.1.104";
            }
        }
    }
}

void MainWindow::on_pushButton_33_toggled(bool checked)
{
    if(checked){
        movie_cyrkulacja->start();
    }else{
        movie_cyrkulacja->stop();
        ui->pushButton_33->setIcon(QIcon("/media/HDD1/admin/iHome/28-02-2018/media/circulation_off.png"));
    }
}

void MainWindow::writescheduler(){
    QFile target("/media/HDD1/admin/iHome/28-02-2018/settings.txt");
    if(target.open(QIODevice::WriteOnly | QIODevice::Text)){
        QDataStream out(&target);
        QDataStream &operator<<(QDataStream &out, const unsigned char& dane);
        out << scheduledhexxout;
        out << scheduledhexxpir;
        out << outmasks;
        out << scheduledcs;
        out << scheduledtime;
        out << scheduledbtns;
        out << outnames;
        out << pirnames;
        out << tspinBox;
        out << startat;
        out << stopat;
        out << gnpos;
        foreach(QDial* dL, dList){
            out << dL->value();
        }
        out << t;
        out << sunsetTime;
        out << sunriseTime;
        out << gn2pos;
        target.flush();
        target.close();
    }
}

void MainWindow::readscheduler(){
    int dLv[10];
    int u=0;
    int dLv_n=0;
    int dpLv_n=0;
    int time2_u=0; //oddzielna numeracja listy start,stop
    QFile target("/media/HDD1/admin/iHome/28-02-2018/settings.txt");
    if(target.open(QIODevice::ReadOnly | QIODevice::Text)){
        QDataStream in(&target);
        QDataStream &operator>>(QDataStream &in, unsigned char& dane);
        in >> scheduledhexxout >> scheduledhexxpir >> outmasks >> scheduledcs >> scheduledtime >> scheduledbtns
                >> outnames >> pirnames >> tspinBox >> startat >> stopat >> gnpos
                >> dLv[0] >> dLv[1] >> dLv[2] >> dLv[3] >> dLv[4] >> dLv[5] >> dLv[6] >> dLv[7] >> dLv[8]
                >> dLv[9] >> t >> sunsetTime >> sunriseTime >> gn2pos;
        target.close();

        foreach(QDial* dL, dList){
            dL->setValue(dLv[dLv_n]);
            dLv_n++;
        }
        foreach (QDial* dpL, dpList) {
            dpL->setValue((t.at(dpLv_n))/1000);
            dpLv_n++;
        }

        foreach(int time, scheduledtime){
            gn=u;
            QListWidgetItem *new_item = new QListWidgetItem;
            if(time==0){
                new_item->setIcon(QIcon("/media/HDD1/admin/iHome/28-02-2018/media/timer_on.png"));
                new_item->setText(pirnames.at(u) + " -> " + outnames.at(u));
                ui->listWidget_3->addItem(new_item);
                MyTimer(this, SLOT(offfff()));
            }
            if(time==1){
                new_item->setIcon(QIcon("/media/HDD1/admin/iHome/28-02-2018/media/timer_2_on.png"));
                new_item->setText(pirnames.at(u) + " -> " + outnames.at(u));
                ui->listWidget_3->addItem(new_item);
                scheduledtimers.insert(gn,0);
            }
            if(time==2){
                if(!(sunriseTime[time2_u]=="0")){
                    new_item->setIcon(QIcon("/media/HDD1/admin/iHome/28-02-2018/media/timer_tracking.png"));
                }else{
                    new_item->setIcon(QIcon("/media/HDD1/admin/iHome/28-02-2018/media/timer_3_on.png"));
                }
                new_item->setText(startat.at(time2_u) + " - " + stopat.at(time2_u) + " -> " + outnames.at(u));
                ui->listWidget_3->addItem(new_item);
                scheduledtimers.insert(gn,0);
                time2_u++;
            }
            u++;
        }
    }
    qDebug() << startat << stopat;
}

void MainWindow::on_dial_12_valueChanged(int value)
{
    if(value==1){
        bcm2835_gpio_write(GEAR1, HIGH);
        bcm2835_gpio_write(GEAR2, HIGH);
    }
    if(value==2){
        bcm2835_gpio_write(GEAR1, LOW);
        bcm2835_gpio_write(GEAR2, HIGH);
    }
    if(value==3){
        bcm2835_gpio_write(GEAR1, HIGH);
        bcm2835_gpio_write(GEAR2, LOW);
    }
    if(humiditywatcher==false){
        humidityholder=value;
        qDebug() << humidityholder;
    }
}

void MainWindow::ClickedlabelFinder(){

    for(int i=0; i<dList.length(); i++){
        if(lList.at(i)==QObject::sender()){
            if(dList.at(i)->isVisible() == true){
                dList.at(i)->setVisible(false);
                ldList.at(i)->setVisible(false);
                ui->label_36->setVisible(false);
                ui->label_32->setVisible(false);
            }else{
                dList.at(i)->setVisible(true);
                ldList.at(i)->setVisible(true);
                if(i>4){
                    ui->label_36->setVisible(true);
                    ui->label_32->setVisible(false);
                }else{
                    ui->label_32->setVisible(true);
                    ui->label_36->setVisible(false);
                }
            }
        }else{
            dList.at(i)->setVisible(false);
            ldList.at(i)->setVisible(false);
        }
    }
}

void MainWindow::settimers(int dial_value)
{
    for(int i=0; i<dpList.length(); i++){
        if(dpList.at(i)==QObject::sender()){
            t.replace(i,(dial_value*1000));
            writescheduler();
        }        
    }
}

void MainWindow::showui()
{
    if(!ui->label_47->isVisible()){
        ui->label_47->setVisible(true);
        ui->label_48->setVisible(true);
    }else{
        ui->label_47->setVisible(false);
        ui->label_48->setVisible(false);
    }
}

void MainWindow::cyrkulacja_rotation()
{
    ui->pushButton_33->setIcon(movie_cyrkulacja->currentPixmap());
    ui->pushButton_33->setIconSize(QSize(45,45));
}

void MainWindow::getHumidity()
{
    int pulseCounts[DHT_BITS*2] = {0};

    bcm2835_gpio_fsel(PIN_TH, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write(PIN_TH, LOW);
    bcm2835_delay(20);
    bcm2835_gpio_fsel(PIN_TH, BCM2835_GPIO_FSEL_INPT);
    bcm2835_delayMicroseconds(30); //opóźnienie włączone na RPI4 (wyłączone na RPI2)
    //generowanie małego poóźnienia < 1us
    for (volatile int i = 0; i < 50; ++i) {
     }

     // Wait for DHT to pull pin low.
     u_int32_t count = 0;
     while (bcm2835_gpio_lev(PIN_TH)==1) {
       if (++count >= DHT_MAXCOUNT) {
         // Timeout waiting for response.
         qDebug() << "READING ERROR";
         break;
       }
     }

    for(int j=0; j<DHT_BITS*2; j+=2){
        while(bcm2835_gpio_lev(PIN_TH)==0){
            if (++pulseCounts[j] >= DHT_MAXCOUNT){
                break;
            }
        }
        while(bcm2835_gpio_lev(PIN_TH)==1){
            if (++pulseCounts[j+1] >= DHT_MAXCOUNT){
                break;
            }
        }
    }
    u_int32_t threshold = 0;
      for (int i=2; i < DHT_BITS*2; i+=2) {
        threshold += pulseCounts[i];
      }
      threshold /= DHT_BITS-1;

      u_int8_t data[5] = {0};
       for (int i=3; i < DHT_BITS*2; i+=2) {
         int index = (i-3)/16;
         data[index] <<= 1;
         if (pulseCounts[i] >= threshold) {
            // One bit for long pulse.
           data[index] |= 1;
         }
            // Else zero bit for short pulse.
       }

    if (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)){
        //qDebug() << "DANE OK";
        humidity = (float) (data[0]);
        float temperature = (float) (data[2]);
        QString hum, temp;
        hum.setNum(humidity);
        temp.setNum(temperature);
        ui->label_13->setText(hum +"%");
        ui->label_14->setText(temp + DC);
        if(humidity>ui->spinBox->value()){
            count_down=0;
            if(++count_up==2){
                count_up=0;
                persistent=true;
            }
        }
        if(humidity<=ui->spinBox->value()){
            count_up=0;
            if(++count_down==2){
                count_down=0;
                persistent=false;
            }
        }
    }
}

void MainWindow::barometer()
{
    QString pressure, temperature;
    QFile weather_data("/home/pi/plik.txt");
    if(weather_data.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream data(&weather_data);
        data >> pressure >> temperature;
        ui->label_35->setText(pressure);
        ui->label_37->setText(temperature);
    }
    weather_data.close();

}

void MainWindow::on_pushButton_32_clicked(bool checked)
{
    if(checked){
        emit simulating(true);
    }else{
        emit simulating(false);
    }
}

void MainWindow::LOff()
{
    for(int x=0; x<=47; x++){
        int y=bPos[x];
        if(bList[y]->isChecked() == true){
            bList[y]->setChecked(false);
        }
    }
    movie_countdown->stop();
}

void MainWindow::on_go_floor_clicked()
{
    int ci = ui->Tab->currentIndex();
    ui->Tab->setCurrentIndex(ci+1);
}

void MainWindow::on_button_wentylator_clicked(bool checked)
{
    if(checked){
        ui->button_wentylator_2->setChecked(false);
        movie_wentylacja->setSpeed(200);
        ui->dial_12->setValue(2);
    }else{
        ui->button_wentylator_2->setChecked(false);
        movie_wentylacja->setSpeed(100);
        ui->dial_12->setValue(1);
    }
}

void MainWindow::on_button_wentylator_2_clicked(bool checked)
{
    if(checked){
        movie_wentylacja->setSpeed(300);
        ui->dial_12->setValue(3);
    }
    if(!checked && ui->button_wentylator->isChecked()){
        movie_wentylacja->setSpeed(200);
        ui->dial_12->setValue(2);
    }
    if(!checked && ui->button_wentylator->isChecked()==false){
        movie_wentylacja->setSpeed(100);
        ui->dial_12->setValue(1);
    }
}

void MainWindow::on_go_ground_clicked()
{
    int ci = ui->Tab->currentIndex();
    ui->Tab->setCurrentIndex(ci-1);
}

void MainWindow::on_button_wentylator_3_clicked(bool checked)
{
    if(checked){
        ui->button_wentylator_2->setChecked(false);
        movie_wentylacja->setSpeed(200);
        ui->dial_12->setValue(2);
    }else{
        ui->button_wentylator_2->setChecked(false);
        movie_wentylacja->setSpeed(100);
        ui->dial_12->setValue(1);
    }
}

void MainWindow::on_button_scene_4_clicked()
{
        ui->timeEdit_3->setTime(QTime::currentTime());
}

void MainWindow::on_pushButton_13_clicked()
{
    mydbs baza;
    /*QSqlQueryModel* mqry = new QSqlQueryModel;
    QSqlQuery* qry = new QSqlQuery(baza.mydb);
    qry->prepare("select data,mask from scenes");
    qry->exec();
    mqry->setQuery(*qry);
    ui->comboBox->setModel(mqry);
    */
    QSqlQuery* qq = new QSqlQuery(baza.mydb);
    if(qq->exec("select * from scenes")){
        MyUDP client;
        while(qq->next()){
            maskawysl[qq->value((qq->record().indexOf(ui->comboBox->currentText()))+1).toInt()]|=qq->value(qq->record().indexOf(ui->comboBox->currentText())).toInt();
            client.WYSUDP("192.168.1.101");
            qDebug() << qq->record().indexOf(ui->comboBox->currentText());
        }
    }
}

void MainWindow::on_comboBox_currentIndexChanged(const QString &arg1)
{
    ui->listWidget_4->clear();
    mydbs baza;
    QSqlQuery* qry = new QSqlQuery(baza.mydb);
    qry->prepare("select * from scenes where description ='"+arg1+"'");
    if(qry->exec()){
        while(qry->next()){
            for (int i=1; i<qry->record().count(); i++){
                if(qry->value(i).toString() == ""){
                    //PUSTO
                }else{
                    if(qry->record().fieldName(i+1).contains("dane")){
                        ui->listWidget_4->addItem(qry->record().fieldName(i) +" " + qry->value(i+1).toString());
                        i++;
                    }else{
                        ui->listWidget_4->addItem(qry->record().fieldName(i));
                    }
                }
            }
        }
    }
    baza.conclose();
}

void MainWindow::on_resetButton_clicked()
{
    MyUDP reset;
    reset.WYSUDP("192.168.1.102");
}

void MainWindow::on_cwu_toggled(bool checked)
{
    //QFile plik("/media/HDD1/admin/iHome/28-02-2018/cwu.txt");
    MyUDP cwu;
    if (checked){  
        maskawysl[5]|=0x08;
        //if(plik.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)){
            //QTextStream on(&plik);
            //on << ui->datatime->text() << " ON: " << time_text << " " << ui->label_21->text();
            //plik.close();
        //}
        write_off=true;
    }else if ((!checked) && write_off){
        maskawysl[5]&=~0x08;
        //if(plik.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)){
            //QTextStream off(&plik);
            //off << " OFF: " << time_text << " " << ui->label_21->text() << "\n";
            //plik.close();
        //}
        write_off=false;
    }
    cwu.WYSUDP("192.168.1.104");
}

void MainWindow::on_pushButton_24_toggled(bool checked)
{
    MyUDP sink;
    if(checked){
        maskawysl[5]|=0x10;
    }else{
        maskawysl[5]&=~0x10;
    }
    sink.WYSUDP("192.168.1.104");
}
//***śledzenie czasu wschodu/zachodu złońca i zmiana czasu świecenia źródła***
void MainWindow::sunTimeWatcher(QList<QString> &suntime, QString source, QListWidget *widget, int pos, QList<QString> &ssTIME)
{
    int i=0;
    qDebug() << "JESTEM PRZED FOREM";
    foreach(QString time, suntime){
        qDebug() << "JESTEM PRZED 0";
        if(!(time=="0")){
            QTime old_sun_time = QTime::fromString(time, "HH:mm");
            QTime now_sun_time = QTime::fromString(source, "HH:mm");
            QTime old_oo_time = QTime::fromString(ssTIME[i], "hh:mm:ss");
            QTime new_oo_time = QTime::fromString(ssTIME[i], "hh:mm:ss").addSecs((now_sun_time.secsTo(old_sun_time))*(-1));
            //QTime dif_time = QTime(0,0).addSecs(now_sun_time.secsTo(old_sun_time)) ;
            suntime.replace(i,now_sun_time.toString("hh:mm"));
            //new_oo_time.setHMS(old_oo_time.hour() - dif_time.hour(), old_oo_time.minute() - dif_time.minute(), 0);
            ssTIME.replace(i, new_oo_time.toString());
            qDebug () << "JESTEM PRZED ZMIANA TEKSTU LISTY";
            QList<QListWidgetItem*> item = widget->findItems(old_oo_time.toString(), Qt::MatchContains);
            int rowi = widget->row(item.at(i));
            QString row = widget->item(rowi)->text();
            row.replace(pos,8, new_oo_time.toString());
            widget->item(rowi)->setText(row);
            writescheduler();
        }
        i++;
    }
}

void MainWindow::on_test_pressed()
{
    sunTimeWatcher(sunsetTime, ui->label_26->text(), ui->listWidget_3, 0, startat);
    sunTimeWatcher(sunriseTime, ui->label_25->text(), ui->listWidget_3, 11, stopat);
    qDebug() << scheduledhexxout;
    qDebug() << outmasks;
    qDebug() << scheduledcs;
    qDebug() << scheduledbtns;
    qDebug() << outnames;
    qDebug() << gnpos;
    qDebug() << gn2pos;
    qDebug() << scheduledhexxpir;
    qDebug() << scheduledtime;
    qDebug() << scheduledtimers;
    qDebug() << tspinBox;
    qDebug() << pirnames;
    qDebug() << startat;
    qDebug() << stopat;
    qDebug() << sunriseTime;
    qDebug() << sunsetTime;
}
