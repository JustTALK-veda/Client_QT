#include "meeting.h"
#include <QAudioSource>
#include <QMediaDevices>
#include <QAudioFormat>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QScreen>
#include <QDebug>
#include <QDialog>


meeting::meeting(QWidget *parent)
    : QWidget{parent}
    , videoThread(nullptr)
    , tcpThread(nullptr)
    , coord(new Coordinate)
    , ui(new Ui::meetingForm)
{
    ui->setupUi(this);



    shared_frame_ptr = new cv::Mat();
    // 4) Next 버튼을 눌렀을 때 페이지 전환하도록 연결
    connect(ui->nextButton, &QPushButton::clicked, ui->stackedWidget, &Stackpage::goToNextPage);
    connect(ui->prevButton, &QPushButton::clicked, ui->stackedWidget, &Stackpage::goToPreviousPage);
    connect(ui->exitButton, &QPushButton::clicked, ui->stackedWidget, &Stackpage::close);

    QString ip;
    int rtspPort, tcpPort;
    // if (!loadConfigFromJson(ip, rtspPort, tcpPort)) {
    //     ip = "192.168.0.50"; rtspPort = 8555; tcpPort = 12345;
    // }

    //meta data 수신 스레드
    tcpThread = new TcpThread(coord, "192.168.0.85", 12345);

    tcpThread->start();

    QString rtspUrl = QString("rtsps://192.168.0.85:8555/test");
    videoThread = new VideoThread(rtspUrl, nullptr, coord);
    connect(videoThread, &VideoThread::fullFrame, this, &meeting::updatePano, Qt::QueuedConnection);
    connect(videoThread, &VideoThread::cropped, ui->stackedWidget, &Stackpage::setLabel);
    videoThread->start();

    // 웹캠 다이얼로그 추가
    camDialog = new QDialog(this, Qt::Window);
    camDialog->setWindowTitle("나");
    camDialog->resize(240, 180);
    camerawidget = new CameraWidget(camDialog, QSize(320, 240));
    camDialog->show();
    //std::thread(start_rtsp_server).detach();
}

meeting::~meeting() {
    videoThread->stop(); videoThread->wait();
    delete videoThread;
    tcpThread->requestInterruption();
    tcpThread->wait();
    delete tcpThread;
    delete coord;
}

void meeting::updatePano(const QPixmap &pix) {
    ui->pano->setPixmap(pix.scaled(
        ui->pano->size(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation));
}

bool meeting::loadConfigFromJson(QString &ip, int &rtspPort, int &tcpPort) {
    QFile file("config/rpi_ip.json");

    // 파일 열기 확인
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "JSON 파일을 열 수 없습니다:" << file.errorString();
        return false;
    }

    // 파일 내용 읽기
    QByteArray data = file.readAll();
    file.close();

    // JSON 파싱
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "JSON 파싱 오류:" << parseError.errorString();
        return false;
    }

    if (!jsonDoc.isArray()) {
        qDebug() << "JSON 루트는 배열이어야 합니다";
        return false;
    }

    QJsonArray jsonArray = jsonDoc.array();
    if (jsonArray.isEmpty()) {
        qDebug() << "JSON 배열이 비어있습니다";
        return false;
    }

    // 첫 번째 객체에서 설정값 읽기
    QJsonObject configObj = jsonArray[0].toObject();

    if (!configObj.contains("ip") || !configObj.contains("rtsp_port") || !configObj.contains("tcp_port")) {
        qDebug() << "필수 필드가 누락되었습니다 (ip, rtsp_port, tcp_port)";
        return false;
    }

    ip = configObj["ip"].toString();
    rtspPort = configObj["rtsp_port"].toInt();
    tcpPort = configObj["tcp_port"].toInt();

    qDebug() << "JSON 설정 로드 성공:";
    qDebug() << "IP:" << ip;
    qDebug() << "RTSP Port:" << rtspPort;
    qDebug() << "TCP Port:" << tcpPort;

    return true;
}

