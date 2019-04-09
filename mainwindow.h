#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#define nullptr NULL
#include <QMainWindow>
#include <QListWidgetItem>
#include <QtWebKit/QWebView>

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

    //static MainWindow * getMainWinPtr(); //dodano w celu dostepnu do MainWindow z innych klas
    //static MainWindow * pMainWindow; //jak powyzej chodzi o klasy
    //Ui::MainWindow *ui;
    ///MainWindow();
    //void mousePressEvent(QMouseEvent *);
    void readscheduler();
    void getHumidity();
    void barometer();

private slots:

  void showui();
  void writescheduler();
  void receiving();
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
  void showTime();
  void wyjscie();
  void stykOff();
  void offfff();
  void ClickedbtnFinder();
  void ClickedlabelFinder();
  void settimers(int dial_value);
  void cyrkulacja_rotation();
  void on_WY25_toggled(bool checked);
  void on_WY26_toggled(bool checked);
  void on_WY27_toggled(bool checked);
  void on_WY28_toggled(bool checked);
  void on_WY29_toggled(bool checked);
  void on_WY30_toggled(bool checked);
  void on_WY31_toggled(bool checked);
  void on_WY32_toggled(bool checked);
  void on_pushButton_13_toggled(bool checked);
  void on_pushButton_16_pressed();
  void on_pushButton_23_clicked();
  void on_pushButton_27_clicked();
  void on_pushButton_28_clicked();
  void on_pushButton_29_clicked();
  void on_pushButton_30_clicked();
  void on_pushButton_31_clicked();
  void readTimeFromWWW();
  void on_pushButton_33_toggled(bool checked);
  void on_dial_12_valueChanged(int value);
  void on_pushButton_32_clicked(bool checked);

private:

    Ui::MainWindow *ui;
    QUdpSocket *socket;
    int timerId;
    QTimer *timer_wyjscie;
    QTimer *timer_bramaStykOff;
    QWebView *webView;
    QMovie *movie_pompa_1;
    QMovie *movie_pompa_2;
    QMovie *movie_cyrkulacja;
    QMovie *movie_countdown;

  protected:
    void timerEvent(QTimerEvent *event);

signals:
    void simulating(bool on);
    void all_off();

};

#endif // MAINWINDOW_H
