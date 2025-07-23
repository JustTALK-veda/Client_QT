#include "CameraWidget.h"
#include "RtspServer.h"

#include <QVBoxLayout>
#include <QPainter>
#include <QPixmap>
#include <QDebug>
#include <QCoreApplication>

CameraWidget::CameraWidget(QWidget *parent, QSize targetSize) : QWidget(parent), targetSize(targetSize) {
    displayLabel = new QLabel(this);
    if (targetSize.isValid()) {
        displayLabel->setMinimumSize(targetSize);   // 최소 크기만 설정
        displayLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    } else {
        displayLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

    displayLabel->setAlignment(Qt::AlignCenter);
    displayLabel->setScaledContents(false);  // 우리가 scaled()를 직접 적용할 것임

    initCamoffImage();  // ← camoff 초기화
    displayLabel->setPixmap(QPixmap::fromImage(fallbackImage));

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(displayLabel);

    connect(&timer, &QTimer::timeout, this, &CameraWidget::captureFrame);
}

CameraWidget::~CameraWidget() {
    stopCam();
}

void CameraWidget::initCamoffImage() {
    QString imagePath = QCoreApplication::applicationDirPath() + "/resources/camoff.png";
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
    if (!cap.isOpened()) {
        displayLabel->setText("cannot open cam");
        return;
    }
    timer.start(30);
}

void CameraWidget::stopCam() {
    timer.stop();
    if (cap.isOpened()) {
        cap.release();
    }
    displayLabel->setPixmap(QPixmap::fromImage(fallbackImage));
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

    // QLabel 크기에 맞게 축소 (전체 프레임을 비율 유지하면서 보여줌)
    QPixmap scaledPix = QPixmap::fromImage(img).scaled(
        displayLabel->size(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        );

    displayLabel->setPixmap(scaledPix);
}
