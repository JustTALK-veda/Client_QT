#include "CameraWidget.h"
#include "RtspServer.h"
#include "audio_control.h"

#include <QVBoxLayout>
#include <QPainter>
#include <QPixmap>
#include <QDebug>
#include <QCoreApplication>

CameraWidget::CameraWidget(QWidget *parent, QSize targetSize) : QWidget(parent), targetSize(targetSize) {
    webcamLabel = new QLabel(this);
    webcamLabel->setAlignment(Qt::AlignCenter);
    webcamLabel->setScaledContents(false);
    webcamLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    if (targetSize.isValid()) {
        webcamLabel->setMinimumSize(targetSize);   // 최소 크기만 설정
    }

    initCamoffImage();  // ← camoff 초기화
    webcamLabel->setPixmap(QPixmap::fromImage(fallbackImage));

    connect(&timer, &QTimer::timeout, this, &CameraWidget::captureFrame);
}

CameraWidget::~CameraWidget() {
    stopCam();
}

void CameraWidget::initCamoffImage() {
    QString imagePath = QCoreApplication::applicationDirPath() + "/config/camoff.png";
    camoffImage = QImage(imagePath);

    if (camoffImage.isNull()) {
        qDebug() << "camoff 이미지 불러오기 실패. 기본 이미지로 대체합니다.";
        camoffImage = QImage(640, 480, QImage::Format_RGB888);
        camoffImage.fill(Qt::black);

        QPainter painter(&camoffImage);
        painter.setPen(Qt::red);
        painter.setFont(QFont("Arial", 24));
        painter.drawText(camoffImage.rect(), Qt::AlignCenter, "CAMERA OFF");
    } else {
        camoffImage = camoffImage.convertToFormat(QImage::Format_BGR888);  // 포맷 통일
    }

    fallbackImage = camoffImage;

    // QImage → cv::Mat 변환 후 전역 img_camoff에 복사
    img_camoff = cv::Mat(camoffImage.height(), camoffImage.width(), CV_8UC3,
                         const_cast<uchar*>(camoffImage.bits())).clone();
}

void CameraWidget::startCam() {
    if (cap.isOpened()) return;

    cap.open(0);  // 또는 GStreamer 파이프라인
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    if (!cap.isOpened()) {
        webcamLabel->setText("cannot open cam");
        return;
    }
    if (!shared_frame_ptr) shared_frame_ptr = new cv::Mat();
    timer.start(30);
}

void CameraWidget::stopCam() {
    timer.stop();
    if (cap.isOpened()) {
        cap.release();
    }
    webcamLabel->setPixmap(QPixmap::fromImage(fallbackImage));
    std::lock_guard<std::mutex> lock(frame_mutex);
    if (shared_frame_ptr)
        shared_frame_ptr->release();
}

void CameraWidget::captureFrame() {
    cv::Mat frame;
    cap >> frame;
    if (frame.empty()) return;
    else {
        std::lock_guard<std::mutex> lock(frame_mutex);
        latest_frame = frame.clone();
    }
    if (shared_frame_ptr) {
        *shared_frame_ptr = latest_frame.clone();  // 전역 공유용
    }
    QImage img(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_BGR888);

    QSize displaySize = webcamLabel->contentsRect().size();
    QPixmap scaledPix = QPixmap::fromImage(img).scaled(
        displaySize,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation);

    webcamLabel->setPixmap(scaledPix);
}

// 마이크 버튼 토글
void CameraWidget::onMicButtonClicked()
{
    micEnabled = !micEnabled;
    set_mic_enabled(micEnabled);
}

// 웹캠 버튼 토글
void CameraWidget::onCamButtonClicked()
{
    camEnabled = !camEnabled;
    if (camEnabled) {
        enable_streaming(true);
        startCam();
    } else {
        enable_streaming(false);
        stopCam();
    }
}

void CameraWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (!latest_frame.empty()) {
        QImage img(latest_frame.data, latest_frame.cols, latest_frame.rows, latest_frame.step, QImage::Format_BGR888);
        QSize displaySize = webcamLabel->contentsRect().size();  // 실제 그려지는 영역
        QPixmap scaledPix = QPixmap::fromImage(img).scaled(
            displaySize,
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation);
        webcamLabel->setPixmap(scaledPix);
    }
}
