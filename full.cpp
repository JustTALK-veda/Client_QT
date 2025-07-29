#include "full.h"
#include "HanwhaThread.h"
#include <QVBoxLayout>
#include <QPainter>
#include <QPixmap>
#include <QDebug>

Full::Full(QWidget *parent)
    : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumSize(320, 240);

    setAutoFillBackground(false);

    /* Set Label for Thread */
    m_label = new QLabel(this);
    m_label->setAlignment(Qt::AlignCenter);

    m_label->setText("CCTV now loading...");

    m_layout = new QVBoxLayout(this);
    m_layout->addWidget(m_label);

    /* Create Streaming Thread */
    m_thread = new HanwhaThread("rtsp://admin:veda333%21@192.168.0.12/profile2/media.smp", this);
    connect(m_thread, &HanwhaThread::frameReady, this, &Full::onFrameReady, Qt::QueuedConnection );
}

Full::~Full()
{
    if (m_thread) {
        m_thread->stop();   // 1. 종료 요청
        m_thread->wait();   // 2. 완전히 끝날 때까지 대기 (필수!!)
        delete m_thread;    // 3. 안전하게 삭제
    }
    delete m_layout;
    delete m_label;
}

void Full::onFrameReady(const QImage& img)
{
    //if (!m_label) return;
    //if (m_label->size().isEmpty()) return;
    // qDebug() <<  m_label->size();
    m_label->setPixmap(QPixmap::fromImage(img).scaled(m_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    m_label->setStyleSheet("border: 1px solid #4A5972; border-radius: 10px;");
    // if (img.isNull()) return;

    // QSize imgSize = img.size();
    // m_label->resize(imgSize);  // QLabel을 이미지 크기에 맞춤

    // // 영상 둥글게 자르기 (mask 적용)
    // QPixmap pixmap = QPixmap::fromImage(img).scaled(
    //     imgSize,
    //     Qt::KeepAspectRatio,
    //     Qt::SmoothTransformation
    //     );

    // QBitmap mask(imgSize);
    // mask.fill(Qt::color0);  // 투명
    // QPainter painter(&mask);
    // painter.setRenderHint(QPainter::Antialiasing);
    // painter.setBrush(Qt::color1);  // 채워질 부분
    // painter.drawRoundedRect(mask.rect(), 20, 20);  // radius 20

    // pixmap.setMask(mask);  // 마스크 적용

    // m_label->setPixmap(pixmap);
    // m_label->setStyleSheet("border: none;");  // border는 필요 없을 수도 있음
}

void Full::onFullPageActive()
{
    m_thread->start();
}
