// #include "MainWindow.h"
// #include "ui_MainWindow.h"
// #include "meeting.h"
// #include "start.h"
// #include "lobby.h"
// #include <QAudioSource>
// #include <QMediaDevices>
// #include <QAudioFormat>
// #include <QJsonDocument>
// #include <QJsonObject>
// #include <QJsonArray>
// #include <QFile>
// #include <QScreen>
// #include <QDebug>

// MainWindow::MainWindow(QWidget *parent)
//     : QMainWindow(parent)
//     //, webcamTimer(new QTimer(this))
//    // , audioEnabled(true)
// {
//     ui.setupUi(this);
//     Start *startPage = new Start(this);
//     Lobby *lobbyPage = new Lobby(this);
//     meeting *meetingPage = new meeting(this);


//     ui.stackedWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);


//     ui.stackedWidget->addWidget(startPage);
//     ui.stackedWidget->addWidget(lobbyPage);
//     ui.stackedWidget->addWidget(meetingPage);

//     ui.stackedWidget->setCurrentWidget(startPage);
//     connect(lobbyPage, &Lobby::enterMeetingRequested, this, [=](){
//         ui.stackedWidget->setCurrentWidget(meetingPage);
//     });
//     connect(startPage, &Start::enterRemotePageRequested, this, [=](){
//         ui.stackedWidget->setCurrentWidget(lobbyPage);
//     });
// }
// MainWindow::~MainWindow(){}
// MainWindow.cpp

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "meeting.h"
#include "meetingLocal.h"
#include "start.h"
#include "lobby.h"

#include <QVBoxLayout>     // 레이아웃 헤더
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

{
    ui.setupUi(this);

/*
    //stackwidget이 부모 레이아웃에서 남는 공간
    ui.stackedWidget->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Expanding
    );*/
    
    // 5) 페이지 생성 및 stackedWidget에 추가
    meetingLocal *meetingLocalPage = new meetingLocal(this);
    Start *startPage = new Start(this);
    Lobby *lobbyPage = new Lobby(this);
    meeting *meetingPage = new meeting(this);
    meetingPage->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    ui.stackedWidget->addWidget(meetingLocalPage);
=======

    Start *startPage = new Start(this);
    Lobby *lobbyPage = new Lobby(this);
    meeting *meetingPage = new meeting(this);



   
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
    connect(lobbyPage, &Lobby::enterMeetingRequested, this, [=](){
        ui.stackedWidget->setCurrentWidget(meetingPage);
    });
    connect(startPage, &Start::enterRemotePageRequested, this, [=](){
        ui.stackedWidget->setCurrentWidget(lobbyPage);
    });

}

MainWindow::~MainWindow(){}
