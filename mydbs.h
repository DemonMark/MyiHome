#ifndef MYDBS_H
#define MYDBS_H

#include <QObject>
#include <QtSql/QtSql>

class mydbs : public QObject
{
    Q_OBJECT
public:
    explicit mydbs(QString dname);
    QSqlDatabase getDatabase();
    void conclose();
    QSqlQuery *query();
    int myqueries(QString tb, const QString &rec, int &val, bool sch, QString col_name, QString update_col_name);

private:
    QString dbName;

signals:

public slots:

};

#endif // MYDBS_H
