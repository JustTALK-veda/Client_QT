#pragma once

#include <QWidget>
#include <QTimer>
#include <QLabel>
#include <QPushButton>
#include <opencv2/opencv.hpp>

class Lobby : public QWidget {
    Q_OBJECT

public:
    explicit Lobby(QWidget *parent = nullptr);
    ~Lobby();

signals:
    void accepted();  // 입장 시그널

private slots:
    void grabFrame();       // 타이머 호출로 프레임 갱신
    void onEnterClicked();  // 버튼 클릭 시

private:
    cv::VideoCapture cap;   // 웹캠 캡처
    QTimer* timer;          // 프레임 타이머
    QLabel* videoLabel;     // 영상 표시용
    QPushButton* enterBtn;  // 입장 버튼
};
// lobby.h
//mac
// #pragma once

// #include <QWidget>
// #include <QCamera>
// #include <QCameraDevice>
// #include <QMediaCaptureSession>
// #include <QVideoWidget>
// #include <QPushButton>
// #include <QVBoxLayout>

// class Lobby : public QWidget {
//     Q_OBJECT
// public:
//     explicit Lobby(QWidget *parent = nullptr);
//     ~Lobby();

// signals:
//     void accepted();  // "입장하기" 버튼 클릭 시

// private slots:
//     void onEnterClicked();

// private:
//     QCamera *camera;
//     QMediaCaptureSession *captureSession;
//     QVideoWidget *videoWidget;
//     QPushButton *enterBtn;
// };
