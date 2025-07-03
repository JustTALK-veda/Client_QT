#include "mytest.h"
#include <QHBoxLayout>
#include <QDebug>
#include "VideoThread.h"

mytest::mytest(QWidget *parent)
    : QMainWindow(parent), videoThread(nullptr)
{
    QWidget *central = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(central);

    setCentralWidget(central);
    setWindowTitle("JustTalk");

    QString rtsp_url = "rtsp://192.168.0.85:8554/test";

    // 기존에 사용한 QLabel은 이제 필요없음
    // 대신, 동적으로 QLabel을 추가하는 방식으로 변경됩니다.

    // VideoThread 객체 생성
    if (videoThread == nullptr) {
        videoThread = new VideoThread(rtsp_url, this);  // 부모 위젯을 전달
        videoThread->start();
    }
}

mytest::~mytest() {
    if (videoThread) {
        videoThread->stop();
        videoThread->wait();
        delete videoThread;
        videoThread = nullptr;  // 포인터를 nullptr로 설정하여 중복 삭제 방지
    }
}
