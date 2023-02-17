#include "mydbs.h"

mydbs::mydbs(QString dname)
{
    if(QSqlDatabase::contains("qt_sql_default_connection")){
        //qDebug() << "BAZA ISTNIEJE " << &dbName;
    }else{
        dbName = dname;
        auto mydb=QSqlDatabase::addDatabase("QSQLITE");
        mydb.setDatabaseName(dbName);
        mydb.open();
        qDebug() << "BAZA UTWORZONA " << &mydb;
    }
}

int mydbs::myqueries(QString tb, const QString &rec, int &val, bool sch, QString col_name, QString update_col_name)
{
    int ton = 0;
    QSqlQuery* qry = new QSqlQuery(getDatabase());
    //READ DATA
    if(!sch){
        qry->prepare("SELECT * FROM "+tb+" WHERE "+col_name+" = '"+rec+"' ");
        if(qry->exec()){
            while(qry->next()){
                ton = qry->value(update_col_name).toInt();
            }
        }
        //WRITE DATA
    }else{
        qry->prepare("UPDATE "+tb+" SET aktywna=? WHERE nazwa = '"+rec+"'");
        qry->addBindValue(val);
        qry->exec();
    }
    delete qry;
    return ton;
}

QSqlQuery *mydbs::query(){

    QSqlQuery* qry = new QSqlQuery(getDatabase());
    return qry;

}

void mydbs::conclose()
{
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}

QSqlDatabase mydbs::getDatabase()
{
    return QSqlDatabase::database(dbName);
}
