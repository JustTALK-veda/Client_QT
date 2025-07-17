
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
#include <QStandardPaths>
#include <QDir>
#include <QDateTime>


VideoThread::VideoThread(const QString& url, QLabel* label, Coordinate* coord)
    : m_url(url), m_label(label), m_coord(coord), m_stop(false) {}

void VideoThread::stop() {
    m_stop = true;
}

void VideoThread::run() {
    gst_init(nullptr, nullptr);

    //녹화 기능
    QString desktopPath = QStandardPaths::writableLocation(
        QStandardPaths::DesktopLocation
        );
    QString recordFile = QDir(desktopPath).filePath(
        QDateTime::currentDateTime()
            .toString("record_yyyyMMdd_hhmmss.mp4")
        );
    qDebug() << "[VideoThread] Recording to:" << recordFile;

//Mac에서 가장 좋은 파이프라인
    QString pipelineStr = QString(
                              "rtspsrc location=%1 latency=100 ! "
                              "application/x-rtp,media=video,encoding-name=H264 ! "
                              "rtph264depay ! "
                              "h264parse ! "
                              "vtdec ! "             // ← 여기서 하드웨어 디코딩
                              "videoconvert ! "
                              "video/x-raw,format=RGB ! "
                              "appsink name=mysink"  // ← Qt에서 프레임을 가져오기 위한 sink
                              ).arg(m_url);

// //원래 파이프라인
//     QString pipelineStr = QString(
//                               "rtspsrc location=%1 "
//                               "latency=100 ! "
//                               "decodebin ! "
//                               "videoconvert ! "
//                               "video/x-raw,format=RGB ! "
//                               "appsink name=mysink" // appsink보다는 vtdec사용할것
//                               ).arg(m_url);
// //영상 녹화 ver.
    // QString pipelineStr = QString(
    //                           "rtspsrc location=%1 latency=100 protocols=GST_RTSP_LOWER_TRANS_TCP "
    //                           "! rtph264depay ! h264parse name=parser ! tee name=t "
    //                           // 화면 표시
    //                           "t. ! queue max-size-buffers=1 leaky=downstream "
    //                           "    ! parser. ! avdec_h264 "
    //                           "    ! videoconvert ! video/x-raw,format=RGB "
    //                           "    ! appsink name=mysink sync=false "
    //                           // 녹화 저장
    //                           "t. ! queue max-size-buffers=1 leaky=downstream "
    //                           "    ! vaapih264enc bitrate=4000 "
    //                           "    ! video/x-h264,profile=baseline "
    //                           "    ! h264parse ! mp4mux "
    //                           "    ! filesink location=" + recordFile
    //                           ).arg(m_url);


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

    fpsTimer.start();
    fpsFrameCount = 0;

    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    qDebug() << "[VideoThread] 파이프라인 재생 시작";

    //크롭 사이즈 고정하기
    constexpr int W = 480, H = 360;
    const int THRESHOLD = 5;


    while (!m_stop)
    {
        qDebug() << "[VideoThread] sample 수신 대기 중";

        qDebug() << "[VideoThread] sample1 수신 성공";
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

        //fps계산( 풀 프레임 기준 )

        fpsFrameCount++;

        if (fpsTimer.elapsed() >= 1000) {
            double fps = fpsFrameCount * 1000.0 / fpsTimer.elapsed();
            qDebug() << "[VideoThread] 현재 FPS ≈" << fps;
            fpsTimer.restart();
            fpsFrameCount = 0;
        }

        QPixmap fullPix = QPixmap::fromImage(fullImg.rgbSwapped());
        emit fullFrame(fullPix);
        int fullW=fullPix.width();
        int fullH=fullPix.height();

        QVector<int> wdata;
        {
            QMutexLocker locker(&m_coord->mutex);
            wdata = m_coord->width_data;
        }

        // ’wdata’ 크기가 4의 배수인지 확인
        int rectCount = wdata.size() / 4;
        if (rectCount == 0) {
             qDebug() << "[VideoThread] 크롭 정보 없음 — 전체 프레임으로 대체";
            // 크롭 정보가 없으면 (0,0)부터 W×H 자르기
            emit cropped(0, fullPix.copy(0, 0, W, H));
            continue;
        }

        QVector<std::pair<int,QPixmap>> crops;
        crops.reserve(rectCount);

        for (int i = 0; i < rectCount; ++i) {
            int cx = wdata[4*i + 0];
            int cy = wdata[4*i + 1];
            // 너비·높이 w,h = wdata[4*i+2], wdata[4*i+3] 은 무시하고
            // 항상 W×H 로 자르고 싶음 아래
            int x0 = std::clamp(cx - W/2, 0, fullW - W);
            int y0 = std::clamp(cy - H/2, 0, fullH - H);

            QRect roi(x0, y0, W, H);
            QPixmap pix = fullPix.copy(roi);

            crops.emplace_back(x0, std::move(pix));
        }

        // 4) x0 기준 정렬 & emit
        std::sort(crops.begin(), crops.end(),
                  [](auto &a, auto &b){ return a.first < b.first; });
        for (int i = 0; i < crops.size(); ++i) {
            qDebug() << "[VideoThread] emit cropped" << i;
            emit cropped(i, crops[i].second);
        }

    }

    qDebug() << "[VideoThread] 스레드 종료 요청됨";
    
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
}

