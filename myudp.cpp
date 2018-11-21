#include "myudp.h"
#include <QUdpSocket>
#include "mainwindow.h"
#include "mytimer.h"
#include "ui_mainwindow.h"

int odb,i;
int q=0;
unsigned char temp[20];
unsigned char tempholder[20];
unsigned char hexx[9]={0,0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80}; //tablica bitów, 0 niewykorzystywane
QList<unsigned char> scheduledhexxpir; //tablica bitów czujek definiowanych przez harmonogramy
QList<QList<unsigned char> > scheduledhexxout; //tablica bitów wyjść definiowanych przez harmonogramy
QList<int> bgi; //wybór grup zharmonogramowanych przycisków do aktywacji
extern QList<QList<int> > outmasks;
extern QList<QList<int> > scheduledcs;
extern QList<int> scheduledtime;
extern QList<int> tspinBox;
QList<QTimer*> scheduledtimers;
unsigned char temperatura[26];
int wej241,wej212;
extern unsigned char maskawysl[10];
QString ipadress;
QString ips;
extern QString time_text;
extern int dzien;
extern int spimy;
extern int flaga;
extern int obecnosc;
extern int t1;
extern int gn;
extern int t[3];
int c[41]; //wejścia (41 pozycji kontenera wliczając 0)
int odliczG;
int PIRs=0, PIRs_2=0;
int LOff=0;
int scheduledaction=0;

MyUDP::MyUDP(QObject *parent) :
    QObject(parent)
{
  socket = new QUdpSocket(this);
  socket->bind(QHostAddress("192.168.1.2"),1200);         // moj ip
  connect(socket,SIGNAL(readyRead()),this,SLOT(readyRead()));

//******************TIMER WYŁĄCZAJĄCY WYJŚCIA*******************//

  timer_LOff = new QTimer(this);
  connect(timer_LOff,SIGNAL(timeout()),this,SLOT(lightsOff()));

//******************TIMER SPRAWDZAJĄCY OBECNOŚĆ****************//

  timer_obecnosc = new QTimer(this);
  connect(timer_obecnosc,SIGNAL(timeout()),this,SLOT(obecnosc_none()));

}
/*********************************************************************************WYSYLANIE RAMEK UDP*******************************************************************************************************************/
void MyUDP::WYSUDP()
{

   QByteArray tab;
   QByteArray Data;
   tab[0]=0x3C;
   tab[1]=0x44;
   tab[2]=0x3e;
/************************TAB[A]*******************************/
   for(i=0;i<=7;i++){
       if(maskawysl[0] & (0x01<<i)) {tab[3]=tab[3]|(0x01<<i);}
      else {tab[3]=tab[3] & ~(0x01<<i);}
                   }
/************************TAB[B]*******************************/
   for(i=0;i<=7;i++){
       if(maskawysl[1] & (0x01<<i)) {tab[4]=tab[4]|(0x01<<i);}
      else {tab[4]=tab[4] & ~(0x01<<i);}
                   }
/************************TAB[E]*******************************/
   for(i=0;i<=7;i++){
       if(maskawysl[2] & (0x01<<i)) {tab[5]=tab[5]|(0x01<<i);}
      else {tab[5]=tab[5] & ~(0x01<<i);}
                     }
/************************TAB[D]*******************************/
    for(i=0;i<=7;i++){
          if(maskawysl[3] & (0x01<<i)) {tab[6]=tab[6]|(0x01<<i);}
         else {tab[6]=tab[6] & ~(0x01<<i);}
                        }
/************************TAB[C]*******************************/
      for(i=0;i<=7;i++){
             if(maskawysl[4] & (0x01<<i)) {tab[7]=tab[7]|(0x01<<i);}
            else {tab[7]=tab[7] & ~(0x01<<i);}
                           }

   Data.append(tab);
   socket->writeDatagram(Data,QHostAddress("192.168.1.101"),1200);              // ip do ktorego wysyla + port

}
/*********************************************************************************ODBIERANIE RAMEK UDP****************************************************************************************************************/
void MyUDP::readyRead(){

     while (socket->hasPendingDatagrams()){
        QByteArray  Buffer;
        unsigned char maska = 0x01;
        unsigned char bufor[30];
        int i;
        Buffer.resize(socket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        socket->readDatagram(Buffer.data(),Buffer.size(),&sender,&senderPort);
        QByteArray k = Buffer.toHex();
        QByteArray z=k.mid(6,8);

        //qDebug() <<" Mss from" << sender.toString();
        //qDebug() << "Mss port" << senderPort;
        //qDebug() << "Ramka: " << k;

        ips=sender.toString();
        emit ips;
        if("192.168.1.102"==ips) {
            emit changes(); //sygnal do odbierania
            for (i=3;i<=(Buffer.length());i++){
                temperatura[i]=Buffer[i];
            }
        }
        if("192.168.1.100"==ips) {

            int n=0; //numeracja wejścia
            for (int i=3; i<=(Buffer.length());i++){

                temp[i-3]=Buffer[i];
            }

            for (int j=0; j<=4;j++){

                for (int i=1; i<=8;i++){
                    n++;
                    if(j!=3){ //ominięcie grupy 3 zajętej przez wyjścia systemu ogrzewania
                    if(temp[j] & hexx[i]){  //szukanie aktywnego wejścia
                        c[n]++;
                        if (c[n]==1){
                            maskawysl[j]|=(hexx[i]);
                            MyUDP client;
                            client.WYSUDP();
                         }
                         if (c[n]==2){
                            maskawysl[j]&=~(hexx[i]);
                            MyUDP client;
                            client.WYSUDP();
                            c[n]=0;
                         }
                    }
                }
            }
            }
        emit changes(); //sygnal do odbierania
        }
//**************************ODBIERANIE CZUJEK PIR***********************//
        if("192.168.1.103"==ips){
            for (int i=3; i<=(Buffer.length());i++)
            {
                temp[i-3]=Buffer[i];
            }
            obecnosc=1;
            timer_obecnosc->start(300000);
            if(temp[1]&0x01){
                qDebug() << "ZALANIE";
            }
            //***TIMER PO URUCHOMIENIU IDE SPAC***//
            if(spimy==1){
                odliczG=600;
                timer_LOff->start(1000);
            }
            //***WYKONYWANIE HARMONOGRAMU***//
                //***PODTRZYMANIE BUFORU DLA BARDZIEJ ZŁOŻONEGO HARMONOGRAMU***//
            if(!temp[0]==0){
                tempholder[0] = temp[0];
                //******//
            for(int j=0; j<=(scheduledhexxpir.length())-1;j++){
                if(tempholder[0] & scheduledhexxpir[j]){
                    if(scheduledtime[j]==0 || (scheduledtime[j]==1 && dzien==0)){

                        bgi.insert(j,j);//czy to zadziała?
                        int i=0;
                        foreach(unsigned char r, scheduledhexxout[j]){

                            maskawysl[outmasks.value(j)[i]]|=r;
                            MyUDP client;
                            client.WYSUDP();
                            c[scheduledcs.value(j)[i]]=1;
                            i++;
                        }
                        if(scheduledtime[j]==0){
                            scheduledtimers.at(j)->setSingleShot(true);
                            scheduledtimers.at(j)->start(t[tspinBox[j]]);
                            //qDebug() << j << tspinBox.at(j);
                        }
                    scheduledaction=1;
                    emit changes(); //sygnal do odbierania
                    }
                }
            }
        }
        }
     }
}

//**********FUNKCJA DO WYŁĄCZENIA WSZYSTKICH WYJŚĆ***************//
void MyUDP :: lightsOff(){

    odliczG--;
    if (odliczG==0){
    MyUDP client;
    maskawysl[0]&=~0xff;
    maskawysl[1]&=~0xff;
    maskawysl[2]&=~0xff;
    maskawysl[4]&=~0xff;
    client.WYSUDP();
    client.zerujWyj();
    timer_LOff->stop();
    LOff=1;
    emit changes(); //sygnal do odbierania
    }
}
//***********FUNKCJA ZERUJĄCA OBECNOŚĆ**************************//
void MyUDP :: obecnosc_none(){
    timer_obecnosc->stop();
    obecnosc=0;
    PIRs=0;
    PIRs_2=0;
}

void MyUDP::zerujWyj()
{
    for(int i=0;i<=40;i++)
    {
       c[i]=0;
    }
}

