#ifndef CAMERAWIDGET_H
#define CAMERAWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QLabel>
#include <QImage>
#include <opencv2/opencv.hpp>
#include "RtspServer.h"

class CameraWidget : public QWidget {
    Q_OBJECT

public:
    explicit CameraWidget(QWidget *parent = nullptr, QSize targetSize = QSize());
    ~CameraWidget();

public slots:
    void startCam();
    void stopCam();

private slots:
    void captureFrame();

private:
    void initCamoffImage();

private:
    QSize targetSize;
    QImage camoffImage;
    QImage fallbackImage;
    QLabel* displayLabel;
    QTimer timer;
    cv::VideoCapture cap;
    cv::Mat latest_frame;
    std::mutex frame_mutex;
};

#endif // CAMERAWIDGET_H
