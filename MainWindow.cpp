

#include "MainWindow.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QApplication> // 화면 비율 맞추기
#include <QScreen> // 화면 비율 맞추기
#include <QScrollArea>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , videoThread(nullptr)
    , tcpThread(nullptr)
    , coord(nullptr)
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    auto *mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(5);

    // 2) 종료 버튼 레이아웃
    auto *btnLayout = new QHBoxLayout;
    btnLayout->addStretch();                                    // 왼쪽 빈공간
    QPushButton *closeBtn = new QPushButton("종료", central);  // 종료 버튼
    connect(closeBtn, &QPushButton::clicked, this, &MainWindow::close);
    btnLayout->addWidget(closeBtn);                             // 버튼 추가
    //btnLayout->addStretch();                                    // 오른쪽 빈공간
    mainLayout->addLayout(btnLayout);

    auto *scroll = new QScrollArea(central);
    scroll->setWidgetResizable(true);
    QWidget *container = new QWidget;
    scroll->setWidget(container);
    //setCentralWidget(scroll);

    // 2) container 에 그리드 레이아웃 붙이기
    layout = new QGridLayout(container);
    layout->setContentsMargins(5,5,5,5);
    layout->setHorizontalSpacing(10);
    layout->setVerticalSpacing(10);

    coord = new Coordinate();

    const int rowCount = 2;
    const int colCount = 2;

    int w = colCount * 480 + 40;
    int h = closeBtn->sizeHint().height() + rowCount * 360 + 80;
    // int w = colCount * 480 + 40;
    // int h = closeBtn->sizeHint().height() + rowCount * 360 + 80;
    resize(w, h);

    for (int i = 0; i < rowCount; ++i) {
        for (int j = 0; j < colCount; ++j) {
            QLabel* label = new QLabel(container);
            label->setStyleSheet("background-color: rgba(169, 169, 169, 0.5);");
            label->setAlignment(Qt::AlignCenter);
            label->setFixedSize(480, 360);  // 크기를 고정
            layout->addWidget(label, i, j);  // 그리드에 배치
            labels.append(label);
        }
    }
    mainLayout->addWidget(scroll);

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
    bool ok = connect(videoThread, &VideoThread::cropped,
            this,       &MainWindow::onCropped,
            Qt::QueuedConnection);
    qDebug()<<"MainWindow cropped ok?" <<ok;

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
   \
        qDebug() << "[onCropped] called! index=" << index
        << " pix.isNull()=" << pix.isNull();

     // if (index < labels.size()) {
     //     QSize labelSize(480,360);
     //     labels[index]->resize(labelSize);
     //     labels[index]->setPixmap(pix.scaled(labelSize, Qt::KeepAspectRatio));
     //     //* labels[index]->resize(pix.size());
     //     //* labels[index]->setPixmap(pix.scaled(labels[index]->size(), Qt::KeepAspectRatio)); // pix는 원본 값을 가져옴
     // }

    if (index >= labels.size()){

         auto *lbl = new QLabel(container);
         lbl->setStyleSheet("background-color: rgba(169, 169, 169, 0.5);");
         lbl->setAlignment(Qt::AlignCenter);
         lbl->setFixedSize(480, 360);
         int row = index / 2;
         int col = index % 2;
         layout->addWidget(lbl, row, col);

         labels.append(lbl);
    }

    // 2) 이미 추가된 레이블에 pixmap 설정 또는 숨기기
    QLabel *target = labels[index];
    if (pix.isNull()) {
        target->hide();
    } else {
        target->show();
        // 이미 480×360 크롭된 이미지이므로 스케일 없이 그대로 세팅
        target->setPixmap(pix);
    }

    //현재 index 이후 레이블은 보이지 않게
    for (int i = index + 1; i < labels.size(); ++i) {
        labels[i]->hide();
    } //새로 그려야 할 이미지가 없는 레이블은 화면에서 사라지게 만들기
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
