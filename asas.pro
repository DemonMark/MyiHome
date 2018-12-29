#-------------------------------------------------
#
# Project created by QtCreator 2015-09-24T08:55:56
#
#-------------------------------------------------

QT       += core gui network webkit webkitwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = asas
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    myudp.cpp \
    mytimer.cpp \
    events.cpp \
    dialog.cpp \
    dialog2.cpp \
    zmienne.cpp \
    label_temp.cpp \
    pir_button.cpp \
    bcm2835.c


HEADERS  += mainwindow.h \
    myudp.h \
    mytimer.h \
    events.h \
    dialog.h \
    dialog2.h \
    zmienne.h \
    label_temp.h \
    pir_button.h \
    bcm2835.h

FORMS    += mainwindow.ui \
    events.ui \
    dialog.ui \
    dialog2.ui

RESOURCES += \
    grafika.qrc \
    ikony.qrc


