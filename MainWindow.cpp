#include "MainWindow.h"
#include <QVBoxLayout>
#include <QFile>

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
    // Read RTSP URL from config/rpi_ip (fallback to default on error)
    QString ip;
    QString rtsp_url;
    QFile cfgFile("config/rpi_ip");
    if (cfgFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&cfgFile);
        ip = in.readLine().trimmed();
        cfgFile.close();
    } else {
        qWarning() << "Could not open config/rpi_ip, using default IP";
        ip = "192.168.0.30";
    }

    rtsp_url = QString("rtsp://%1:8554/test").arg(ip);

    videoThread = new VideoThread(rtsp_url, label, coord);
    tcpThread = new TcpThread(coord, ip, 12345);

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
