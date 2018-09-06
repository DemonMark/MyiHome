#ifndef EVENTS_H
#define EVENTS_H

#include <QTabWidget>
#include "zmienne.h"
#include <QSettings>
#include <QDebug>
#include <dialog.h>
#include <dialog2.h>

namespace Ui {
class events;
}

class events : public QTabWidget
{
    Q_OBJECT

public:
    explicit events(QWidget *parent = 0);
    ~events();
    QString zmiany;
    QString z1;
    
private slots:

    void on_zdarzeniesave_clicked();
    void on_usunwyj_clicked();
    void on_test_clicked();
    void on_dodajwyj_clicked();
    void on_dodajwej_clicked();
    void on_usunwej_clicked();
    void on_otworzwyj_clicked();
    void on_otworzwej_clicked();
    void closeTab(int);

signals :
    void dane(QString);

private:
    Ui::events *ui;
     int timerId;
     void SaveSetting();
     void LoadSetting();
     void odczytwyj();
     void odczytwej();

protected:
  void timerEvent(QTimerEvent *event);
};

#endif // EVENTS_H



