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
  void showTime();
  void wyjscie();
  void stykOff();
  void offfff();
  void ClickedbtnFinder();

  void ClickedscenebtnFinder(bool checked);

  void ClickedlabelFinder();

  void settimers(int dial_value);

  void cyrkulacja_rotation();

  void LOff();

  void on_button_scene_1_toggled(bool checked);
  void on_pushButton_16_pressed();
  void on_pushButton_23_clicked();
  void on_pushButton_27_clicked();
  void on_pushButton_28_clicked();
  void on_pushButton_31_toggled(bool checked);
  void readTimeFromWWW();
  void on_pushButton_33_toggled(bool checked);
  void on_dial_12_valueChanged(int value);
  void on_pushButton_32_clicked(bool checked);

  void on_button_scene_3_clicked();

  void on_go_floor_clicked();

  void on_button_wentylator_clicked(bool checked);

  void on_button_wentylator_2_clicked(bool checked);

  void on_go_ground_clicked();

  void on_button_wentylator_3_clicked(bool checked);

  void on_button_scene_4_clicked(bool checked);

  void on_pushButton_13_clicked();

  void on_comboBox_currentIndexChanged(const QString &arg1);

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
    QMovie *movie_wentylacja;
    QMovie *movie_countdown;

  protected:
    void timerEvent(QTimerEvent *event);

signals:
    void simulating(bool on);
    void all_off();

};

#endif // MAINWINDOW_H
