#include "lobby.h"
#include "ui_lobby.h"
#include <QTimer>
#include <QDateTime>
#include <QPainter>
#include <QStyleOption>
#include <QSizePolicy>
#include "audio_control.h"


Lobby::Lobby(QWidget *parent, CameraWidget* webcamFrame)
    : QWidget(parent)
    , ui(new Ui::Lobby)
    , isConnecting(false)
    , meetingInProgress(false)
    , isCheckingServer(false)
    , cameraWidget(webcamFrame)
{
    ui->setupUi(this);

    //camerawidget 배치
    ui->videoFrame->layout()->addWidget(cameraWidget);

    setStyleSheet(
        "QWidget#Lobby { background-color: #101828; }"
        "QWidget#content { background-color: #101828; }"

        "QLabel { color: white; }"
        "QPushButton#enterButton { "
        "background-color: #ED6B06; "
        "color: white; "
        "border: none; "
        "border-radius: 6px; "
        "padding: 12px 24px; "
        "font-weight: bold; "
        "}"
        "QPushButton#enterButton:hover { background-color: #d55a05; }"
        "QPushButton#enterButton:disabled { "
        "background-color: #4A5565; "
        "color: #9CA3AF; "
        "}"
        "QFrame#headerFrame { "
        "background-color: #1E2939; "
        "border-bottom: 1px solid #304159; "
        "}"
        "QFrame#videoFrame { "
        "background-color: #364153; "
        "border: 1px solid #4A5972; "
        "border-radius: 8px; "
        "}"
        "QFrame#controlsFrame { "
        "background-color: #364153; "
        "border: 1px solid #4A5972; "
        "border-radius: 8px; "
        "}"
        "QCheckBox { color: white; }"
        );
    
    // Setup timer for clock
    timeTimer = new QTimer(this);
    connect(timeTimer, &QTimer::timeout, this, &Lobby::updateTime);
    timeTimer->start(1000);
    updateTime();
    
    // Connect signals
    connect(ui->enterButton, &QPushButton::clicked, this, &Lobby::handleJoinMeeting);
    connect(ui->backButton, &QPushButton::clicked, this, &Lobby::goBackRequested);
    connect(ui->cameraToggleButton, &QPushButton::toggled, this, [=](bool checked) {
        ui->cameraToggleButton->setIcon(QIcon(checked ? ":/Image/config/video_on.png" : ":/Image/config/video_off.png"));
        cameraWidget->setCamEnabled(checked);
    });

    connect(ui->micToggleButton, &QPushButton::toggled, this, [=](bool checked) {
        ui->micToggleButton->setIcon(QIcon(checked ? ":/Image/config/mic_on.png" : ":/Image/config/mic_off.png"));
        cameraWidget->setMicEnabled(checked);
    });
    
    // Initialize UI state
    ui->connectionStatusLabel->setText("회의가 시작되면 참가할 수 있습니다.");
    ui->connectionStatusLabel->setVisible(true);
    //checkRtspServer();
    serverCheckTimer = new QTimer(this);
    connect(serverCheckTimer, &QTimer::timeout, this, &Lobby::checkRtspServer);
    serverCheckTimer->start(3000);
}

Lobby::~Lobby()
{
    delete ui;
}

void Lobby::updateTime()
{
    QString currentTime = QDateTime::currentDateTime().toString("  hh:mm  ");
    ui->timeLabel->setText(currentTime);
}

void Lobby::checkRtspServer() {
    if (isCheckingServer) return;
    isCheckingServer = true;

    QString rtspUrl = QString("rtsps://192.168.0.50:8555/test");
    videoThread = new VideoThread(rtspUrl, nullptr, nullptr, true); //checkonly

    connect(videoThread, &VideoThread::serverReady, this, &Lobby::onServerReady);
    connect(videoThread, &QThread::finished, videoThread, &QObject::deleteLater);

    videoThread->start();
}

void Lobby::onServerReady(bool success) {
    meetingInProgress = success;

    if (success) {
        qDebug() << "[Lobby] RTSP 서버 작동 확인됨.";
    } else {
        qDebug() << "[Lobby] RTSP 서버 연결 실패.";
    }

    updateMeetingStatus();
    updateJoinButton();

    isCheckingServer = false;  // 다음 체크 허용
}

void Lobby::updateMeetingStatus()
{
    if (meetingInProgress) {
        ui->meetingStatusLabel->setText("회의중");
        ui->meetingStatusIndicator->setStyleSheet(
            "QLabel { "
                "background-color: #ED6B06; "
                "border: 2px solid #ED6B06; "
                "border-radius: 7px; "
                "min-width: 14px; "
                "min-height: 14px; "
            "}"
        );
        ui->meetingDescLabel->setText("현재 회의가 진행 중입니다. 참가할 수 있습니다.");
    } else {
        //ui->meetingStatusLabel->setText("회의 없음");
        ui->meetingStatusIndicator->setStyleSheet(
            "QLabel { "
                "background-color: transparent; "
                "border: 2px solid #ED6B06; "
                "border-radius: 7px; "
                "min-width: 14px; "
                "min-height: 14px; "
            "}"
        );
        ui->meetingDescLabel->setText("회의가 시작되면 참가할 수 있습니다.");
    }
}

void Lobby::updateJoinButton()
{
    if (isConnecting) {
        ui->enterButton->setText("연결 중...");
        ui->enterButton->setEnabled(false);
        ui->connectionStatusLabel->setText("회의실에 연결하고 있습니다...");
        ui->connectionStatusLabel->setVisible(true);
    } else if (!meetingInProgress) {
        ui->enterButton->setText("회의 참가");
        ui->enterButton->setEnabled(false);
        ui->connectionStatusLabel->setText("회의가 시작되면 참가할 수 있습니다.");
        ui->connectionStatusLabel->setVisible(true);
    } else {
        ui->enterButton->setText("회의 참가");
        ui->enterButton->setEnabled(true);
        ui->connectionStatusLabel->setVisible(false);
    }
}

void Lobby::handleJoinMeeting()
{
    if (!meetingInProgress || isConnecting) return;
    
    isConnecting = true;
    updateJoinButton();
    
    // Simulate connection delay
    QTimer::singleShot(2000, this, [this]() {
        isConnecting = false;
        emit enterMeetingRequested();
    });
}

