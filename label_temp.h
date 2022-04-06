#ifndef LABEL_TEMP_H
#define LABEL_TEMP_H

#include <QLabel>
#include <QMouseEvent>
#include "mainwindow.h"
#include "mydbs.h"

class label_temp : public QLabel
{
    Q_OBJECT
public:
    explicit label_temp(QWidget *parent = nullptr);

    void mousePressEvent(QMouseEvent *ev);

    MainWindow *p_mw;

signals:

    void mouse_clicked();

public slots:

    void setdial();

};

#endif // LABEL_TEMP_H
