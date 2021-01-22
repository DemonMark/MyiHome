#ifndef MYDBS_H
#define MYDBS_H

#include <QObject>
#include <QtSql/QtSql>

class mydbs : public QObject
{
    Q_OBJECT
public:
    explicit mydbs(QObject *parent = 0);
    QSqlDatabase getDatabase();
    void conclose();
    int myqueries(QString tb, const QString &rec, int &val, bool sch, QString col_name);

private:
    QString dbName;

signals:

public slots:

};

#endif // MYDBS_H
