#ifndef MYDBS_H
#define MYDBS_H

#include <QObject>
#include <QtSql/QtSql>

class mydbs : public QObject
{
    Q_OBJECT
public:
    explicit mydbs(QObject *parent = 0);
    QSqlDatabase mydb;
    void conclose();

signals:

public slots:

};

#endif // MYDBS_H
