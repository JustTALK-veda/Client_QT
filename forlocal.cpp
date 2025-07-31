#include "forlocal.h"
#include "ui_forlocal.h"

#include <QTimer>
#include <QDateTime>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QImage>
#include <QPixmap>
#include <QStyle>
#include <QNetworkRequest>
#include <QUrl>
#include <QTcpSocket>

ForLocal::ForLocal(QWidget *parent, const QString &roomCode)
    : QWidget(parent)
    , ui(new Ui::ForLocal)
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
        "background-color: #333D4F; "
        "border: 1px solid #4A5972; "
        "border-radius: 8px; "
        "}"
        );
    // Initialize timers
    timeTimer = new QTimer(this);
    connect(timeTimer, &QTimer::timeout, this, &ForLocal::updateTime);
    timeTimer->start(1000);

    webcamUrl = "rtsp://192.168.0.63:8554/test";

    // Initialize UI
    updateTime();
    showWaitingState();
    webcamRetryTimer = new QTimer(this);
    connect(webcamRetryTimer, &QTimer::timeout, this, &ForLocal::checkWebcamServer);
    webcamRetryTimer->start(3000);

}

ForLocal::~ForLocal()
{
    delete ui;
}

void ForLocal::updateTime()
{
    QString currentTime = QDateTime::currentDateTime().toString("  hh:mm  ");
    ui->timeLabel->setText(currentTime);
}

void ForLocal::showWaitingState()
{
    ui->videoStatusLabel->setText("원격 참가자를 기다리는 중...");
    ui->videoStatusLabel->setVisible(true);
    //ui->participantNameLabel->setVisible(false);

    QImage loadImg = QImage(QCoreApplication::applicationDirPath() + "/config/loading.png");

    // Create waiting state visual
    //waitingImg = QImage(QCoreApplication::applicationDirPath() + "/config/loading.png");
    ui->mainVideoLabel->setPixmap(QPixmap::fromImage(loadImg));
}

void ForLocal::checkWebcamServer()
{
    if (isCheckingServer)
        return;

    isCheckingServer = true;

    QTcpSocket* socket = new QTcpSocket(this);

    // 웹캠 주소에서 IP만 분리
    QUrl url(webcamUrl);
    QString host = url.host();
    int port = url.port(8554);

    socket->connectToHost(host, port);

    connect(socket, &QTcpSocket::connected, this, [=]() {
        qDebug() << "[ForLocal] RTSP 서버 포트 열림. 연결 가능";

        isCheckingServer = false;
        socket->close();
        socket->deleteLater();

        if (webcamRetryTimer && webcamRetryTimer->isActive())
            webcamRetryTimer->stop();

        startWebcamThread();  // RTSP 클라이언트 시작
    });

    connect(socket, &QTcpSocket::errorOccurred, this, [=](QAbstractSocket::SocketError) {
        qDebug() << "[ForLocal] RTSP 서버 아직 안 열림";
        isCheckingServer = false;
        socket->deleteLater();
    });
}

void ForLocal::startWebcamThread() {
    webcamStreamThread  = new VideoThreadWebcam(webcamUrl, ui->mainVideoLabel);

    connect(webcamStreamThread, &VideoThreadWebcam::connected, this, [=]() {
        if (webcamRetryTimer && webcamRetryTimer->isActive())
            webcamRetryTimer->stop();
        qDebug() << "[ForLocal] RTSP 비디오 수신 연결 성공";
        ui->videoStatusLabel->setVisible(false);
    });

    connect(webcamStreamThread, &QThread::finished, webcamStreamThread, &QObject::deleteLater);
    webcamStreamThread->start();
    connect(webcamStreamThread, &VideoThreadWebcam::disconnected, this, &ForLocal::showWaitingState);
    qDebug() << "webcamStreamThread 시작";
}

