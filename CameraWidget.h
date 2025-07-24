#ifndef CAMERA_WIDGET_H
#define CAMERA_WIDGET_H
#include "ui_CameraWidget.h"
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
    void captureFrame();
    void onMicButtonClicked();
    void onCamButtonClicked();

private:
    void initCamoffImage();
    void resizeEvent(QResizeEvent *event);

private:
    Ui::cameraWidgetForm *ui;
    QSize targetSize;
    QImage camoffImage;
    QImage fallbackImage;
    QLabel* displayLabel;
    QTimer timer;
    cv::VideoCapture cap;
    cv::Mat latest_frame;
    std::mutex frame_mutex;
    bool micEnabled = false;
    bool camEnabled = false;
};

#endif // CAMERA_WIDGET_H
