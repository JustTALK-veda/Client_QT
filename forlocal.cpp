#include "forlocal.h"
#include "ui_forlocal.h"

#include <QTimer>
#include <QDateTime>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QPainter>
#include <QPixmap>
#include <QStyle>

ForLocal::ForLocal(QWidget *parent, const QString &roomCode)
    : QWidget(parent)
    , ui(new Ui::ForLocal)
    , isMuted(false)
    , isCameraEnabled(true)
    , hasParticipants(false)
    , roomCode(roomCode)
    , participantCount(1)
    , hostName("호스트" + QString::number(QRandomGenerator::global()->bounded(1000)))
{
    ui->setupUi(this);

    // Apply consistent styling with the existing project
    setStyleSheet(
        "QWidget#ForLocal { background-color: #101828; }"
        "QFrame#headerFrame { "
        "background-color: #1E2939; "
        "border-bottom: 1px solid #304159; "
        "}"
        "QFrame#videoFrame { "
        "background-color: #364153; "
        "border: 4px solid #ED6B06; "
        "border-radius: 8px; "
        "}"
        "QFrame#controlFrame { "
        "background-color: #1E2939; "
        "border-top: 1px solid #5A5D89; "
        "}"
        "QLabel { color: white; }"
        "QPushButton { "
        "background-color: #4A5565; "
        "border: 2px solid #4A5972; "
        "border-radius: 30px; "
        "color: white; "
        "font-size: 16px; "
        "}"
        "QPushButton:hover { background-color: #5A6575; }"
        "QPushButton:pressed { background-color: #3A4555; }"
        "QPushButton#endCallButton { "
        "background-color: #ED6B06; "
        "border-color: #ED6B06; "
        "}"
        "QPushButton#endCallButton:hover { background-color: #DC5A00; }"
        "QPushButton#muteButton[muted=\"true\"] { "
        "background-color: #DC2626; "
        "border-color: #DC2626; "
        "}"
        "QPushButton#cameraButton[disabled=\"true\"] { "
        "background-color: #DC2626; "
        "border-color: #DC2626; "
        "}"
        "QPushButton#layoutButton { "
        "background-color: #364153; "
        "border: 1px solid #4A5972; "
        "border-radius: 4px; "
        "padding: 8px 16px; "
        "font-size: 12px; "
        "}"
        "QPushButton#layoutButton:hover { background-color: #4A5972; }"
        );

    //setupConnections();

    // Initialize timers
    timeTimer = new QTimer(this);
    connect(timeTimer, &QTimer::timeout, this, &ForLocal::updateTime);
    timeTimer->start(1000);

    // participantTimer = new QTimer(this);
    webcamThread = new VideoThreadWebcam("127.0.0.1:8554/test", ui->mainVideoLabel);
    // connect(webcamThread, &VideoThreadWebcam::connected, this, &ForLocal::onConnected);
    // connect(webcamThread, &VideoThreadWebcam::receiveFrame, this, &ForLocal::showParticipantVideo);

    webcamThread->start();
    // connect(participantTimer, &QTimer::timeout, this, &ForLocal::acceptParticipantJoin);
    // participantTimer->setSingleShot(true);
    // participantTimer->start(5000); // Simulate participant joining after 5 seconds

    // Initialize UI
    updateTime();
    //updateControlButtons();
    showWaitingState();

    // Set room code if provided
    // if (!roomCode.isEmpty()) {
    //     ui->roomCodeLabel->setText("Room: " + roomCode);
    // }
}

ForLocal::~ForLocal()
{
    delete ui;
}

void ForLocal::setupConnections()
{
    //connect(ui->endCallButton, &QPushButton::clicked, this, &ForLocal::onEndCallClicked);
    //connect(ui->muteButton, &QPushButton::clicked, this, &ForLocal::onMuteToggleClicked);
    //connect(ui->cameraButton, &QPushButton::clicked, this, &ForLocal::onCameraToggleClicked);
}

void ForLocal::updateTime()
{
    QString currentTime = QDateTime::currentDateTime().toString("  hh:mm  ");
    ui->timeLabel->setText(currentTime);
}

void ForLocal::updateControlButtons()
{
    // Update mute button
    // if (isMuted) {
    //     ui->muteButton->setText("🔇");
    //     ui->muteButton->setProperty("muted", "true");
    //     ui->muteButton->setToolTip("마이크 켜기");
    // } else {
    //     ui->muteButton->setText("🎤");
    //     ui->muteButton->setProperty("muted", "false");
    //     ui->muteButton->setToolTip("마이크 끄기");
    // }

    // // Update camera button
    // if (!isCameraEnabled) {
    //     ui->cameraButton->setText("📹❌");
    //     ui->cameraButton->setProperty("disabled", "true");
    //     ui->cameraButton->setToolTip("카메라 켜기");
    // } else {
    //     ui->cameraButton->setText("📹");
    //     ui->cameraButton->setProperty("disabled", "false");
    //     ui->cameraButton->setToolTip("카메라 끄기");
    // }

    // // Force style update
    // ui->muteButton->style()->unpolish(ui->muteButton);
    // ui->muteButton->style()->polish(ui->muteButton);
    // ui->cameraButton->style()->unpolish(ui->cameraButton);
    // ui->cameraButton->style()->polish(ui->cameraButton);
}

void ForLocal::showWaitingState()
{
    ui->videoStatusLabel->setText("원격 참가자를 기다리는 중...");
    ui->videoStatusLabel->setVisible(false);
    //ui->participantNameLabel->setVisible(false);

    // Create waiting state visual
    // QPixmap waitingPixmap(800, 450);
    // waitingPixmap.fill(QColor("#364153"));

    // QPainter painter(&waitingPixmap);
    // painter.setRenderHint(QPainter::Antialiasing);

    // // Draw waiting icon
    // QFont iconFont = painter.font();
    // iconFont.setPointSize(72);
    // painter.setFont(iconFont);
    // painter.drawText(waitingPixmap.rect(), Qt::AlignCenter, "⏳");

    // ui->mainVideoLabel->setPixmap(waitingPixmap);
}

// void ForLocal::showParticipantVideo(QPixmap& videoPixMap)
// {
//     // // Create participant video placeholder
//     // QPixmap videoPixmap(800, 450);
//     // videoPixmap.fill(QColor("#1E2939"));

//     // QPainter painter(&videoPixmap);
//     // painter.setRenderHint(QPainter::Antialiasing);

//     // // Draw participant avatar
//     // painter.setBrush(QBrush(QColor("#ED6B06")));
//     // painter.setPen(Qt::NoPen);
//     // painter.drawEllipse(350, 175, 100, 100);

//     // // Draw participant initial
//     // painter.setPen(QColor("white"));
//     // QFont font = painter.font();
//     // font.setPointSize(36);
//     // font.setBold(true);
//     // painter.setFont(font);
//     // painter.drawText(350, 175, 100, 100, Qt::AlignCenter, "참");

//     // // Add speaking indicator
//     // painter.setBrush(QBrush(QColor("#ED6B06")));
//     // painter.drawEllipse(videoPixmap.width() - 30, 20, 20, 20);

//     ui->mainVideoLabel->setPixmap(videoPixMap);
// }

void ForLocal::onEndCallClicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "통화 종료",
        "정말로 통화를 종료하시겠습니까?",
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        emit leaveMeetingRequested();
    }
}

void ForLocal::onMuteToggleClicked()
{
    isMuted = !isMuted;
    updateControlButtons();
}

void ForLocal::onCameraToggleClicked()
{
    isCameraEnabled = !isCameraEnabled;
    updateControlButtons();
}

// void ForLocal::onConnected()
// {
//     ui->videoStatusLabel->setVisible(false);
//     ui->participantNameLabel->setText("참가자123 (원격)");
//     ui->participantNameLabel->setVisible(true);
//     hasParticipants = true;
    
//     qDebug() << "[ForLocal] Webcam thread connected successfully";

//     // Create participant video placeholder
//     QPixmap tempPixmap(800, 450);
//     tempPixmap.fill(QColor("#1E2939"));

//     QPainter painter(&tempPixmap);
//     painter.setRenderHint(QPainter::Antialiasing);

//     // Draw participant avatar
//     painter.setBrush(QBrush(QColor("#ED6B06")));
//     painter.setPen(Qt::NoPen);
//     painter.drawEllipse(350, 175, 100, 100);

//     // Draw participant initial
//     painter.setPen(QColor("white"));
//     QFont font = painter.font();
//     font.setPointSize(36);
//     font.setBold(true);
//     painter.setFont(font);
//     painter.drawText(350, 175, 100, 100, Qt::AlignCenter, "참");

//     // Add speaking indicator
//     painter.setBrush(QBrush(QColor("#ED6B06")));
//     painter.drawEllipse(tempPixmap.width() - 30, 20, 20, 20);
//     // this->showParticipantVideo(tempPixmap);
// }
