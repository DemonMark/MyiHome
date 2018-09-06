#include "wejscia.h"
#include "ui_wejscia.h"

wejscia::wejscia(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::wejscia)
{
    ui->setupUi(this);
}

wejscia::~wejscia()
{
    delete ui;
}
