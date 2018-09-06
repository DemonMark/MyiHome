#include "events.h"
#include "ui_events.h"
#include <QUdpSocket>
#include "mainwindow.h"
#include "dialog.h"
#include "dialog2.h"
#include "myudp.h"
#include <QSettings>
#include <QStringList>
#include <QTabWidget>
#include <QWidget>
#include "zmienne.h"
#include "mytimer.h"

extern int p,ks;
QString ipadresslist;
QString wprowadzona;
QString zmiany;
QString input;
QByteArray ipadres;
QString wyslij2,wyslij3;
QString nazwa,wyslij;
QString zamien;
QStringList zmiany6;
QString zmiany65;
unsigned char adres;
unsigned char wprowadz[10];
int licz=0;
int liczz=0;
int znajdz,eve;
int zap;
QByteArray ktorytab;

extern QString adrsip;
extern unsigned char maskawysl[10];
extern unsigned char maskawysl[10];

events::events(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::events){
    ui->setupUi(this);
    timerId= startTimer(10);                //czas timera 10ms
    ui->ipwyj->setInputMask("000.000.000.000");
    ui->ipwej->setInputMask("000.000.000.000");
    LoadSetting();
    connect (ui->tabWidget, SIGNAL (tabCloseRequested(int)),this,SLOT (closeTab(int)));
    if(licz>0){
      odczytwyj();
       }
    if(liczz>0){
      odczytwej();
       }
}

events::~events(){

    delete ui;
}

void events ::timerEvent(QTimerEvent *event){
 //  ui->label_9->setText(ui->ipwyj->displayText() + " "+ui->nazwamodwyj->displayText());
 //  ui->label_12->setText(ui->ipwej->displayText() + " "+ui->nazwamodwej->displayText());
   ipadresslist= ui->ipwyj->displayText();
      if(licz==0){
       p=0;
   }
}

void events::odczytwyj(){
    for (int y=0; y<licz;y++){
        Dialog *event = new Dialog();
        zmiany65=zmiany6[y+2];
        ui->tabWidget->addTab(event,"Modul wyjsciowy");
        }
}

void events::odczytwej(){
    for (int s=0; s<liczz;s++){
        Dialog2 *events= new Dialog2();
        ui->tabWidget->addTab(events,"Modul wejsciowy");
        }
}

void events::SaveSetting(){
    QSettings setting ("MyCompany", "myapp");
    setting.beginGroup("events");
    QStringList zmiany2;
    QStringList zmiany1;
    QStringList zmiany3;
    QStringList zmiany4;
    zmiany6;
    QWidget sze;
    for (int index=0; index < ui->ipdodmodwej->count();index++) {
        zmiany2.append (ui->ipdodmodwej->itemText(index));
        setting.setValue("modulwejsciowy" , zmiany2 );
            }
   for ( int index=0; index < ui->tabWidget->count();index++) {
       zmiany6.append (ui->tabWidget->tabText(index));
            setting.setValue("nazwytab" , zmiany6 );
            setting.setValue("lcizbatab",ui->tabWidget->count());
            }
     for (int index=0; index < ui->nazwydodmodwyj->count();index++) {
           zmiany3.append(ui->nazwydodmodwyj->itemText(index));
         setting.setValue("modulwejsciowyip" , zmiany3 );
            }
     for (int index=0; index < ui->ipdodmodwyj->count();index++) {
            zmiany1.append (ui->ipdodmodwyj->itemText(index));
            setting.setValue("modulwyjsciowy" , zmiany1 );
            }
     for (int index=0; index < ui->nazwydodmodwej->count();index++) {
           zmiany4.append (ui->nazwydodmodwej->itemText(index));
           setting.setValue("modulwyjsciowyip" , zmiany4 );
            }
    setting.setValue("tab" , licz);
    setting.setValue("tab2" , liczz);
    setting.endGroup();
    qDebug() << "Saved";
}
void events::LoadSetting(){
    QSettings setting ("MyCompany", "myapp");
    setting.beginGroup("events");
    QStringList zmiany1 = setting.value("modulwyjsciowy").toStringList();
    QStringList zmiany2 = setting.value("modulwejsciowy").toStringList();
    QStringList zmiany3 = setting.value("modulwejsciowyip").toStringList();
    QStringList zmiany4 = setting.value("modulwyjsciowyip").toStringList();
    zmiany6 = setting.value("nazwytab").toStringList();
    int zmiany5 = setting.value("tab").toInt();
    int zmiany12 = setting.value("tab2").toInt();
    QString zmiany15 = setting.value("tab12").toString();
    int zmiany7=setting.value ("lcizbatab").toInt();

    if(!zmiany1.isEmpty()){
        for (int i=1; i< zmiany1.count();i++){
            ui->ipdodmodwyj->addItem(zmiany1[i]);
            ui->ipdodmodwyj2->addItem(zmiany1[i]);
                    }
           }
    if(!zmiany2.isEmpty()){
        for (int i=2; i< zmiany2.count();i++){
            ui->ipdodmodwej->addItem(zmiany2[i]);
            ui->ipdodmodwej2->addItem(zmiany2[i]);
                    }
           }
    if(!zmiany3.isEmpty()){
        for (int i=1; i< zmiany3.count();i++){
            ui->nazwydodmodwyj->addItem(zmiany3[i]);
                    }
           }
    if(!zmiany4.isEmpty()){
       for (int i=2; i< zmiany4.count();i++){
            ui->nazwydodmodwej->addItem(zmiany4[i]);
                    }
           }
    licz=zmiany5;
    liczz=zmiany12;
    znajdz=zmiany7;
    setting.endGroup();
    qDebug() << "Loaded";
}

void events::on_usunwyj_clicked(){

    int usun=ui->ipdodmodwyj->currentIndex();
    ui->ipdodmodwyj->removeItem(usun);
     ui->nazwydodmodwyj->removeItem(usun);
}
void events::on_dodajwyj_clicked(){

     if (ui->ipwyj->text() != ( "...")){
         ui->ipdodmodwyj->addItem(/*ui->nazwamodwyj->text() +" " + */ui->ipwyj->text());
         ui->ipdodmodwyj2->addItem(ui->nazwamodwej->text() +" " + ui->ipwej->text());
   //      ui->nazwamodwej->addItem(ui->nazwamodwyj->text());
         ui->nazwydodmodwyj->addItem(ui->nazwamodwyj->text());
        }
}
void events::on_dodajwej_clicked(){

 if (ui->ipwej->text() != ( "...")){
        ui->ipdodmodwej->addItem(/*ui->nazwamodwej->text() +" " + */ui->ipwej->text());
        ui->ipdodmodwej2->addItem(/*ui->nazwamodwej->text() +" " + */ui->ipwej->text());
    //    ui->nazwamodwej->addItem(ui->nazwamodwej->text());
        ui->nazwydodmodwej->addItem(ui->nazwamodwej->text());
         }
}
void events::on_usunwej_clicked(){

    int usun=ui->ipdodmodwej->currentIndex();
    ui->ipdodmodwej->removeItem(usun);
    ui->nazwydodmodwej->removeItem(usun);

}
void events::on_zdarzeniesave_clicked()
{
    SaveSetting();
    p=0;
}
void events::on_test_clicked()
{
     if (ui->ipdodmodwyj2->currentIndex()==0){
        if (ui->ktorewej->currentIndex()==0){       //wej1
            if (ui->czynnosc->currentIndex()==0){   //zalacz
                MyUDP client;
                maskawysl[0]|=(0x01);
                client.WYSUDP();
                     }
            if (ui->czynnosc->currentIndex()==1){
                MyUDP client;
                maskawysl[0]&=~(0x01);
                client.WYSUDP();
                     }
                }
        if (ui->ktorewyj->currentIndex()==1){       //wyj2
            if(ui->czynnosc->currentIndex()==0){   //zalacz
                MyUDP client;
                maskawysl[0]|=(0x02);
                client.WYSUDP();
                     }
            if (ui->czynnosc->currentIndex()==1){//wylcz
                MyUDP client;
                maskawysl[0]&=~(0x02);
                client.WYSUDP();
                     }
                }
        if (ui->ktorewyj->currentIndex()==2){       //wyj3
            if (ui->czynnosc->currentIndex()==0){   //zalacz
                MyUDP client;
                maskawysl[0]|=(0x04);
                client.WYSUDP();
                     }
            if (ui->czynnosc->currentIndex()==1){ //wyl
                MyUDP client;
                maskawysl[0]&=~(0x04);
                client.WYSUDP();
                     }
        }
    }
}
void events::on_otworzwyj_clicked()
{
    licz++;
    Dialog *event = new Dialog();
    ui->tabWidget->addTab(event,"Modul wyjsciowy");
    ui->tabWidget->setCurrentIndex(licz+liczz);

}
void events::on_otworzwej_clicked()
{
    liczz++;
    Dialog2 *events= new Dialog2();
    ui->tabWidget->addTab(events,"Modul wejsciowy");
    ui->tabWidget->setCurrentIndex(licz+liczz);
}
void events::closeTab(int)
{
    if(licz>0){
    licz--;
    p--;
    }
    if(liczz>0){
    liczz--;
    ks--;
    }
    if (ui->tabWidget->currentIndex()>0){
    ui->tabWidget->removeTab(ui->tabWidget->currentIndex());
    }
}

