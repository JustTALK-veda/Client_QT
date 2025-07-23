#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QAudioSource>
#include <QMediaDevices>
#include <QAudioFormat>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QScreen>
#include <QDebug>
#include "VideoThread.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , videoThread(nullptr)
    , tcpThread(nullptr)
    , coord(new Coordinate)
    //, webcamTimer(new QTimer(this))
   // , audioEnabled(true)
{
    ui.setupUi(this);

    // 연결: 페이지 네비게이션
    connect(ui.prevBtn, &QPushButton::clicked, this, &MainWindow::showPrevPage);
    connect(ui.nextBtn, &QPushButton::clicked, this, &MainWindow::showNextPage);
    connect(ui.exitBtn, &QPushButton::clicked, this, &MainWindow::close);
    connect(ui.pagesStack, &QStackedWidget::currentChanged,
            this, &MainWindow::updateNavButtons);
    updateNavButtons(ui.pagesStack->currentIndex());



    // JSON 설정 로드, 스레드 생성
    QString ip;
    int rtspPort, tcpPort;
    if (!loadConfigFromJson(ip, rtspPort, tcpPort)) {
        ip = "192.168.0.85"; rtspPort = 8554; tcpPort = 12345;
    }

    //meta data 수신 스레드
    tcpThread = new TcpThread(coord, ip, tcpPort);
    tcpThread->start();

    QString rtspUrl = QString("rtsps://%1:%2/test").arg(ip).arg(rtspPort);
    videoThread = new VideoThread(rtspUrl, nullptr, coord);
    connect(videoThread, &VideoThread::fullFrame, this, &MainWindow::updatePano, Qt::QueuedConnection);
    //connect(videoThread, &VideoThread::cropped, this, &MainWindow::onCropped, Qt::QueuedConnection);
    videoThread->start();

    setupPages();
}

MainWindow::~MainWindow() {
    videoThread->stop(); videoThread->wait();
    delete videoThread;
    tcpThread->requestInterruption();
    tcpThread->wait();
    delete tcpThread;
    delete coord;
}

void MainWindow::setupPages() {
    const int cols = 2;
    int total = (labels.size() + perPage -1)/perPage;
    for(int p=0;p<total;++p) {
        QWidget* pg = new QWidget;
        auto *g = new QGridLayout(pg);
        for(int i=0;i<perPage;++i) {
            int idx = p*perPage + i;
            if(idx>=labels.size()) break;
            g->addWidget(labels[idx], idx/cols, idx%cols);
        }
        ui.pagesStack->addWidget(pg);
    }
}

void MainWindow::updateNavButtons(int idx) {
    int cnt = ui.pagesStack->count();
    ui.prevBtn->setEnabled(idx>0);
    ui.nextBtn->setEnabled(idx<cnt-1);
}

void MainWindow::showPrevPage() {
    int idx = ui.pagesStack->currentIndex();
    if(idx>0) ui.pagesStack->setCurrentIndex(idx-1);
}

void MainWindow::showNextPage() {
    int idx = ui.pagesStack->currentIndex();
    if(idx<ui.pagesStack->count()-1) ui.pagesStack->setCurrentIndex(idx+1);
}
void MainWindow::updatePano(const QPixmap &pix) {
    pano->setPixmap(pix.scaled(
        pano->size(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation));
}
void MainWindow::onCropped(int index, const QPixmap &pix) {

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
}

bool MainWindow::loadConfigFromJson(QString &ip, int &rtspPort, int &tcpPort) {
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
