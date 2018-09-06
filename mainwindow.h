#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>

namespace Ui {
class MainWindow;
}
class QUdpSocket;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    //Ui::MainWindow *ui;
    ///MainWindow();
    //void MyTimerSlot();

private slots:

  void on_WY3_toggled(bool checked);
  void on_WY1_toggled(bool checked);
  void on_WY2_toggled(bool checked);
  void on_WY4_toggled(bool checked);
  void on_WY5_toggled(bool checked);
  void on_WY9_toggled(bool checked);
  void on_WY10_toggled(bool checked);
  void on_WY20_toggled(bool checked);
  void on_WY21_toggled(bool checked);
  void on_WY22_toggled(bool checked);
  void on_WY23_toggled(bool checked);
  void on_WY24_toggled(bool checked);
  void on_slider_sliderMoved(int position);
  void showTime();
  void wyjscie();
  void stykOff();

  void on_WY25_toggled(bool checked);

  void on_WY26_toggled(bool checked);

  void on_WY27_toggled(bool checked);

  void on_WY28_toggled(bool checked);

  void on_WY29_toggled(bool checked);

  void on_WY30_toggled(bool checked);

  void on_WY31_toggled(bool checked);

  void on_WY32_toggled(bool checked);

  void on_WY39_toggled(bool checked);

  void on_WY40_toggled(bool checked);

  void on_pushButton_toggled(bool checked);

  void on_pushButton_2_toggled(bool checked);

  void on_pushButton_3_toggled(bool checked);

  void on_pushButton_4_toggled(bool checked);

  void on_pushButton_5_toggled(bool checked);

  void on_pushButton_6_toggled(bool checked);

  void on_pushButton_7_toggled(bool checked);

  void on_pushButton_8_toggled(bool checked);

  void on_pushButton_9_toggled(bool checked);

  void on_pushButton_10_toggled(bool checked);

  void on_pushButton_11_toggled(bool checked);

  void on_pushButton_12_toggled(bool checked);

  void on_pushButton_13_toggled(bool checked);

  void on_pushButton_15_toggled(bool checked);

  void on_pushButton_16_pressed();

  void on_pushButton_16_released();

  void on_pushButton_17_toggled(bool checked);

  void on_pushButton_19_toggled(bool checked);

  void on_pushButton_18_toggled(bool checked);

  void on_pushButton_20_toggled(bool checked);

  void on_pushButton_21_toggled(bool checked);

  void on_pushButton_23_clicked();

  void on_pushButton_24_clicked();

  void on_pushButton_25_clicked();

  void on_pushButton_26_clicked();

  void on_pushButton_27_clicked();

  void on_pushButton_28_clicked();

  void on_spinBox_valueChanged(int arg1);

private:
    Ui::MainWindow *ui;
    QUdpSocket *socket;
    int timerId;
    QTimer *timer_wyjscie;
    QTimer *timer_bramaStykOff;
  protected:
    void timerEvent(QTimerEvent *event);
    void readSetting();
    void writeSetting();

};

#endif // MAINWINDOW_H
