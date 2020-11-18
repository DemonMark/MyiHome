#include "mydbs.h"

mydbs::mydbs(QObject *parent) :
    QObject(parent)
{
    mydb=QSqlDatabase::addDatabase("QSQLITE");
    mydb.setDatabaseName("/media/HDD2/Moje projekty/MyiHome/scene.db");
    if(mydb.open())
    {
        qDebug() << "OPENED";
    }
}

void mydbs::conclose()
{
    mydb.close();
    QSqlDatabase::removeDatabase("QSQLITE");
    mydb.removeDatabase(QSqlDatabase::defaultConnection);

}
