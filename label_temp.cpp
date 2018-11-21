#include "label_temp.h"

label_temp::label_temp(QWidget *parent) :
    QLabel(parent)
{
}

void label_temp::mousePressEvent(QMouseEvent *ev)
{
    emit mouse_clicked();
}
