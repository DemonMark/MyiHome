#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "events.h"
#include <QLineEdit>


namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

    

    QString zp1;
private slots:
    void on_WY1_toggled(bool checked);
    void on_WY3_toggled(bool checked);
    void on_WY2_toggled(bool checked);
    void on_WY4_toggled(bool checked);
    void on_WY5_toggled(bool checked);
    void on_WY6_toggled(bool checked);
    void on_WY7_toggled(bool checked);
    void on_WY8_toggled(bool checked);
    void on_WY9_toggled(bool checked);
    void on_WY10_toggled(bool checked);
    void on_WY11_toggled(bool checked);
    void on_WY12_toggled(bool checked);
    void on_WY13_toggled(bool checked);
    void on_WY14_toggled(bool checked);
    void on_WY15_toggled(bool checked);
    void on_WY16_toggled(bool checked);
    void on_WY17_toggled(bool checked);
    void on_WY18_toggled(bool checked);
    void on_WY19_toggled(bool checked);
    void on_WY20_toggled(bool checked);
    void on_WY21_toggled(bool checked);
    void on_WY22_toggled(bool checked);
    void on_WY23_toggled(bool checked);
    void on_WY24_toggled(bool checked);
    void on_pushButton_3_clicked();
public slots:
    void odczytDanych();


private:
    Ui::Dialog *ui;
     int timerId;
     void SaveSetting();
     void LoadSetting();
     void init();
     void ciapek_save();


protected:
  void timerEvent(QTimerEvent *event);
};

#endif // DIALOG_H

