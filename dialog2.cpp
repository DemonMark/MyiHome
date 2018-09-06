#include "dialog2.h"
#include "ui_dialog2.h"
#include "events.h"
#include "ui_events.h"
#include <QUdpSocket>
#include "myudp.h"

extern unsigned char temp[10];
extern unsigned char maskawysl[10];
extern QString ips;
extern QString wyslij2;
int zlicz,ile,ks;

Dialog2::Dialog2(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog2)
{
    ui->setupUi(this);
    LoadSetting();

    timerId= startTimer(10);


    ui->comboBox->setCurrentIndex(ks);
    ks++;
  if(ks>=ui->comboBox->count()){

      ks=0;
    }

}

void Dialog2::SaveSetting(){
    QSettings setting ("MyCompany", "myapp");
    setting.beginGroup("events");
    QStringList zmiany2;
    int zmiany3;
    for (int index=0; index < ui->comboBox->count();index++) {

        zmiany2.append (ui->comboBox->itemText(index));
        int masz=(ui->comboBox->count());
        setting.setValue("ilekombo", masz);
        setting.setValue("modulwejsciowy" , zmiany2 );
            }

    setting.endGroup();
    qDebug() << "Saved";
}

void Dialog2::LoadSetting(){
    QSettings setting ("MyCompany", "myapp");
    setting.beginGroup("events");
    QStringList zmiany2 = setting.value("modulwejsciowy").toStringList();
    int zmiany3=setting.value("ilekombo").toInt();

    if(!zmiany2.isEmpty()){
        for (int i=0; i< zmiany2.count();i++){
            ui->comboBox->addItem(zmiany2[i]);
                    }
           }
    ile= zmiany3;
    setting.endGroup();
    qDebug() << "Loaded";
}
Dialog2::~Dialog2()
{
    delete ui;
}
void Dialog2::timerEvent(QTimerEvent *event){


    QString test=ui->comboBox->currentText();
    if(test==ips) {

if(temp[0] & 0x01) {
  ui->WE1->setPalette(Qt::green);
  ui->WE1->setAutoFillBackground(true);
 }
else {
     ui->WE1->setPalette(Qt::red);
      ui->WE1->setAutoFillBackground(true);
}

if(temp[0] & 0x02) {

    ui->WE2->setPalette(Qt::green);
    ui->WE2->setAutoFillBackground(true);
}
else {
     ui->WE2->setPalette(Qt::red);
      ui->WE2->setAutoFillBackground(true);
}
if(temp[0] & 0x04) {

    ui->WE3->setPalette(Qt::green);
    ui->WE3->setAutoFillBackground(true);
     }
else {
     ui->WE3->setPalette(Qt::red);
      ui->WE3->setAutoFillBackground(true);
}
if(temp[0] & 0x08) {

    ui->WE4->setPalette(Qt::green);
    ui->WE4->setAutoFillBackground(true);
      }
else {
     ui->WE4->setPalette(Qt::red);
      ui->WE4->setAutoFillBackground(true);
}
if(temp[0] & 0x10) {

    ui->WE5->setPalette(Qt::green);
    ui->WE5->setAutoFillBackground(true);
      }
else {
     ui->WE5->setPalette(Qt::red);
      ui->WE5->setAutoFillBackground(true);
}
if(temp[0] & 0x20) {

    ui->WE6->setPalette(Qt::green);
    ui->WE6->setAutoFillBackground(true);
     }
else {
     ui->WE6->setPalette(Qt::red);
      ui->WE6->setAutoFillBackground(true);
}
if(temp[0] & 0x40) {

    ui->WE7->setPalette(Qt::green);
    ui->WE7->setAutoFillBackground(true);
       }
else {
     ui->WE7->setPalette(Qt::red);
      ui->WE7->setAutoFillBackground(true);
}
if(temp[0] & 0x80) {

    ui->WE8->setPalette(Qt::green);
    ui->WE8->setAutoFillBackground(true);
      }
else {
     ui->WE8->setPalette(Qt::red);
      ui->WE8->setAutoFillBackground(true);
}
if(temp[1] & 0x01) {

    ui->WE9->setPalette(Qt::green);
    ui->WE9->setAutoFillBackground(true);
    }
else {
     ui->WE9->setPalette(Qt::red);
      ui->WE9->setAutoFillBackground(true);
}
if(temp[1] & 0x02) {

    ui->WE10->setPalette(Qt::green);
    ui->WE10->setAutoFillBackground(true);
      }
else {
     ui->WE10->setPalette(Qt::red);
      ui->WE10->setAutoFillBackground(true);
}
if(temp[1] & 0x04) {

    ui->WE11->setPalette(Qt::green);
    ui->WE11->setAutoFillBackground(true);
      }
else {
     ui->WE11->setPalette(Qt::red);
      ui->WE11->setAutoFillBackground(true);
}
if(temp[1] & 0x08) {

    ui->WE12->setPalette(Qt::green);
    ui->WE12->setAutoFillBackground(true);
       }
else {
     ui->WE12->setPalette(Qt::red);
      ui->WE12->setAutoFillBackground(true);
}
if(temp[1] & 0x10) {

    ui->WE13->setPalette(Qt::green);
    ui->WE13->setAutoFillBackground(true);
      }
else {
     ui->WE13->setPalette(Qt::red);
      ui->WE13->setAutoFillBackground(true);
}
if(temp[1] & 0x20) {

    ui->WE14->setPalette(Qt::green);
    ui->WE14->setAutoFillBackground(true);
      }
else {
     ui->WE14->setPalette(Qt::red);
      ui->WE14->setAutoFillBackground(true);
}
if(temp[1] & 0x40) {

    ui->WE15->setPalette(Qt::green);
    ui->WE15->setAutoFillBackground(true);
       }
else {
     ui->WE15->setPalette(Qt::red);
      ui->WE15->setAutoFillBackground(true);
}
if(temp[1] & 0x80) {

    ui->WE16->setPalette(Qt::green);
    ui->WE16->setAutoFillBackground(true);
      }
else {
     ui->WE16->setPalette(Qt::red);
      ui->WE16->setAutoFillBackground(true);
}
if(temp[2] & 0x01) {

    ui->WE17->setPalette(Qt::green);
    ui->WE17->setAutoFillBackground(true);
     }
else {
     ui->WE17->setPalette(Qt::red);
      ui->WE17->setAutoFillBackground(true);
}
if(temp[2] & 0x02) {

    ui->WE18->setPalette(Qt::green);
    ui->WE18->setAutoFillBackground(true);
      }
else {
     ui->WE18->setPalette(Qt::red);
      ui->WE18->setAutoFillBackground(true);
}
if(temp[2] & 0x04) {

    ui->WE19->setPalette(Qt::green);
    ui->WE19->setAutoFillBackground(true);
    }
else {
     ui->WE19->setPalette(Qt::red);
      ui->WE19->setAutoFillBackground(true);
}
if(temp[2] & 0x08) {

    ui->WE20->setPalette(Qt::green);
    ui->WE20->setAutoFillBackground(true);
      }
else {
     ui->WE20->setPalette(Qt::red);
      ui->WE20->setAutoFillBackground(true);
}
if(temp[2] & 0x10) {

    ui->WE21->setPalette(Qt::green);
    ui->WE21->setAutoFillBackground(true);
      }
else {
     ui->WE21->setPalette(Qt::red);
      ui->WE21->setAutoFillBackground(true);
}
if(temp[2] & 0x20) {

    ui->WE22->setPalette(Qt::green);
    ui->WE22->setAutoFillBackground(true);
      }
else {
     ui->WE22->setPalette(Qt::red);
      ui->WE22->setAutoFillBackground(true);
}
if(temp[2] & 0x40) {

    ui->WE23->setPalette(Qt::green);
    ui->WE23->setAutoFillBackground(true);
      }
else {
     ui->WE23->setPalette(Qt::red);
      ui->WE23->setAutoFillBackground(true);
}
if(temp[2] & 0x80) {

    ui->WE24->setPalette(Qt::green);
    ui->WE24->setAutoFillBackground(true);
    }
else {
     ui->WE24->setPalette(Qt::red);
      ui->WE24->setAutoFillBackground(true);
  }
    }

}
