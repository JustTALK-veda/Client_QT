/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
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
    QStackedWidget *pagesStack;
    QWidget *page;
    QWidget *gridLayoutWidget;
    QGridLayout *labels;
    QLabel *labels1;
    QLabel *labels2;
    QLabel *label3;
    QLabel *labels4;
    QWidget *page2;
    QHBoxLayout *horizontalLayout;
    QPushButton *prevBtn;
    QPushButton *exitBtn;
    QPushButton *nextBtn;
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
        verticalLayoutWidget->setGeometry(QRect(60, 50, 591, 431));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        widget = new QWidget(verticalLayoutWidget);
        widget->setObjectName("widget");
        pano = new QLabel(widget);
        pano->setObjectName("pano");
        pano->setGeometry(QRect(-120, 0, 589, 16));

        verticalLayout->addWidget(widget);

        pagesStack = new QStackedWidget(verticalLayoutWidget);
        pagesStack->setObjectName("pagesStack");
        page = new QWidget();
        page->setObjectName("page");
        gridLayoutWidget = new QWidget(page);
        gridLayoutWidget->setObjectName("gridLayoutWidget");
        gridLayoutWidget->setGeometry(QRect(30, 20, 521, 341));
        labels = new QGridLayout(gridLayoutWidget);
        labels->setObjectName("labels");
        labels->setContentsMargins(0, 0, 0, 0);
        labels1 = new QLabel(gridLayoutWidget);
        labels1->setObjectName("labels1");

        labels->addWidget(labels1, 0, 0, 1, 1);

        labels2 = new QLabel(gridLayoutWidget);
        labels2->setObjectName("labels2");

        labels->addWidget(labels2, 0, 1, 1, 1);

        label3 = new QLabel(gridLayoutWidget);
        label3->setObjectName("label3");

        labels->addWidget(label3, 1, 0, 1, 1);

        labels4 = new QLabel(gridLayoutWidget);
        labels4->setObjectName("labels4");

        labels->addWidget(labels4, 1, 1, 1, 1);

        pagesStack->addWidget(page);
        page2 = new QWidget();
        page2->setObjectName("page2");
        pagesStack->addWidget(page2);

        verticalLayout->addWidget(pagesStack);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        prevBtn = new QPushButton(verticalLayoutWidget);
        prevBtn->setObjectName("prevBtn");

        horizontalLayout->addWidget(prevBtn);

        exitBtn = new QPushButton(verticalLayoutWidget);
        exitBtn->setObjectName("exitBtn");

        horizontalLayout->addWidget(exitBtn);

        nextBtn = new QPushButton(verticalLayoutWidget);
        nextBtn->setObjectName("nextBtn");

        horizontalLayout->addWidget(nextBtn);


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

        pagesStack->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        pano->setText(QCoreApplication::translate("MainWindow", "pano", nullptr));
        labels1->setText(QCoreApplication::translate("MainWindow", "crop1", nullptr));
        labels2->setText(QCoreApplication::translate("MainWindow", "crop2", nullptr));
        label3->setText(QCoreApplication::translate("MainWindow", "crop3", nullptr));
        labels4->setText(QCoreApplication::translate("MainWindow", "crop4", nullptr));
        prevBtn->setText(QCoreApplication::translate("MainWindow", "prevBtn", nullptr));
        exitBtn->setText(QCoreApplication::translate("MainWindow", "exitBtn", nullptr));
        nextBtn->setText(QCoreApplication::translate("MainWindow", "nextBtn", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
