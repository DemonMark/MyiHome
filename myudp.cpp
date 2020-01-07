#include "myudp.h"
#include <QUdpSocket>
#include "mainwindow.h"
#include "mytimer.h"
#include "ui_mainwindow.h"

int i;
int q=0;
unsigned char temp[20];
unsigned char tempholder[20];
unsigned char hexx[9]={0,0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80}; //tablica bitów, 0 niewykorzystywane
QList<unsigned char> scheduledhexxpir; //tablica bitów czujek definiowanych przez harmonogramy
QList<QList<unsigned char> > scheduledhexxout; //tablica bitów wyjść definiowanych przez harmonogramy
QList<int> bgi; //wybór grup harmonogramowanych przycisków do aktywacji
extern QList<QList<int> > outmasks;
QList<QList<int> > rand_masks;
QList<QList<unsigned char> > rand_hexxs;

//QList<QList<int> > scene_masks;
//QList<QList<unsigned char> > scene_hexs;
//QList<int> scene_mask;
//QList<unsigned char> scene_hex;
unsigned char scene_pir=0x10;

extern QList<QList<int> > scheduledcs;
extern QList<int> scheduledtime;
extern QList<int> tspinBox;
QList<QTimer*> scheduledtimers;
QList<QTimer*> rand_timers;
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
extern int gn;
extern QList<int> t;
int c[49]; //wejścia (49 pozycji kontenera wliczając 0)
int odliczG;
int scheduledaction=0;
bool simulating_on;
extern bool scene_active;
extern bool scene_driving;

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

  //timer do losowego włączania symulacji, użycie funkcji posrednij (qt4.8) w przyszlosci zmiana na lambda expression(qt 5.2)//
  action = new QTimer(this);
  connect(action, SIGNAL(timeout()), this, SLOT(simulation_holder()));

}
/*********************************************************************************WYSYLANIE RAMEK UDP*******************************************************************************************************************/
void MyUDP::WYSUDP(QString addr)
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
//*************************TAB[6-CH]*****BAJT WYKORZYSTYWANY PRZEZ MODUŁ 6-CH/
      for(i=0;i<=7;i++){
             if(maskawysl[5] & (0x01<<i)) {tab[8]=tab[8]|(0x01<<i);}
            else {tab[8]=tab[8] & ~(0x01<<i);}
      }

   Data.append(tab);
   socket->writeDatagram(Data,QHostAddress(addr),1200);              // ip do ktorego wysyla + port

}
/*********************************************************************************ODBIERANIE RAMEK UDP****************************************************************************************************************/
void MyUDP::readyRead(){

     while (socket->hasPendingDatagrams()){
        QByteArray Buffer;
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

        if("192.168.1.102"==ips){
            emit changes(); //sygnal do odbierania
            for (i=3;i<(Buffer.length());i++){
                if((i%2) && (Buffer[i]==0)){ //przerwanie odczytu temperatury dla pól nieparzystych w przypadku braku poprawnego pakietu
                    i++;//ominięcie części dziesiętnej
                }else{
                    temperatura[i]=Buffer[i];
                }
            }
        }
        if("192.168.1.100"==ips){

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
                            client.WYSUDP("192.168.1.101");
                         }
                         if (c[n]==2){
                            maskawysl[j]&=~(hexx[i]);
                            MyUDP client;
                            client.WYSUDP("192.168.1.101");
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
            emit changes();
            if(temp[1]&0x01){
                qDebug() << "ZALANIE";
            }
            //***TIMER PO URUCHOMIENIU IDE SPAC LUB AKTYWACJI SCENY***//
            if(spimy==1 || scene_active){
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
                            client.WYSUDP("192.168.1.101");
                            c[scheduledcs.value(j)[i]]=1;
                            i++;
                        }
                        if(scheduledtime[j]==0){
                            scheduledtimers.at(j)->setSingleShot(true);
                            scheduledtimers.at(j)->start(t.at(tspinBox[j]));
                            //qDebug() << j << tspinBox.at(j);
                        }
                    scheduledaction=1;
                    emit changes(); //sygnal do odbierania
                    }
                }
            }
            }
        //************WYKONYWANIE SCEN**********//
        //***********SCENA WYJEZDZAM************//
            if(scene_driving && tempholder[0] & scene_pir){
                emit gate();
            }
        }
     }
}

//**********FUNKCJA DO WYŁĄCZENIA WSZYSTKICH WYJŚĆ***************//
void MyUDP::lightsOff(){

    odliczG--;
    if (odliczG==0){
        maskawysl[0]&=~0xff;
        maskawysl[1]&=~0xff;
        maskawysl[2]&=~0xff;
        maskawysl[4]&=~0xff;
        maskawysl[5]&=~0xff;
        WYSUDP("192.168.1.101");
        WYSUDP("192.168.1.104");
        zerujWyj();
        timer_LOff->stop();
        emit all_off_();
    }
    if(odliczG<0){
        odliczG=0;
        timer_LOff->stop();
    }
}
//***********FUNKCJA ZERUJĄCA OBECNOŚĆ**************************//
void MyUDP::obecnosc_none(){
    timer_obecnosc->stop();
    obecnosc=0;
}

void MyUDP::zerujWyj()
{
    for(int i=0;i<=48;i++)
    {
       c[i]=0;
    }
}
//*****SYMULACJA OBECNOŚCI*****//
void MyUDP::simulation(bool on)
{
    if(on){
        action->start((5+qrand()%20)*60000);
        QList<int> tmp_masks;
        QList<unsigned char> tmp_hexx;
        QTime randtime = QTime::currentTime();
        qsrand(uint(randtime.msec()));
        int rand_loops = 1+qrand()%4;
        for(int i=1; i<=rand_loops; i++){
            int rand_mask = qrand()%5;
            unsigned char rand_hexx = hexx[1+qrand()%9];
            if(rand_mask!=3 && !(rand_mask==4 && rand_hexx==0x80)){
                maskawysl[rand_mask]|=rand_hexx;//zostawiamy póki wszystkie swiatla nie beda mialy ikon
                WYSUDP("192.168.1.101");// jak powyzej
                simulating_on=true;
                temp[rand_mask]=rand_hexx;
                emit changes();
                tmp_masks.insert(i-1,rand_mask);
                tmp_hexx.insert(i-1, rand_hexx);
                qDebug() << rand_mask << rand_hexx;
            }else{
                tmp_masks.insert(i-1,0);
                tmp_hexx.insert(i-1, 0);
                qDebug() << "zle" <<rand_mask << rand_hexx;
            }
        }
        QTimer *rand_off = new QTimer(this);
        connect(rand_off, SIGNAL(timeout()), this, SLOT(random_off()));
        rand_off->setSingleShot(true);
        rand_timers.append(rand_off);
        rand_masks.append(tmp_masks);
        rand_hexxs.append(tmp_hexx);
        rand_off->start((1+(qrand()%15))*1000);
        qDebug() << rand_masks << rand_hexxs;
        }else{
        action->stop();
        }
}

void MyUDP::random_off()
{
    temp[0]=0;
    temp[1]=0;
    temp[2]=0;
    temp[4]=0;
    int i=0,j=0;
    foreach(QTimer *rand_t, rand_timers){
        if(!(rand_t->isActive())){
            foreach(unsigned char r, rand_hexxs[j]){
                maskawysl[rand_masks.value(j)[i]]&=~r;
                //c[scheduledcs.value(j)[i]]=0;
                temp[rand_masks.value(j)[i]]+=r;
                i++;
            }
            WYSUDP("192.168.1.101");
            simulating_on=true;
            emit changes();
            i=0;
            rand_timers.removeAt(j);
            rand_masks.removeAt(j);
            rand_hexxs.removeAt(j);
            qDebug() << "LOSOWY TIMER NR" << j << "OFF";
        }
    j++;
    }
}
void MyUDP::simulation_holder() //do usunięcia w QT 5.2 (lambda exspression)
{
    simulation(true);
}
