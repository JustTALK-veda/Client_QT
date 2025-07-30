#include "pipwidget.h"

#include <QMouseEvent>

PipWidget::PipWidget(QWidget *parent, CameraWidget* webcamFrame)
    : QWidget{parent}, m_dragging(false), m_resizeMargin(8), webcam(webcamFrame)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground, true);
    //setMinimumSize(120,90);
    setFixedSize(240,180);
    //setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    topLayout = new QVBoxLayout(this);
    topLayout->setContentsMargins(4,4,4,4);
    topLayout->addWidget(webcam);



    // btnCam : Cam On/Off
    btnCam = new QPushButton(this);
    btnCam->setIcon(QIcon(":/Image/config/video_on.png"));
    // btnCam->setFixedSize(28,28);
    // btnCam->move(webcam->width()-64, 12); // 위치 조정 필요
    // connect(btnCam, &QPushButton::clicked, this, [this]() {
    //     webcam->onCamButtonClicked();
    // });

    // btnHide : Widget Off(Hide)
    btnHide = new QPushButton("—", this);
    btnHide->setFixedSize(28,28);
    btnHide->move(webcam->width()-32, 12);
    connect(btnHide, &QPushButton::clicked, this, [this]() {
        this->hide();
    });

    btnCam->hide();
    btnHide->hide();
}
//events about Buttons
//show btns when it is on Widget
void PipWidget::enterEvent(QEnterEvent*){
    btnCam->show();
    btnHide->show();
}
void PipWidget::leaveEvent(QEvent*){
    btnCam->hide();
    btnHide->hide();
}

//events about Widget window
void PipWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_dragPos = event->globalPosition().toPoint();
        m_dragging = true;
    }
    QWidget::mousePressEvent(event);
}
void PipWidget::mouseMoveEvent(QMouseEvent* event) {
    if (m_dragging) {
        QPoint delta = event->globalPosition().toPoint() - m_dragPos;
        move(pos() + delta);
        m_dragPos = event->globalPosition().toPoint();
    }
    QWidget::mouseMoveEvent(event);
}
void PipWidget::mouseReleaseEvent(QMouseEvent* event) {
    m_dragging = false;
    QWidget::mouseReleaseEvent(event);
}

void PipWidget::resizeEvent(QResizeEvent*){
    // 창 크기 바뀌면 버튼 위치 자동 조정
    btnCam->move(webcam->width()-64, 12);
    btnHide->move(webcam->width()-32, 12);
}
