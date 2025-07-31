#include "VideoThread.h"
#include <QImage>
#include <QPixmap>
#include <QPainterPath>
#include <QDebug>
#include <opencv2/opencv.hpp>
#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include "Coordinate.h"
#include <QMutexLocker>
#include <QPainter>
#include <QDateTime>



VideoThread::VideoThread(const QString& url, QLabel* label, Coordinate* coord, bool checkOnly)
    : m_url(url), m_label(label), m_coord(coord), m_checkOnly(checkOnly), m_stop(false) {}

void VideoThread::stop() {
    m_stop = true;
}

void VideoThread::run() {
    gst_init(nullptr, nullptr);

    QString pipelineStr;

#ifdef _WIN32
    pipelineStr = QString(
                      "rtspsrc location=%1 latency=100 tls-validation-flags=0 ! "
                      "decodebin ! "
                      "videoconvert ! "
                      "video/x-raw,format=RGB ! "
                      "appsink name=mysink"
                      ).arg(m_url);
    //audio
    // pipelineStr = QString(
    //                   "rtspsrc location=%1 latency=100 tls-validation-flags=0 protocols=tcp name=src "
    //                   "src. ! application/x-rtp,media=video ! rtph264depay ! avdec_h264 ! videoconvert ! video/x-raw,format=RGB ! appsink name=mysink "
    //                   "src. ! application/x-rtp,media=audio ! rtpmp4gdepay ! avdec_aac ! audioconvert ! audioresample ! autoaudiosink sync=false"
    //                   ).arg(m_url);
#elif __APPLE__
    // pipelineStr = QString(
    //                   "rtspsrc location=%1 latency=100 tls-validation-flags=0 ! "
    //                   "application/x-rtp,media=video,encoding-name=H264 ! "
    //                   "rtph264depay ! "
    //                   "h264parse ! "
    //                   "vtdec ! "
    //                   "videoconvert ! "
    //                   "video/x-raw,format=RGB ! "
    //                   "appsink name=mysink"
    //                   ).arg(m_url);
    // audio
    pipelineStr = QString(
                      "rtspsrc location=%1 latency=100 tls-validation-flags=0 protocols=tcp name=src "
                      "src. ! application/x-rtp,media=video,encoding-name=H264 ! rtph264depay ! h264parse ! vtdec ! videoconvert ! video/x-raw,format=RGB ! appsink name=mysink "
                      "src. ! application/x-rtp,media=audio ! rtpmp4gdepay ! avdec_aac ! audioconvert ! audioresample ! osxaudiosink sync=false"
                      ).arg(m_url);
#endif

    qDebug() << "[VideoThread] pipeline =" << pipelineStr;

    GstElement* pipeline = gst_parse_launch(pipelineStr.toStdString().c_str(), nullptr);
    if (!pipeline)
    {
        qDebug() << "[VideoThread] 파이프라인 생성 실패";
        emit serverReady(false);
        return;
    }

    GstElement* sink = gst_bin_get_by_name(GST_BIN(pipeline), "mysink");
    if (!sink)
    {
        qDebug() << "[VideoThread] appsink 핸들 가져오기 실패";
        gst_object_unref(pipeline);
        emit serverReady(false);
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

    if (m_checkOnly) {
        // ✅ 확인 모드: 최대 3초 동안 한 프레임만 시도
        bool success = false;
        QElapsedTimer timer;
        timer.start();

        while (timer.elapsed() < 3000) {
            GstSample* sample = gst_app_sink_try_pull_sample(GST_APP_SINK(sink), 100 * GST_MSECOND);
            if (sample) {
                qDebug() << "[VideoThread] 서버에서 첫 sample 수신 성공";
                gst_sample_unref(sample);
                success = true;
                break;
            }
        }

        gst_element_set_state(pipeline, GST_STATE_NULL);
        gst_object_unref(pipeline);

        emit serverReady(success);
        return;
    }

    //크롭 사이즈 고정하기
    constexpr int W = 480, H = 360;
    const int THRESHOLD = 5;

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
        //cv::Mat copy = mat.clone();

        // gst_buffer_unmap(buffer, &map);
        // gst_sample_unref(sample);

        // QImage fullImg(copy.data, copy.cols, copy.rows, copy.step, QImage::Format_BGR888);

        // if (fullImg.isNull())
        // {
        //     qDebug() << "[VideoThread] QImage 생성 실패";
        //     continue;
        // }
        // qDebug() << "[VideoThread] QImage 생성 성공";
        // //fps계산( 풀 프레임 기준 )

        // fpsTimer.start();

        // fpsFrameCount = 0;

        // fpsFrameCount++;

        // if (fpsTimer.elapsed() >= 1000) {
        //     double fps = fpsFrameCount * 1000.0 / fpsTimer.elapsed();
        //     qDebug() << "[VideoThread] 현재 FPS ≈" << fps;
        //     fpsTimer.restart();
        //     fpsFrameCount = 0;
        // }

        // QPixmap fullPix = QPixmap::fromImage(fullImg.rgbSwapped());

        // qDebug() << "[VideoThread] fullPix size:" << fullPix.size() << " isNull:" << fullPix.isNull();
        // emit fullFrame(fullPix);
        // int fullW=fullPix.width();
        // int fullH=fullPix.height();

        QVector<int> wdata;

        {
            QMutexLocker locker(&m_coord->mutex);
            wdata = m_coord->width_data;
            qDebug() << "[VideoThread] 받은 width_data:" << wdata;

        }

        QVector<int> angles;
        {
            QMutexLocker locker(&m_coord->mutex);
            angles = m_coord->angle_data;
        }

        if (angles.isEmpty())
        {
            // qDebug() << "[VideoThread] angle_data가 비어있음, 하이라이팅 생략";
            continue;
        }

        int angle = angles.last();

        int angle_px = (angle * mat.cols) / 360;
        cv::line(mat, cv::Point(angle_px, 0), cv::Point(angle_px, mat.rows), cv::Scalar(0, 0, 255), 5);

        // draw each box onto the raw OpenCV frame
        for (size_t k = 0; k + 3 < wdata.size(); k += 4) {
            int cx = wdata[k];
            int cy = wdata[k + 1];
            int w = wdata[k + 2];
            int h = wdata[k + 3];

            cv::rectangle(mat,
                          cv::Point(cx - w/2, cy - h/2),
                          cv::Point(cx + w/2, cy + h/2),
                          cv::Scalar(0, 255, 0), 2);
        }

        /*하이라이팅 테스트하려고 밑으로 내렸음*/
        cv::Mat copy = mat.clone();

        gst_buffer_unmap(buffer, &map);
        gst_sample_unref(sample);

        QImage fullImg(copy.data, copy.cols, copy.rows, copy.step, QImage::Format_BGR888);



        if (fullImg.isNull())
        {
            // qDebug() << "[VideoThread] QImage 생성 실패";
            continue;
        }
        //  qDebug() << "[VideoThread] QImage 생성 성공";
        //fps계산( 풀 프레임 기준 )

        fpsFrameCount++;

        if (fpsTimer.elapsed() >= 1000) {
            double fps = fpsFrameCount * 1000.0 / fpsTimer.elapsed();
            qDebug() << "[VideoThread] 현재 FPS ≈" << fps;
            fpsTimer.restart();
            fpsFrameCount = 0;
        }

        QPixmap fullPix = QPixmap::fromImage(fullImg.rgbSwapped());

        // make panorama
        int fullW=fullPix.width();
        int fullH=fullPix.height();

        cv::Mat pano(fullH, fullW, CV_8UC3, (char*)map.data);
        cv::cvtColor(pano, pano, cv::COLOR_BGR2RGB);
        cv::Mat left(pano, cv::Rect(0, 0, fullW / 3, fullH));
        cv::Mat right(pano, cv::Rect(fullW * 2 / 3, 0, fullW / 3, fullH));
        cv::Mat center(pano, cv::Rect(fullW / 3, 0, fullW / 3, fullH));
        double overlap = 0.3; // 30% 오버랩
        // concatenate three regions horizontally
        std::vector<cv::Mat> mats;
        mats.emplace_back(left(cv::Rect(0, 0, static_cast<int>(left.cols * (1 - overlap)), fullH)));
        mats.emplace_back(center);
        mats.emplace_back(right(cv::Rect(static_cast<int>(right.cols * overlap), 0, static_cast<int>(right.cols * (1 - overlap)), fullH)));
        cv::hconcat(mats, pano);

        // qDebug() << "[VideoThread] fullPix size:" << fullPix.size() << " isNull:" << fullPix.isNull();
        emit fullFrame(QPixmap::fromImage(QImage((const uchar*)pano.data, pano.cols, pano.rows, pano.step[0], QImage::Format_BGR888)));

        /*하이라이팅 테스트하려고 밑으로 내렸음*/

        // 'wdata' 크기가 4의 배수인지 확인
        int rectCount = wdata.size() / 4;
        if (rectCount == 0) {
            qDebug() << "[VideoThread] 크롭 정보 없음 — 전체 프레임으로 대체";
            // 크롭 정보가 없으면 (0,0)부터 W×H 자르기
            emit cropped(0,QPixmap());
            //emit cropped(0, fullPix.copy(0, 0, W, H));
            continue;
        }

        QVector<std::pair<int,QPixmap>> crops;

        crops.reserve(rectCount);

        int highlighted_rect_index = -1; // 하이라이팅할 인덱스
        int min_distance = fullW;
        QColor myColor(237, 107, 6);

        for (int i = 0; i < rectCount; ++i) {
            int cx = wdata[4*i + 0];
            int cy = wdata[4*i + 1];

            // 너비·높이 w,h = wdata[4*i+2], wdata[4*i+3] 은 무시하고
            // 항상 W×H 로 자르고 싶음 아래
            int x0 = std::clamp(cx - W/2, 0, fullW - W);
            int y0 = std::clamp(cy - H/2, 0, fullH - H);

            QRect roi(x0, y0, W, H);
            QPixmap pix = fullPix.copy(roi);
            pix = roundedPixmap(pix, 10);

            qDebug() << "[VideoThread] crop" << i << ": roi =" << roi << ", isNull =" << pix.isNull();

            int dx = std::abs(cx - angle_px);
            if (dx > fullW/2) dx = fullW - dx;
            if (dx < min_distance) {
                min_distance = dx;
                highlighted_rect_index = i;
            }
            crops.emplace_back(x0, std::move(pix));

        }
        emit peoplecount(crops.size());

        if (highlighted_rect_index != -1) {
            QPixmap &highlighted_pix = crops[highlighted_rect_index].second;
            drawHighlightOverlay(highlighted_pix, myColor, 5, 10);
            highlighted_pix = roundedPixmap(highlighted_pix, 10);
        }
        else {
            qDebug() << "[VideoThread] highlighted index가 -1, 하이라이팅 없음";
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

QPixmap VideoThread::roundedPixmap(const QPixmap& src, int radius) {
    QPixmap dest(src.size());
    dest.fill(Qt::transparent);

    QPainter painter(&dest);
    painter.setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    path.addRoundedRect(src.rect(), radius, radius);
    painter.setClipPath(path);
    painter.drawPixmap(0, 0, src);
    return dest;
}

void VideoThread::drawHighlightOverlay(QPixmap& pixmap, const QColor& color, int dotRadius, int cornerRadius) {
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    // 원형 점
    int margin = 10;
    int center_x = pixmap.width() - dotRadius - margin;
    int center_y = dotRadius + margin;
    painter.setBrush(color);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(QPoint(center_x, center_y), dotRadius, dotRadius);

    // 모서리 둥근 테두리
    QRect borderRect(0, 0, pixmap.width(), pixmap.height());
    QPainterPath roundedRect;
    roundedRect.addRoundedRect(borderRect, cornerRadius, cornerRadius);

    QPen borderPen(color, 6);
    painter.setPen(borderPen);
    painter.setBrush(Qt::NoBrush);
    painter.drawPath(roundedRect);
}
