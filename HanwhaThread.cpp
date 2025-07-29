#include "HanwhaThread.h"
#include <QImage>
#include <QPixmap>
#include <QDebug>
#include <opencv2/opencv.hpp>
#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <QDateTime>

HanwhaThread::HanwhaThread(const QString& rtspUrl, QObject* parent)
    : QThread(parent), m_rtspUrl(rtspUrl), m_stop(false)
{}

void HanwhaThread::stop()
{
    m_stop = true;
}

void HanwhaThread::run()
{
    gst_init(nullptr, nullptr);
    const int TIMEOUT_MS = 3000;
    QString pipelineStr = QString(
                              "rtspsrc location=%1 latency=100 tls-validation-flags=0 ! "
                              "decodebin ! "
                              "videoconvert ! "
                              "video/x-raw,format=NV12 ! "
                              "appsink name=mysink"
                              ).arg(m_rtspUrl);
    qDebug() << pipelineStr;

    while(!m_stop)
    {
        GstElement* pipeline = gst_parse_launch(pipelineStr.toStdString().c_str(), nullptr);
        if (!pipeline)
        {
            qDebug() << "[HanwhaThread] 파이프라인 생성 실패";
            return;
        }

        GstElement* sink = gst_bin_get_by_name(GST_BIN(pipeline), "mysink");
        if (!sink)
        {
            qDebug() << "[HanwhaThread] appsink 핸들 가져오기 실패";
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
        qDebug() << "[HanwhaThread] 파이프라인 재생 시작";

        while (!m_stop) {
            qDebug() << "[HanwhaThread] sample 수신 대기 중";

            GstSample* sample = gst_app_sink_try_pull_sample(GST_APP_SINK(sink), TIMEOUT_MS * GST_MSECOND);
            if (!sample)
            {
                qDebug() << "[HanwhaThread] sample 수신 실패";
                break;
            }
            qDebug() << "[HanwhaThread] sample 수신 성공";

            GstBuffer* buffer = gst_sample_get_buffer(sample);
            GstMapInfo map;
            if (!gst_buffer_map(buffer, &map, GST_MAP_READ))
            {
                qDebug() << "[HanwhaThread] 버퍼 매핑 실패";
                gst_sample_unref(sample);
                continue;
            }

            int width, height;
            GstCaps* caps = gst_sample_get_caps(sample);
            GstStructure* s = gst_caps_get_structure(caps, 0);
            gst_structure_get_int(s, "width", &width);
            gst_structure_get_int(s, "height", &height);

            qDebug() << "[HanwhaThread] width =" << width << ", height =" << height;

            cv::Mat mat(height+height / 2, width, CV_8UC1, (char*)map.data);
            cv::Mat rgb;
            cv::cvtColor(mat, rgb, cv::COLOR_YUV2BGR_NV12);

            cv::Mat copy = rgb.clone();
            QImage fullImg(copy.data, copy.cols, copy.rows, copy.step, QImage::Format_BGR888);

            gst_buffer_unmap(buffer, &map);
            gst_sample_unref(sample);
            QImage safeImg = fullImg.copy();  // 데이터 전체 복사

            if (fullImg.isNull())
            {
                qDebug() << "[HanwhaThread] QImage 생성 실패";
                continue;
            }
            qDebug() << "[HanwhaThread] QImage 생성 성공";
            //fps계산( 풀 프레임 기준 )

            fpsTimer.start();

            fpsFrameCount = 0;

            fpsFrameCount++;

            if (fpsTimer.elapsed() >= 1000) {
                double fps = fpsFrameCount * 1000.0 / fpsTimer.elapsed();
                qDebug() << "[HanwhaThread] 현재 FPS ≈" << fps;
                fpsTimer.restart();
                fpsFrameCount = 0;
            }

            //QPixmap fullPix = QPixmap::fromImage(fullImg.rgbSwapped());

            qDebug() << "[HanwhaThread] fullPix size:" << fullImg.size() << " isNull:" << fullImg.isNull();
            emit frameReady(safeImg);
        }

        qDebug() << "[HanwhaThread] 스레드 종료 요청됨";

        gst_element_set_state(pipeline, GST_STATE_NULL);
        gst_object_unref(pipeline);
    }
}

HanwhaThread::~HanwhaThread()
{
    this->stop();
}
