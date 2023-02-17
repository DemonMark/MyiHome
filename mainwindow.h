#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <cstddef>
#include <QMainWindow>
#include <QListWidgetItem>
#include <QtWebKitWidgets>
#include <QLabel>

namespace Ui {
class MainWindow;
}
class QUdpSocket;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    static MainWindow *getMainWinPtr(); //dodano w celu dostepnu do MainWindow z innych klas

    //Ui::MainWindow *ui;
    ///MainWindow();
    //void mousePressEvent(QMouseEvent *);
    void readscheduler();
    void getHumidity();
    void barometer();
    void system_fan(double temp);
    void sunTimeWatcher(QString sunTime, QString source, int pos, QString editingTime);
    void WoL(QString macc, QString addr);
    void scene_executor(int *arg, QString &aktywna_scena, QString &buttons);
    void gates(int type, bool timer, int ms);
    void selected_sources(QList<QString> &scheduledbtn, QString &tempnames);
    void buttons_run(QString j, bool tof, QString c);
    void pir_status();
    void translator(QString &text_to_translate);
    void data_logger(const QString &input_data);

    enum scenes {activated, deactivated, garage_gate, main_gate, both, Matylda, main_gate_prt, close_1, close_2} driving;
    void show_item(bool state, QWidget *parentW, QString txt);
    void show_item2(QWidget *parentW, QWidget *widget, QRect geometry);
    void show_item_temp(bool state, QWidget *parentW, QString txt);
    void delete_schedule(int itm);
    void rekuperator(int speed);
    bool expanded(QPushButton *exd_btn, const char *prop);
    void btn_exp(QString b_name);
    void create_icon(QPushButton *btn, QString prefix, QPixmap &pix, int x);
    void remove_icon(QPushButton *btn, QString prefix, QString property);
    void read_btn_property(int mode);
    void database_access(int value, QString btn_name);

public slots:

      void LOff();
      void auto_room_off(QString room);

private slots:

  void settemperature(int temp_value);

  void receiving();

  void showTime();

  void wyjezdzam();

  void offfff();

  void ClickedbtnFinder();

  void btnFinderfromHEX();

  void ClickedscenebtnFinder(bool checked);

  void settimers(int dial_value);

  void on_button_23_clicked();
  void on_button_27_clicked();
  void on_button_OFF_clicked();
  void on_button_31_toggled(bool checked);
  void readTimeFromWWW();

  void on_pushButton_33_toggled(bool checked);

  void on_button_32_clicked(bool checked);

  void on_go_floor_clicked();

  void on_button_wentylator_2_toggled(bool checked);

  void on_go_ground_clicked();

  void on_config_clicked();

  void on_comboBox_currentIndexChanged(const QString &arg1);

  void on_resetButton_clicked();

  void on_config_cl_clicked();

  void on_listWidget_itemClicked(QListWidgetItem *item);

  void mqtt_processor(QString msg);

  void on_button_list_currentIndexChanged(int arg1);

  void on_button_set_clicked();

  void on_button_read_clicked();

  void on_button_clear_clicked();

  void on_button_clear_2_clicked();

  void on_button_read_2_clicked();

  void on_button_nooff_clicked();

  void on_Tab_currentChanged(int index);

private:

    Ui::MainWindow *ui;
    static MainWindow *pMainWindow; //jak powyzej chodzi o dostęp do MainWindow z innych klass
    int timerId;

    QTimer *timer_bramaStykOff;
    QTimer *reku_timer;
    QTimer *humidity_timer;
    QTimer *ventilation_timer;
    QTimer *temporary_cwu_checker;
    QWebPage *webView;
    QMovie *movie_pompa_1;
    QMovie *movie_pompa_2;
    QMovie *movie_cyrkulacja;
    QMovie *movie_wentylacja;
    QMovie *movie_countdown;
    QMovie *movie_reku;
    QMovie *movie_heat_fan;
    QPixmap pompa_off;
	QPixmap temp_on;
	QPixmap temp_off;
    QPixmap temp_dis;
	QPixmap con_err_off;
	QPixmap con_err_on;
    QPixmap ex_button;
    QPixmap map_button;

  protected:
    void timerEvent(QTimerEvent *event);

signals:
    void simulating(bool on);
    void UDP_ReadytoSend(QString addr);

};

#endif // MAINWINDOW_H
