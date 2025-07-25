/********************************************************************************
** Form generated from reading UI file 'Meeting.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MEETING_H
#define UI_MEETING_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "stackpage.h"

QT_BEGIN_NAMESPACE

class Ui_meetingForm
{
public:
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QWidget *widget;
    QLabel *pano;
    Stackpage *stackedWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton_3;
    QPushButton *nextButton;
    QPushButton *prevButton;

    void setupUi(QWidget *meetingForm)
    {
        if (meetingForm->objectName().isEmpty())
            meetingForm->setObjectName("meetingForm");
        meetingForm->resize(777, 528);
        verticalLayoutWidget = new QWidget(meetingForm);
        verticalLayoutWidget->setObjectName("verticalLayoutWidget");
        verticalLayoutWidget->setGeometry(QRect(20, 10, 731, 491));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        widget = new QWidget(verticalLayoutWidget);
        widget->setObjectName("widget");
        widget->setMinimumSize(QSize(0, 200));
        pano = new QLabel(widget);
        pano->setObjectName("pano");
        pano->setGeometry(QRect(11, 0, 711, 200));
        pano->setMinimumSize(QSize(0, 200));

        verticalLayout->addWidget(widget);

        stackedWidget = new Stackpage(verticalLayoutWidget);
        stackedWidget->setObjectName("stackedWidget");

        verticalLayout->addWidget(stackedWidget);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        pushButton_3 = new QPushButton(verticalLayoutWidget);
        pushButton_3->setObjectName("pushButton_3");

        horizontalLayout->addWidget(pushButton_3);

        nextButton = new QPushButton(verticalLayoutWidget);
        nextButton->setObjectName("nextButton");

        horizontalLayout->addWidget(nextButton);

        prevButton = new QPushButton(verticalLayoutWidget);
        prevButton->setObjectName("prevButton");

        horizontalLayout->addWidget(prevButton);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(meetingForm);

        stackedWidget->setCurrentIndex(-1);


        QMetaObject::connectSlotsByName(meetingForm);
    } // setupUi

    void retranslateUi(QWidget *meetingForm)
    {
        meetingForm->setWindowTitle(QCoreApplication::translate("meetingForm", "Form", nullptr));
        pano->setText(QCoreApplication::translate("meetingForm", "TextLabel", nullptr));
        pushButton_3->setText(QCoreApplication::translate("meetingForm", "PushButton", nullptr));
        nextButton->setText(QCoreApplication::translate("meetingForm", "PushButton", nullptr));
        prevButton->setText(QCoreApplication::translate("meetingForm", "PushButton", nullptr));
    } // retranslateUi

};

namespace Ui {
    class meetingForm: public Ui_meetingForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MEETING_H
