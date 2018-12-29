#ifndef LABEL_TEMP_H
#define LABEL_TEMP_H

#include <QLabel>
#include <QMouseEvent>

class label_temp : public QLabel
{
    Q_OBJECT
public:
    explicit label_temp(QWidget *parent = 0);

    void mousePressEvent(QMouseEvent *ev);

signals:

    void mouse_clicked();

public slots:

};

#endif // LABEL_TEMP_H
