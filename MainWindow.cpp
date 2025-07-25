#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "meeting.h"
#include "meetingLocal.h"
#include "start.h"
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
    meetingLocal *meetingLocalPage = new meetingLocal(this);
    Start *startPage = new Start(this);
    Lobby *lobbyPage = new Lobby(this);
    meeting *meetingPage = new meeting(this);

    ui.stackedWidget->addWidget(meetingLocalPage);
    ui.stackedWidget->addWidget(startPage);
    ui.stackedWidget->addWidget(lobbyPage);
    ui.stackedWidget->addWidget(meetingPage);

    ui.stackedWidget->setCurrentWidget(startPage);
    connect(lobbyPage, &Lobby::enterMeetingRequested, this, [=](){
        ui.stackedWidget->setCurrentWidget(meetingPage);
    });
    connect(startPage, &Start::enterRemotePageRequested, this, [=](){
        ui.stackedWidget->setCurrentWidget(lobbyPage);
    });
    connect(startPage, &Start::enterLocalPageRequested, this, [=](){
        ui.stackedWidget->setCurrentWidget(meetingLocalPage);
    });
}
MainWindow::~MainWindow(){}
