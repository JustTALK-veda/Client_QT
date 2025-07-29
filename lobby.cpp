#include "lobby.h"
#include "ui_lobby.h"
#include <QTimer>
#include <QDateTime>
#include <QPainter>
#include <QStyleOption>
#include <QSizePolicy>
#include "audio_control.h"


Lobby::Lobby(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Lobby)
    , videoEnabled(true)
    , audioEnabled(true)
    , isConnecting(false)
    , meetingInProgress(true)
    , cameraWidget(nullptr)
{
    ui->setupUi(this);

    cameraWidget=ui->videoPreview;

    // this->setFixedSize(this->sizeHint());
    // this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // ui->contentLayout->insertStretch(0,1);
    // ui->contentLayout->addStretch(1);

    // ui->contentLayout->setAlignment(Qt::AlignCenter);
    // this->adjustSize();

    // Set dark theme colors
    // setStyleSheet(
    //     "QWidget#Lobby { background-color: #101828; }"
    //     "QWidget#content { background-color: #101828; }"

    //     "QLabel { color: white; }"
    //     "QPushButton { "
    //         "background-color: #ED6B06; "
    //         "color: white; "
    //         "border: none; "
    //         "border-radius: 6px; "
    //         "padding: 12px 24px; "
    //         "font-weight: bold; "
    //     "}"
    //     "QPushButton:hover { background-color: #d55a05; }"
    //     "QPushButton:disabled { "
    //         "background-color: #4A5565; "
    //         "color: #9CA3AF; "
    //     "}"
    //     "QFrame#headerFrame { "
    //         "background-color: #1E2939; "
    //         "border-bottom: 1px solid #304159; "
    //     "}"
    //     "QFrame#videoFrame { "
    //         "background-color: #364153; "
    //         "border: 1px solid #4A5972; "
    //         "border-radius: 8px; "
    //     "}"
    //     "QFrame#controlsFrame { "
    //         "background-color: #364153; "
    //         "border: 1px solid #4A5972; "
    //         "border-radius: 8px; "
    //     "}"
    //     "QCheckBox { color: white; }"
    //     "QCheckBox::indicator { "
    //         "width: 40px; "
    //         "height: 20px; "
    //         "border-radius: 2px; "
    //         "background-color: #4A5972; "
    //     "}"
    //     "QCheckBox::indicator:checked { "
    //         "background-color: #ED6B06; "
    //     "}"
    //     "QCheckBox::indicator::handle { "
    //         "width: 16px; "
    //         "height: 16px; "
    //         "border-radius: 8px; "
    //         "background-color: white; "
    //     "}"
    // );
    setStyleSheet(
        "QWidget#Lobby { background-color: #101828; }"
        "QWidget#content { background-color: #101828; }"

        "QLabel { color: white; }"
        "QPushButton { "
        "background-color: #ED6B06; "
        "color: white; "
        "border: none; "
        "border-radius: 6px; "
        "padding: 12px 24px; "
        "font-weight: bold; "
        "}"
        "QPushButton:hover { background-color: #d55a05; }"
        "QPushButton:disabled { "
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
    connect(ui->videoToggle, &QCheckBox::toggled, this, &Lobby::setVideoEnabled);
    connect(ui->audioToggle, &QCheckBox::toggled, this, &Lobby::setAudioEnabled);
    connect(ui->settingsButton, &QPushButton::clicked, this, &Lobby::showSettings);
    //connect(ui->statusToggleButton, &QPushButton::clicked, this, &Lobby::toggleMeetingStatus);
    
    // Initialize UI state
    updateVideoPreview();
    updateMeetingStatus();
    updateJoinButton();
}

Lobby::~Lobby()
{
    delete ui;
}

void Lobby::setVideoEnabled(bool enabled)
{
    videoEnabled = enabled;
    updateVideoPreview();

    if(enabled){
        cameraWidget->startCam();
        //enable_streaming(true); 나중에 서버에서 호출
    }
    else{
        cameraWidget->stopCam();
       // enable_streaming(false); 나중에 서버에서 호출
    }
}

void Lobby::setAudioEnabled(bool enabled)
{
    audioEnabled = enabled;
    // Update audio icon or indicator as needed
    set_mic_enabled(enabled);
    qDebug() << "[Lobby] setAudioEnabled called, enabled =" << enabled;
}

void Lobby::updateTime()
{
    QString currentTime = QDateTime::currentDateTime().toString("hh:mm");
    ui->timeLabel->setText(currentTime);
}

void Lobby::updateVideoPreview()
{
    if (videoEnabled) {
        // ui->videoStatusLabel->setText("카메라 미리보기"); //이거 살리면 웹캠 안됨
        // ui->videoPreview->setStyleSheet(
        //     "QLabel { "
        //         "background-color: #1E2939; "
        //         "border-radius: 50px; "
        //         "color: white; "
        //         "font-size: 24px; "
        //         "font-weight: bold; "
        //     "}"
        // );
        //ui->videoPreview->setText("WebcamPreview appear here");
        //ui->videoPreview->setAlignment(Qt::AlignCenter);
    } else {
        // ui->videoStatusLabel->setText("카메라가 꺼져있습니다");
        ui->videoPreview->setStyleSheet(
            "QLabel { "
                "background-color: #1E2939; "
                "color: #9CA3AF; "
            "}"
        );
        //ui->videoPreview->setText("📷 camera off"); // Camera off icon
        //ui->videoPreview->setAlignment(Qt::AlignCenter);
    }
}

void Lobby::updateMeetingStatus()
{
    if (meetingInProgress) {
        ui->meetingStatusLabel->setText("회의중");
        ui->meetingStatusIndicator->setStyleSheet(
            "QLabel { "
                "background-color: #ED6B06; "
                "border: 2px solid #ED6B06; "
                "border-radius: 8px; "
                "min-width: 16px; "
                "min-height: 16px; "
            "}"
        );
        ui->meetingDescLabel->setText("현재 회의가 진행 중입니다. 참가할 수 있습니다.");
    } else {
        ui->meetingStatusLabel->setText("회의 없음");
        ui->meetingStatusIndicator->setStyleSheet(
            "QLabel { "
                "background-color: transparent; "
                "border: 2px solid #ED6B06; "
                "border-radius: 8px; "
                "min-width: 16px; "
                "min-height: 16px; "
            "}"
        );
        ui->meetingDescLabel->setText("현재 진행 중인 회의가 없습니다.");
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
        ui->enterButton->setText("회의 없음");
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

void Lobby::toggleMeetingStatus()
{
    meetingInProgress = !meetingInProgress;
    updateMeetingStatus();
    updateJoinButton();
}

void Lobby::showSettings()
{
    // Implement settings dialog
}
