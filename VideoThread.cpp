#include "VideoThread.h"
#include <QImage>
#include <QPixmap>
#include <QMetaObject>
#include <QDebug>
#include <opencv2/opencv.hpp>
#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include "Coordinate.h"
#include <QMutexLocker>
#include <QPainter>

VideoThread::VideoThread(const QString& url, QLabel* label, Coordinate* coord)
    : m_url(url), m_label(label), m_coord(coord), m_stop(false) {}

void VideoThread::stop() {
    m_stop = true;
}

void VideoThread::run() {
    gst_init(nullptr, nullptr);

    QString pipelineStr = QString(
                              "rtspsrc location=%1 latency=100 ! "
                              "decodebin ! "
                              "videoconvert ! "
                              "video/x-raw,format=RGB !"
                              "appsink name=mysink"
                              ).arg(m_url);

    qDebug() << "[VideoThread] pipeline =" << pipelineStr;

    GstElement* pipeline = gst_parse_launch(pipelineStr.toStdString().c_str(), nullptr);
    if (!pipeline) 
    {
        qDebug() << "[VideoThread] 파이프라인 생성 실패";
        return;
    }

    GstElement* sink = gst_bin_get_by_name(GST_BIN(pipeline), "mysink");
    if (!sink) 
    {
        qDebug() << "[VideoThread] appsink 핸들 가져오기 실패";
        gst_object_unref(pipeline);
        return;
    }

    g_object_set(sink,
                 "emit-signals", FALSE,
                 "sync", FALSE,
                 "max-buffers", 1,
                 "drop", TRUE,
                 nullptr);

    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    qDebug() << "[VideoThread] 파이프라인 재생 시작";

    while (!m_stop) 
    {
        qDebug() << "[VideoThread] sample 수신 대기 중";

        GstSample* sample = gst_app_sink_pull_sample(GST_APP_SINK(sink));
        if (!sample) 
        {
            qDebug() << "[VideoThread] sample 수신 실패";
            continue;
        }
        qDebug() << "[VideoThread] sample 수신 성공";

        GstBuffer* buffer = gst_sample_get_buffer(sample);
        GstMapInfo map;
        if (!gst_buffer_map(buffer, &map, GST_MAP_READ)) 
        {
            qDebug() << "[VideoThread] 버퍼 매핑 실패";
            gst_sample_unref(sample);
            continue;
        }

        int width = 0, height = 0;
        GstCaps* caps = gst_sample_get_caps(sample);
        GstStructure* s = gst_caps_get_structure(caps, 0);
        gst_structure_get_int(s, "width", &width);
        gst_structure_get_int(s, "height", &height);

        qDebug() << "[VideoThread] width =" << width << ", height =" << height;
        
        cv::Mat mat(height, width, CV_8UC3, (char*)map.data);
        cv::Mat copy = mat.clone();

        gst_buffer_unmap(buffer, &map);
        gst_sample_unref(sample);

        QImage fullImg(copy.data, copy.cols, copy.rows, copy.step, QImage::Format_BGR888);
        if (fullImg.isNull()) 
        {
            qDebug() << "[VideoThread] QImage 생성 실패";
            continue;
        }
        QPixmap fullPix = QPixmap::fromImage(fullImg.rgbSwapped());

        QVector<QRect> cropRects;

        if(m_coord)
        {   
            QMutexLocker locker(&m_coord->mutex);
            const auto& qv = m_coord->width_data;
            int count = qv.size() / 4;
            for (int i = 0; i < count; ++i) 
            {
                int x = qv[4*i + 0];
                int y = qv[4*i + 1];
                int w = qv[4*i + 2];
                int h = qv[4*i + 3];
                cropRects.append(QRect(x, y, w, h));
            }
        }
        if (cropRects.isEmpty()) 
        {
            qDebug() << "[VideoThread] 크롭 영역이 설정되지 않았습니다.";
            emit cropped(0, fullPix); // 전체 이미지를 기본으로 emit
            continue;
        }
        // 각 영역을 크롭하고 QLabel에 모두 표시

        int angle = 359;
        int px = (angle * fullPix.width()) / 360;
        qDebug() << "각도:" << angle << "픽셀 위치(px):" << px;

        for (int i = 0; i < cropRects.size(); ++i)
        {
            const QRect& rect = cropRects[i];

            int x = std::clamp(rect.x(), 0, fullPix.width() - 1);
            int y = std::clamp(rect.y(), 0, fullPix.height() - 1);
            int w = std::clamp(rect.width(), 1, fullPix.width() - x);
            int h = std::clamp(rect.height(), 1, fullPix.height() - y);
            QRect roiRect(x, y, w, h);

            QPixmap croppedPix = fullPix.copy(roiRect);
            qDebug() << "[VideoThread] emit cropped area" << i << ":" << roiRect;

            //하이라이팅

            if (px >= x && px < x + w) {
                qDebug() << "테두리 그리기 시작";
                QPainter painter(&croppedPix);
                painter.setPen(QPen(Qt::blue, 5));  // 파란색, 두께 5px 테두리
                painter.setBrush(Qt::NoBrush);     // 채우지 않음
                painter.drawRect(0, 0, w - 1, h - 1); // 크롭된 영역에 테두리 그리기
                painter.end();  // QPainter 종료ㄱ
                qDebug() << "테두리 그리기 완료";
            }

            // invokeMethod 대신신호로 대체
            emit cropped(i, croppedPix);
        }
    }

    qDebug() << "[VideoThread] 스레드 종료 요청됨";
    
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
}


