#include "dialog.h"
#include "ui_dialog.h"
#include "events.h"
#include "ui_events.h"
#include <QUdpSocket>
#include "myudp.h"
#include <QLineEdit>
#include "zmienne.h"

extern QString ipadress;
extern QString wyslij;
extern QString wyslij3;
extern QString nazwa;
int p;

extern unsigned char maskawysl[10];

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
     timerId= startTimer(10);
    LoadSetting();


        ui->comboBox->setCurrentIndex(p);
        p++;
      if(p>=ui->comboBox->count()){
            p=0;
        }

}

void Dialog::on_pushButton_3_clicked()
{
    int usun=ui->comboBox->currentIndex();
    ui->comboBox->removeItem(usun);
}

void Dialog ::timerEvent(QTimerEvent *event){

}

void Dialog ::odczytDanych(){

}

Dialog::~Dialog()
{
    delete ui;
    p=0;
}


void Dialog::SaveSetting(){
    QSettings setting ("MyCompany", "myapp");
    setting.beginGroup("events");
    QStringList zmiany2;
    for (int index=0; index < ui->comboBox->count();index++) {
        zmiany2.append (ui->comboBox->itemText(index));
        setting.setValue("modulwyjsciowy" , zmiany2 );
            }

    setting.endGroup();
    qDebug() << "Saved";
}

void Dialog::LoadSetting(){
    QSettings setting ("MyCompany", "myapp");
    setting.beginGroup("events");
    QStringList zmiany2 = setting.value("modulwyjsciowy").toStringList();

    if(!zmiany2.isEmpty()){
        for (int i=0; i< zmiany2.count();i++){
            ui->comboBox->addItem(zmiany2[i]);
                    }
           }
    setting.endGroup();
    qDebug() << "Loaded";
}
void Dialog::on_WY1_toggled(bool checked)
{
ipadress= ui->comboBox->currentText();
  //nazwa->setText(ip);


    QByteArray tab;
    MyUDP serwer;
    MyUDP client;
     if(checked) {
    maskawysl[0]|=0x01;
     client.WYSUDP();
    }
    else {
        maskawysl[0]&=~0x01;
        client.WYSUDP();
        }
}
void Dialog::on_WY2_toggled(bool checked){
    ipadress= ui->comboBox->currentText();
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
void Dialog::on_WY3_toggled(bool checked){
    ipadress= ui->comboBox->currentText();
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
void Dialog::on_WY4_toggled(bool checked){
 QByteArray tab;
 MyUDP serwer;
 MyUDP client;
  if(checked) {
 maskawysl[0]|=0x08;
ipadress= ui->comboBox->currentText();
  client.WYSUDP();
 }
 else {
     maskawysl[0]&=~0x08;
     client.WYSUDP();
     }
}
void Dialog::on_WY5_toggled(bool checked){
 QByteArray tab;
 MyUDP serwer;
 MyUDP client;
     if(checked) {
 maskawysl[0]|=0x10;
ipadress= ui->comboBox->currentText();
  client.WYSUDP();
 }
 else {
     maskawysl[0]&=~0x10;
     client.WYSUDP();
     }
}
void Dialog::on_WY6_toggled(bool checked){
 QByteArray tab;
 MyUDP serwer;
 MyUDP client;
 if(checked) {
 maskawysl[0]|=0x20;
ipadress= ui->comboBox->currentText();
  client.WYSUDP();
 }
 else {
     maskawysl[0]&=~0x20;
     client.WYSUDP();
     }
}
void Dialog::on_WY7_toggled(bool checked){
 QByteArray tab;
 MyUDP serwer;
 MyUDP client;
  if(checked) {
 maskawysl[0]|=0x40;
ipadress= ui->comboBox->currentText();
  client.WYSUDP();
 }
 else {
     maskawysl[0]&=~0x40;
     client.WYSUDP();
     }
}
void Dialog::on_WY8_toggled(bool checked){
 QByteArray tab;
 MyUDP serwer;
 MyUDP client;
  if(checked) {
 maskawysl[0]|=0x80;
ipadress= ui->comboBox->currentText();
  client.WYSUDP();
 }
 else {
     maskawysl[0]&=~0x80;
     client.WYSUDP();
     }
}
void Dialog::on_WY9_toggled(bool checked){
 QByteArray tab;
 MyUDP serwer;
 MyUDP client;
  if(checked) {
 maskawysl[1]|=0x01;
ipadress= ui->comboBox->currentText();
  client.WYSUDP();
 }
 else {
     maskawysl[1]&=~0x01;
     client.WYSUDP();
     }
}
void Dialog::on_WY10_toggled(bool checked){
 QByteArray tab;
 MyUDP serwer;
 MyUDP client;
  if(checked) {
 maskawysl[1]|=0x02;
ipadress= ui->comboBox->currentText();
  client.WYSUDP();
 }
    else {
     maskawysl[1]&=~0x02;
     client.WYSUDP();
     }
}
void Dialog::on_WY11_toggled(bool checked){
 QByteArray tab;
 MyUDP serwer;
 MyUDP client;
  if(checked) {
 maskawysl[1]|=0x04;
ipadress= ui->comboBox->currentText();
  client.WYSUDP();
 }
 else {
    maskawysl[1]&=~0x04;
     client.WYSUDP();
     }
}
void Dialog::on_WY12_toggled(bool checked){
 QByteArray tab;
 MyUDP serwer;
 MyUDP client;
   if(checked) {
 maskawysl[1]|=0x08;
ipadress= ui->comboBox->currentText();
  client.WYSUDP();
 }
 else {
     maskawysl[1]&=~0x08;
     client.WYSUDP();
     }
}
void Dialog::on_WY13_toggled(bool checked){
 QByteArray tab;
 MyUDP serwer;
 MyUDP client;
  if(checked) {
 maskawysl[1]|=0x10;
ipadress= ui->comboBox->currentText();
  client.WYSUDP();
 }
 else {
     maskawysl[1]&=~0x10;
     client.WYSUDP();
     }
}
void Dialog::on_WY14_toggled(bool checked){
 QByteArray tab;
 MyUDP serwer;
 MyUDP client;
     if(checked) {
 maskawysl[1]|=0x20;
ipadress= ui->comboBox->currentText();
  client.WYSUDP();
 }
 else {
     maskawysl[1]&=~0x20;
     client.WYSUDP();
     }
}
void Dialog::on_WY15_toggled(bool checked){
 QByteArray tab;
 MyUDP serwer;
 MyUDP client;
  if(checked) {
 maskawysl[1]|=0x40;
ipadress= ui->comboBox->currentText();
  client.WYSUDP();
 }
 else {
     maskawysl[1]&=~0x40;
     client.WYSUDP();
     }
}
void Dialog::on_WY16_toggled(bool checked){
 QByteArray tab;
 MyUDP serwer;
 MyUDP client;
  if(checked) {
 maskawysl[1]|=0x80;
ipadress= ui->comboBox->currentText();
  client.WYSUDP();
 }
 else {
     maskawysl[1]&=~0x80;
     client.WYSUDP();
     }
}
void Dialog::on_WY17_toggled(bool checked){
 QByteArray tab;
 MyUDP serwer;
 MyUDP client;
     if(checked) {
 maskawysl[2]|=0x01;
ipadress= ui->comboBox->currentText();
  client.WYSUDP();
 }
 else {
     maskawysl[2]&=~0x01;
     client.WYSUDP();
     }
}
void Dialog::on_WY18_toggled(bool checked){
 QByteArray tab;
 MyUDP serwer;
 MyUDP client;
   if(checked) {
 maskawysl[2]|=0x02;
ipadress= ui->comboBox->currentText();
  client.WYSUDP();
 }
 else {
     maskawysl[2]&=~0x02;
     client.WYSUDP();
     }
}
void Dialog::on_WY19_toggled(bool checked){
 QByteArray tab;
 MyUDP serwer;
 MyUDP client;
  if(checked) {
 maskawysl[2]|=0x04;
ipadress= ui->comboBox->currentText();
  client.WYSUDP();
 }
 else {
     maskawysl[2]&=~0x04;
     client.WYSUDP();
     }
}
void Dialog::on_WY20_toggled(bool checked){
 QByteArray tab;
 MyUDP serwer;
 MyUDP client;
  if(checked) {
 maskawysl[2]|=0x08;
ipadress= ui->comboBox->currentText();
  client.WYSUDP();
 }
 else {
     maskawysl[2]&=~0x08;
     client.WYSUDP();
     }
}
void Dialog::on_WY21_toggled(bool checked){
 QByteArray tab;
 MyUDP serwer;
 MyUDP client;
  if(checked) {
 maskawysl[2]|=0x10;
ipadress= ui->comboBox->currentText();
  client.WYSUDP();
 }
 else {
     maskawysl[2]&=~0x10;
     client.WYSUDP();
     }
}
void Dialog::on_WY22_toggled(bool checked){
 QByteArray tab;
 MyUDP serwer;
 MyUDP client;
  if(checked) {
 maskawysl[2]|=0x20;
ipadress= ui->comboBox->currentText();
  client.WYSUDP();
 }
 else {
     maskawysl[2]&=~0x20;
     client.WYSUDP();
     }
}
void Dialog::on_WY23_toggled(bool checked){
 QByteArray tab;
 MyUDP serwer;
 MyUDP client;
  if(checked) {
 maskawysl[2]|=0x40;
ipadress= ui->comboBox->currentText();
  client.WYSUDP();
 }
 else {
     maskawysl[2]&=~0x40;
     client.WYSUDP();
     }
}
void Dialog::on_WY24_toggled(bool checked){
 QByteArray tab;
 MyUDP serwer;
 MyUDP client;
  if(checked) {
 maskawysl[2]|=0x80;
ipadress= ui->comboBox->currentText();
  client.WYSUDP();
 }
 else {
     maskawysl[2]&=~0x80;
     client.WYSUDP();
     }
}
