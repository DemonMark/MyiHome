#include "myudp.h"
#include <QUdpSocket>
#include "mytimer.h"
#include "ui_mainwindow.h"

int i;
unsigned char shell[1];
unsigned char temp[20];
unsigned char tempholder[20];
unsigned char hexx[9]={0,0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80}; //tablica bitów, 0 niewykorzystywane
QList<QList<int> > rand_masks;
QList<QList<unsigned char> > rand_hexxs;
unsigned char scene_pir=0x10;
QList<QTimer*> rand_timers;
unsigned char temperatura[63];
extern unsigned char maskawysl[10];
QString ipadress;
QString ips;
extern QString time_text;
extern int dzien;
extern int spimy;
extern int obecnosc;
extern int arg[12];
extern int arg_check;
int odliczG;
bool simulating_on;
extern bool scene_active;
extern bool scene_driving;
extern bool jestem;
#define DC '\xb0' //znak stopnia
#define baza "/media/HDD2/Moje projekty/MyiHome/scene.db"

MyUDP::MyUDP(QObject *parent) :
    QObject(parent)
{
  socket = new QUdpSocket(this);
  socket->bind(QHostAddress("192.168.1.2"),1200);         // moj ip
  connect(socket,SIGNAL(readyRead()),this,SLOT(readyRead()));
    qDebug() << "UTWORZONO UDP";

//******************TIMER WYŁĄCZAJĄCY WYJŚCIA*******************//
  timer_LOff = new QTimer(this);
  connect(timer_LOff, &QTimer::timeout, [=](){
      if(odliczG==0){
          emit all_off_();
          arg_check--;
          qDebug() << "ARG_CH: " << arg_check;
      }
      odliczG--;
      if(odliczG<0){
          odliczG=0;
          timer_LOff->stop();
      }
  });

//******************TIMER SPRAWDZAJĄCY OBECNOŚĆ****************//
  timer_obecnosc = new QTimer(this);
  connect(timer_obecnosc,&QTimer::timeout,[=](){
      timer_obecnosc->stop();
      obecnosc=0;
  });

  //timer do losowego włączania symulacji//
  action = new QTimer(this);
  connect(action, &QTimer::timeout, [=] (){
      simulation(true);
  });

  //timer do pukania do modułu (nowy router usypia połączenie z hostami)
  ping = new QTimer(this);
  connect(ping, &QTimer::timeout, [=]() {
      WYSUDP("192.168.1.101");
      WYSUDP("192.168.1.104");
  });
  ping->start(60000);  
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

        //qDebug() <<" Mss from" << sender.toString() << " Ramka: " << k;

        ips=sender.toString();

        if("192.168.1.102"==ips){
            //qDebug() << "Ramka: " << k;
            emit changes(); //sygnal do odbierania
            for (i=3;i<(Buffer.length());i++){
                temperatura[i]=Buffer[i];
            }
        }
        if(("192.168.1.100"==ips)||("192.168.1.104"==ips)){

            for (int i=3; i<=(Buffer.length());i++){
                temp[i-3]=Buffer[i];
            }
            qDebug() << Buffer.toHex();
            emit hex_comming(); //sygnal do odbierania
        }
        //**************************ODBIERANIE CZUJEK PIR***********************//
        if("192.168.1.103"==ips){

            mydbs pir_chck(baza);
            QSqlQuery* qry = new QSqlQuery(pir_chck.getDatabase());

            for (int i=3; i<=(Buffer.length());i++){
                if((temp[i-3]=Buffer[i])!=0){
                    //"SELECT * FROM main INNER JOIN sources ON main.id = sources.main_id INNER JOIN PIR ON main.pir_desc = PIR.nazwa WHERE main.pir_hex = '"+QString::number(temp[0])+"' AND PIR.aktywna=1"
                    qry->prepare("SELECT * FROM PIR LEFT JOIN (SELECT * FROM main INNER JOIN sources ON main.id = sources.main_id WHERE main.pir_hex = '"+QString::number(temp[0])+"') WHERE PIR.pir_hex = '"+QString::number(temp[0])+"' AND aktywna=1");
                    if(qry->exec()){
                        while(qry->next()){
                            MainWindow *mw = MainWindow::getMainWinPtr();
                            if(mw!=nullptr){
                                QString code_PIRname = qry->value("code_name").toString();
                                int PIRtime = qry->value("timer_time").toInt();
                                pir_button *pirbtn_holder = mw->findChild<pir_button*>(code_PIRname);
                                pirbtn_holder->setChecked(true);
                                pirbtn_holder->naruszeniestrefy(PIRtime);
                            }
                            obecnosc=1;
                            timer_obecnosc->start(300000);
                            emit changes();
                            //***TIMER PO URUCHOMIENIU IDE SPAC LUB AKTYWACJI SCENY***//
                            if(spimy==1 || scene_active){
                                switch(scene_active){
                                case true:
                                    odliczG=arg[2]*60;
                                    break;
                                default:
                                    odliczG=600;
                                }
                                timer_LOff->start(1000);
                            }
                            //***WYKONYWANIE HARMONOGRAMU***//
                            int type_result = qry->value("type_id").toInt();
                            QString tName = qry->value("timer_name").toString();
                            QString btn_no = qry->value("btn_no").toString();
                            if(qry->value("main.id").toString()!=""){
                                if((type_result==3 && dzien==1) || (type_result==1 && dzien==0) || type_result==0){
                                    if(btn_no.contains("reku_")){
                                        mw->rekuperator(btn_no.split("_")[1].toInt());
                                    }else{
                                        mw->buttons_run(btn_no, true, "no");
                                    }
                                    if(tName!=""){
                                        //MainWindow *mw = qobject_cast<MainWindow*>(QApplication::activeWindow());
                                        if(mw!=nullptr){
                                            QTimer *sql_tHolder = mw->findChild<QTimer*>(tName);
                                            sql_tHolder->setSingleShot(true);
                                            sql_tHolder->start(qry->value("timer_time").toInt());

                                            mw->data_logger(sql_tHolder->objectName());
                                        }
                                    }
                                }
                            }
                            //************WYKONYWANIE SCEN WYMAGAJACYCH RUCHU W DANEJ SEKCJI**********//
                            //***********SCENA WYJEZDZAM************//
                            if(scene_driving && (qry->value("PIR.pir_hex").toUInt()&scene_pir)){
                                emit gate();
                            }
                            //usuwanie pozycji jednorazowej harmonogramu//
                            if(qry->value("tmp").toInt() == 1 && tName == ""){
                                mw->delete_schedule(qry->value("id").toInt());
                            }
                        }
                    }
                }
            }
            delete qry;
        }
        //****************SHELLY1**********************//
        if(ips!="192.168.1.100" && ips!="192.168.1.102" && ips!="192.168.1.103" && ips!="192.168.1.104"){
            MainWindow *udp_mw = MainWindow::getMainWinPtr();
            shelly *shelly_ptr = udp_mw->findChild<shelly*>("shelly_"+ips.split(".")[3]);
            QLabel *rsi_label = udp_mw->findChild<QLabel*>("rsi_shelly_"+ips.split(".")[3]);
            if(shelly_ptr!=nullptr){

                if((Buffer[1]&0x53)&&!(Buffer[2]&0x01)&&shelly_ptr->property("mute").toInt()!=1){
                    shelly_ptr->setIcon(shelly_ptr->property("off").value<QIcon>());
                }
                if((Buffer[1]&0x53)&&!(Buffer[2]&0x01)&&shelly_ptr->property("mute").toInt()==1){
                    shelly_ptr->setIcon(shelly_ptr->property("mute_icon").value<QIcon>());
                }
                if((Buffer[2]&0x01)&&(shelly_ptr->property("mute").toInt()!=1)){
                    shelly_ptr->setIcon(shelly_ptr->property("on").value<QIcon>());
                }
                //dzwonek
                if((Buffer[3]&0x01)&&(ips=="192.168.1.105")){

                    shelly_ptr = udp_mw->findChild<shelly*>("shelly_106");
                    if(shelly_ptr->property("mute").toInt()!=1){
                        QByteArray plugsockett;
                        QByteArray psDataa;

                        QUdpSocket *shellsockk = new QUdpSocket(this);

                        plugsockett[0]=0x53;
                        plugsockett[1]=0x01;

                        psDataa.clear();
                        psDataa.append(plugsockett);
                        shellsockk->writeDatagram(psDataa,QHostAddress("192.168.1.106"),4210);
                        delete shellsockk;
                    }
                }
                if(rsi_label!=nullptr){
                    bool ok;
                    uint signal = ((Buffer.toHex()).mid(0,2)).toUInt(&ok,16);
                    rsi_label->setText(QString::number(signal) + "%");
                    //wyjątek dla Shelly2.5 - termistor + drugie skrzydło
                    if(ips=="192.168.1.107"){
                        signal = ((Buffer.toHex()).mid(8,2)).toUInt(&ok,16);
                        rsi_label = udp_mw->findChild<QLabel*>("temp_shelly_"+ips.split(".")[3]);
                        rsi_label->setText(QString::number(signal)+DC);
                        //drugie skrzydło
                        if(Buffer[2]&0x02){
                            shelly_ptr = udp_mw->findChild<shelly*>("_helly_107_2");
                            shelly_ptr->setIcon(shelly_ptr->property("on").value<QIcon>());
                        }
                    }
                }
            }
        }

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
