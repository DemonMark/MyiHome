#include "label_temp.h"

#define baza "/media/HDD2/Moje projekty/MyiHome/scene.db"

label_temp::label_temp(QWidget *parent) :
    QLabel(parent)
{
    connect(this, SIGNAL(mouse_clicked()), this, SLOT(setdial()));
}

void label_temp::mousePressEvent(QMouseEvent *ev)
{
    emit mouse_clicked();
}

void label_temp::setdial(){

    bool state;
    QString code_TEMPname, temp_name;

    p_mw = MainWindow::getMainWinPtr();
    if(p_mw!=nullptr){
        QDial *temp_dial = p_mw->findChild<QDial*>("temp_dial");
        temp_dial->setProperty("temp_name", this->objectName());
        code_TEMPname = temp_dial->property("temp_name").toString();

        if(temp_dial->isVisible()==true && temp_dial->property("old_temp_name").toString()==code_TEMPname){
            state = false;
        }else{
            state = true;
            temp_dial->setProperty("old_temp_name", code_TEMPname);
        }

        mydbs read_value(baza);
        QSqlQuery *qry = read_value.query();
        qry->prepare("SELECT value, name FROM temperature WHERE code_name='"+code_TEMPname+"'");
        if(qry->exec()){
            qry->next();
            temp_dial->setValue((qry->value("value").toInt()));
            temp_name = qry->value("name").toString();
        }
        p_mw->show_item_temp(state, this->parentWidget(), temp_name);
        delete qry;
    }
}
