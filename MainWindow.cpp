// MainWindow.cpp

#include "MainWindow.h"
#include "VideoThread.h"
#include "TcpThread.h"
#include "Coordinate.h"

#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , videoThread(nullptr)
    , tcpThread(nullptr)
    , coord(nullptr)
{
    auto central = new QWidget(this);
    layout = new QHBoxLayout(central);
    setCentralWidget(central);

    coord = new Coordinate();

    //Metadata 수신 스레드 먼저 시작
    tcpThread = new TcpThread(coord, "192.168.0.85", 12345);
    tcpThread->start();

    videoThread = new VideoThread("rtsp://192.168.0.85:8554/test", nullptr, coord);
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
    if (index >= labels.size()) {
        auto lbl = new QLabel;
        lbl->setStyleSheet("background-color:black;");
        lbl->setScaledContents(true);
        layout->addWidget(lbl);
        labels.append(lbl);
    }
    // 해당 QLabel에 pixmap 설정
    labels[index]->setPixmap(pix);
    //labels[index]->setFixedSize(pix.size()); 고정 크기 없애기
}
