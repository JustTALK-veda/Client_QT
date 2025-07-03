#include "VideoThread.h"
#include <QImage>
#include <QPixmap>
#include <QMetaObject>
#include <QDebug>
#include <opencv2/opencv.hpp>  // Mat 사용 위해 유지
#include <gst/gst.h>
#include <gst/app/gstappsink.h>

using namespace cv;

VideoThread::VideoThread(const QString& url, QLabel* label, Coordinate* coord)
    : m_url(url), m_label(label), m_coord(coord), m_stop(false) {}

void VideoThread::stop() {
    m_stop = true;
}

void VideoThread::run() {
    gst_init(nullptr, nullptr);

    QString pipelineStr = QString(
                              "rtspsrc location=%1 latency=100 ! "
                              "decodebin ! videoconvert ! video/x-raw,format=RGB ! appsink name=mysink"
                              ).arg(m_url);

    qDebug() << "[VideoThread] pipeline =" << pipelineStr;

    GstElement* pipeline = gst_parse_launch(pipelineStr.toStdString().c_str(), nullptr);
    if (!pipeline) {
        qDebug() << "[VideoThread] 파이프라인 생성 실패";
        return;
    }

    GstElement* sink = gst_bin_get_by_name(GST_BIN(pipeline), "mysink");
    if (!sink) {
        qDebug() << "[VideoThread] appsink 핸들 가져오기 실패";
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

    while (!m_stop) {
        GstSample* sample = gst_app_sink_pull_sample(GST_APP_SINK(sink));
        if (!sample) {
            qDebug() << "[VideoThread] sample 수신 실패";
            continue;
        }

        GstBuffer* buffer = gst_sample_get_buffer(sample);
        GstCaps* caps = gst_sample_get_caps(sample);
        GstStructure* s = gst_caps_get_structure(caps, 0);

        int width = 0, height = 0;
        gst_structure_get_int(s, "width", &width);
        gst_structure_get_int(s, "height", &height);

        GstMapInfo map;
        if (!gst_buffer_map(buffer, &map, GST_MAP_READ)) {
            gst_sample_unref(sample);
            continue;
        }

        cv::Mat mat(height, width, CV_8UC3, (char*)map.data);
        cv::Mat copy = mat.clone();
        gst_buffer_unmap(buffer, &map);
        gst_sample_unref(sample);

        // 좌표값 읽기
        int x = 0, y = 0;
        if (m_coord) {
            m_coord->mutex.lock();
            x = m_coord->x;
            y = m_coord->y;
            m_coord->mutex.unlock();
        }

        qDebug() << "[VideoThread] 좌표 x =" << x << ", frame width =" << width;

        int pixels_per_cm = 37;
        int crop_half = 3 * pixels_per_cm;

        int roi_x = std::max(0, x - crop_half);
        int roi_w = std::min(crop_half * 2, copy.cols - roi_x);

        if (roi_w <= 0 || roi_x + roi_w > copy.cols) {
            qDebug() << "[VideoThread] ROI 오류: roi_x =" << roi_x << ", roi_w =" << roi_w;
            continue;
        }

        cv::Mat cropped = copy(Rect(roi_x, 0, roi_w, copy.rows)).clone();
        if (cropped.empty()) {
            qDebug() << "[VideoThread] 잘린 영상이 비어 있음";
            continue;
        }

        QImage img(cropped.data, cropped.cols, cropped.rows, cropped.step, QImage::Format_BGR888);
        if (img.isNull()) {
            qDebug() << "[VideoThread] QImage 생성 실패";
            continue;
        }

        QPixmap pix = QPixmap::fromImage(img.rgbSwapped());

        if (m_label) {
            QMetaObject::invokeMethod(m_label, [=]() {
                m_label->setPixmap(pix);
            }, Qt::QueuedConnection);
        }
    }

    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
}
