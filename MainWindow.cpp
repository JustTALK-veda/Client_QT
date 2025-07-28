#include "MainWindow.h"
#include "grid.h"
#include "meeting.h"
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
#include <QSizePolicy>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // 중앙 위젯 설정
    QWidget* central = new QWidget(this);
    central->setStyleSheet("background-color: #101828;");
    this->setCentralWidget(central);

    // 전체 레이아웃
    QVBoxLayout* mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // QStackedWidget 생성 및 추가
    stackedWidget = new QStackedWidget(this);
    stackedWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainLayout->addWidget(stackedWidget);

    // 페이지 생성
    Start* startPage = new Start(this);
    Lobby* lobbyPage = new Lobby(this);
    meeting* meetingPage = new meeting(this);

    // 페이지 등록
    stackedWidget->addWidget(startPage);
    stackedWidget->addWidget(lobbyPage);
    stackedWidget->addWidget(meetingPage);

    // 초기 페이지 설정
    stackedWidget->setCurrentWidget(startPage);

    // 페이지 전환 연결
    connect(startPage, &Start::enterRemotePageRequested, this, [=]() {
        stackedWidget->setCurrentWidget(lobbyPage);
    });

    connect(lobbyPage, &Lobby::enterMeetingRequested, this, [=]() {
        stackedWidget->setCurrentWidget(meetingPage);
    });
}
MainWindow::~MainWindow(){}


