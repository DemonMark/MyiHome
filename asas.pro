#-------------------------------------------------
#
# Project created by QtCreator 2015-09-24T08:55:56
#
#-------------------------------------------------
#
QT       += core gui network sql webkit webkitwidgets qmqtt

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = asas
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += main.cpp\
        mainwindow.cpp \
    myudp.cpp \
    mytimer.cpp \
    label_temp.cpp \
    pir_button.cpp \
    bcm2835.c \
    mydbs.cpp \
    shelly.cpp \
    mqtt_client.cpp


HEADERS  += mainwindow.h \
    myudp.h \
    mytimer.h \
    label_temp.h \
    pir_button.h \
    bcm2835.h \
    mydbs.h \
    shelly.h \
    mqtt_client.h

FORMS    += mainwindow.ui

RESOURCES += \
    grafika.qrc \
    ikony.qrc

DISTFILES +=

INCLUDEPATH += /home/pi/qmqtt-master/src/mqtt/

QMAKE_CXXFLAGS += -std=gnu++11

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
