/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QWidget *widget;
    QLabel *pano;
    QStackedWidget *stackedWidget;
    QWidget *page1;
    QWidget *page2;
    QHBoxLayout *horizontalLayout;
    QPushButton *preBtn;
    QPushButton *nextBtn;
    QPushButton *CCTV;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayoutWidget = new QWidget(centralwidget);
        verticalLayoutWidget->setObjectName("verticalLayoutWidget");
        verticalLayoutWidget->setGeometry(QRect(40, 60, 721, 471));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        widget = new QWidget(verticalLayoutWidget);
        widget->setObjectName("widget");
        pano = new QLabel(widget);
        pano->setObjectName("pano");
        pano->setGeometry(QRect(0, 0, 711, 121));

        verticalLayout->addWidget(widget);

        stackedWidget = new QStackedWidget(verticalLayoutWidget);
        stackedWidget->setObjectName("stackedWidget");
        page1 = new QWidget();
        page1->setObjectName("page1");
        stackedWidget->addWidget(page1);
        page2 = new QWidget();
        page2->setObjectName("page2");
        stackedWidget->addWidget(page2);

        verticalLayout->addWidget(stackedWidget);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        preBtn = new QPushButton(verticalLayoutWidget);
        preBtn->setObjectName("preBtn");

        horizontalLayout->addWidget(preBtn);

        nextBtn = new QPushButton(verticalLayoutWidget);
        nextBtn->setObjectName("nextBtn");

        horizontalLayout->addWidget(nextBtn);

        CCTV = new QPushButton(verticalLayoutWidget);
        CCTV->setObjectName("CCTV");

        horizontalLayout->addWidget(CCTV);


        verticalLayout->addLayout(horizontalLayout);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 24));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        pano->setText(QCoreApplication::translate("MainWindow", "pano", nullptr));
        preBtn->setText(QCoreApplication::translate("MainWindow", "preBtn", nullptr));
        nextBtn->setText(QCoreApplication::translate("MainWindow", "nextBtn", nullptr));
        CCTV->setText(QCoreApplication::translate("MainWindow", "CCTV", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
