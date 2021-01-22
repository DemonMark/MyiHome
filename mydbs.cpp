#include "mydbs.h"

mydbs::mydbs(QObject *parent) :
    QObject(parent)
{
    if(QSqlDatabase::contains("qt_sql_default_connection")){
        qDebug() << "BAZA ISTNIEJE";
    }else{
        dbName = "/media/HDD2/Moje projekty/MyiHome/scene.db";
        auto mydb=QSqlDatabase::addDatabase("QSQLITE");
        mydb.setDatabaseName(dbName);
        mydb.open();
        qDebug() << "BAZA UTWORZONA";
    }
}

int mydbs::myqueries(QString tb, const QString &rec, int &val, bool sch, QString col_name)
{
    int ton;
    QSqlQuery* qry = new QSqlQuery(getDatabase());
    if(!sch){
        qry->prepare("SELECT * FROM "+tb+" WHERE "+col_name+" = '"+rec+"' ");
        if(qry->exec()){
            while(qry->next()){
                ton = qry->value(2).toInt();
            }
        }
    }else{
    qry->prepare("UPDATE "+tb+" SET aktywna=? WHERE nazwa = '"+rec+"'");
    qry->addBindValue(val);
    qry->exec();
    }
    return ton;
}

void mydbs::conclose()
{
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}

QSqlDatabase mydbs::getDatabase()
{
    return QSqlDatabase::database(dbName);
}
