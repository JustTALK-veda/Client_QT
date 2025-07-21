#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QVector>
#include <QPixmap>
#include "VideoThread.h"
#include "TcpThread.h"
#include "Coordinate.h"
#include <QHBoxLayout>
#include <QScrollArea>
#include <QAudioSource>
#include <QAudioFormat>
#include <QPushButton>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QVector<QLabel*> labels;
    QGridLayout *layout;
    VideoThread* videoThread;
    TcpThread* tcpThread;
    Coordinate* coord;
    QScrollArea *scroll;
    QWidget *container;
    QLabel* panoLabel;               //파노라마용 QLabel
    // JSON 설정 읽기 함수
    bool loadConfigFromJson(QString &ip, int &rtspPort, int &tcpPort);
    //웹캠
    cv::VideoCapture cap;
    QTimer         *webcamTimer;
    QLabel         *webcamLabel;
    QAudioSource *audioSource;
    QIODevice *audioDevice;
    QPushButton *AudioBtn;
    bool audioEnabled; //음소거


private slots:
    void onCropped(int index,const QPixmap &pix);
    void updatePano(const QPixmap &pix);
    void webcam();
    //void webCamData();
    //오디오
    void processAudio();
};

#endif // MAINWINDOW_H
