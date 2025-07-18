#include "lobby.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QImage>
#include <QPixmap>
#include <QDebug>

Lobby::Lobby(QWidget *parent)
    : QWidget(parent), timer(new QTimer(this))
{
    // 레이아웃
    auto *mainLayout = new QVBoxLayout(this);

    videoLabel = new QLabel(this);
    videoLabel->setFixedSize(640, 480);  // 해상도 고정
    videoLabel->setAlignment(Qt::AlignCenter);
    videoLabel->setStyleSheet("background-color: black;");
    mainLayout->addWidget(videoLabel);

    enterBtn = new QPushButton("입장하기", this);
    connect(enterBtn, &QPushButton::clicked, this, &Lobby::onEnterClicked);
    mainLayout->addWidget(enterBtn, 0, Qt::AlignCenter);

    // 웹캠 초기화
    if (!cap.open(0)) {
        qDebug() << "웹캠 열기 실패";
    }

    // 프레임 타이머
    connect(timer, &QTimer::timeout, this, &Lobby::grabFrame);
    timer->start(30);  // 약 33fps
}

Lobby::~Lobby() {
    if (cap.isOpened()) cap.release();
}

void Lobby::grabFrame() {
    if (!cap.isOpened()) return;

    cv::Mat frame;
    cap >> frame;
    if (frame.empty()) return;

    // BGR → RGB
    cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
    QImage qimg(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
    videoLabel->setPixmap(QPixmap::fromImage(qimg));
}

void Lobby::onEnterClicked() {
    emit accepted();  // 시그널 발생
}
