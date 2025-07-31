#include "VideoThread.h"
#include <QImage>
#include <QPixmap>
#include <QPainterPath>
#include <QDebug>
#include <opencv2/opencv.hpp>
#include <opencv2/core/ocl.hpp>
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
    std::vector<std::pair<cv::Mat,cv::Mat>> maps(3);
    std::vector<std::pair<cv::UMat, cv::UMat>> umaps(3);
    int cam_W = 640, cam_H = 480;
    int undistorted_w;
    int undistorted_h;
    cv::Mat undistorted_frames[3];
    cv::UMat srcU, dstU;
    
    int angle = 0;
    QVector<int> wdata;

    if(cv::ocl::haveOpenCL())
    {
        cv::ocl::setUseOpenCL(true);
        qDebug() << "[VideoThread] OpenCL 사용 가능:" << cv::ocl::Device::getDefault().name();
        
        cv::Mat transform_map_x, transform_map_y;
        getUndistortionMap_hemi(transform_map_x, transform_map_y, cam_W, cam_H);
        
        for (auto& um : umaps)
        {
            qDebug() << "[VideoThread] OpenCL 사용 시도";
            transform_map_x.copyTo(um.first);
            transform_map_y.copyTo(um.second);
        }

        undistorted_w = static_cast<int>(umaps[0].first.cols);
        undistorted_h = static_cast<int>(umaps[0].first.rows);
    }
    else
    {
        qDebug() << "[VideoThread] OpenCL 사용 불가, CPU 모드로 실행";
        for (auto& m : maps)
            getUndistortionMap_hemi(m.first, m.second, cam_W, cam_H);

        undistorted_w = static_cast<int>(maps[0].first.cols);
        undistorted_h = static_cast<int>(maps[0].first.rows);
    }
    
    qDebug() << "[VideoThread] undistorted_w =" << undistorted_w << ", undistorted_h =" << undistorted_h;
    
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

        cv::Mat received_full_frames(height, width, CV_8UC3, (char*)map.data);
        for (size_t i = 0; i < 3; ++i) 
        {
            // if (false) {
            if (cv::ocl::haveOpenCL()) {
                received_full_frames(cv::Rect(i*cam_W, 0, cam_W, cam_H)).copyTo(srcU);
                // remap 수행 (GPU 가속)
                cv::remap(srcU, dstU, umaps[i].first, umaps[i].second, cv::INTER_LINEAR);
                // 다시 Mat으로 복사
                dstU.copyTo(undistorted_frames[i]);
            }
            else
            {
                cv::remap(received_full_frames(cv::Rect(i*cam_W, 0, cam_W, cam_H)), undistorted_frames[i], maps[i].first, maps[i].second, cv::INTER_LINEAR);
                // cv::remap(mat(cv::Rect(i*cam_W, 0, cam_W, cam_H)), undistorted_frames[i], maps[i].first, maps[i].second, cv::INTER_NEAREST);
                // undistorted_frames[i] = mat(cv::Rect(i*cam_W, 0, cam_W, cam_H)).clone();
            }
        }

        cv::Mat concatenated_undistorted;
        cv::hconcat(undistorted_frames, 3, concatenated_undistorted);        

        {
            QMutexLocker locker(&m_coord->mutex);
            wdata = m_coord->width_data;
            qDebug() << "[VideoThread] 받은 width_data:" << wdata;
        }

        {
            QMutexLocker locker(&m_coord->mutex);
            if(!m_coord->angle_data.isEmpty())
            {
                angle = m_coord->angle_data.last();
                qDebug() << "[VideoThread] 받은 angle_data:" << angle;
            }
            else
            {
                qDebug() << "[VideoThread] angle_data가 비어있음, 기본값 사용";
                // 기본값 설정
                angle = 0; 
            }
        }

        int angle_px = (angle * concatenated_undistorted.cols) / 360;
        cv::line(concatenated_undistorted, cv::Point(angle_px, 0), cv::Point(angle_px, concatenated_undistorted.rows), cv::Scalar(0, 0, 255), 5);

        // draw each box onto the raw OpenCV frame
        for (size_t k = 0; k + 3 < wdata.size(); k += 4) 
        {
            int cx = wdata[k];
            int cy = wdata[k + 1];
            int w = wdata[k + 2];
            int h = wdata[k + 3];

            cv::rectangle(concatenated_undistorted,
                          cv::Point(cx - w/2, cy - h/2),
                          cv::Point(cx + w/2, cy + h/2),
                          cv::Scalar(0, 255, 0), 2);
        }

        gst_buffer_unmap(buffer, &map);
        gst_sample_unref(sample);

        QPixmap fullPix = QPixmap::fromImage(
            QImage(concatenated_undistorted.data,
                   concatenated_undistorted.cols,
                   concatenated_undistorted.rows,
                   concatenated_undistorted.step,
                   QImage::Format_BGR888
            ).rgbSwapped()
        );
        
        // make panorama by removing overlapped areas
        int fullW=fullPix.width();
        int fullH=fullPix.height();

        cv::Mat pano(fullH, fullW, CV_8UC3, (char*)map.data);
        cv::cvtColor(pano, pano, cv::COLOR_BGR2RGB);
        cv::Mat left(pano, cv::Rect(0, 0, fullW / 3, fullH));
        cv::Mat right(pano, cv::Rect(fullW * 2 / 3, 0, fullW / 3, fullH));
        cv::Mat center(pano, cv::Rect(fullW / 3, 0, fullW / 3, fullH));
        
        // concatenate three regions horizontally
        double overlap = 0.3; // 30% 오버랩
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

        //fps계산
        fpsFrameCount++;

        if (fpsTimer.elapsed() >= 1000) {
            double fps = fpsFrameCount * 1000.0 / fpsTimer.elapsed();
            qDebug() << "[VideoThread] 현재 FPS ≈" << fps;
            fpsTimer.restart();
            fpsFrameCount = 0;
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

int VideoThread::getUndistortionMap_hemi(cv::Mat& transform_map_x, cv::Mat& transform_map_y, int n_width, int n_height)
{
    transform_map_x.create(n_height, n_width, CV_32F);
    transform_map_y.create(n_height, n_width, CV_32F);
    
    // distortion parameters
    int Cx = n_width / 2; 
    int Cy = n_height / 2;
    double F = (double)n_width / CV_PI;
    
    for (int v = 0; v < n_height; v++) 
    {
        for (int u = 0; u < n_width; u++) 
        {
            // implement hemi-cylinder target model
            double xt = double(u);
            double yt = double(v - Cy);

            double r = (double)n_width / CV_PI;
            double alpha = double(n_width - xt) / r;
            double xp = r * cos(alpha);
            double yp = /*((double)nWidth / (double)nHeight) **/ yt;
            double zp = r * fabs(sin(alpha));

            double rp = sqrt(xp * xp + yp * yp);
            double theta = atan(rp / zp);

            double x1 = F * theta * xp / rp;
            double y1 = F * theta * yp / rp;

            transform_map_x.at<float>(v, u) = (float)x1 + (float)Cx;
            transform_map_y.at<float>(v, u) = (float)y1 + (float)Cy;
        }
    }

    return 0;
}

// //*/ // //복호화 추가해서 수정한 ver.

// #include "VideoThread.h"
// #include <QImage>
// #include <QPixmap>
// #include <QMetaObject>
// #include <QDebug>
// #include <opencv2/opencv.hpp>
// #include <gst/gst.h>
// #include <gst/app/gstappsink.h>
// #include "Coordinate.h"
// #include <ㄴQMutexLocker>
// #include <QPainter>
// #include <QStandardPaths>
// #include <QDir>
// #include <QDateTime>




// VideoThread::VideoThread(const QString& url, QLabel* label, Coordinate* coord)
//     : m_url(url), m_label(label), m_coord(coord), m_stop(false) {}

// void VideoThread::stop() {
//     m_stop = true;
// }
// //Tls buscall
// static gboolean bus_call (GstBus *bus,GstMessage *msg, gpointer data) {
//     GMainLoop *loop = (GMainLoop *) data;
//     switch (GST_MESSAGE_TYPE (msg)) {
//     case GST_MESSAGE_EOS:
//         g_print ("Stream Ends\n");
//         g_main_loop_quit (loop);
//         break;
//     case GST_MESSAGE_ERROR: {
//         gchar  *debug;
//         GError *error;
//         gst_message_parse_error (msg, &error, &debug);
//         g_free (debug);
//         g_printerr ("Error: %s\n", error->message);
//         g_error_free (error);
//         g_main_loop_quit (loop);
//         break;
//     }
//     default:
//         break;
//     }
//     return TRUE;
// }

// void VideoThread::run() {
//     gst_init(nullptr, nullptr);

//     //Mac에서 가장 좋은 파이프라인
//     // QString pipelineStr = QString(
//     //                           "rtspsrc name=src "
//     //                           "protocols=2 "
//     //                           "location=%1 "
//     //                           "latency=100 "
//     //                           "tls-validation-flags=0x00000001 ! "
//     //                           "tls-hostname=%2 "
//     //                           "application/x-rtp,media=video,encoding-name=H264 ! "
//     //                           "rtph264depay ! "
//     //                           "h264parse ! "
//     //                           "vtdec ! "
//     //                           "videoconvert ! "
//     //                           "video/x-raw,format=RGB ! "
//     //                           "appsink name=mysink"
//     //                           ).arg(m_url);

//     QString pipelineStr = QString(
//                               "rtspsrc name=src "
//                               "protocols=2 "
//                               "location=%1 "
//                               "latency=100 "
//                               "tls-validation-flags=0x00000002 "
//                               "tls-hostname=%2 "           // SNI host
//                               "! application/x-rtp,media=video,encoding-name=H264 "
//                               "! rtph264depay "
//                               "! h264parse "
//                               "! vtdec "
//                               "! videoconvert "
//                               "! video/x-raw,format=RGB "
//                               "! appsink name=mysink"
//                               ).arg(m_url);




//     qDebug() << "[VideoThread] pipeline =" << pipelineStr;

//     GstElement* pipeline = gst_parse_launch(pipelineStr.toStdString().c_str(), nullptr);
//     if (!pipeline)
//     {
//         qDebug() << "[VideoThread] 파이프라인 생성 실패";
//         return;
//     }


//     //<복호화과정>
//     //rtspsrc요소 꺼내기

//     GstElement* src = gst_bin_get_by_name(GST_BIN(pipeline),"src");

//     if(!src){
//         qDebug()<<"[videothread] src가져오기 실패";
//         return;
//     }
//     else if(src){
//         qDebug()<<"src가져오기 성공";
//     }

//     //CA데이터 베이스 로드
//     GError *error=nullptr;
//     GTlsDatabase *database=
//         g_tls_file_database_new(rtsp_config.rtsp_ca_cert, &error);
//     if(!database){
//         qDebug()<<"[videothread CA database로드 실패" << error->message;
//         g_clear_error(&error);
//         gst_object_unref(pipeline);//오류나면 파이프라인 정리하고 리턴
//         return;
//     }
//     else if(database){
//         qDebug()<<"[videothread CA database로드 성공";
//                }
//     gboolean flags = 0;
//     g_object_get(G_OBJECT(src), "tls-validation-flags", &flags, nullptr);
//     qDebug() << "tls-validation-flags =" << flags;


//     //TLS검증 설정
//     g_object_set(G_OBJECT(src),
//                  "tls-validation-flags", G_TLS_CERTIFICATE_VALIDATE_ALL, //검증방법
//                  "tls-database",database, //실제 ca 인증서 리스트
//                  nullptr
//                  );

//     //message hadler

//     GMainLoop *loop;
//     GstBus *bus;
//     bus=gst_pipeline_get_bus (GST_PIPELINE(pipeline));
//     gst_bus_add_watch(bus,bus_call,loop);
//     gst_object_unref(bus);

//     GstElement* sink = gst_bin_get_by_name(GST_BIN(pipeline), "mysink");
//     if (!sink)
//     {
//         qDebug() << "[VideoThread] appsink 핸들 가져오기 실패";
//         gst_object_unref(pipeline);
//         return;
//     }

//     g_object_set(sink,
//                  "emit-signals", FALSE,
//                  "sync", FALSE,
//                  "max-buffers", 1,
//                  "drop", TRUE,
//                  nullptr);

//     fpsTimer.start();
//     fpsFrameCount = 0;

//     gst_element_set_state(pipeline, GST_STATE_PLAYING);
//     qDebug() << "[VideoThread] 파이프라인 재생 시작";

//     //크롭 사이즈 고정하기
//     constexpr int W = 480, H = 360;
//     const int THRESHOLD = 5;


//     while (!m_stop)
//     {
//         qDebug() << "[VideoThread] sample 수신 대기 중";

//         GstSample* sample = gst_app_sink_pull_sample(GST_APP_SINK(sink));
//         if (!sample)
//         {
//             qDebug() << "[VideoThread] sample 수신 실패";
//             continue;
//         }
//         qDebug() << "[VideoThread] sample 수신 성공";

//         GstBuffer* buffer = gst_sample_get_buffer(sample);
//         GstMapInfo map;
//         if (!gst_buffer_map(buffer, &map, GST_MAP_READ))
//         {
//             qDebug() << "[VideoThread] 버퍼 매핑 실패";
//             gst_sample_unref(sample);
//             continue;
//         }

//         int width = 0, height = 0;
//         GstCaps* caps = gst_sample_get_caps(sample);
//         GstStructure* s = gst_caps_get_structure(caps, 0);
//         gst_structure_get_int(s, "width", &width);
//         gst_structure_get_int(s, "height", &height);

//         qDebug() << "[VideoThread] width =" << width << ", height =" << height;

//         cv::Mat mat(height, width, CV_8UC3, (char*)map.data);
//         cv::Mat copy = mat.clone();

//         gst_buffer_unmap(buffer, &map);
//         gst_sample_unref(sample);

//         QImage fullImg(copy.data, copy.cols, copy.rows, copy.step, QImage::Format_BGR888);

//         if (fullImg.isNull())
//         {
//             qDebug() << "[VideoThread] QImage 생성 실패";
//             continue;
//         }
//         qDebug() << "[VideoThread] QImage 생성 성공";
//         //fps계산( 풀 프레임 기준 )

//         fpsFrameCount++;

//         if (fpsTimer.elapsed() >= 1000) {
//             double fps = fpsFrameCount * 1000.0 / fpsTimer.elapsed();
//             qDebug() << "[VideoThread] 현재 FPS ≈" << fps;
//             fpsTimer.restart();
//             fpsFrameCount = 0;
//         }

//         QPixmap fullPix = QPixmap::fromImage(fullImg.rgbSwapped());

//         qDebug() << "[VideoThread] fullPix size:" << fullPix.size() << " isNull:" << fullPix.isNull();
//         emit fullFrame(fullPix);
//         int fullW=fullPix.width();
//         int fullH=fullPix.height();

//         QVector<int> wdata;
//         {
//             QMutexLocker locker(&m_coord->mutex);
//             wdata = m_coord->width_data;
//             qDebug() << "[VideoThread] 받은 width_data:" << wdata;

//         }

//         // ’wdata’ 크기가 4의 배수인지 확인
//         int rectCount = wdata.size() / 4;
//         if (rectCount == 0) {
//             qDebug() << "[VideoThread] 크롭 정보 없음 — 전체 프레임으로 대체";
//             // 크롭 정보가 없으면 (0,0)부터 W×H 자르기
//             emit cropped(0, fullPix.copy(0, 0, W, H));
//             continue;
//         }

//         QVector<std::pair<int,QPixmap>> crops;
//         crops.reserve(rectCount);

//         for (int i = 0; i < rectCount; ++i) {
//             int cx = wdata[4*i + 0];
//             int cy = wdata[4*i + 1];
//             // 너비·높이 w,h = wdata[4*i+2], wdata[4*i+3] 은 무시하고
//             // 항상 W×H 로 자르고 싶음 아래
//             int x0 = std::clamp(cx - W/2, 0, fullW - W);
//             int y0 = std::clamp(cy - H/2, 0, fullH - H);

//             QRect roi(x0, y0, W, H);
//             QPixmap pix = fullPix.copy(roi);
//             qDebug() << "[VideoThread] crop" << i << ": roi =" << roi << ", isNull =" << pix.isNull();
//             //             //하드코딩으로 확인
//             //             {
//             //                 QMutexLocker locker(&m_coord->mutex);
//             //                 m_coord->angle_data = QVector<int>{90};
//             //             }

//             //             // 로컬 복사해서 작업
//             //             QVector<int> angle_data;
//             //             {
//             //                 QMutexLocker locker(&m_coord->mutex);
//             //                 angle_data = m_coord->angle_data;
//             //             }
//             // //여기까지
//             // 하이라이팅 조건 검사
//             for (int j = 0; j < m_coord->angle_data.size(); ++j) {
//                 int angle = m_coord->angle_data[j];
//                 int px = (angle * fullW) / 360;

//                 qDebug() << "[하이라이트 검사] angle:" << angle
//                          << "→ px:" << px
//                          << ", 크롭영역: [" << x0 << "~" << (x0 + W) << "]";

//                 if (px >= x0 && px < x0 + W) {
//                     QPainter painter(&pix);
//                     painter.setPen(QPen(Qt::yellow, 5));
//                     painter.setBrush(Qt::NoBrush);
//                     painter.drawRect(0, 0, W - 1, H - 1);
//                     painter.end();
//                     break; // 하나라도 걸리면 테두리 그리기
//                 }
//             }

//             crops.emplace_back(x0, std::move(pix));
//         }

//         // 4) x0 기준 정렬 & emit
//         std::sort(crops.begin(), crops.end(),
//                   [](auto &a, auto &b){ return a.first < b.first; });
//         for (int i = 0; i < crops.size(); ++i) {
//             qDebug() << "[VideoThread] emit cropped" << i;
//             emit cropped(i, crops[i].second);
//         }




//     }

//     qDebug() << "[VideoThread] 스레드 종료 요청됨";

//     gst_element_set_state(pipeline, GST_STATE_NULL);
//     gst_object_unref(pipeline);
//
