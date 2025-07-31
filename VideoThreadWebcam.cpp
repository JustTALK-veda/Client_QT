#include "VideoThreadWebcam.h"
#include <QImage>
#include <QPixmap>
#include <QPainterPath>
#include <QDebug>
#include <opencv2/opencv.hpp>
#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <QMutexLocker>
#include <QPainter>
#include <QDateTime>

VideoThreadWebcam::VideoThreadWebcam(const QString& url, QLabel* label)
    : m_url(url), m_label(label), m_stop(false) {}

void VideoThreadWebcam::stop() {
    m_stop = true;
}

void VideoThreadWebcam::run() {
    gst_init(nullptr, nullptr);

    QString pipelineStr;

#ifdef _WIN32
    pipelineStr = QString(
          "rtspsrc location=%1 latency=0 name=src protocols=tcp "
          "src. ! queue ! rtph264depay ! h264parse ! avdec_h264 ! videoconvert ! video/x-raw,format=RGB ! appsink name=video_sink "
          "src. ! queue ! rtpmp4adepay ! aacparse ! avdec_aac ! audioconvert ! audioresample ! autoaudiosink sync=false"
                      ).arg(m_url);
#elif __APPLE__
    pipelineStr = QString(
                        "rtspsrc location=rtsp://192.168.0.27:8554/test latency=100 tls-validation-flags=0 ! "
                        "application/x-rtp,media=video,encoding-name=H264 ! "
                        "rtph264depay ! "
                        "h264parse ! "
                        "vtdec ! "
                        "videoconvert ! "
                        "video/x-raw,format=BGR ! "
                        "appsink name=video_sink"
                        ).arg(m_url);
    // audio
    // pipelineStr = QString(
    //                   "rtspsrc location=%1 latency=100 tls-validation-flags=0 protocols=tcp name=src "
    //                   "src. ! application/x-rtp,media=video,encoding-name=H264 ! rtph264depay ! h264parse ! vtdec ! videoconvert ! video/x-raw,format=RGB ! appsink name=mysink "
    //                   "src. ! application/x-rtp,media=audio ! rtpmp4gdepay ! avdec_aac ! audioconvert ! audioresample ! autoaudiosink sync=false"
    //                   ).arg(m_url);
#endif

    qDebug() << "[VideoThreadWebcam] pipeline =" << pipelineStr;

    GstElement* pipeline = gst_parse_launch(pipelineStr.toStdString().c_str(), nullptr);
    if (!pipeline)
    {
        qDebug() << "[VideoThreadWebcam] 파이프라인 생성 실패";
        return;
    }

    GstElement* sink = gst_bin_get_by_name(GST_BIN(pipeline), "video_sink");
    if (!sink)
    {
        qDebug() << "[VideoThreadWebcam] appsink 핸들 가져오기 실패";
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
    qDebug() << "[VideoThreadWebcam] 파이프라인 재생 시작";
    
    emit connected();

    while (!m_stop)
    {
        //qDebug() << "[VideoThreadWebcam] sample 수신 대기 중";

        GstSample* sample = gst_app_sink_pull_sample(GST_APP_SINK(sink));
        if (!sample)
        {
            //qDebug() << "[VideoThreadWebcam] sample 수신 실패";
            continue;
        }
        //qDebug() << "[VideoThreadWebcam] sample 수신 성공";

        GstBuffer* buffer = gst_sample_get_buffer(sample);
        GstMapInfo map;
        if (!gst_buffer_map(buffer, &map, GST_MAP_READ))
        {
            qDebug() << "[VideoThreadWebcam] 버퍼 매핑 실패";
            gst_sample_unref(sample);
            continue;
        }

        int width = 0, height = 0;
        GstCaps* caps = gst_sample_get_caps(sample);
        GstStructure* s = gst_caps_get_structure(caps, 0);
        gst_structure_get_int(s, "width", &width);
        gst_structure_get_int(s, "height", &height);

        // convert raw RGB data into a QImage
        QImage image(reinterpret_cast<const uchar*>(map.data),
                     width,
                     height,
                     QImage::Format_RGB888);
        // copy the image so it owns its data
        QImage imageCopy = image.copy();
        // convert the QImage into a QPixmap
        QPixmap pixmap = QPixmap::fromImage(imageCopy);
        // update the QLabel safely from this worker thread
        QMetaObject::invokeMethod(
            m_label, "setPixmap",
            Qt::QueuedConnection,
            Q_ARG(QPixmap, pixmap)
        );

        gst_buffer_unmap(buffer, &map);
        gst_sample_unref(sample);
        
        //fps계산( 풀 프레임 기준 )
        fpsFrameCount++;

        if (fpsTimer.elapsed() >= 1000) {
            double fps = fpsFrameCount * 1000.0 / fpsTimer.elapsed();
            qDebug() << "[VideoThreadWebcam] 현재 FPS ≈" << fps;
            fpsTimer.restart();
            fpsFrameCount = 0;
        }

    }

    qDebug() << "[VideoThreadWebcam] 스레드 종료 요청됨";

    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
}
