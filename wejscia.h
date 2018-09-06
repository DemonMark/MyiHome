#ifndef WEJSCIA_H
#define WEJSCIA_H

#include <QDialog>

namespace Ui {
class wejscia;
}

class wejscia : public QDialog
{
    Q_OBJECT
    
public:
    explicit wejscia(QWidget *parent = 0);
    ~wejscia();
    
private:
    Ui::wejscia *ui;
};

#endif // WEJSCIA_H
