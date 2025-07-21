

#include "MainWindow.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QApplication> // 화면 비율 맞추기
#include <QScreen> // 화면 비율 맞추기
#include <QScrollArea>
#include <QPushButton>
#include <QTimer>
#include <QMediaDevices>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , videoThread(nullptr)
    , tcpThread(nullptr)
    , coord(nullptr)
    , webcamTimer(new QTimer(this))
    , audioEnabled(true)

{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    auto *mainLayout = new QVBoxLayout(central);

    panoLabel = new QLabel;
    panoLabel->setMinimumHeight(200);
    panoLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(panoLabel);

    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(5);

    auto *hLayout = new QHBoxLayout;
    hLayout->setContentsMargins(0,0,0,0);
    hLayout->setSpacing(10);



    //웹캠
    webcamLabel = new QLabel(central);
    webcamLabel->setFixedSize(320, 240);
    webcamLabel->setAlignment(Qt::AlignCenter);
    webcamLabel->setStyleSheet("background-color: black;");


    if (!cap.open(0)) {
        qDebug() << "웹캠 열기 실패";
    } else {
        connect(webcamTimer, &QTimer::timeout,
                this,        &MainWindow::webcam);
        webcamTimer->start(30);
    }



    //그리드 레이아웃 붙이기
    container=new QWidget(central);
    layout = new QGridLayout(container);
    layout->setContentsMargins(5,5,5,5);
    layout->setHorizontalSpacing(10);
    layout->setVerticalSpacing(10);

    coord = new Coordinate();

    const int rowCount = 2;
    const int colCount = 2;

    for (int i = 0; i < rowCount; ++i) {
        for (int j = 0; j < colCount; ++j) {
            QLabel *lbl = new QLabel(container);
            lbl->setFixedSize(300, 220);
            lbl->setAlignment(Qt::AlignCenter);
            lbl->setStyleSheet("background-color: #F0F0F0;");
            layout->addWidget(lbl, i, j);
            labels.append(lbl);
        }
    }
    auto *scroll = new QScrollArea(central);
    scroll->setWidgetResizable(true);
    scroll->setWidget(container);
    hLayout->addWidget(scroll, 1);

    //오른쪽: 웹캠 + 종료 버튼
    auto *rightLay = new QVBoxLayout;
    rightLay->setContentsMargins(0,0,0,0);
    rightLay->setSpacing(5);

    //웹캠위치
    rightLay->addWidget(webcamLabel, 0, Qt::AlignCenter);

    //웹캠 오디오
    QAudioFormat fmt;
       fmt.setSampleRate(44100);
       fmt.setChannelCount(1);
       fmt.setSampleFormat(QAudioFormat::Int16);


       audioSource  = new QAudioSource(QMediaDevices::defaultAudioInput(), fmt, this);
        audioDevice = audioSource->start();
           connect(audioDevice, &QIODevice::readyRead,
                            this,        &MainWindow::processAudio);

              // 음소거 버튼 생성 & 삽입
              AudioBtn = new QPushButton(tr("음소거"), central);
          connect(AudioBtn, &QPushButton::clicked, this, [=]() {
                  if (audioEnabled) {
                          audioSource->stop();
                          AudioBtn->setText(tr("음소거 해제"));
                      } else {
                          audioDevice = audioSource->start();
                          connect(audioDevice, &QIODevice::readyRead,
                                           this,        &MainWindow::processAudio);
                          AudioBtn->setText(tr("음소거"));
                      }
                  audioEnabled = !audioEnabled;
              });
          rightLay->insertWidget(1, AudioBtn, 0, Qt::AlignCenter);




    //종료버튼
    auto *closeBtn = new QPushButton("종료", central);
    connect(closeBtn, &QPushButton::clicked, this, &MainWindow::close);
    rightLay->addWidget(closeBtn, 0, Qt::AlignCenter);

    //남는 공간
    rightLay->addStretch(1);

    hLayout->addLayout(rightLay, 0);

    //완성된 HBox 레이아웃을 메인 레이아웃에 추가
    mainLayout->addLayout(hLayout, 1);




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

    QString rtspUrl = QString("rtsps://%1:%2/test").arg(ip).arg(rtspPort);
    videoThread = new VideoThread(rtspUrl, nullptr, coord);
    connect(videoThread, &VideoThread::fullFrame,
            this,       &MainWindow::updatePano,
            Qt::QueuedConnection);
    bool ok = connect(videoThread, &VideoThread::cropped,
            this,       &MainWindow::onCropped,
            Qt::QueuedConnection);
    qDebug()<<"MainWindow cropped ok?" <<ok;

    videoThread->start();


}
// 슬롯 정의
void MainWindow::updatePano(const QPixmap &pix) {
    panoLabel->setPixmap(pix.scaled(
        panoLabel->size(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation));
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


    if (index >= labels.size()){

         auto *lbl = new QLabel(container);
         lbl->setStyleSheet("background-color: rgba(169, 169, 169, 0.5);");
         lbl->setAlignment(Qt::AlignCenter);
         lbl->setFixedSize(300, 220);
         int row = index / 2;
         int col = index % 2;
         layout->addWidget(lbl, row, col);

         labels.append(lbl);
    }

    //이미 추가된 레이블에 pixmap 설정 또는 숨기기
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
//웹캠
void MainWindow::webcam() {
    cv::Mat frame;
    cap >> frame;
    if (frame.empty()) return;
    cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
    QImage img(frame.data,
               frame.cols,
               frame.rows,
               frame.step,
               QImage::Format_RGB888);
    QPixmap pix = QPixmap::fromImage(img);
    QSize lblSize = webcamLabel->size();
    QPixmap scaled = pix.scaled(
        lblSize,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        );

    webcamLabel->setPixmap(scaled);;
}

//웹캠 마이크
void MainWindow::processAudio() {
    if (!audioDevice) return;
    QByteArray chunk = audioDevice->readAll();
    qDebug() << "오디오 데이터 들어옴, 바이트 수:" << chunk.size();

}
