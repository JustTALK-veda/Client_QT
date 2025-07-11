

#include "MainWindow.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QApplication> // 화면 비율 맞추기
#include <QScreen> // 화면 비율 맞추기


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , videoThread(nullptr)
    , tcpThread(nullptr)
    , coord(nullptr)
{
    auto central = new QWidget(this);
    layout = new QGridLayout(central);
    setCentralWidget(central);

    coord = new Coordinate();

    layout->setSpacing(0);  // 그리드 셀 간의 간격을 없앰

    const int rowCount = 4;
    const int colCount = 4;

    for (int i = 0; i < rowCount; ++i) {
        for (int j = 0; j < colCount; ++j) {
            QLabel* label = new QLabel(this);
            label->setStyleSheet("background-color: rgba(169, 169, 169, 0.5);");
            label->setAlignment(Qt::AlignCenter);
            label->setFixedSize(480, 360);  // 크기를 고정
            layout->addWidget(label, i, j);  // 그리드에 배치
            labels.append(label);
        }
    }

    // JSON 파일에서 설정 읽기
    QString ip;
    int rtspPort, tcpPort;
    if (!loadConfigFromJson(ip, rtspPort, tcpPort)) {
        qDebug() << "JSON 설정 파일 읽기 실패, 기본값 사용";
        ip = "192.168.0.85";
        rtspPort = 8554;
        tcpPort = 12345;
    }

    //Metadata 수신 스레드 먼저 시작
    tcpThread = new TcpThread(coord, ip, tcpPort);
    tcpThread->start();

    QString rtspUrl = QString("rtsp://%1:%2/test").arg(ip).arg(rtspPort);
    videoThread = new VideoThread(rtspUrl, nullptr, coord);
    connect(videoThread, &VideoThread::cropped,
            this,       &MainWindow::onCropped,
            Qt::QueuedConnection);
    videoThread->start();


}

MainWindow::~MainWindow() {
    if (videoThread) {
        videoThread->stop();
        videoThread->wait();
        delete videoThread;
    }
    if (tcpThread) {
        tcpThread->requestInterruption();
        tcpThread->wait();
        delete tcpThread;
    }
    qDeleteAll(labels);
    delete coord;
}

void MainWindow::onCropped(int index, const QPixmap &pix) {
    // 새로운 index면 QLabel 생성
    // if (index >= labels.size()) {
    //     auto lbl = new QLabel;
    //     lbl->setStyleSheet("background-color:black;");
    //     lbl->setScaledContents(true);
    //     layout->addWidget(lbl);
    //     labels.append(lbl);
    // }

    // //<화면 비율 맞추기 test>
    // // 화면 크기 가져오기
    // QSize screenSize = qApp->primaryScreen()->size();

    // // QLabel 크기를 화면 크기에 맞게 설정
    // labels[index]->resize(screenSize.width(), screenSize.height());
    // // 해당 QLabel에 pixmap 설정
     if (index < labels.size()) {
         labels[index]->resize(pix.size());
         labels[index]->setPixmap(pix.scaled(labels[index]->size(), Qt::KeepAspectRatio));
     }
   // labels[index]->setPixmap(pix);
//     labels[index]->setPixmap(pix.scaled(labels[index]->size(), Qt::KeepAspectRatio));
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
