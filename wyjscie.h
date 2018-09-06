/********************************************************************************
** Form generated from reading UI file 'designerp12062.ui'
**
** Created: Wed Oct 14 13:15:19 2015
**      by: Qt User Interface Compiler version 4.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef DESIGNERP12062_H
#define DESIGNERP12062_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QStatusBar>
#include <QtGui/QTabWidget>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE


namespace Ui {
class MainWindow;
}
class QUdpSocket;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QWidget *centralwidget;
    QTabWidget *tabWidget;
    QWidget *tab;
    QWidget *layoutWidget;
    QGridLayout *gridLayout_2;
    QCheckBox *WY1;
    QCheckBox *WY13;
    QCheckBox *WY2;
    QCheckBox *WY14;
    QCheckBox *WY3;
    QCheckBox *WY15;
    QCheckBox *WY4;
    QCheckBox *WY16;
    QCheckBox *WY5;
    QCheckBox *WY17;
    QCheckBox *WY6;
    QCheckBox *WY18;
    QCheckBox *WY7;
    QCheckBox *WY19;
    QCheckBox *WY8;
    QCheckBox *WY20;
    QCheckBox *WY9;
    QCheckBox *WY21;
    QCheckBox *WY10;
    QCheckBox *WY22;
    QCheckBox *WY11;
    QCheckBox *WY23;
    QCheckBox *WY12;
    QCheckBox *WY24;
    QPushButton *pushButton;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow*MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(800, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setGeometry(QRect(0, 0, 821, 581));
        tabWidget->setTabsClosable(false);
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        layoutWidget = new QWidget(tab);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(100, 70, 431, 296));
        gridLayout_2 = new QGridLayout(layoutWidget);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        WY1 = new QCheckBox(layoutWidget);
        WY1->setObjectName(QString::fromUtf8("WY1"));

        gridLayout_2->addWidget(WY1, 0, 0, 1, 1);

        WY13 = new QCheckBox(layoutWidget);
        WY13->setObjectName(QString::fromUtf8("WY13"));

        gridLayout_2->addWidget(WY13, 0, 1, 1, 1);

        WY2 = new QCheckBox(layoutWidget);
        WY2->setObjectName(QString::fromUtf8("WY2"));

        gridLayout_2->addWidget(WY2, 1, 0, 1, 1);

        WY14 = new QCheckBox(layoutWidget);
        WY14->setObjectName(QString::fromUtf8("WY14"));

        gridLayout_2->addWidget(WY14, 1, 1, 1, 1);

        WY3 = new QCheckBox(layoutWidget);
        WY3->setObjectName(QString::fromUtf8("WY3"));

        gridLayout_2->addWidget(WY3, 2, 0, 1, 1);

        WY15 = new QCheckBox(layoutWidget);
        WY15->setObjectName(QString::fromUtf8("WY15"));

        gridLayout_2->addWidget(WY15, 2, 1, 1, 1);

        WY4 = new QCheckBox(layoutWidget);
        WY4->setObjectName(QString::fromUtf8("WY4"));

        gridLayout_2->addWidget(WY4, 3, 0, 1, 1);

        WY16 = new QCheckBox(layoutWidget);
        WY16->setObjectName(QString::fromUtf8("WY16"));

        gridLayout_2->addWidget(WY16, 3, 1, 1, 1);

        WY5 = new QCheckBox(layoutWidget);
        WY5->setObjectName(QString::fromUtf8("WY5"));

        gridLayout_2->addWidget(WY5, 4, 0, 1, 1);

        WY17 = new QCheckBox(layoutWidget);
        WY17->setObjectName(QString::fromUtf8("WY17"));

        gridLayout_2->addWidget(WY17, 4, 1, 1, 1);

        WY6 = new QCheckBox(layoutWidget);
        WY6->setObjectName(QString::fromUtf8("WY6"));

        gridLayout_2->addWidget(WY6, 5, 0, 1, 1);

        WY18 = new QCheckBox(layoutWidget);
        WY18->setObjectName(QString::fromUtf8("WY18"));

        gridLayout_2->addWidget(WY18, 5, 1, 1, 1);

        WY7 = new QCheckBox(layoutWidget);
        WY7->setObjectName(QString::fromUtf8("WY7"));

        gridLayout_2->addWidget(WY7, 6, 0, 1, 1);

        WY19 = new QCheckBox(layoutWidget);
        WY19->setObjectName(QString::fromUtf8("WY19"));

        gridLayout_2->addWidget(WY19, 6, 1, 1, 1);

        WY8 = new QCheckBox(layoutWidget);
        WY8->setObjectName(QString::fromUtf8("WY8"));

        gridLayout_2->addWidget(WY8, 7, 0, 1, 1);

        WY20 = new QCheckBox(layoutWidget);
        WY20->setObjectName(QString::fromUtf8("WY20"));

        gridLayout_2->addWidget(WY20, 7, 1, 1, 1);

        WY9 = new QCheckBox(layoutWidget);
        WY9->setObjectName(QString::fromUtf8("WY9"));

        gridLayout_2->addWidget(WY9, 8, 0, 1, 1);

        WY21 = new QCheckBox(layoutWidget);
        WY21->setObjectName(QString::fromUtf8("WY21"));

        gridLayout_2->addWidget(WY21, 8, 1, 1, 1);

        WY10 = new QCheckBox(layoutWidget);
        WY10->setObjectName(QString::fromUtf8("WY10"));

        gridLayout_2->addWidget(WY10, 9, 0, 1, 1);

        WY22 = new QCheckBox(layoutWidget);
        WY22->setObjectName(QString::fromUtf8("WY22"));

        gridLayout_2->addWidget(WY22, 9, 1, 1, 1);

        WY11 = new QCheckBox(layoutWidget);
        WY11->setObjectName(QString::fromUtf8("WY11"));

        gridLayout_2->addWidget(WY11, 10, 0, 1, 1);

        WY23 = new QCheckBox(layoutWidget);
        WY23->setObjectName(QString::fromUtf8("WY23"));

        gridLayout_2->addWidget(WY23, 10, 1, 1, 1);

        WY12 = new QCheckBox(layoutWidget);
        WY12->setObjectName(QString::fromUtf8("WY12"));

        gridLayout_2->addWidget(WY12, 11, 0, 1, 1);

        WY24 = new QCheckBox(layoutWidget);
        WY24->setObjectName(QString::fromUtf8("WY24"));

        gridLayout_2->addWidget(WY24, 11, 1, 1, 1);

        pushButton = new QPushButton(tab);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(560, 210, 83, 24));
        pushButton->setCheckable(false);
        tabWidget->addTab(tab, QString());
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 19));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        WY1->setText(QApplication::translate("MainWindow", "WYJSCIE-1", 0, QApplication::UnicodeUTF8));
        WY13->setText(QApplication::translate("MainWindow", "WYJSCIE-13", 0, QApplication::UnicodeUTF8));
        WY2->setText(QApplication::translate("MainWindow", "WYJSCIE-2", 0, QApplication::UnicodeUTF8));
        WY14->setText(QApplication::translate("MainWindow", "WYJSCIE-14", 0, QApplication::UnicodeUTF8));
        WY3->setText(QApplication::translate("MainWindow", "WYJSCIE-3 - przek", 0, QApplication::UnicodeUTF8));
        WY15->setText(QApplication::translate("MainWindow", "WYJSCIE-15", 0, QApplication::UnicodeUTF8));
        WY4->setText(QApplication::translate("MainWindow", "WYJSCIE-4", 0, QApplication::UnicodeUTF8));
        WY16->setText(QApplication::translate("MainWindow", "WYJSCIE-16", 0, QApplication::UnicodeUTF8));
        WY5->setText(QApplication::translate("MainWindow", "WYJSCIE-5 - przek", 0, QApplication::UnicodeUTF8));
        WY17->setText(QApplication::translate("MainWindow", "WYJSCIE-17", 0, QApplication::UnicodeUTF8));
        WY6->setText(QApplication::translate("MainWindow", "WYJSCIE-6", 0, QApplication::UnicodeUTF8));
        WY18->setText(QApplication::translate("MainWindow", "WYJSCIE-18", 0, QApplication::UnicodeUTF8));
        WY7->setText(QApplication::translate("MainWindow", "WYJSCIE-7 - przek", 0, QApplication::UnicodeUTF8));
        WY19->setText(QApplication::translate("MainWindow", "WYJSCIE-19", 0, QApplication::UnicodeUTF8));
        WY8->setText(QApplication::translate("MainWindow", "WYJSCIE-8", 0, QApplication::UnicodeUTF8));
        WY20->setText(QApplication::translate("MainWindow", "WYJSCIE-20", 0, QApplication::UnicodeUTF8));
        WY9->setText(QApplication::translate("MainWindow", "WYJSCIE-9", 0, QApplication::UnicodeUTF8));
        WY21->setText(QApplication::translate("MainWindow", "WYJSCIE-21", 0, QApplication::UnicodeUTF8));
        WY10->setText(QApplication::translate("MainWindow", "WYJSCIE-10", 0, QApplication::UnicodeUTF8));
        WY22->setText(QApplication::translate("MainWindow", "WYJSCIE-22", 0, QApplication::UnicodeUTF8));
        WY11->setText(QApplication::translate("MainWindow", "WYJSCIE-11", 0, QApplication::UnicodeUTF8));
        WY23->setText(QApplication::translate("MainWindow", "WYJSCIE-23", 0, QApplication::UnicodeUTF8));
        WY12->setText(QApplication::translate("MainWindow", "WYJSCIE-12", 0, QApplication::UnicodeUTF8));
        WY24->setText(QApplication::translate("MainWindow", "WYJSCIE-24", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("MainWindow", "Wlacz", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("MainWindow", "Tab 1", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

QT_END_NAMESPACE

#endif // DESIGNERP12062_H
