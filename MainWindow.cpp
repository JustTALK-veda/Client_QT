#include "MainWindow.h"
#include "ui_MainWindow.h"
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
    //, webcamTimer(new QTimer(this))
   // , audioEnabled(true)
{
    ui.setupUi(this);
    this->resize(1920, 1080);
    this->setMinimumSize(800, 600);

    Start *startPage = new Start(this);
    Lobby *lobbyPage = new Lobby(this);
    meeting *meetingPage = new meeting(this);

    QVBoxLayout* layout = new QVBoxLayout(ui.centralwidget);
    layout->addWidget(ui.stackedWidget);

    setCentralWidget(ui.stackedWidget);
    ui.stackedWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

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
}
MainWindow::~MainWindow(){}


