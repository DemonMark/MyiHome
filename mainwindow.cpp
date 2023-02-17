#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "myudp.h"
#include "zdarzenie.h"
#include "pir_button.h"
#include "shelly.h"
#include "mydbs.h"
#include "icon_temp.h"
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
#include "bcm2835.h"

#define REKU_SPEED_2 RPI_V2_GPIO_P1_16
#define REKU_SPEED_3 RPI_V2_GPIO_P1_18
#define REKU_STOP RPI_V2_GPIO_P1_22
#define FAN_ON RPI_V2_GPIO_P1_15
#define PIN_TH RPI_V2_GPIO_P1_07
#define DHT_MAXCOUNT 35000 //liczba dobrana do szybkości RPi2 - dotyczy max czasu odczytu powyżej TIMEOUT
#define DHT_BITS 41
#define DC '\xb0' //znak stopnia
#define sceny "/media/HDD2/Moje projekty/MyiHome/scene.db"

QList<QLabel*> c_e;
QList<QPushButton*> bList; //lista przycisków oświetlenia
QList<QPushButton*> sbList; //lista przycisków scen
QList<QDial*> dList; //lista regulatorów temperatury
QList<QLabel*> lList; //lista etykiet temperatur
QList<QLabel*> ldList; //lista opisów regulatorów
QList<QLabel*> pir_label; //lista etykiet timerów PIR
QList<QCheckBox*> chlist;
QString time_text, time_h_m;
extern QString ips;
int num=0;
extern unsigned char shell[1];
extern unsigned char temp[20];
extern unsigned char temperatura[63];
extern unsigned char hexx[9];
unsigned char maskawysl[10];
extern int odliczG;
int qu;
int ventilation_time;
int dzien;  //jeśli = 1 czujki nie włączają świateł
int spimy;  //jeśli = 1 odliczanie 10min od detekcji do wyłączenia światła(wyjść)
int obecnosc=0; //jeśli = 1 wyłączenie świateł(wyjść) o ustalonej godzinie nie odpala się
int otwarta=0; //status bramy garażowej
QDate dateTime;
QString dateStamptext;
float humidity;
int humidityholder;
bool humiditywatcher=false;
bool ventilation=false;
extern bool simulating_on;
bool scene_active;
bool scene_driving;
bool jestem = false;
extern QByteArray plugsocket;
int arg[17];
int arg_check=-3;//potwierdzenie wykonania składników sceny
int arg_config[17];
QString last_scene;
QStringList EN_WORD({"library","Mouse room","bedroom","wardrobe","bathroom"});
bool locked;

MainWindow *MainWindow::pMainWindow = nullptr; //dostep do MainWindow

//inicjalizacja pinów GPIO Raspberry
void initGPIO()
{
    bcm2835_gpio_fsel(REKU_SPEED_2, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(REKU_SPEED_3, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(REKU_STOP, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(FAN_ON, BCM2835_GPIO_FSEL_OUTP);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    bcm2835_init();
    initGPIO();

    ui->setupUi(this);
    pMainWindow = this;
    timerId = startTimer(1000);

    /******************************ZEGAR************************/
    QTimer *timer = new QTimer(this);
    connect (timer,SIGNAL(timeout()),this,SLOT (showTime()));
    timer->start();

    bList = MainWindow::findChildren<QPushButton*>(QRegExp ("pushButton*")) + MainWindow::findChildren<QPushButton*>(QRegExp ("shelly*"));
    sbList = MainWindow::findChildren<QPushButton*>(QRegExp ("scene_*"));
    chlist = MainWindow::findChildren<QCheckBox*>(QRegExp ("conf_ch_*"));
    dList = ui->tab_5->findChildren<QDial*>(QRegExp ("dial_temp_*")) + ui->tab_2->findChildren<QDial*>(QRegExp ("dial_temp_*"));
    lList = ui->tab_5->findChildren<QLabel*>(QRegExp ("label_temp_*")) + ui->tab_2->findChildren<QLabel*>(QRegExp ("label_temp_*"));
    ldList = ui->tab_5->findChildren<QLabel*>(QRegExp ("label_dsc_*")) + ui->tab_2->findChildren<QLabel*>(QRegExp ("label_dsc_*"));
    pir_label = MainWindow::findChildren<QLabel*>(QRegExp ("label_pir_btn_*"));

    int tmp = (MainWindow::findChildren<QLabel*>(QRegExp ("con_err_*")).count());
    for(int lf=1; lf<=tmp; lf++){
        QString ln=QString::number(lf);
        c_e.append(MainWindow::findChildren<QLabel*>("con_err_"+ln));
    }

    mydbs baza(sceny);
    QSqlQuery *qry = baza.query();

    //***DODANIE PRZYCISKÓW DO LIST***//
    QStandardItemModel *myModel = new QStandardItemModel;
    int modelRowCounter = 0;

    foreach(QPushButton *btn, bList)
    {
        connect(btn, SIGNAL(toggled(bool)), this, SLOT(ClickedbtnFinder()));
        if(btn->dynamicPropertyNames().contains("proper_name")){
            qry->prepare("SELECT no_off FROM defaults WHERE button='"+btn->objectName()+"'");
            qry->exec();
            qry->next();
            QStandardItem *myItem = new QStandardItem(btn->property("proper_name").toString());
            myItem->setFlags(Qt::ItemIsUserCheckable| Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            myItem->setData(qry->value("no_off").toInt(), Qt::CheckStateRole);
            myItem->setData(btn->objectName(), Qt::UserRole);
            myModel->setItem(modelRowCounter++,0,myItem);

            ui->button_list_hold->addItem(btn->property("proper_name").toString(),btn->objectName());
            ui->button_list_click->addItem(btn->property("proper_name").toString(),btn->objectName());
        }
    }
    ui->button_list->setModel(myModel);
    ui->button_list->model()->sort(0);
    ui->button_list_hold->model()->sort(0);
    ui->button_list_click->model()->sort(0);
    ui->button_list_click->setCurrentIndex(-1);
    ui->button_list_hold->setCurrentIndex(-1);
    ui->button_list->setStyleSheet("combobox-popup: 0;");
    //

    foreach (QPushButton *sbtn, sbList) {
        connect(sbtn, SIGNAL(toggled(bool)) , this, SLOT(ClickedscenebtnFinder(bool)));
    }

    connect(ui->temp_dial, SIGNAL(valueChanged(int)), this, SLOT(settemperature(int)));

    connect(ui->pir_dial, SIGNAL(valueChanged(int)), this, SLOT(settimers(int)));

    connect(ui->timery_pir, &QCheckBox::toggled, [=](bool checked){
        qDebug() << checked;
        foreach(QLabel* lb, pir_label){
            lb->setHidden(checked);
        }
    });
    temporary_cwu_checker = new QTimer(this);
    connect(temporary_cwu_checker, &QTimer::timeout, [=](){
        data_logger("");
    });
    temporary_cwu_checker->start(30000);
    //***timer styku bramy***//
    timer_bramaStykOff = new QTimer(this);
    connect(timer_bramaStykOff, &QTimer::timeout, [=]() {
        timer_bramaStykOff->stop();
        ui->pushButton_16->setChecked(false);
    });

    //PARAMETRY POCZĄTKOWE//
    plugsocket[2]=0x00;
    ui->label_32->setVisible(false);
    ui->label_47->setVisible(false);
    ui->label_48->setVisible(false);
    ui->label_shelly_106->setVisible(false);
    ui->label_ventilation->setVisible(false);
    ui->pir_dial->setVisible(false);
    ui->temp_dial->setVisible(false);
    ui->th_master->setVisible(false);
    ui->th_priority->setVisible(false);
    ui->a_off->setVisible(false);

    webView = new QWebPage(this);
    webView->setVisibilityState(QWebPage::VisibilityStateHidden);
    webView->mainFrame()->load(QUrl("https://calendar.zoznam.sk/sunset-pl.php"));
    QObject::connect(webView,SIGNAL(loadFinished(bool)), this, SLOT(readTimeFromWWW()));

    dateTime = QDate::currentDate();
    dateStamptext = dateTime.toString("dd/MM/yyyy");

    movie_pompa_1 = new QMovie("/media/HDD1/admin/iHome/28-02-2018/media/pompa_on.gif");
    movie_pompa_2 = new QMovie("/media/HDD1/admin/iHome/28-02-2018/media/pompa_on.gif");
    movie_cyrkulacja = new QMovie("/media/HDD1/admin/iHome/28-02-2018/media/circulation_on.gif");
    movie_wentylacja = new QMovie("/media/HDD1/admin/iHome/28-02-2018/media/wentylator.gif");
    movie_countdown = new QMovie("/media/HDD1/admin/iHome/28-02-2018/media/countdown.gif");
    movie_heat_fan = new QMovie("/media/HDD1/admin/iHome/28-02-2018/media/pump_fan.gif");

    movie_reku = new QMovie("/media/HDD1/admin/iHome/28-02-2018/media/reku_old.gif");
    movie_reku->setCacheMode(QMovie::CacheAll);
    movie_reku->jumpToFrame(ui->button_wentylator->property("current_speed").toInt());

    ui->label_19->setMovie(movie_countdown);
    ui->label_wentylator->setMovie(movie_wentylacja);
    ui->pump_fan->setMovie(movie_heat_fan);
    movie_wentylacja->start();
    movie_heat_fan->start();
    pir_status();
    rekuperator(1);

    pompa_off.load("/media/HDD1/admin/iHome/28-02-2018/media/pompa_off.png");
    temp_on.load("/media/HDD1/admin/iHome/28-02-2018/media/thermo_true.png");
    temp_off.load("/media/HDD1/admin/iHome/28-02-2018/media/thermo_false.png");
    temp_dis.load("/media/HDD1/admin/iHome/28-02-2018/media/thermo_0.png");
    con_err_off.load("/media/HDD1/admin/iHome/28-02-2018/media/con_err_off.png");
    con_err_on.load("/media/HDD1/admin/iHome/28-02-2018/media/con_err_on.png");
    ex_button.load("/media/HDD1/admin/iHome/28-02-2018/media/ex.png");
    map_button.load("/media/HDD1/admin/iHome/28-02-2018/media/map.png");

    //cyrkulacja
    connect(movie_cyrkulacja, &QMovie::frameChanged, [=]() {
        ui->pushButton_33->setIcon(movie_cyrkulacja->currentPixmap());
        ui->pushButton_33->setIconSize(QSize(45,45));
    });

    //REKUPERACJA
    //przełącznik
    reku_timer = new QTimer(this);
    connect(reku_timer, &QTimer::timeout, [=](){
        const int speed = ui->button_wentylator->property("current_speed").toInt();
        const int step = speed == 0 ? 1:
                         speed == 2 ? -2: ui->button_wentylator->property("current_speed").toInt();

        ui->button_wentylator->setProperty("current_speed", speed + step);
        movie_reku->jumpToFrame(speed + step);
        ui->speed_label->setPixmap(movie_reku->currentPixmap());
    });
    connect(ui->button_wentylator, &QPushButton::pressed, [=] (){
        ui->speed_label->setVisible(true);
        ui->speed_label->setPixmap(movie_reku->currentPixmap());
        reku_timer->start(700);
    });

    connect(ui->button_wentylator, &QPushButton::released, [=] (){

        reku_timer->stop();
        rekuperator(ui->button_wentylator->property("current_speed").toInt());
        ui->speed_label->setVisible(false);
        if(ui->button_wentylator_2->isChecked()){ui->button_wentylator_2->setChecked(false);}
    });

    //auto OFF wietrzenia
    ventilation_timer = new QTimer(this);
    connect(ventilation_timer, &QTimer::timeout, [=](){
        if(!(--ventilation_time<0)){
        ui->label_ventilation->setText(QDateTime::fromTime_t(ventilation_time).toUTC().toString("mm:ss"));
        }else{
            ui->button_wentylator_2->setChecked(false);
        }
    });

    // WILGOTNOŚĆ, CIŚNIENIE, TEMPERATURA SYSTEMU
    humidity_timer = new QTimer(this);
    connect(humidity_timer, &QTimer::timeout, [=](){
        getHumidity();
        barometer();
        system_fan(ui->label_37->text().toDouble());
    });
    humidity_timer->start(45000);
    //
    //AKTYWACJA OGRZEWANIA / AKTYWACJA STREF OGRZEWANIA
    connect(ui->CO, &QPushButton::toggled, [=](bool checked){
        if(!checked){
            maskawysl[2]&=~0xf8;
            maskawysl[3]&=~0xfe;
            emit UDP_ReadytoSend("192.168.1.101");
            movie_pompa_1->stop();
            movie_pompa_2->stop();
            ui->label_pompa_1->setPixmap(pompa_off);
            ui->label_pompa_2->setPixmap(pompa_off);
        }
    });

    QSqlQuery *heat_qry = baza.query();
    connect(ui->th_master, &QPushButton::toggled, [=](bool checked){

        heat_qry->prepare("UPDATE temperature SET aktywna=? WHERE th='"+ui->th_master->property("th").toString()+"'");
        heat_qry->addBindValue(checked);
        heat_qry->exec();

        QLabel *p_heat_label = MainWindow::findChild<QLabel*>(ui->th_master->property("th").toString());
        p_heat_label->setProperty("status", checked);

        if(checked==true){
            ui->th_master->setStyleSheet("QPushButton {background-image: url(:/HDD1/admin/iHome/28-02-2018/media/thermo_"+QVariant(p_heat_label->property("ON").toBool()).toString()+".png);"
                                                                                                                                                        "background-repeat: none;"
                                                                                                                                                        "background-color: transparent;"
                                                                                                                                                        "border: none;}");
        }else{
            p_heat_label->setPixmap(QPixmap("/media/HDD1/admin/iHome/28-02-2018/media/thermo_" + QString::number(checked) + ".png"));
            ui->th_master->setStyleSheet("QPushButton {background-image: url(:/HDD1/admin/iHome/28-02-2018/media/thermo_0.png);"
                                       "background-repeat: none;"
                                       "background-color: transparent;"
                                       "border: none;}");
        }
    });
    //automatyczne wyłączenie świateł w pokoju
    connect(ui->a_off, &QPushButton::toggled, [=](bool checked){
        qDebug() << "POINTER CONNECTED";
        qry->prepare("UPDATE PIR SET auto_off=? WHERE code_name='"+ui->pir_dial->property("pir_name").toString()+"'");
        qry->addBindValue(checked);
        qry->exec();

        pir_button *pbp = MainWindow::findChild<pir_button*>(ui->pir_dial->property("pir_name").toString());
        pbp->setProperty("auto_off", checked);
    });


    //ALARM - w trakcie
    connect(ui->alarm_btn, &QPushButton::toggled, [=](bool checked){
        locked = checked;
    });
    //default-owe ustawienia buttonów
    QTimer::singleShot(3000, [=] (){
        read_btn_property(0);
    });

    //sprawdzenie źródeł do włączenia/wyłączenia w czasie
    QTimer *scheduledTimer = new QTimer(this);

    connect(scheduledTimer, &QTimer::timeout, [=](){
        qry->prepare("SELECT btn_no, id, tmp, stop_at, CASE WHEN strftime('%H:%M',main.start_at)='"+time_h_m+"' THEN 'true' WHEN strftime('%H:%M',main.stop_at)='"+time_h_m+"' THEN 'false' ELSE 'no_match' END AS on_off FROM main INNER JOIN sources ON main.id = sources.main_id WHERE main.type_id=2");
        if(qry->exec()){
            qDebug() << "TIMER SPRAWDZA";
            while(qry->next()){
                QString btn_no = qry->value("btn_no").toString();
                bool OF = qry->value("on_off").toBool();
                if(qry->value("on_off").toString()!="no_match"){
                    if(btn_no.contains("reku_")){
                        if(OF==true){
                            humiditywatcher=OF;
                            rekuperator(btn_no.split("_")[1].toInt());
                        }else{
                            humiditywatcher=OF;
                            rekuperator(humidityholder);
                        }
                    }else{
                        buttons_run(btn_no, OF, "no");
                    }
                    qDebug() << "AUTO_ON/OFF";
                    //usuwanie pozycji jednorazowej harmonogramu//
                    if((qry->value("tmp").toInt()==1 && OF==true && qry->value("stop_at").toString()=="") || (qry->value("tmp").toInt()==1 && OF==false)){
                        delete_schedule(qry->value("id").toInt());
                    }
                }
            }
        }
    });
    scheduledTimer->start(60000);
}

MainWindow::~MainWindow()
{
    delete ui;
    killTimer(timerId);
}

void MainWindow::showTime(){
    QTime time=QTime::currentTime();
    time_text=time.toString("hh:mm:ss");
    time_h_m=time.toString("hh:mm");
    ui->digitalclock->setText(time_text);

//*****************WYŁĄCZANIE WSZYSTKICH WYJŚĆ*****************//
     if(time_text==ui->timeEdit_3->text() && obecnosc==0){
         spimy=1;
         ui->scene_ide_spac->setChecked(true);
         LOff();
     }
     else if (time_text==ui->timeEdit_3->text() && obecnosc==1){
         spimy=1;
         ui->scene_ide_spac->setChecked(true);
     }
//****************DZIEŃ LUB NOC*********************************//

     if(time_text>=ui->label_25->text() && time_text<=ui->label_26->text()){
         dzien=1;
         spimy=0;
     }
     else{
         dzien=0;
     }
     if(time_h_m==ui->label_25->text()){
         ui->shelly_106->setProperty("mute", 0);
     }

    dateTime=QDate::currentDate();
    QString datetimetext= dateTime.toString("dd/MM/yyyy");
    ui->datatime->setText(datetimetext);

//***********ZMIANA DNIA**************//
    if(datetimetext!=dateStamptext){
        webView->mainFrame()->load(QUrl("https://calendar.zoznam.sk/sunset-pl.php"));
        //QObject::connect(webView,SIGNAL(loadFinished(bool)), this, SLOT(readTimeFromWWW()));
        dateStamptext = dateTime.toString("dd/MM/yyyy");
    }

//***************WYŚWIETLANIE TIMERA ORAZ AKTYWACJA ODLICZANIA PO NARUSZENIU STREFY W CZASIE NOCNYM****************//
        ui->label_11->setText(QDateTime::fromTime_t(odliczG).toUTC().toString("mm:ss")); //zamiana ms na minuty
        if(spimy==1 && obecnosc==1){
            movie_countdown->start();
            ui->label_19->setStyleSheet("background-image:none");
        }
}

/************************************************************** ODBIERANIE *********************************************************************/
void MainWindow::timerEvent(QTimerEvent *event){

    //*************GEOLOKALIZACJA**************
   QFile comming("/home/pi/GDrive/comming_home.csv");
   if(comming.exists()){
        if(!jestem){
            comming.remove();
            ui->scene_wracam->setChecked(true);
            jestem=true; //flaga wprowadzona ze wzgledu na sprawdzanie obecnosci pliku 2x/s a odswiezanie GDrive co 1s
        }
   }else{
       jestem=false;
   }
   //********WYLACZANIE SCEN PO WYKONANIU*************
   if(arg_check<=0 && arg_check>=(-2)){//warunek ze względu na timer 500ms - możliwe ominięcie warunku ==0
       arg_check=(-3); //wykonywanie stop
       foreach(QPushButton *sbtn, sbList){
           sbtn->setChecked(false);
       }
       qDebug() <<"Wylaczenie SCEN: " << arg_check;
   }
}

void MainWindow::receiving(){

    /**********ZAMIANA RAMKI Z TEMPERATURA NA QSTRING+WYSWIETLENIE*************/
    QString b,t,b1,t1,b2,t2,b3,t3,b4,t4,b5,t5,b6,t6,b7,t7,b8,t8,b9,t9,b10,t10,b11,t11,b12,t12,b13,t13,b14,t14,b15,t15,subzero;

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
    //****POKOJ MISI****//
    b8.append(QString("%1").arg(temperatura[27]));
    t8.append(QString("%1").arg(temperatura[28]));
    //****LOFT****//
    b9.append(QString("%1").arg(temperatura[30]));
    t9.append(QString("%1").arg(temperatura[31]));
    //****ŁAZIENKA GÓRA****//
    b11.append(QString("%1").arg(temperatura[33]));
    t11.append(QString("%1").arg(temperatura[34]));
    //****ZASOBNIK C.O.*****//
    b14.append(QString("%1").arg(temperatura[45]));
    t14.append(QString("%1").arg(temperatura[46]));
    //****KOTŁOWNIA****//
    b12.append(QString("%1").arg(temperatura[36]));
    t12.append(QString("%1").arg(temperatura[37]));
    //****ZEWNATRZ****//
    b10.append(QString("%1").arg(temperatura[39]));
    t10.append(QString("%1").arg(temperatura[40]));
    subzero.append(QString("%1").arg(temperatura[41]));
    //****GARAŻ****//
    b13.append(QString("%1").arg(temperatura[42]));
    t13.append(QString("%1").arg(temperatura[43]));
    //*****ZBIORNIK CWU*****//
    b15.append(QString("%1").arg(temperatura[50]));
    t15.append(QString("%1").arg(temperatura[51]));

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
    ui->label_22->setText(b15+"."+t15 + DC);

    ui->label_pompa_1->setProperty("status", false);
    ui->label_pompa_2->setProperty("status", false);

    //warunek do wyswietlenia temperatury ujemnej (zastosowano tylko do czujnika zewnętrznego)
    if(subzero=="1"){
        ui->label_31->setText("-"+b10+"."+t10 + DC);
    }else{
        ui->label_31->setText(b10+"."+t10 + DC);
    }
    //

    qu++;
    if (qu==30){
        //***ODCZYT NASTAW TEMPERATURY Z BAZY I PORÓWNANIE Z TEMPARATURĄ ZMIERZONĄ***
        mydbs baza(sceny);
        QSqlQuery *qry = baza.query();

        qry->prepare("SELECT * FROM temperature");
        if(qry->exec()){
            while(qry->next()){
                int INT = qry->value("frame_integer").toInt();
                int DECIMAL = qry->value("frame_decimal").toInt();
                int SET_TEMP = qry->value("value").toInt();
                int maska = qry->value("maska").toInt();
                int hex = qry->value("hex").toInt();
                QLabel *th = MainWindow::findChild<QLabel*>(qry->value("th").toString());

                //inicjacja statusu ikony
                if(qry->value("aktywna")==0 && th->property("status")==true){
                    th->setProperty("status", false);
                }
                //

                if(th->property("status")==true){
                    if(temperatura[INT]*10+temperatura[DECIMAL]-ui->dial_7->value() >= SET_TEMP){
                        th->setPixmap(temp_off);
                        th->setProperty("ON", false);
                        maskawysl[maska]&=~hex;
                    }
                    if(temperatura[INT]*10+temperatura[DECIMAL]+ui->dial_7->value() <= SET_TEMP){
                        th->setPixmap(temp_on);
                        th->setProperty("ON", true);
                        maskawysl[maska]|=hex;
                        if(ui->label_pompa_1->property("id")==maska){
                            ui->label_pompa_1->setProperty("status", true);
                        }
                        if(ui->label_pompa_2->property("id")==maska){
                            ui->label_pompa_2->setProperty("status", true);
                        }
                    }
                }else{
                    th->setPixmap(temp_dis);
                    maskawysl[maska]&=~hex;
                }
            }
        }
        delete qry;

        if(ui->CO->isChecked()){
            //*************flaga dla pomp podłogówki*******
            if(ui->label_pompa_1->property("status")==true){
                maskawysl[3]|=0x40;
                ui->label_pompa_1->setMovie(movie_pompa_1);
                movie_pompa_1->start();
            }
            if(ui->label_pompa_1->property("status")==false){
                maskawysl[3]&=~0x40;
                movie_pompa_1->stop();
                ui->label_pompa_1->setPixmap(pompa_off);
            }
            if(ui->label_pompa_2->property("status")==true){
                maskawysl[3]|=0x80;
                ui->label_pompa_2->setMovie(movie_pompa_2);
                movie_pompa_2->start();
            }
            if(ui->label_pompa_2->property("status")==false){
                maskawysl[3]&=~0x80;
                movie_pompa_2->stop();
                ui->label_pompa_2->setPixmap(pompa_off);
            }

            emit UDP_ReadytoSend("192.168.1.101");
        }
        qu=0;
    }
//****************sprawdzanie obecności czujników CT**********************************//
    uint16_t id[16]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80}; //numer czujnika
    int tn[16]={48,48,48,48,48,48,48,48,49,49,49,49,49,49,49,49}; //numer bajtu kontrolnego stan czujnika
    for(int i=0; i<16 ;i++){
        if(temperatura[tn[i]]&id[i]){
            c_e.at(i)->setPixmap(con_err_off);
        }else{
            c_e.at(i)->setPixmap(con_err_on);
        }
    }

    //*********************WYŚWIETLANIE AKTYWNOŚCI CZUJEK PIR***********************//
    if("192.168.1.103"==ips){
        if(temp[1]&0x01){
            ui->pushButton_24->setChecked(true);
        }
    }
}

void MainWindow::btnFinderfromHEX()
{
    foreach(QPushButton *wanted_btn, bList)
    {
        if(temp[wanted_btn->property("mask").toInt()] & wanted_btn->property("hexx").toInt()){
            if(expanded(wanted_btn, "expanded")==true){
                QTimer::singleShot(200, [=]() {
                    if(temp[wanted_btn->property("mask").toInt()] & wanted_btn->property("hexx").toInt()){
                        btn_exp(wanted_btn->property("expanded_btn_name").toString());
                    }else if(expanded(wanted_btn, "mapped")==true){
                        btn_exp(wanted_btn->property("mapped_btn_name").toString());
                    }else{
                        wanted_btn->setChecked(!wanted_btn->isChecked());
                    }
                });
            }else if(expanded(wanted_btn, "mapped")==true){
                btn_exp(wanted_btn->property("mapped_btn_name").toString());
            }else{
                if(temp[0]||temp[1]||temp[2]||temp[4]||temp[5]){wanted_btn->setChecked(!wanted_btn->isChecked());}
            }
        }
    }

    //wejścia z krańcówek bramy
    if(temp[3] & 0x01) {
        //otwarta = ui->pushButton_16->property("open_state").toInt();
        if (otwarta==0){
            QPixmap bBrama("/media/HDD1/admin/iHome/28-02-2018/media/gdoorup.png");
            QIcon biBrama(bBrama);
            ui->pushButton_16->setIcon(biBrama);
            ui->pushButton_16->setIconSize(bBrama.rect().size());
            otwarta=1;
        }
        if (otwarta==2){
            QPixmap bBrama("/media/HDD1/admin/iHome/28-02-2018/media/gdoordown.png");
            QIcon biBrama(bBrama);
            ui->pushButton_16->setIcon(biBrama);
            ui->pushButton_16->setIconSize(bBrama.rect().size());
            otwarta=1;
        }
    }
    if(temp[3] & 0x02) {
        QPixmap bBrama("/media/HDD1/admin/iHome/28-02-2018/media/gdooropen.png");
        QIcon biBrama(bBrama);
        ui->pushButton_16->setIcon(biBrama);
        ui->pushButton_16->setIconSize(bBrama.rect().size());
        otwarta=2;
        QTime GOT=QTime::currentTime();
        ui->label_otime->setText(GOT.toString());
    }

    //**************zmiana statusu/ikony bramy na ZAMKNIĘTA po zamknięciu***********
    if(otwarta == 1){

        if(temp[3] & 0x01){
            qDebug() << "NADAL W RUCHU";
        }
        else{
            QPixmap bBrama("/media/HDD1/admin/iHome/28-02-2018/media/gdoorclose.png");
            QIcon biBrama(bBrama);
            ui->pushButton_16->setIcon(biBrama);
            ui->pushButton_16->setIconSize(bBrama.rect().size());
            otwarta=0;
            QTime GOT=QTime::currentTime();
            ui->label_ctime->setText(GOT.toString());
        }
    }
}

void MainWindow::ClickedscenebtnFinder(bool checked)
{
    if(checked){
        arg_check=0;
        QString buttons;
        QString objname = QObject::sender()->objectName();
        QString scena = objname.split("_")[1];

        mydbs baza(sceny);
        qDebug() << &baza;
        QSqlQuery *qry = new QSqlQuery(baza.getDatabase());
        qry->prepare("select * from scenes where description LIKE '%"+scena+"%'");
        if(qry->exec()){
            while(qry->next()){
                for (int i=1; i<qry->record().count(); i++){//nie pobieramy wartości z opcji: źródła
                    if(qry->value(i).toString() == "0"){
                        arg[i] = 0;
                    }else{
                        arg[i] = qry->value(i).toInt();
                        arg_check++;
                    }
                }
                //buttony z opcji źródla
                buttons = qry->value(15).toString();
            }
            scene_executor(arg, scena, buttons);
        }
        delete qry;
}else{
        scene_active=false;
        scene_driving=false;
        movie_countdown->stop();
        odliczG=-1;
    }

}

//*******************************WYJAZD Z DOMU********************//
void MainWindow::wyjezdzam(){

    int gate_time = ui->brama_t->value()*60000;
    switch(driving){
    case garage_gate:
        if(otwarta==0){
            gates(0,false,0);
        }
        break;
    case main_gate:
    {
        gates(1,true,gate_time);
        break;
    }
    case both:
    {
        if(otwarta==0){
            gates(0,true,20000);
        }
        if(otwarta==2){
            gates(1,true,gate_time);
        }
        break;
    }
    case Matylda:
        gates(0,true, 4000);
        driving=deactivated;
        break;
    case deactivated:
        gates(0,false,0);
        break;
    case main_gate_prt:
        gates(2,true,gate_time);
        break;
    case close_1:
        ui->shelly_107->setChecked(false);
        break;
    case close_2:
        ui->_helly_107_2->setChecked(false);
        break;
    }
}

//konfiguracja harmonogramów
void MainWindow::on_button_23_clicked()
{
    int id_rand = QRandomGenerator::global()->bounded(1,100000);
    QString db_time,
            db_pirhex = nullptr,
            db_pirtext = nullptr,
            db_start = nullptr,
            db_stop = nullptr,
            db_sunset = nullptr,
            db_sunrise = nullptr,
            db_timername = nullptr;
    int db_suntimewatch = NULL;
    int it = 0,
        db_tmp = NULL;

    if((ui->listWidget->currentRow()>=0 && ui->listWidget_2->currentRow()>=0) || (ui->button_31->isChecked() && ui->listWidget_2->currentRow()>=0 && (ui->checkBox_t1->isChecked() || ui->checkBox_t2->isChecked()))){

        QListWidgetItem *new_item = new QListWidgetItem;
        new_item->setData(Qt::UserRole, QVariant(id_rand));
        QList<QString> scheduledbtn;
        QString tempnames, icon;
        QList<QListWidgetItem*> itempirlist = ui->listWidget->selectedItems();
        selected_sources(scheduledbtn, tempnames);

        if(ui->button_29->isChecked()){ //akcje wieczorne - znacznik 1
            db_pirhex = QString::number(hexx[(ui->listWidget->currentRow())+1]);
            db_pirtext = ui->listWidget->currentItem()->text();
            db_time = "1";
            icon = "/media/HDD1/admin/iHome/28-02-2018/media/evening.png";
            new_item->setText(itempirlist.takeFirst()->text()+" -> "+tempnames);

            if(ui->sensorButton->isChecked()){
                db_timername = QString("timer_%1").arg(time_text.split(":")[2]);
                MyTimer(this, SLOT(offfff()),db_timername);
                icon = "/media/HDD1/admin/iHome/28-02-2018/media/evening_timer.png";
            }
        }
        if(ui->button_43->isChecked()){ //akcje dzienne - znacznik 3
            db_pirhex = QString::number(hexx[(ui->listWidget->currentRow())+1]);
            db_pirtext = ui->listWidget->currentItem()->text();
            db_time = "3";
            icon = "/media/HDD1/admin/iHome/28-02-2018/media/day.png";
            new_item->setText(itempirlist.takeFirst()->text()+" -> "+tempnames);
            if(ui->sensorButton_3->isChecked()){
                db_timername = QString("timer_%1").arg(time_text.split(":")[2]);
                MyTimer(this, SLOT(offfff()),db_timername);
                icon = "/media/HDD1/admin/iHome/28-02-2018/media/day_timer.png";
            }
        }
        if(ui->button_30->isChecked()){ //akcje stałe - znacznik 0
            db_pirhex = QString::number(hexx[(ui->listWidget->currentRow())+1]);
            db_pirtext = ui->listWidget->currentItem()->text();
            db_time = "0";
            icon = "/media/HDD1/admin/iHome/28-02-2018/media/allday.png";
            new_item->setText(itempirlist.takeFirst()->text()+" -> "+tempnames);
            if(ui->sensorButton_2->isChecked()){
                db_timername = QString("timer_%1").arg(time_text.split(":")[2]);
                MyTimer(this, SLOT(offfff()),db_timername);
                icon = "/media/HDD1/admin/iHome/28-02-2018/media/allday_timer.png";
            }
        }
        if(ui->button_31->isChecked()){ //akcje czasowe - znacznik 2
            db_time = "2";
            db_pirhex = nullptr;
            db_pirtext = nullptr;
            if(ui->checkBox_t1->isChecked()){db_start = ui->timeEdit->text();}
            if(ui->checkBox_t2->isChecked()){db_stop = ui->timeEdit_2->text();}
            db_timername = nullptr;
            if(ui->trackButton->isChecked()){
                db_suntimewatch = 1;
                db_sunset = ui->label_26->text();
                db_sunrise = ui->label_25->text();
                icon = "/media/HDD1/admin/iHome/28-02-2018/media/timer_tracking.png";
            }else{
                icon = "/media/HDD1/admin/iHome/28-02-2018/media/timer_3_on.png";
            }
            new_item->setText(db_start + " - " + db_stop + " -> " + tempnames);
        }
        if(ui->button_tmp->isChecked()){
            db_tmp = 1;
            icon.replace(QString(".png"), QString("_tmp.png"));
        }
        new_item->setIcon(QIcon(icon));
        ui->listWidget_3->addItem(new_item);
        ui->listWidget->clearSelection();
        ui->listWidget_2->clearSelection();

        mydbs baza(sceny);
        QSqlQuery *qry = new QSqlQuery(baza.getDatabase());

        qry->prepare("INSERT INTO main VALUES ('"+QString::number(id_rand)+"','"+db_time+"','"+db_pirhex+"','"+db_pirtext+"','"+db_start+"','"+db_stop+"','"+QString::number(db_suntimewatch)+"','"+db_sunset+"','"+db_sunrise+"','"+db_timername+"','"+QString::number(db_tmp)+"')");
        if(qry->exec()){
            foreach(QString db_btn, scheduledbtn){
                QString f = tempnames.split(",")[it];
                qry->prepare("INSERT INTO sources VALUES ('"+QString::number(id_rand)+"', '"+db_btn+"', '"+f+"')");
                qry->exec();
                it++;
            }
        }
        delete qry;
    }
}

//usuwanie pozycji z harmonogramu//
void MainWindow::on_button_27_clicked()
{
    delete_schedule(QVariant(ui->listWidget_3->currentItem()->data(Qt::UserRole)).toInt());

}

void MainWindow::on_button_OFF_clicked()
{
    LOff();
}

void MainWindow::offfff(){
    QString t_name = QObject::sender()->objectName();
    //data_logger(t_name);
    mydbs baza(sceny);
    QSqlQuery *qry = new QSqlQuery(baza.getDatabase());
    qry->prepare("SELECT btn_no,id,tmp FROM main INNER JOIN sources ON main.id = sources.main_id WHERE timer_name='"+t_name+"'");
    if(qry->exec()){
        while(qry->next()){
            QString btn_no = qry->value("btn_no").toString();
            if(btn_no.contains("reku_")){
                rekuperator(ui->button_wentylator->property("current_speed").toInt());
            }else{
                buttons_run(btn_no, false, "no");
            }
            if(qry->value("tmp").toInt() == 1){
                delete_schedule(qry->value("id").toInt());
            }
            //data_logger(qry->value("btn_no").toString());
        }
    }
    delete qry;
}

void MainWindow::on_button_31_toggled(bool checked)
{
    if (checked){
        ui->listWidget->setDisabled(true);
    }else{
        ui->listWidget->setDisabled(false);
    }
}

void MainWindow::readTimeFromWWW(){

    QWebElement wTime = webView->mainFrame()->findFirstElement("h1");
    QString swTime = wTime.toPlainText();
    ui->label_25->setText(swTime.mid(51,5));
    ui->label_26->setText(swTime.mid(73,5));

    sunTimeWatcher("sunset_time", ui->label_26->text(), 0, "start_at");
    sunTimeWatcher("sunrise_time", ui->label_25->text(), 11, "stop_at");
}

//****************************WYSYŁANIE Z IKON PULPITU**********************//

void MainWindow::ClickedbtnFinder(){

    QAbstractButton* button = static_cast<QAbstractButton*>(sender());
    qDebug() << button->objectName();
    if(button->isChecked()){
        maskawysl[button->property("mask").toInt()]|=button->property("hexx").toInt();
        database_access(1, button->objectName());
    }else{
        maskawysl[button->property("mask").toInt()]&=~button->property("hexx").toInt();
        database_access(0, button->objectName());
    }
    emit UDP_ReadytoSend(button->property("IP_holder").toString());

    //wyjątki
    if(button->objectName()=="pushButton_16" && button->isChecked()){
        timer_bramaStykOff->start(700);
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

void MainWindow::settemperature(int temp_value){

    mydbs baza(sceny);
    QSqlQuery *qry = baza.query();
    QString code_TEMPname = QObject::sender()->property("temp_name").toString();

    qry->prepare("UPDATE temperature SET value=? WHERE code_name='"+code_TEMPname+"' AND value <> '"+temp_value+"'");
    qry->addBindValue(temp_value);
    qry->exec();
    delete qry;

    QLabel *tmp_temp_label = MainWindow::findChild<QLabel*>(ui->temp_dial->property("temp_name").toString());
    tmp_temp_label->setText(QString::number(temp_value).insert(2,".") + DC);
}

void MainWindow::readscheduler(){

    ui->listWidget_3->clear();

    mydbs baza(sceny);
    QSqlQuery *qry = new QSqlQuery(baza.getDatabase());

    int typeID, ID;
    int sunTime;
    int tmp;
    QString pirName;
    QString btnName;
    QString start;
    QString stop;
    QString timerName,
            icon;

    qry->prepare("SELECT *, group_concat(sources.btn_desc) AS btns FROM main INNER JOIN sources ON main.id=sources.main_id GROUP BY id");
    qry->exec();
    if(qry->exec()){
        while(qry->next()){
            pirName = qry->value("pir_desc").toString();
            btnName = qry->value("btns").toString();
            typeID = qry->value("type_id").toInt();
            sunTime = qry->value("sun_time_watch").toInt();
            start = qry->value("start_at").toString();
            stop = qry->value("stop_at").toString();
            timerName = qry->value("timer_name").toString();
            tmp = qry->value("tmp").toInt();
            ID = qry->value("id").toInt();
            QListWidgetItem *new_item = new QListWidgetItem;
            new_item->setData(Qt::UserRole, QVariant(ID));
            if(typeID==0){
                icon = "/media/HDD1/admin/iHome/28-02-2018/media/allday.png";
                new_item->setText(pirName + " -> " + btnName);
                if(timerName!=nullptr){MyTimer(this, SLOT(offfff()),timerName);
                    icon = "/media/HDD1/admin/iHome/28-02-2018/media/allday_timer.png";
                }
            }
            if(typeID==1){
                icon = "/media/HDD1/admin/iHome/28-02-2018/media/evening.png";
                new_item->setText(pirName + " -> " + btnName);
                if(timerName!=nullptr){MyTimer(this, SLOT(offfff()),timerName);
                    icon = "/media/HDD1/admin/iHome/28-02-2018/media/evening_timer.png";
                }
            }
            if(typeID==2){
                if(sunTime==1){
                    icon = "/media/HDD1/admin/iHome/28-02-2018/media/timer_tracking.png";
                }else{
                    icon = "/media/HDD1/admin/iHome/28-02-2018/media/timer_3_on.png";
                }
                new_item->setText(start + " - " + stop + " -> " + btnName);
                if(timerName!=nullptr){MyTimer(this, SLOT(offfff()),timerName);}
            }
            if(typeID==3){
                icon = "/media/HDD1/admin/iHome/28-02-2018/media/day.png";
                new_item->setText(pirName + " -> " + btnName);
                if(timerName!=nullptr){MyTimer(this, SLOT(offfff()),timerName);
                    icon = "/media/HDD1/admin/iHome/28-02-2018/media/day_timer.png";
                }
            }
            if(tmp==1){
                icon.replace(QString(".png"), QString("_tmp.png"));
            }
            new_item->setIcon(QIcon(icon));
            ui->listWidget_3->addItem(new_item);
        }
    }
    delete qry;
}
//OBSŁUGA REKUPERATORA
void MainWindow::rekuperator(int speed)
{
    switch (speed) {
    case 0:
        bcm2835_gpio_write(REKU_SPEED_2, HIGH);
        bcm2835_gpio_write(REKU_SPEED_3, HIGH);
        bcm2835_gpio_write(REKU_STOP, LOW);
        break;
    case 1:
        bcm2835_gpio_write(REKU_SPEED_2, HIGH);
        bcm2835_gpio_write(REKU_SPEED_3, HIGH);
        bcm2835_gpio_write(REKU_STOP, HIGH);
        break;
    case 2:
        bcm2835_gpio_write(REKU_SPEED_3, HIGH);
        bcm2835_gpio_write(REKU_STOP, HIGH);
        bcm2835_gpio_write(REKU_SPEED_2, LOW);
        break;
    case 3:
        bcm2835_gpio_write(REKU_SPEED_2, HIGH);
        bcm2835_gpio_write(REKU_STOP, HIGH);
        bcm2835_gpio_write(REKU_SPEED_3, LOW);
        break;
    }

    movie_wentylacja->setSpeed(speed*100);
    movie_reku->jumpToFrame(speed);
    qDebug() << "USTAWIAM MOVIE REKU NA: " << movie_reku->currentFrameNumber();
    if(humiditywatcher==false){
        humidityholder=speed;
        qDebug() << "H_HOLDER:" << humidityholder;
    }
}

void MainWindow::settimers(int dial_value)
{
    mydbs baza(sceny);
    QSqlQuery *qry = baza.query();
    QString code_PIRname = QObject::sender()->property("pir_name").toString();

    qry->prepare("UPDATE PIR SET timer_time=? WHERE code_name='"+code_PIRname+"' AND timer_time <> '"+dial_value*1000+"'");
    qry->addBindValue(dial_value*1000);
    qry->exec();
    delete qry;
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

        if(humidity>ui->spinBox->value() && ui->button_wentylator->property("current_speed")==1 && spimy==0){
            ventilation=true;
            humiditywatcher=true;
            rekuperator(2);
            //ui->button_wentylator->setProperty("current_speed", 2);
        }
        if(humidity<ui->spinBox->value() && ventilation==true){
            ventilation=false;
            humiditywatcher=false;
            rekuperator(humidityholder);
            ui->button_wentylator->setProperty("current_speed", humidityholder);
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

void MainWindow::on_button_32_clicked(bool checked)
{
        emit simulating(checked);
}

void MainWindow::LOff()
{
    mydbs baza(sceny);
    QSqlQuery *qry = baza.query();

    qry->prepare("SELECT button, pir, no_off, CASE WHEN PIR.zone_activ=1 THEN 'true' WHEN PIR.zone_activ=0 THEN 'false' END AS stat FROM defaults LEFT JOIN PIR ON PIR.code_name=defaults.pir WHERE no_off=0 AND (stat=0 OR pir='null')");
    if(qry->exec()){
        while(qry->next()){
                    MainWindow::findChild<QPushButton*>(qry->value("button").toString())->setChecked(false);
        }
    }
    delete qry;
    movie_countdown->stop();
    scene_active=false;
}

void MainWindow::auto_room_off(QString room)
{
    mydbs baza(sceny);
    QSqlQuery *auto_off_qry = baza.query();
    auto_off_qry->prepare("SELECT button, room, type_id, start_at, stop_at, CASE WHEN sources.btn_no = defaults.button AND ((type_id=1 AND "+QString::number(dzien)+"=0) OR (type_id=3 AND "+QString::number(dzien)+"=1) OR (type_id=2 AND ((strftime('%H:%M',main.start_at)<'"+time_h_m+"' AND main.stop_at='') OR (strftime('%H:%M',main.start_at)<'"+time_h_m+"' AND strftime('%H:%M',main.stop_at)>'"+time_h_m+"') OR (strftime('%H:%M',main.stop_at)>'"+time_h_m+"' AND main.start_at='')))) THEN 'true' ELSE 'false' END AS SCHEDULER FROM defaults LEFT JOIN sources ON sources.btn_no = defaults.button LEFT JOIN main ON sources.main_id = main.id WHERE room = '"+room+"'");
    if(auto_off_qry->exec()){
        while(auto_off_qry->next()){
            //int type_result = auto_off_qry->value("type_id").toInt();
            if(auto_off_qry->value("SCHEDULER")==false){
                MainWindow::findChild<QPushButton*>(auto_off_qry->value("button").toString())->setChecked(false);
            }
        }
    }

}

void MainWindow::on_go_floor_clicked()
{
    int ci = ui->Tab->currentIndex();
    ui->Tab->setCurrentIndex(ci+1);
}

void MainWindow::on_button_wentylator_2_toggled(bool checked)
{
    if(checked){
        rekuperator(3);
        ventilation_timer->start(1000);
        ventilation_time = ui->ventilation_t->value()*60;
    }else{
        rekuperator(ui->button_wentylator->property("current_speed").toInt());
        ventilation_timer->stop();
    }
    ui->label_ventilation->setVisible(checked);
}

void MainWindow::on_go_ground_clicked()
{
    int ci = ui->Tab->currentIndex();
    ui->Tab->setCurrentIndex(ci-1);
}

//konfiguracja sceny
void MainWindow::on_config_clicked()
{
    int i=1;
    int j=0;
    int dt[2];
    int sci = ui->comboBox->currentIndex();
    QString sc = ui->comboBox->currentText();
    dt[0] = ui->spinBox_2->value();
    dt[1] = ui->spinBox_3->value();
    mydbs baza(sceny);
    QSqlQuery *qry = new QSqlQuery(baza.getDatabase());
    //QSqlQuery* qry = new QSqlQuery(baza.mydb);
    /*qry->prepare("INSERT INTO scenes (odliczanie, dane, 'wylacz wszystkie swiatla', 'wylacz rekuperacje', "
                 "'obniz temperature o', dane_2, 'uzbroj alarm', 'TV on', 'Brama garazowa', 'Brama wjazdowa', 'uchyl brame garazowa',"
                 "'uchyl brame wjazdowa' ) VALUES (1,1,1,1,1,1,1,1,1,1,1,1)");*/
    qry->prepare("UPDATE scenes SET odliczanie=?, 'wylacz wszystkie swiatla'=?, 'wylacz rekuperacje'=?, "
                     "'obniz temperature o'=?, 'uzbroj alarm'=?, 'TV on'=?, 'Brama garazowa'=?, 'Brama wjazdowa'=?, 'uchyl brame garazowa'=?,"
                     "'uchyl brame wjazdowa'=?, zrodla=?, 'wycisz dzwonek'=?, dane=?, dane_2=?, names=?, buttons=? WHERE description='"+sc+"'");
    foreach(QCheckBox *chbox, chlist){
        if(chbox->isChecked()==false){
            if(chbox->text().contains("[")){
                dt[j]=0;
                j++;
            }
            arg_config[i]=0;
        }else{
            arg_config[i]=1;
            if(chbox->text().contains("[")){
                j++;
            }
        }
        qry->addBindValue(arg_config[i]);
        i++;
    }
    //aktaulizacja komórek 'dane', 'dane_2' oraz źródeł do załączenia w bazie danych
    QList<QString> scene_scheduledbtn;
    QString scene_tempnames;
    QString array_buttons_holder = "";
    if(arg_config[11] && ui->listWidget_2->currentRow()>=0){
        selected_sources(scene_scheduledbtn,scene_tempnames);

        for(int i=0; i<scene_scheduledbtn.count(); i++){
            array_buttons_holder += scene_scheduledbtn.value(i) + ",";
        }
    }else{
        arg_config[11]=0;
        array_buttons_holder="0";
        scene_tempnames = "0";
    }
    qry->addBindValue(dt[0]);
    qry->addBindValue(dt[1]);
    qry->addBindValue(scene_tempnames);
    qry->addBindValue(array_buttons_holder);

    qry->exec();
    ui->comboBox->setCurrentIndex(-1);
    ui->comboBox->setCurrentIndex(sci);
    ui->config_cl->click();
    ui->listWidget_2->clearSelection();
}

//wyswietlanie konfiguracji sceny
void MainWindow::on_comboBox_currentIndexChanged(const QString &arg1)
{
    ui->listWidget_4->clear();
    mydbs baza(sceny);
    QSqlQuery *qry = new QSqlQuery(baza.getDatabase());
    qry->prepare("SELECT * FROM scenes WHERE description ='"+arg1+"'");
    if(qry->exec()){
        while(qry->next()){
            for (int i=1; i<qry->record().count(); i++){
                if(qry->value(i).toString() == "0" || qry->record().fieldName(i).contains("buttons")){
                    //PUSTO
                }else{
                    if(qry->record().fieldName(i+1).contains("dane") || qry->record().fieldName(i+1).contains("names")) {
                        ui->listWidget_4->addItem(qry->record().fieldName(i) + ": " + qry->value(i+1).toString());
                        i++;
                    }else{
                        ui->listWidget_4->addItem(qry->record().fieldName(i));
                    }
                }
            }
        }
    }
}

void MainWindow::on_resetButton_clicked()
{
    emit UDP_ReadytoSend("192.168.1.102");
}

//***śledzenie czasu wschodu/zachodu złońca i zmiana czasu świecenia źródła***
void MainWindow::sunTimeWatcher(QString sunTime, QString source, int pos, QString editingTime)
{
    QString time, ssTIME;
    QListWidget *widget = ui->listWidget_3;
    mydbs baza(sceny);
    QSqlQuery *qry = new QSqlQuery(baza.getDatabase());
    qry->prepare("SELECT * FROM main WHERE sun_time_watch=1");
    if(qry->exec()){
        qry->next();
        time = qry->value(sunTime).toString();
        ssTIME = qry->value(editingTime).toString();
    }
    if(!(time=="0")){
        QTime old_sun_time = QTime::fromString(time, "HH:mm");
        QTime now_sun_time = QTime::fromString(source, "HH:mm");
        QTime old_oo_time = QTime::fromString(ssTIME, "hh:mm:ss");
        QTime new_oo_time = QTime::fromString(ssTIME, "hh:mm:ss").addSecs((now_sun_time.secsTo(old_sun_time))*(-1));

        QList<QListWidgetItem*> item = widget->findItems(old_oo_time.toString(), Qt::MatchContains);
        int rowi = widget->row(item.at(0));
        QString row = widget->item(rowi)->text();
        row.replace(pos,8, new_oo_time.toString());
        widget->item(rowi)->setText(row);
        //qDebug() << old_sun_time;
        //qDebug() << old_oo_time;

        qry->prepare("UPDATE main SET '"+sunTime+"'=?, '"+editingTime+"'=? WHERE sun_time_watch=1");
        qry->addBindValue(now_sun_time.toString("hh:mm"));
        qry->addBindValue(new_oo_time.toString());
        qry->exec();
    }
}

void MainWindow::WoL(QString macc, QString addr)
{
    char mac[6];
    char woldata[102];
    int mac_size=6;
    QUdpSocket *WOL = new QUdpSocket(this);

    int noused = sscanf (macc.toLatin1().data(), "%2x-%2x-%2x-%2x-%2x-%2x",
                        & mac[0], & mac[1], & mac[2], & mac[3], & mac[4], & mac[5]);

    for(int i=0; i<6; i++){
        woldata[i]=0xff;
    }
    for(int i=0; i<16; i++){
        for(int j=0; j<6; j++){
            woldata[j+mac_size]=mac[j];
        }     
        mac_size+=6;
    }
    qDebug() << noused;
    qDebug() << woldata;
    WOL->writeDatagram(woldata, QHostAddress(addr),7);
}

void MainWindow::scene_executor(int *arg, QString &aktywna_scena, QString &buttons)
{
    QString backi;
    QList<int> backv;

    if(arg[1]==1){//timer
        scene_active = true;
        movie_countdown->start();
        ui->label_19->setStyleSheet("background-image:none");
        arg_check--;
    }
    if(arg[3]==1){//swiatla
        LOff();
    }
    if(arg[4]>=1){ //rekuperator
        if(!(aktywna_scena=="wracam")){//***dodanie do sceny 'wracam' w celu odwołania po powrocie***
            //backv.append(ui->dial_12->value());
        }else {
            backv.append(0);
        }
        backi.append("'wylacz rekuperacje'=?");
        //ui->dial_12->setValue(arg[4]);
        arg_check--;
    }
    if(arg[5]==1){ //temperatura
        foreach(QDial* dL, dList){
            dL->setValue(dL->value()-(arg[6]*10));
        }
        arg_check-=2;
        if(!(aktywna_scena=="wracam")){//***dodanie do sceny 'wracam' w celu odwołania po powrocie***
            backv.append(1);
            backv.append(-1*(arg[6]));
        }else{
            backv.append(0);
            backv.append(0);
        }
        if(backi.isNull()){
            backi.append("'obniz temperature o'=?,dane_2=?");
        }else{
            backi.append(",'obniz temperature o'=?,dane_2=?");
        }
    }
    if(arg[7]==1){ //alarm
        //funkcja do wprowadzenia
        arg_check--;
        if(!(aktywna_scena=="wracam")){//***dodanie do sceny 'wracam' w celu odwołania po powrocie***
            backv.append(1);
        }else{
            backv.append(0);
        }
        if(backi.isNull()){
            backi.append("'uzbroj alarm'=?");
        }else{
            backi.append(",'uzbroj alarm'=?");
        }
    }
    if(arg[8]==1){ //TV
        WoL("44-5C-E9-9F-48-3A", "192.168.1.17");
        arg_check--;
    }
    if(arg[9]==1){//brama garazowa
        scene_driving=true;
        driving=garage_gate;
        if(aktywna_scena=="wracam"){
            wyjezdzam();
        }
    }
    if(arg[10]==1){//brama wjazdowa
        scene_driving=true;
        driving=main_gate;
        if(aktywna_scena=="wracam"){
            wyjezdzam();
        }
    }
    if(arg[9]==1 && arg[10]==1 && !(aktywna_scena=="wracam")){//obie
        driving=both;
    }
    if(arg[11]){//Matylka
        driving=Matylda;
        wyjezdzam();
    }
    if(arg[12]){//Skrzydło bramy wjazdowej
        driving=main_gate_prt;
        wyjezdzam();
        arg_check--;
    }
    if(arg[13]){//źródła
        if(!dzien){
            QList<QString> scene_scheduledbtn;

            for(int i=0;i<buttons.split(",").count(); i++){
                scene_scheduledbtn.append(buttons.split(",")[i]);
            }
            foreach(QString btn, scene_scheduledbtn){
                //bList.at(btn)->setChecked(true);
                MainWindow::findChild<QPushButton*>(btn)->setChecked(true);
            }
        }
        arg_check-=3;
    }
    if(arg[16]){//dzwonek
        ui->shelly_106->setProperty("mute", 1);
    }
    //***dodanie do sceny 'wracam' składników do odwołania po powrocie***
    mydbs baza(sceny);
    QSqlQuery *qry = new QSqlQuery(baza.getDatabase());
    qry->prepare("UPDATE scenes SET "+backi+" WHERE description='wracam'");
    foreach(int x, backv){
        qry->addBindValue(x);
        qDebug() << backi << " " << backv;
    }
    qry->exec();
    delete qry;
}

void MainWindow::gates(int type, bool timer, int ms)
{
    switch(type){
    case 0:
        ui->pushButton_16->click();
        break;
    case 1:
        ui->shelly_107->setChecked(true);
        driving=close_1;
        break;
    case 2:
        ui->_helly_107_2->setChecked(true);
        driving=close_2;
        break;
    }
    if(timer){QTimer::singleShot(ms,this,SLOT(wyjezdzam()));}
    arg_check--;
    scene_driving=false;
}

void MainWindow::on_config_cl_clicked()
{
    foreach(QCheckBox *chbox, chlist){
        chbox->setAutoExclusive(false);
        chbox->setChecked(false);
        qDebug() << &chbox;
    }
    ui->conf_ch_1->setAutoExclusive(true);
    ui->conf_ch_2->setAutoExclusive(true);

}

void MainWindow::selected_sources(QList<QString> &scheduledbtn, QString &tempnames)
{
    int a=0;
    QList<QListWidgetItem*> itemtextlist = ui->listWidget_2->selectedItems();
    QModelIndexList rowsy = ui->listWidget_2->selectionModel()->selectedRows(0);
    foreach(QModelIndex rows, rowsy){
        tempnames.append(itemtextlist.at(a)->text() + ", ");
        scheduledbtn.append(itemtextlist.at(a)->whatsThis());
        a++;
        //qDebug() << itemtextlist.at(a)->whatsThis();
    }
}

void MainWindow::buttons_run(QString j, bool tof, QString c)
{
    QAbstractButton *the_button = MainWindow::findChild<QPushButton*>(j);
    if(c!="click"){
        the_button->setChecked(tof);
    }else{
        the_button->click();
    }

}
//aktywowanie/deaktywowanie czujek PIR
void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    int value;
    mydbs pir_set(sceny);
    if(item->checkState()==Qt::CheckState::Checked){
        value=1;
    }else{
        value=0;
    }
    pir_set.myqueries("PIR", item->text(),value, true, "nazwa", "aktywna");
}

void MainWindow::pir_status()
{
    QString name;
    mydbs pir_check(sceny);
    QSqlQuery *pir_check_qry = new QSqlQuery(pir_check.getDatabase());

    int pirs = ui->listWidget->count();
    for(int x=0; x<pirs; x++){
        name = ui->listWidget->item(x)->text();
        pir_check_qry->prepare("SELECT * FROM PIR WHERE nazwa = '"+name+"' ");
        if(pir_check_qry->exec()){
            while(pir_check_qry->next()){
                if(pir_check_qry->value("aktywna").toInt()==1){
                    ui->listWidget->item(x)->setCheckState(Qt::CheckState::Checked);
                }else{
                    ui->listWidget->item(x)->setCheckState(Qt::CheckState::Unchecked);
                }
                pir_button *pbp = MainWindow::findChild<pir_button*>(pir_check_qry->value("code_name").toString());
                pbp->setProperty("auto_off", pir_check_qry->value("auto_off").toBool());
            }
        }
    }
    delete pir_check_qry;
}

void MainWindow::mqtt_processor(QString msg)
{
    translator(msg);
    //obsługa głosowa scen
    foreach(QPushButton *sbtn, sbList){
        QRegularExpression re_msg(sbtn->accessibleName(), QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch m_msg = re_msg.match(msg);
        if(m_msg.hasMatch()){
            sbtn->setChecked(true);
        }
    }
    //obsługa głosowa świateł
    QList<QString> split_msg = msg.split(" ");
    int w_count = 0;
    QListWidgetItem* source;
    for(int i=0; i<split_msg.count();i++){
        w_count++;
        QList<QListWidgetItem*> init_fi = ui->listWidget_2->findItems(split_msg[i], Qt::MatchFlag::MatchContains);
        if((init_fi.count()>1) && (split_msg.count()>w_count)){
            QList<QListWidgetItem*> main_fi = ui->listWidget_2->findItems(split_msg[i] + " " + split_msg[w_count], Qt::MatchFlag::MatchFixedString);
            if(main_fi.count()>0){
                source = main_fi.takeFirst();
                i++;
                buttons_run(source->whatsThis(), true, "click");
            }
        }else if(init_fi.count()==1){
            source = init_fi.takeFirst();
            buttons_run(source->whatsThis(), true, "click");
        }
    }
}
//do skończenia
void MainWindow::translator(QString &text_to_translate)
{
    for(int i=0; i<EN_WORD.count(); i++){
        QRegularExpression re(EN_WORD[i],QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch rem = re.match(text_to_translate);
        if(rem.hasMatch()){
            text_to_translate.clear();
            text_to_translate.append(ui->listWidget_2->item(i)->text());
        }
    }
}

void MainWindow::data_logger(const QString &input_data)
{
    QFile plik("/media/HDD1/admin/iHome/28-02-2018/log.txt");
    if(plik.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)){
        QTextStream on(&plik);
        on << dateStamptext << " " << time_text << " " << input_data << " " << ui->label_21->text() << "\n";
        plik.close();
    }
}

void MainWindow::show_item(bool state, QWidget *parentW, QString txt)
{
    ui->label_48->setParent(parentW);
    ui->label_48->setVisible(state);
    ui->label_48->setGeometry(111,24,128,111);

    ui->pir_dial->setParent(parentW);
    ui->pir_dial->setGeometry(130,32,90,100);
    ui->pir_dial->setVisible(state);

    ui->a_off->setParent(parentW);
    ui->a_off->setGeometry(227,28,36,36);
    ui->a_off->setVisible(state);

    ui->label_47->setParent(parentW);
    ui->label_47->setGeometry(145,57,60,41);
    ui->label_47->raise();
    ui->label_47->setVisible(state);
    ui->label_47->setText(txt);
}

void MainWindow::show_item2(QWidget *parentW, QWidget *widget, QRect geometry)
{
    widget->setParent(parentW);
    widget->setGeometry(geometry);
    if(widget->objectName()!="speed_label"){widget->setVisible(true);}

}

void MainWindow::show_item_temp(bool state, QWidget *parentW, QString txt)
{
    ui->label_32->setParent(parentW);
    ui->label_32->setVisible(state);
    ui->label_32->setGeometry(1037,14,178,160);

    ui->temp_dial->setParent(parentW);
    ui->temp_dial->setGeometry(1060,32,131,131);
    ui->temp_dial->setVisible(state);

    ui->label_dsc_1->setParent(parentW);
    ui->label_dsc_1->setGeometry(1097,84,61,30);
    ui->label_dsc_1->raise();
    ui->label_dsc_1->setVisible(state);
    ui->label_dsc_1->setText(txt);
}

void MainWindow::delete_schedule(int itm)
{
    if(itm>0){
        int count = 0;
        mydbs baza(sceny);
        QSqlQuery *qry = new QSqlQuery(baza.getDatabase());
        qry->exec("PRAGMA foreign_keys = ON");
        qry->exec("DELETE FROM main WHERE id='"+QString::number(itm)+"'");
        while(count < ui->listWidget_3->count()){
            if(QVariant(ui->listWidget_3->item(count)->data(Qt::UserRole)).toInt()==itm){
                delete ui->listWidget_3->item(count);
                break;
            }
            count++;
        }
        delete qry;
    }
}

void MainWindow::system_fan(double temp)
{
    if(temp>29){
        bcm2835_gpio_write(FAN_ON, LOW);
    }else if(temp<28.1){
        bcm2835_gpio_write(FAN_ON, HIGH);
    }
}

void MainWindow::on_button_list_currentIndexChanged(int arg1)
{
    QVariant btn_name = ui->button_list->itemData(arg1);
    QPushButton *btn = MainWindow::findChild<QPushButton*>(btn_name.toString());
    QVariant exd_btn_name = btn->property("expanded_btn_name").toString();
    QVariant map_btn_name = btn->property("mapped_btn_name").toString();
    int inx1 = ui->button_list_hold->findData(exd_btn_name, Qt::UserRole, Qt::MatchCaseSensitive);
    int inx2 = ui->button_list_hold->findData(map_btn_name, Qt::UserRole, Qt::MatchCaseSensitive);
    ui->button_list_hold->setCurrentIndex(inx1);
    ui->button_list_click->setCurrentIndex(inx2);
}

void MainWindow::on_button_set_clicked()
{
    if(ui->button_list->currentIndex()!=-1){
        QVariant btn_name = ui->button_list->currentData();
        QVariant exd_btn_name = ui->button_list_hold->currentData();
        QVariant mapped_btn_name = ui->button_list_click->currentData();
        QPushButton *btn = MainWindow::findChild<QPushButton*>(btn_name.toString());
        if(ui->button_list_hold->currentIndex()!=-1){
            btn->setProperty("expanded", true);

            //dodanie symbolu informującego o rozszerzeniu przycisku
            if(MainWindow::findChild<QLabel*>("ex_" + btn->objectName())==nullptr){
                create_icon(btn, "ex_", ex_button, 7);
            }
        }else{
            remove_icon(btn, "ex_", "expanded");
        }

        if(ui->button_list_click->currentIndex()!=-1){
            btn->setProperty("mapped", true);

            //dodanie symbolu informującego o mapowaniu przycisku
            if(MainWindow::findChild<QLabel*>("map_" + btn->objectName())==nullptr){
                create_icon(btn, "map_", map_button, 17);
            }
        }else{
            remove_icon(btn, "map_", "mapped");
        }

        btn->setProperty("expanded_btn_name", exd_btn_name.toString());
        btn->setProperty("mapped_btn_name", mapped_btn_name.toString());

        mydbs baza(sceny);
        QSqlQuery *qry = baza.query();
        qry->prepare("UPDATE defaults SET hold=?, click=? WHERE button='"+btn_name.toString()+"'");
        qry->addBindValue(exd_btn_name);
        qry->addBindValue(mapped_btn_name);
        qry->exec();
        delete qry;
    }
}

void MainWindow::on_button_read_clicked()
{
    read_btn_property(1);
}

void MainWindow::on_button_read_2_clicked()
{
    read_btn_property(0);
}

void MainWindow::btn_exp(QString b_name)
{
    QPushButton *btn = MainWindow::findChild<QPushButton*>(b_name);
    btn->setChecked(!btn->isChecked());
}

void MainWindow::on_button_clear_clicked()
{
    ui->button_list_click->setCurrentIndex(-1);
}

void MainWindow::on_button_clear_2_clicked()
{
    ui->button_list_hold->setCurrentIndex(-1);
}

void MainWindow::create_icon(QPushButton *btn, QString prefix, QPixmap &pix, int x)
{
    QLabel *label = new QLabel(btn->parentWidget());
    label->setObjectName(prefix + btn->objectName());
    label->setGeometry(btn->x()+btn->width()-x,btn->y()+btn->height()-2, 7,7);
    label->setPixmap(pix);
    label->setScaledContents(true);
    label->setStyleSheet("QLabel { background-color: transparent;"
                         "border: none;}");
    label->show();
}

void MainWindow::remove_icon(QPushButton *btn, QString prefix, QString property)
{
    btn->setProperty(property.toLocal8Bit(), false);
    QLabel *label = MainWindow::findChild<QLabel*>(prefix + btn->objectName());
    delete label;
}

void MainWindow::read_btn_property(int mode)
{
    QString btn_name, exd_btn_name, map_btn_name;
    mydbs baza(sceny);
    QSqlQuery *qry = new QSqlQuery(baza.getDatabase());
    qry->prepare("SELECT * FROM defaults");
    if(qry->exec()){
        while(qry->next()){
            btn_name = qry->value("button").toString();
            QPushButton *btn = MainWindow::findChild<QPushButton*>(btn_name);

            switch(mode){
            case 1:
                exd_btn_name = qry->value("hold").toString();
                map_btn_name = qry->value("click").toString();
                break;
            default:
                map_btn_name = "";
                exd_btn_name = qry->value("d_hold").toString();
                remove_icon(btn, "map_", "mapped");
                remove_icon(btn, "ex_", "expanded");
                break;
            }

            btn->setProperty("mapped", map_btn_name!="");
            btn->setProperty("mapped_btn_name", map_btn_name);
            btn->setProperty("expanded", exd_btn_name!="");
            btn->setProperty("expanded_btn_name", exd_btn_name);

            if((map_btn_name!="")==true){
                create_icon(btn, "map_", map_button, 17);
            }
            if((exd_btn_name!="")==true){
                create_icon(btn, "ex_", ex_button, 7);
            }

            if(qry->value("active").toInt()==1){btn->setChecked(true);}
        }
    }
    delete qry;
}

void MainWindow::database_access(int value, QString btn_name)
{
    mydbs baza(sceny);
    QSqlQuery *qry = baza.query();
    qry->prepare("UPDATE defaults SET active=? WHERE button='"+btn_name+"'");
    qry->addBindValue(value);
    qry->exec();
    delete qry;
}

void MainWindow::on_button_nooff_clicked()
{
    int index = ui->button_list->currentIndex();
    QVariant current_flag = ui->button_list->itemData(index, Qt::CheckStateRole).toInt();
    QVariant btn_name = ui->button_list->currentData();

    const int new_flag = current_flag == 0 ? 2: 0;

    ui->button_list->setItemData(index, new_flag, Qt::CheckStateRole);

    mydbs baza(sceny);
    QSqlQuery *qry = baza.query();
    qry->prepare("UPDATE defaults SET no_off=? WHERE button='"+btn_name.toString()+"'");
    qry->addBindValue(new_flag);
    qry->exec();
    delete qry;
}

void MainWindow::on_Tab_currentChanged(int index)
{
    if(ui->Tab->tabText(index) == "Parter" || ui->Tab->tabText(index) == "Piętro"){
        show_item2(ui->Tab->currentWidget(), ui->button_OFF, ui->button_OFF->geometry());
        show_item2(ui->Tab->currentWidget(), ui->label_19, ui->label_19->geometry());
        show_item2(ui->Tab->currentWidget(), ui->label_11, ui->label_11->geometry());
        show_item2(ui->Tab->currentWidget(), ui->pushButton_42, ui->pushButton_42->geometry());
        show_item2(ui->Tab->currentWidget(), ui->shelly_105, ui->shelly_105->geometry());
        show_item2(ui->Tab->currentWidget(), ui->rsi_shelly_105, ui->rsi_shelly_105->geometry());
        show_item2(ui->Tab->currentWidget(), ui->shelly_107, ui->shelly_107->geometry());
        show_item2(ui->Tab->currentWidget(), ui->_helly_107_2, ui->_helly_107_2->geometry());
        show_item2(ui->Tab->currentWidget(), ui->temp_shelly_107, ui->temp_shelly_107->geometry());
        show_item2(ui->Tab->currentWidget(), ui->rsi_shelly_107, ui->rsi_shelly_107->geometry());
        show_item2(ui->Tab->currentWidget(), ui->label_wentylator, ui->label_wentylator->geometry());
        show_item2(ui->Tab->currentWidget(), ui->button_wentylator, ui->button_wentylator->geometry());
        show_item2(ui->Tab->currentWidget(), ui->pushButton_8, ui->pushButton_8->geometry());
        show_item2(ui->Tab->currentWidget(), ui->speed_label, ui->speed_label->geometry());
        show_item2(ui->Tab->currentWidget(), ui->label_31, ui->label_31->geometry());
        show_item2(ui->Tab->currentWidget(), ui->label_35, ui->label_35->geometry());
        show_item2(ui->Tab->currentWidget(), ui->label_8, ui->label_8->geometry());
        show_item2(ui->Tab->currentWidget(), ui->label_17, ui->label_17->geometry());
    }
}

//RETURN FUNCTIONS
bool MainWindow::expanded(QPushButton *exd_btn, const char *prop)
{
    if(exd_btn->property(prop).toBool() == true){
        return true;
    }else{
        return false;
    }
}
//dostep do MainWindow z innych klas
MainWindow *MainWindow::getMainWinPtr()
{
    return pMainWindow;
}
