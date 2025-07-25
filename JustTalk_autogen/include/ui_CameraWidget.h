/********************************************************************************
** Form generated from reading UI file 'CameraWidget.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CAMERAWIDGET_H
#define UI_CAMERAWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_cameraWidgetForm
{
public:
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QLabel *webcam;
    QHBoxLayout *horizontalLayout;
    QPushButton *CamButton;
    QPushButton *MicButton;

    void setupUi(QWidget *cameraWidgetForm)
    {
        if (cameraWidgetForm->objectName().isEmpty())
            cameraWidgetForm->setObjectName("cameraWidgetForm");
        cameraWidgetForm->resize(551, 384);
        verticalLayoutWidget = new QWidget(cameraWidgetForm);
        verticalLayoutWidget->setObjectName("verticalLayoutWidget");
        verticalLayoutWidget->setGeometry(QRect(50, 49, 471, 331));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        webcam = new QLabel(verticalLayoutWidget);
        webcam->setObjectName("webcam");

        verticalLayout->addWidget(webcam);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        CamButton = new QPushButton(verticalLayoutWidget);
        CamButton->setObjectName("CamButton");

        horizontalLayout->addWidget(CamButton);

        MicButton = new QPushButton(verticalLayoutWidget);
        MicButton->setObjectName("MicButton");

        horizontalLayout->addWidget(MicButton);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(cameraWidgetForm);

        QMetaObject::connectSlotsByName(cameraWidgetForm);
    } // setupUi

    void retranslateUi(QWidget *cameraWidgetForm)
    {
        cameraWidgetForm->setWindowTitle(QCoreApplication::translate("cameraWidgetForm", "Form", nullptr));
        webcam->setText(QCoreApplication::translate("cameraWidgetForm", "cam", nullptr));
        CamButton->setText(QCoreApplication::translate("cameraWidgetForm", "\354\271\264\353\251\224\353\235\274 OFF", nullptr));
        MicButton->setText(QCoreApplication::translate("cameraWidgetForm", "\353\247\210\354\235\264\355\201\254 OFF", nullptr));
    } // retranslateUi

};

namespace Ui {
    class cameraWidgetForm: public Ui_cameraWidgetForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CAMERAWIDGET_H
