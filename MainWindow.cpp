#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "meeting.h"
#include "lobby.h"
#include <QAudioSource>
#include <QMediaDevices>
#include <QAudioFormat>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QScreen>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    //, webcamTimer(new QTimer(this))
   // , audioEnabled(true)
{
    ui.setupUi(this);
    //Lobby *lobbyPage = new Lobby(this);
    meeting *meetingPage = new meeting(this);
    ui.stackedWidget->addWidget(meetingPage);
    ui.stackedWidget->setCurrentWidget(meetingPage);
}
MainWindow::~MainWindow(){}
