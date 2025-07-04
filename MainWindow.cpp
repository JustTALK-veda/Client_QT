#include "MainWindow.h"
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), videoThread(nullptr), tcpThread(nullptr) {

    QWidget *central = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(central);
    setCentralWidget(central);

    label = new QLabel(this);
    label->setFixedSize(640, 480);
    label->setStyleSheet("background-color: black;");
    label->setScaledContents(true);
    layout->addWidget(label);

    coord = new Coordinate();
    QString url = "rtsp://192.168.0.85:8554/test";

    videoThread = new VideoThread(url, label, coord);
    tcpThread = new TcpThread(coord, "192.168.0.171", 12345);

    videoThread->start();
    tcpThread->start();
}

MainWindow::~MainWindow() {
    videoThread->stop();
    videoThread->wait();

    tcpThread->requestInterruption();
    tcpThread->wait();

    delete videoThread;
    delete tcpThread;
    delete coord;
}
