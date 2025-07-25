/********************************************************************************
** Form generated from reading UI file 'lobby.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOBBY_H
#define UI_LOBBY_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "CameraWidget.h"

QT_BEGIN_NAMESPACE

class Ui_Lobby
{
public:
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QLabel *wait_label;
    QLabel *status_label;
    CameraWidget *cameraWidget;
    QPushButton *enterButton;

    void setupUi(QWidget *Lobby)
    {
        if (Lobby->objectName().isEmpty())
            Lobby->setObjectName("Lobby");
        Lobby->resize(741, 551);
        verticalLayoutWidget = new QWidget(Lobby);
        verticalLayoutWidget->setObjectName("verticalLayoutWidget");
        verticalLayoutWidget->setGeometry(QRect(9, 11, 721, 531));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        wait_label = new QLabel(verticalLayoutWidget);
        wait_label->setObjectName("wait_label");
        wait_label->setMaximumSize(QSize(16777215, 50));
        wait_label->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout->addWidget(wait_label);

        status_label = new QLabel(verticalLayoutWidget);
        status_label->setObjectName("status_label");
        status_label->setMaximumSize(QSize(16777215, 50));
        status_label->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout->addWidget(status_label);

        cameraWidget = new CameraWidget(verticalLayoutWidget);
        cameraWidget->setObjectName("cameraWidget");

        verticalLayout->addWidget(cameraWidget);

        enterButton = new QPushButton(verticalLayoutWidget);
        enterButton->setObjectName("enterButton");

        verticalLayout->addWidget(enterButton);


        retranslateUi(Lobby);

        QMetaObject::connectSlotsByName(Lobby);
    } // setupUi

    void retranslateUi(QWidget *Lobby)
    {
        Lobby->setWindowTitle(QCoreApplication::translate("Lobby", "Form", nullptr));
        wait_label->setText(QCoreApplication::translate("Lobby", "\353\214\200\352\270\260\354\213\244", nullptr));
        status_label->setText(QCoreApplication::translate("Lobby", "\355\232\214\354\235\230\354\203\201\355\203\234: \354\247\204\355\226\211\354\244\221", nullptr));
        enterButton->setText(QCoreApplication::translate("Lobby", "\354\236\205\354\236\245\355\225\230\352\270\260", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Lobby: public Ui_Lobby {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOBBY_H
