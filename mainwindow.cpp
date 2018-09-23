#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "myudp.h"
#include "zdarzenie.h"
#include <QtGui>
#include <QLCDNumber>
#include <QUdpSocket>
#include <qabstractbutton.h>
#include <QTimer>
#include "QDebug"
#include <QSlider>
#include <QLabel>
#include <QDateTime>
#include "mytimer.h"
#include "events.h"
#include "zmienne.h"
#include <QtCore/QCoreApplication>
#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QStyle>
#include <QPixmap>
#include <QList>
#include <QIcon>
#include <QProcess>
#include <QtWebKit/QtWebKit>

QList<QPushButton*> bList; //lista przycisków
int bPos[40]={19,19,19,19,19,12,1,0,19,19,15,17,8,11,5,10,4,14,7,19,19,19,19,19,19,19,19,19,19,19,19,19,9,16,6,18,2,3,19,19}; //pozycja przycisku na liście
QList<QList<int> > outmasks; //lista masek dla harmonogramu czujek
QList<QList<int> > scheduledcs; //lista zmiennych "c" harmonogramów
QList<QList<int> > scheduledbtns; //lista przycisków harmonogramów
QList<int> scheduledtime; //lista znaczników dla włączania harmonogramu wg czasu wieczornego
QList<int> tspinBox;
QString time_text;
extern QString ips;
extern int odb;        //brak odbioru ramki
extern int q;           //odebranie konkretnej ramki
extern int c[32];
int m; //numeracja pozycji przycisku na liście
int t[3];
int liczpetle=0;
int wlaczony,reszta,zal;
extern unsigned char temp[20];
extern unsigned char temperatura[27];
extern unsigned char hexx[9];
extern QList<unsigned char> scheduledhexxpir;
extern QList<QList<unsigned char> > scheduledhexxout;
extern QList<int> bgi;
extern QList<QTimer*> scheduledtimers;
unsigned char maskawysl[10];
extern int odliczG;
int a=0;
int gn=-1;
int qu;
int flaga=0,flaga_1=0,flaga_2=0,flaga_4=0,flaga_5=0,flaga_6=0,flaga_7=0;
int dzien;  //jeśli = 1 czujki nie włączają świateł
int spimy;  //jeśli = 1 odliczanie 10min od detekcji do wyłączenia światła(wyjść)
int obecnosc; //jeśli = 1 wyłączenie świateł(wyjść) o ustalonej godzinie nie odpala się
int otwarta=0; //status bramy garażowej
extern int PIRs,PIRs_2;
extern int LOff;
extern int scheduledaction;
QDate dateTime;
QString dateStamptext;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    timerId = startTimer(10);                //czas timera 10ms
     /******************************ZEGAR************************/
    QTimer *timer = new QTimer(this);
    connect (timer,SIGNAL(timeout()),this,SLOT (showTime()));
    timer->start();
    bList = ui->tab_5->findChildren<QPushButton*>();
    //bList = MainWindow::findChildren<QPushButton*>();

    foreach(QPushButton *x, bList)
    {
        qDebug() << x->objectName() ;
    }
    //***timer styku bramy***//
    timer_bramaStykOff = new QTimer(this);
    connect(timer_bramaStykOff, SIGNAL(timeout()), this, SLOT(stykOff()));

    //PARAMETRY POCZĄTKOWE//
    ui->spinBox_3->setVisible(false);
    webView = new QWebView(this);
    webView->setVisible(false);
    webView->load(QUrl("https://calendar.zoznam.sk/sunset-pl.php"));
    QObject::connect(webView,SIGNAL(loadFinished(bool)), this, SLOT(readTimeFromWWW()));
    dateTime = QDate::currentDate();
    dateStamptext = dateTime.toString("dd/MM/yyyy");
}

MainWindow::~MainWindow()
{
    //writeSetting();
    delete ui;
    killTimer(timerId);
}

void MainWindow::showTime(){
    QTime time=QTime::currentTime();
    time_text=time.toString("hh:mm:ss");
    ui->digitalclock->setText(time_text);

//*****************WYŁĄCZANIE WSZYSTKICH WYJŚĆ*****************//
     if (time_text==ui->timeEdit_3->text() && obecnosc==0){
         spimy=1;
         odliczG=1;
         MyUDP client;
         client.lightsOff();         
     }
     else if (time_text==ui->timeEdit_3->text() && obecnosc==1){
         spimy=1;
     }
//****************DZIEŃ LUB NOC*********************************//

     if (time_text>=ui->label_25->text() && time_text<=ui->label_26->text()){
         dzien=1;
         spimy=0;
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

//***************WYŚWIETLANIE TIMERÓW****************//

    ui->label_11->setText(QDateTime::fromTime_t(odliczG).toUTC().toString("mm:ss")); //zamiana ms na minuty
  }

/************************************************************** ODBIERANIE *********************************************************************/
void MainWindow::timerEvent(QTimerEvent *event){


/******************TESTOWY MODUL INZ MARKA*********************/

    /**********ZAMIANA RAMKI Z TEMPERATURA NA QSTRING+WYSWIETLENIE*************/
        QString b,t,d,b1,t1,d1,b2,t2,d2,b3,t3,d3,b4,t4,d4,b5,t5,d5,b6,t6,d6,b7,t7,d7,b8,t8,d8,b9,t9,d9,b10,d10,t10;
        QString a = ".";
        a.split(" ")[0].toInt();
        //****BIBLIOTEKA****//
        b.append(QString("%1").arg(temperatura[3]));
        d=((ui->doubleSpinBox->value())+(t.append(QString("%1").arg(temperatura[4]))));
        //****SALON****//
        b1.append(QString("%1").arg(temperatura[5]));
        d1=((ui->doubleSpinBox->value())+(t1.append(QString("%1").arg(temperatura[6]))));
        //****SYPIALNIA****//
        b2.append(QString("%1").arg(temperatura[7]));
        d2=((ui->doubleSpinBox->value())+(t2.append(QString("%1").arg(temperatura[8]))));
        //****KORYTARZ DOL****//
        b3.append(QString("%1").arg(temperatura[9]));
        d3=((ui->doubleSpinBox->value())+(t3.append(QString("%1").arg(temperatura[10]))));
        //****MARYNARSKI****//
        b4.append(QString("%1").arg(temperatura[11]));
        d4=((ui->doubleSpinBox->value())+(t4.append(QString("%1").arg(temperatura[12]))));
        //****LAZIENKA DOL****//
        b5.append(QString("%1").arg(temperatura[13]));
        d5=((ui->doubleSpinBox->value())+(t5.append(QString("%1").arg(temperatura[14]))));
        //****KUCHNIA-JADALNIA****//
        b6.append(QString("%1").arg(temperatura[15]));
        d6=((ui->doubleSpinBox->value())+(t6.append(QString("%1").arg(temperatura[16]))));
        //****WIATROLAP****//
        b7.append(QString("%1").arg(temperatura[17]));
        d7=((ui->doubleSpinBox->value())+(t7.append(QString("%1").arg(temperatura[18]))));
        //****POKOJ MYSZKI****//
        b8.append(QString("%1").arg(temperatura[19]));
        d8=((ui->doubleSpinBox->value())+(t8.append(QString("%1").arg(temperatura[20]))));
        //****LOFT****//
        b9.append(QString("%1").arg(temperatura[21]));
        d9=((ui->doubleSpinBox->value())+(t9.append(QString("%1").arg(temperatura[22]))));
        //****ZEWNATRZ****//
        b10.append(QString("%1").arg(temperatura[25]));
        d10=((ui->doubleSpinBox->value())+(t10.append(QString("%1").arg(temperatura[26]))));

        ui->label_2->setText(b+"."+t);
        ui->label_6->setText(b1+"."+t1);
        ui->label_10->setText(b2+"."+t2);
        ui->label_12->setText(b3+"."+t3);
        ui->label_14->setText(b4+"."+t4);
        ui->label_16->setText(b5+"."+t5);
        ui->label_18->setText(b6+"."+t6);
        ui->label_23->setText(b7+"."+t7);
        ui->label_21->setText(b8+"."+t8);
        ui->label_29->setText(b9+"."+t9);
        ui->label_31->setText(b10+"."+t10);

        QPixmap temp_on("/media/HDD1/admin/iHome/28-02-2018/media/thermo_on.png");
        QPixmap temp_off("/media/HDD1/admin/iHome/28-02-2018/media/thermo.png");

            //**HISTEREZA BIBLIOTEKA**//
     //   if (((temp[0]*10)+temp[1]-(ui->doubleSpinBox->value())*10) >=(((ui->spinBox->value())*10)+(ui->spinBox_2->value()))){
        if (((temperatura[3]*10)+temperatura[4]-(ui->doubleSpinBox->value())*10) >=((ui->doubleSpinBox_2->value())*10)){

            ui->label_3->setPalette(Qt::red);
            ui->label_3->setAutoFillBackground(true);
            flaga=1;
        }
     //  if (((temp[0]*10)+temp[1]+(ui->doubleSpinBox->value())*10) <=(((ui->spinBox->value())*10)+(ui->spinBox_2->value()))){
        if (((temperatura[3]*10)+temperatura[4]+(ui->doubleSpinBox->value())*10) <=((ui->doubleSpinBox_2->value())*10)){
             ui->label_3->setPalette(Qt::green);
             ui->label_3->setAutoFillBackground(true);
             flaga=0;
       }
        //**HISTEREZA SYPIALNIA**//
        if (((temperatura[7]*10)+temperatura[8]-(ui->doubleSpinBox->value())*10) >=((ui->doubleSpinBox_9->value())*10)){

        ui->label_27->setPalette(Qt::red);
        ui->label_27->setAutoFillBackground(true);
        flaga_7=1;
        }
        if (((temperatura[7]*10)+temperatura[8]+(ui->doubleSpinBox->value())*10) <=((ui->doubleSpinBox_9->value())*10)){
         ui->label_27->setPalette(Qt::green);
          ui->label_27->setAutoFillBackground(true);
         flaga_7=0;
        }
        //**HISTEREZA SALON**//
         if (((temperatura[5]*10)+temperatura[6]-(ui->doubleSpinBox->value())*10) >=((ui->doubleSpinBox_3->value())*10)){

            ui->th_2->setPixmap(temp_off);

            flaga_1=1;
         }
        if (((temperatura[5]*10)+temperatura[6]+(ui->doubleSpinBox->value())*10) <=((ui->doubleSpinBox_3->value())*10)){

            ui->th_2->setPixmap(temp_on);

            flaga_1=0;
        }
        //**KUCHNIA-JADALNIA**//
         if (((temperatura[15]*10)+temperatura[16]-(ui->doubleSpinBox->value())*10) >=((ui->doubleSpinBox_4->value())*10)){

             ui->th_5->setPixmap(temp_off);
             flaga_2=1;
         }
        if (((temperatura[15]*10)+temperatura[16]+(ui->doubleSpinBox->value())*10) <=((ui->doubleSpinBox_4->value())*10)){

             ui->th_5->setPixmap(temp_on);
             flaga_2=0;
        }
        //**MARYNARSKI**//
         if (((temperatura[11]*10)+temperatura[12]-(ui->doubleSpinBox->value())*10) >=((ui->doubleSpinBox_6->value())*10)){

             ui->th_3->setPixmap(temp_off);
             flaga_4=1;
         }
        if (((temperatura[11]*10)+temperatura[12]+(ui->doubleSpinBox->value())*10) <=((ui->doubleSpinBox_6->value())*10)){

            ui->th_3->setPixmap(temp_on);
            flaga_4=0;
        }
        //**LAZIENKA DOL**//
        if (((temperatura[13]*10)+temperatura[14]-(ui->doubleSpinBox->value())*10) >=((ui->doubleSpinBox_7->value())*10)){

            ui->th_1->setPixmap(temp_off);
            flaga_5=1;
        }
        if (((temperatura[13]*10)+temperatura[14]+(ui->doubleSpinBox->value())*10) <=((ui->doubleSpinBox_7->value())*10)){

            ui->th_1->setPixmap(temp_on);
            flaga_5=0;
        }
        //**WIATROLAP**//
         if (((temperatura[17]*10)+temperatura[18]-(ui->doubleSpinBox->value())*10) >=((ui->doubleSpinBox_8->value())*10)){

            ui->th_6->setPixmap(temp_off);
            flaga_6=1;
         }
        if (((temperatura[17]*10)+temperatura[18]+(ui->doubleSpinBox->value())*10) <=((ui->doubleSpinBox_8->value())*10)){

            ui->th_6->setPixmap(temp_on);
            flaga_6=0;
        }

       qu++;

       if (qu==3000){
           if (flaga_7==0){
           maskawysl[3]|=0x80;
           MyUDP client;
           client.WYSUDP();
         }
           if (flaga_7==1){
             maskawysl[3]&=~0x80;
             MyUDP client;
             client.WYSUDP();
         }
           if (flaga_1==0){
           maskawysl[3]|=0x04;
           MyUDP client;
           client.WYSUDP();
         }
           if (flaga_1==1){
             maskawysl[3]&=~0x04;
             MyUDP client;
             client.WYSUDP();
         }
           if (flaga_2==0){
           maskawysl[3]|=0x03;
           MyUDP client;
           client.WYSUDP();
         }
           if (flaga_2==1){
             maskawysl[3]&=~0x03;
             MyUDP client;
             client.WYSUDP();
         }
           if (flaga_4==0){
           maskawysl[3]|=0x08;
           MyUDP client;
           client.WYSUDP();
         }
           if (flaga_4==1){
             maskawysl[3]&=~0x08;
             MyUDP client;
             client.WYSUDP();
         }
           if (flaga_5==0){
           maskawysl[3]|=0x10;
           MyUDP client;
           client.WYSUDP();
         }
           if (flaga_5==1){
             maskawysl[3]&=~0x10;
             MyUDP client;
             client.WYSUDP();
         }
           if (flaga_6==0){
           maskawysl[3]|=0x20;
           MyUDP client;
           client.WYSUDP();
         }
           if (flaga_6==1){
             maskawysl[3]&=~0x20;
             MyUDP client;
             client.WYSUDP();
         }
            if (flaga_1==0 || flaga_2==0 || flaga_4==0 || flaga_5==0 || flaga_6==0){
                maskawysl[3]|=0x40;
                MyUDP client;
                client.WYSUDP();
            } else {
                maskawysl[3]&=~0x40;
                MyUDP client;
                client.WYSUDP();
            }
            if (flaga==0 || flaga_7==0){
                maskawysl[3]|=0x80;
                MyUDP client;
                client.WYSUDP();
            } else {
                maskawysl[3]&=~0x80;
                MyUDP client;
                client.WYSUDP();
            }
           qu=0;

       }

//****************zaznaczanie buttonów po wykryciu pakietu************************//
if("192.168.1.100"==ips){

    m=0;

    for (int j=0; j<=4;j++){

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
                        temp[3]&=~0x01;
                        otwarta=1;

                    }
                    if (otwarta==2){
                        QPixmap bBrama("/media/HDD1/admin/iHome/28-02-2018/media/gdoordown.png");
                        QIcon biBrama(bBrama);
                        bList.at(13)->setIcon(biBrama);
                        bList.at(13)->setIconSize(bBrama.rect().size());
                        otwarta=3;

                    }

                }
                if(temp[3] & 0x02) {
                    QPixmap bBrama("/media/HDD1/admin/iHome/28-02-2018/media/gdooropen.png");
                    QIcon biBrama(bBrama);
                    bList.at(13)->setIcon(biBrama);
                    bList.at(13)->setIconSize(bBrama.rect().size());
                    otwarta=2;
                    temp[3]&=~0x03;

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
    if(otwarta == 3){

        if(temp[3] & 0x01){
            qDebug() << "NADAL ZAMYKA";
        }
        else{
            QPixmap bBrama("/media/HDD1/admin/iHome/28-02-2018/media/gdoorclose.png");
            QIcon biBrama(bBrama);
            bList.at(13)->setIcon(biBrama);
            bList.at(13)->setIconSize(bBrama.rect().size());
            otwarta=0;
        }
    }
}

//*********************WYŚWIETLANIE AKTYWNOŚCI CZUJEK PIR***********************//
    if("192.168.1.103"==ips){
        QPixmap PIR_on("/media/HDD1/admin/iHome/28-02-2018/media/pir_LD_on.png");
        if(temp[0] & 0x04){
            ui->label_pir_LD->setPixmap(PIR_on);
            temp[0]&=~0x04;
            PIRs=1;
        }
        if(temp[0] & 0x02){
            ui->label_pir_LD_2->setPixmap(PIR_on);
            temp[0]&=~0x02;
            PIRs_2=1;
        }

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



    if (PIRs == 0){
        QPixmap PIR_off("/media/HDD1/admin/iHome/28-02-2018/media/pir_off.png");
        ui->label_pir_LD->setPixmap(PIR_off);
    }
    if (PIRs_2 == 0){
        QPixmap PIR_off("/media/HDD1/admin/iHome/28-02-2018/media/pir_off.png");
        ui->label_pir_LD_2->setPixmap(PIR_off);
    }

    if (LOff==1){

        foreach(QPushButton *x, bList)
        {
            if(x->isChecked() == true){

                x->setChecked(false);
            }
        }
        LOff=0;
    }

  }


/************************************************************** WYSYLANIE ZAZNACZONYCH WYJSC (CHECKBOX)*********************************************************************/
   void MainWindow::on_WY1_toggled(bool checked){
    QByteArray tab;
    MyUDP serwer;
    MyUDP client;
     if(checked) {
         maskawysl[0]|=(0x01);

         c[1]=1;
    }
    else {
        maskawysl[0]&=~(0x01);
        c[1]=0;
    }
    client.WYSUDP();
}

   void MainWindow::on_WY2_toggled(bool checked){
    QByteArray tab;
    MyUDP serwer;
    MyUDP client;
    if(checked) {
    maskawysl[0]|=0x02;

     client.WYSUDP();
    }
    else {
        maskawysl[0]&=~0x02;
        client.WYSUDP();
        }
 }
   void MainWindow::on_WY3_toggled(bool checked){
    QByteArray tab;
    MyUDP serwer;
    MyUDP client;
     if(checked) {
    maskawysl[0]|=(0x04);


    }
    else {
        maskawysl[0]&=~(0x04);
      //  client.WYSUDP();
        }
     client.WYSUDP();
 }
   void MainWindow::on_WY4_toggled(bool checked){
    QByteArray tab;
    MyUDP serwer;
    MyUDP client;
     if(checked) {
    maskawysl[0]|=0x08;

     client.WYSUDP();
    }
    else {
        maskawysl[0]&=~0x08;
        client.WYSUDP();
        }
 }
   void MainWindow::on_WY5_toggled(bool checked){
    QByteArray tab;
    MyUDP serwer;
    MyUDP client;
        if(checked) {
    maskawysl[0]|=0x10;

     client.WYSUDP();
    }
    else {
        maskawysl[0]&=~0x10;
        client.WYSUDP();
        }
}


void MainWindow::on_WY9_toggled(bool checked){
    QByteArray tab;
    MyUDP serwer;
    MyUDP client;
     if(checked) {
    maskawysl[1]|=0x01;

     client.WYSUDP();
    }
    else {
        maskawysl[1]&=~0x01;
        client.WYSUDP();
        }
}
   void MainWindow::on_WY10_toggled(bool checked){
    QByteArray tab;
    MyUDP serwer;
    MyUDP client;
     if(checked) {
    maskawysl[1]|=0x02;

     client.WYSUDP();
    }
       else {
        maskawysl[1]&=~0x02;
        client.WYSUDP();
        }
}

   void MainWindow::on_WY20_toggled(bool checked){
    QByteArray tab;
    MyUDP serwer;
    MyUDP client;
     if(checked) {
    maskawysl[2]|=0x08;

     client.WYSUDP();
    }
    else {
        maskawysl[2]&=~0x08;
        client.WYSUDP();
        }
}
   void MainWindow::on_WY21_toggled(bool checked){
    QByteArray tab;
    MyUDP serwer;
    MyUDP client;
     if(checked) {
    maskawysl[2]|=0x10;

     client.WYSUDP();
    }
    else {
        maskawysl[2]&=~0x10;
        client.WYSUDP();
        }
}
   void MainWindow::on_WY22_toggled(bool checked){
    QByteArray tab;
    MyUDP serwer;
    MyUDP client;
     if(checked) {
    maskawysl[2]|=0x20;

     client.WYSUDP();
    }
    else {
        maskawysl[2]&=~0x20;
        client.WYSUDP();
        }
}
   void MainWindow::on_WY23_toggled(bool checked){
    QByteArray tab;
    MyUDP serwer;
    MyUDP client;
     if(checked) {
    maskawysl[2]|=0x40;

     client.WYSUDP();
    }
    else {
        maskawysl[2]&=~0x40;
        client.WYSUDP();
        }
}
   void MainWindow::on_WY24_toggled(bool checked){
    QByteArray tab;
    MyUDP serwer;
    MyUDP client;
     if(checked) {
    maskawysl[2]|=0x80;

     client.WYSUDP();
    }
    else {
        maskawysl[2]&=~0x80;
        client.WYSUDP();
        }
}
/***************************************************************ZEROWANIE ODLICZANIA WYLACZENIA CZUJNIKOW******************************************************************************/
   void MainWindow::on_slider_sliderMoved(int position){
    liczpetle=0;
}



void MainWindow::on_WY25_toggled(bool checked)
{
    QByteArray tab;
    MyUDP serwer;
    MyUDP client;
     if(checked) {
    maskawysl[3]|=0x01;

     client.WYSUDP();
    }
    else {
        maskawysl[3]&=~0x01;
        client.WYSUDP();
        }

}

void MainWindow::on_WY26_toggled(bool checked)
{
    QByteArray tab;
    MyUDP serwer;
    MyUDP client;
     if(checked) {
    maskawysl[3]|=0x02;

     client.WYSUDP();
    }
    else {
        maskawysl[3]&=~0x02;
        client.WYSUDP();
        }

}

void MainWindow::on_WY27_toggled(bool checked)
{
    QByteArray tab;
    MyUDP serwer;
    MyUDP client;
     if(checked) {
    maskawysl[3]|=0x04;

     client.WYSUDP();
    }
    else {
        maskawysl[3]&=~0x04;
        client.WYSUDP();
        }

}

void MainWindow::on_WY28_toggled(bool checked)
{
    QByteArray tab;
    MyUDP serwer;
    MyUDP client;
     if(checked) {
    maskawysl[3]|=0x08;

     client.WYSUDP();
    }
    else {
        maskawysl[3]&=~0x08;
        client.WYSUDP();
        }
}


void MainWindow::on_WY29_toggled(bool checked)
{
    QByteArray tab;
    MyUDP serwer;
    MyUDP client;
     if(checked) {
    maskawysl[3]|=0x10;

     client.WYSUDP();
    }
    else {
        maskawysl[3]&=~0x10;
        client.WYSUDP();
        }
}

void MainWindow::on_WY30_toggled(bool checked)
{
    QByteArray tab;
    MyUDP serwer;
    MyUDP client;
     if(checked) {
    maskawysl[3]|=0x20;

     client.WYSUDP();
    }
    else {
        maskawysl[3]&=~0x20;
        client.WYSUDP();
        }
}

void MainWindow::on_WY31_toggled(bool checked)
{
    QByteArray tab;
    MyUDP serwer;
    MyUDP client;
     if(checked) {
    maskawysl[3]|=0x40;

     client.WYSUDP();
    }
    else {
        maskawysl[3]&=~0x40;
        client.WYSUDP();
        }
}

void MainWindow::on_WY32_toggled(bool checked)
{
    QByteArray tab;
    MyUDP serwer;
    MyUDP client;
     if(checked) {
    maskawysl[3]|=0x80;

     client.WYSUDP();
    }
    else {
        maskawysl[3]&=~0x80;
        client.WYSUDP();
        }
}





void MainWindow::on_WY39_toggled(bool checked)
{
    QByteArray tab;
    MyUDP serwer;
    MyUDP client;
     if(checked) {
    maskawysl[4]|=0x40;

     client.WYSUDP();
    }
    else {
        maskawysl[4]&=~0x40;
        client.WYSUDP();
        }
}


void MainWindow::on_WY40_toggled(bool checked)
{
    QByteArray tab;
    MyUDP serwer;
    MyUDP client;
     if(checked) {
    maskawysl[4]|=0x80;

     client.WYSUDP();
    }
    else {
        maskawysl[4]&=~0x80;
        client.WYSUDP();
        }
}
//****************************WYSYŁANIE Z IKON PULPITU**********************//
void MainWindow::on_pushButton_toggled(bool checked)
{
    QByteArray tab;
    MyUDP client;
    qDebug() << timerId;
    if(checked && c[8]==0){
        qDebug() << "*************WARUNEK 1************" << c[8];
        maskawysl[0]|=0x80;
        client.WYSUDP();
        c[8]=1;
    }

    if(!checked && c[8]==1){
        qDebug() << "*************WARUNEK 3***********" << c[8];
        maskawysl[0]&=~0x80;
        client.WYSUDP();
        c[8]=0;
    }
}

void MainWindow::on_pushButton_2_toggled(bool checked)
{
    QByteArray tab;
    MyUDP client;

    if(checked && c[7]==0){
        maskawysl[0]|=0x40;
        client.WYSUDP();
        c[7]=1;
    }

    if(!checked && c[7]==1){
        maskawysl[0]&=~0x40;
        client.WYSUDP();
        c[7]=0;
    }
}

void MainWindow::on_pushButton_3_toggled(bool checked)
{
    QByteArray tab;
    MyUDP client;

    if(checked && c[29]==0){
        maskawysl[4]|=0x10;
        client.WYSUDP();
        c[29]=1;
    }

    if(!checked && c[29]==1){
        maskawysl[4]&=~0x10;
        client.WYSUDP();
        c[29]=0;
    }
}

void MainWindow::on_pushButton_4_toggled(bool checked)
{
    QByteArray tab;
    MyUDP client;

    if(checked && c[30]==0){
        maskawysl[4]|=0x20;
        client.WYSUDP();
        c[30]=1;
    }

    if(!checked && c[30]==1){
        maskawysl[4]&=~0x20;
        client.WYSUDP();
        c[30]=0;
    }
}

void MainWindow::on_pushButton_5_toggled(bool checked)
{
    QByteArray tab;
    MyUDP client;

    if(checked && c[24]==0){
        maskawysl[2]|=0x01;
        client.WYSUDP();
        c[24]=1;
    }

    if(!checked && c[24]==1){
        maskawysl[2]&=~0x01;
        client.WYSUDP();
        c[24]=0;
    }
}

void MainWindow::on_pushButton_6_toggled(bool checked)
{
    QByteArray tab;
    MyUDP client;

    if(checked && c[15]==0){
        maskawysl[1]|=0x40;
        client.WYSUDP();
        c[15]=1;
    }

    if(!checked && c[15]==1){
        maskawysl[1]&=~0x40;
        client.WYSUDP();
        c[15]=0;
    }
}

void MainWindow::on_pushButton_7_toggled(bool checked)
{
    QByteArray tab;
    MyUDP client;

    if(checked && c[27]==0){
        maskawysl[4]|=0x04;
        client.WYSUDP();
        c[27]=1;
    }

    if(!checked && c[27]==1){
        maskawysl[4]&=~0x04;
        client.WYSUDP();
        c[27]=0;
    }
}

void MainWindow::on_pushButton_8_toggled(bool checked)
{
    QByteArray tab;
    MyUDP client;

    if(checked && c[19]==0){
        maskawysl[2]|=0x04;
        client.WYSUDP();
        c[19]=1;
    }

    if(!checked && c[19]==1){
        maskawysl[2]&=~0x04;
        client.WYSUDP();
        c[19]=0;
    }
}

void MainWindow::on_pushButton_9_toggled(bool checked)
{
    QByteArray tab;
    MyUDP client;

    if(checked && c[13]==0){
        maskawysl[1]|=0x10;
        client.WYSUDP();
        c[13]=1;
    }

    if(!checked && c[13]==1){
        maskawysl[1]&=~0x10;
        client.WYSUDP();
        c[13]=0;
    }
}

void MainWindow::on_pushButton_10_toggled(bool checked)
{
    QByteArray tab;
    MyUDP client;

    if(checked && c[25]==0){
        maskawysl[4]|=0x01;
        client.WYSUDP();
        c[25]=1;
    }

    if(!checked && c[25]==1){
        maskawysl[4]&=~0x01;
        client.WYSUDP();
        c[25]=0;
    }
}

void MainWindow::on_pushButton_11_toggled(bool checked)
{
    QByteArray tab;
    MyUDP client;

    if(checked && c[16]==0){
        maskawysl[1]|=0x80;
        client.WYSUDP();
        c[16]=1;
    }

    if(!checked && c[16]==1){
        maskawysl[1]&=~0x80;
        client.WYSUDP();
        c[16]=0;
    }
}

void MainWindow::on_pushButton_12_toggled(bool checked)
{
    QByteArray tab;
    MyUDP client;

    if(checked && c[14]==0){
        maskawysl[1]|=0x20;
        client.WYSUDP();
        c[14]=1;
    }

    if(!checked && c[14]==1){
        maskawysl[1]&=~0x20;
        client.WYSUDP();
        c[14]=0;
    }
}

void MainWindow::on_pushButton_15_toggled(bool checked)
{
    QByteArray tab;
    MyUDP client;

    if(checked && c[6]==0){
        maskawysl[0]|=0x20;
        client.WYSUDP();
        c[6]=1;
    }

    if(!checked && c[6]==1){
        maskawysl[0]&=~0x20;
        client.WYSUDP();
        c[6]=0;
    }
}

void MainWindow::on_pushButton_16_pressed()
{
    QByteArray tab;
    MyUDP client;

    maskawysl[4]|=0x80;
    client.WYSUDP();
    timer_bramaStykOff->start(500);
}

void MainWindow::on_pushButton_16_released()
{
    /*QByteArray tab;
    MyUDP client;

    maskawysl[4]&=~0x80;
    client.WYSUDP();*/
}

void MainWindow::on_pushButton_17_toggled(bool checked)
{
    QByteArray tab;
    MyUDP client;

    if(checked && c[18]==0){
        maskawysl[2]|=0x02;
        client.WYSUDP();
        c[18]=1;
    }

    if(!checked && c[18]==1){
        maskawysl[2]&=~0x02;
        client.WYSUDP();
        c[18]=0;
    }
}

void MainWindow::on_pushButton_18_toggled(bool checked)
{
    QByteArray tab;
    MyUDP client;

    if(checked && c[11]==0){
        maskawysl[1]|=0x04;
        client.WYSUDP();
        c[11]=1;
    }

    if(!checked && c[11]==1){
        maskawysl[1]&=~0x04;
        client.WYSUDP();
        c[11]=0;
    }
}

void MainWindow::on_pushButton_19_toggled(bool checked)
{
    QByteArray tab;
    MyUDP client;

    if(checked && c[26]==0){
        maskawysl[4]|=0x02;
        client.WYSUDP();
        c[26]=1;
    }

    if(!checked && c[26]==1){
        maskawysl[4]&=~0x02;
        client.WYSUDP();
        c[26]=0;
    }
}

void MainWindow::on_pushButton_20_toggled(bool checked)
{
    QByteArray tab;
    MyUDP client;

    if(checked && c[12]==0){
        maskawysl[1]|=0x08;
        client.WYSUDP();
        c[12]=1;
    }

    if(!checked && c[12]==1){
        maskawysl[1]&=~0x08;
        client.WYSUDP();
        c[12]=0;
    }
}

void MainWindow::on_pushButton_21_toggled(bool checked)
{
    QByteArray tab;
    MyUDP client;

    if(checked && c[28]==0){
        maskawysl[4]|=0x08;
        client.WYSUDP();
        c[28]=1;
    }

    if(!checked && c[28]==1){
        maskawysl[4]&=~0x08;
        client.WYSUDP();
        c[28]=0;
    }
}

//**************wyłączony do czasu rozwiązania problemu zera na liście bList
void MainWindow::on_pushButton_13_toggled(bool checked)
{

/*
    if(checked){

        timer_wyjscie = new QTimer(this);
        connect(timer_wyjscie,SIGNAL(timeout()),this,SLOT(wyjscie()));
        timer_wyjscie->start(30000);
    }

    if(!checked){

        timer_wyjscie->stop();

    }
    */
}
//*******************************WYJŚCIE Z DOMU********************//
void MainWindow::wyjscie(){

    QByteArray tab;
    MyUDP client;
    maskawysl[4]|=0x80;
    client.WYSUDP();
    ui->pushButton_13->setChecked(false);

    timer_bramaStykOff->start(500);
}

void MainWindow::stykOff(){

    QByteArray tab;
    MyUDP client;
    maskawysl[4]&=~0x80;
    client.WYSUDP();
    timer_bramaStykOff->stop();
}

void MainWindow::on_pushButton_23_clicked()
{
    if(ui->listWidget->currentRow()>=0 && ui->listWidget_2->currentRow()>=0){
    int hexxpos;
    int om;
    gn++;
    QList<unsigned char> ingridiens;
    QList<int> outmask;
    QList<int> scheduledc;
    QList<int> scheduledbtn;
    QString tempnames;
    QString isTime;
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
            om=4;
        }
        ingridiens.insert(a,hexx[(rows.row())-hexxpos+1]);
        outmask.insert(a,om);
        scheduledc.insert(a,(rows.row())+1);
        tempnames.append(itemtextlist.at(a)->text() + ", ");
        scheduledbtn.insert(a,bPos[rows.row()]);
        a++;

    }    
    scheduledhexxpir.insert(gn,hexx[(ui->listWidget->currentRow())+1]);
    scheduledhexxout.insert(gn,ingridiens);
    outmasks.insert(gn,outmask);
    scheduledcs.insert(gn, scheduledc);
    scheduledbtns.insert(gn,scheduledbtn);
    a=0;  
    if(ui->pushButton_29->isChecked()){
        scheduledtime.insert(gn,1);
        scheduledtimers.insert(gn,0);
        isTime = "| *";
    }
    else{
        MyTimer(this, SLOT(offfff()));
        tspinBox.insert(gn,ui->listWidget->currentRow());
        scheduledtime.insert(gn,0);
        isTime = "";
    }
    ui->listWidget_3->addItem(itempirlist.takeFirst()->text()+" -> "+tempnames+isTime);
    ui->listWidget->clearSelection();
    ui->listWidget_2->clearSelection();
}
}

void MainWindow::on_pushButton_24_clicked()
{
     QProcess reku;
     QStringList params;

     params << "/home/pi/HAP-NodeJS/python/fan2.py";
     reku.start("python", params);
     reku.waitForFinished(-1);

}

void MainWindow::on_pushButton_25_clicked()
{
    QProcess reku;
    QStringList params;

    params << "/home/pi/HAP-NodeJS/python/fan1.py";
    reku.start("python", params);
    reku.waitForFinished(-1);
}

void MainWindow::on_pushButton_26_clicked()
{
    QProcess reku;
    QStringList params;
    params << "/home/pi/HAP-NodeJS/python/fanoff.py";
    reku.start("python", params);
    reku.waitForFinished(-1);
}

void MainWindow::on_pushButton_27_clicked()
{
    int cr = ui->listWidget_3->currentRow();
    delete ui->listWidget_3->currentItem();
    scheduledhexxout.removeAt(cr);
    scheduledhexxpir.removeAt(cr);
    outmasks.removeAt(cr);
    scheduledcs.removeAt(cr);
    scheduledtime.removeAt(cr);
    scheduledtimers.removeAt(cr);
    gn--;
}

void MainWindow::on_pushButton_28_clicked()
{
    odliczG=1;
    MyUDP client;
    client.lightsOff();
}

void MainWindow::on_spinBox_valueChanged(int arg1)
{
    t[0]=arg1*60000;
}

void MainWindow::on_spinBox_2_valueChanged(int arg1)
{
    t[1]=arg1*60000;
}

void MainWindow::on_spinBox_3_valueChanged(int arg1)
{
    t[2]=arg1*60000;
}

void MainWindow::offfff(){
    for(int j=0; j<=(scheduledhexxpir.length())-1;j++){
        if(scheduledtimers[j]->isActive()){
            qDebug() << "Timer aktywny:" << j;
        }
        else{
            int i=0;
            foreach(unsigned char r, scheduledhexxout[j]){
                maskawysl[outmasks.value(j)[i]]&=~r;
                MyUDP client;
                client.WYSUDP();
                c[scheduledcs.value(j)[i]]=0;
                bList.at(scheduledbtns.value(j)[i])->setChecked(false);
                i++;
            }
            scheduledtimers.at(j)->stop();
         }
    }
}

void MainWindow::on_pushButton_29_clicked()
{
    ui->pushButton_29->isChecked();
    ui->pushButton_30->setChecked(false);
    ui->pushButton_31->setChecked(false);
}

void MainWindow::on_pushButton_30_clicked()
{
    ui->pushButton_30->isChecked();
    ui->pushButton_29->setChecked(false);
    ui->pushButton_31->setChecked(false);
}

void MainWindow::on_pushButton_31_clicked()
{
    ui->pushButton_31->isChecked();
    ui->pushButton_29->setChecked(false);
    ui->pushButton_30->setChecked(false);
}

void MainWindow::on_pushButton_32_clicked()
{
    if(ui->spinBox_3->isVisible()){
        ui->spinBox_3->setVisible(false);
    }
    else{
        ui->spinBox_3->setVisible(true);
    }
}

void MainWindow::readTimeFromWWW(){
    qDebug() << "ZMIANA DATY";
    QWebElement wTime = webView->page()->mainFrame()->findFirstElement("h1");
    QString swTime = wTime.toPlainText();
    ui->label_25->setText(swTime.mid(51,5));
    ui->label_26->setText(swTime.mid(73,5));
    QObject::disconnect(webView,SIGNAL(loadFinished(bool)), this, SLOT(readTimeFromWWW()));
}
