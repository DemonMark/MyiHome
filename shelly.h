#ifndef SHELLY_H
#define SHELLY_H
#include <QWidget>
#include <QUdpSocket>
#include <QPushButton>
#include <QTimer>
#include <QDebug>
#include <QMouseEvent>

class shelly : public QPushButton
{
    Q_OBJECT
public:
    explicit shelly(QWidget *parent = nullptr);

    QUdpSocket *shellsock;
    void mousePressEvent(QMouseEvent *ev);

private:

signals:

public slots:

     void answer();
     void offline();
};

#endif // SHELLY_H
