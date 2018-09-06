/*


#include "wyjscia.h"
#include "ui_wyjscia.h"
#include <QUdpSocket>
#include "myudp.h"

extern unsigned char maskawysl[10];
wyjscia::wyjscia(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::wyjscia)
{
    ui->setupUi(this);
}

wyjscia::~wyjscia()
{
    delete ui;
}

void wyjscia::on_WY1_toggled(bool checked)
{

     QByteArray tab;
     MyUDP serwer;
     MyUDP client;
      if(checked) {
     maskawysl[0]|=(0x01);

     }
     else {
         maskawysl[0]&=~(0x01);
     }
     client.WYSUDP();

}
*/
