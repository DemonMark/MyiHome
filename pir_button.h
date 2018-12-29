#ifndef PIR_BUTTON_H
#define PIR_BUTTON_H

#include <QPushButton>
#include <QMouseEvent>
#include <QDebug>
#include <QTimer>

class pir_button : public QPushButton
{
    Q_OBJECT
public:
    explicit pir_button(QWidget *parent = 0);
    void mousePressEvent(QMouseEvent *ev);

signals:

    void klik();

public slots:

    void naruszeniestrefy();
    void setdial();
    void absent();


};

#endif // PIR_BUTTON_H
