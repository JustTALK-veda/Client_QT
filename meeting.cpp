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

    QString ip;
    int rtspPort, tcpPort;
    // if (!loadConfigFromJson(ip, rtspPort, tcpPort)) {
    //     ip = "192.168.0.50"; rtspPort = 8555; tcpPort = 12345;
    // }

    //meta data 수신 스레드
    tcpThread = new TcpThread(coord, "192.168.0.30", 12345);

    tcpThread->start();

    QString rtspUrl = QString("rtsps://192.168.0.50:8555/test");
    videoThread = new VideoThread(rtspUrl, nullptr, coord);
    connect(videoThread, &VideoThread::fullFrame, this, &meeting::updatePano, Qt::QueuedConnection);
    // bool ok = connect(videoThread, &VideoThread::fullFrame,
    //                   this, &meeting::updatePano,
    //                   Qt::QueuedConnection);
    // qDebug() << "VideoThread::fullFrame connected?" << ok;
    //connect(videoThread, &VideoThread::cropped, this, &meeting::onCropped, Qt::QueuedConnection);
    connect(videoThread, &VideoThread::cropped, ui->stackedWidget, &Stackpage::setLabel);
    videoThread->start();

    // 웹캠 다이얼로그 추가
    camDialog = new QDialog(this, Qt::Window);
    camDialog->setWindowTitle("나");
    camDialog->resize(240, 180);
    camerawidget = new CameraWidget(camDialog, QSize(320, 240));
    camDialog->show();
    std::thread(start_rtsp_server).detach();
}

meeting::~meeting() {
    videoThread->stop(); videoThread->wait();
    delete videoThread;
    tcpThread->requestInterruption();
    tcpThread->wait();
    delete tcpThread;
    delete coord;
}
/*
void meeting::setupPages() {
    const int cols = 2;
    const int total = (labels.size() + perPage - 1) / perPage;

    // 첫 페이지(ui->page)는 Designer에서 만든 gridLayout을 그대로 사용
    QGridLayout *g = ui->gridLayout;
    g->setContentsMargins(0, 0, 0, 0);
    g->setSpacing(5);

    // 기존 첫 페이지에 참여자(라벨) 추가
    for (int i = 0; i < perPage && i < labels.size(); ++i) {
        g->addWidget(labels[i], i / cols, i % cols);
    }

    // 2번째 페이지부터 동적으로 생성
    for (int p = 1; p < total; ++p) {
        QWidget *page = new QWidget(ui->stackedWidget);
        QGridLayout *grid = new QGridLayout(page);
        grid->setContentsMargins(0, 0, 0, 0);
        grid->setSpacing(5);

        for (int i = 0; i < perPage; ++i) {
            int idx = p * perPage + i;
            if (idx >= labels.size()) break;
            grid->addWidget(labels[idx], i / cols, i % cols);
        }
        ui->stackedWidget->addWidget(page);
    }
}*/


void meeting::updatePano(const QPixmap &pix) {
    qDebug() << "pano size =";

        //ui->pano->size();  // ← 확인
    ui->pano->setPixmap(pix.scaled(
        ui->pano->size(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation));
}
/*
void meeting::onCropped(int index, const QPixmap &pix) {

    QLabel *target = labels[index];

    if (pix.isNull()) {
        target->hide();
    } else {
        target->show();
        target->setPixmap(pix);
    }

    // 현재 index 이후 레이블은 보이지 않게
    for (int i = index + 1; i < labels.size(); ++i) {
        labels[i]->hide();
    }
    if (index < labels.size()) {
        labels[index]->setPixmap(pix);  // 해당 크롭된 이미지를 QLabel에 설정
    }
}*/

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

