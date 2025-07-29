
#include "VideoThread.h"
#include <QImage>
#include <QPixmap>
#include <QMetaObject>
#include <QDebug>
#include <opencv2/opencv.hpp>
#include <opencv2/core/ocl.hpp>
#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include "Coordinate.h"
#include <QMutexLocker>
#include <QPainter>
#include <QStandardPaths>
#include <QDir>
#include <QDateTime>

int getUndistortionMap_hemi(cv::Mat& transform_map_x, cv::Mat& transform_map_y, int cam_W, int cam_H);
int getUndistortionMap_fisheye(cv::Mat& map1, cv::Mat& map2, int cam_W, int cam_H, QString path);

VideoThread::VideoThread(const QString& url, QLabel* label, Coordinate* coord)
    : m_url(url), m_label(label), m_coord(coord), m_stop(false) {}

void VideoThread::stop() {
    m_stop = true;
}

void VideoThread::run() {
    gst_init(nullptr, nullptr);

//Mac에서 가장 좋은 파이프라인
    QString pipelineStr = QString(
                              "rtspsrc location=%1 latency=100 tls-validation-flags=0 ! "
                              "decodebin ! "
                              "videoconvert ! "
                              "video/x-raw,format=RGB ! "
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

    fpsTimer.start();
    fpsFrameCount = 0;

    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    qDebug() << "[VideoThread] 파이프라인 재생 시작";

    //크롭 사이즈 고정하기
    constexpr int W = 480, H = 360;
    constexpr int cam_W = 640, cam_H = 480; // 카메라 해상도
    const int THRESHOLD = 5;

    std::vector<std::pair<cv::Mat,cv::Mat>> maps(3);
    std::vector<std::pair<cv::UMat, cv::UMat>> umaps(3);
    std::vector<QString> paths = {"config/fisheye_calibration_L.yml",
                                 "config/fisheye_calibration_C.yml",
                                 "config/fisheye_calibration_R.yml"};
    
    int undistorted_w = 0, undistorted_h = 0;
    if(cv::ocl::haveOpenCL())
    // if(false)
    {
        qDebug() << "[VideoThread] OpenCL 사용 가능";
        cv::ocl::Device dev = cv::ocl::Device::getDefault();
        qDebug() << "Default OpenCL device:" << dev.name() << dev.type();

        cv::ocl::setUseOpenCL(true);
        for (size_t i = 0; i < umaps.size(); i++)
        {
            cv::Mat map1, map2;
            getUndistortionMap_fisheye(map1, map2, cam_W, cam_H, paths[i]);

            umaps[i].first = map1.getUMat(cv::ACCESS_READ);
            umaps[i].second = map2.getUMat(cv::ACCESS_READ);
        }
        undistorted_w = static_cast<int>(umaps[0].first.cols);
        undistorted_h = static_cast<int>(umaps[0].first.rows);
    }
    else
    {
        qDebug() << "[VideoThread] OpenCL 사용 불가";
        for (size_t i=0; i<maps.size(); i++)
        {
            cv::Mat& map1 = maps[i].first;
            cv::Mat& map2 = maps[i].second;
            getUndistortionMap_fisheye(map1, map2, cam_W, cam_H, paths[i]);
        }
        undistorted_w = static_cast<int>(maps[0].first.cols);
        undistorted_h = static_cast<int>(maps[0].first.rows);
    }

    while (!m_stop)
    {
        // qDebug() << "[VideoThread] sample 수신 대기 중";

        GstSample* sample = gst_app_sink_pull_sample(GST_APP_SINK(sink));
        if (!sample)
        {
            // qDebug() << "[VideoThread] sample 수신 실패";
            continue;
        }
        // qDebug() << "[VideoThread] sample 수신 성공";

        GstBuffer* buffer = gst_sample_get_buffer(sample);
        GstMapInfo map;
        if (!gst_buffer_map(buffer, &map, GST_MAP_READ))
        {
            // qDebug() << "[VideoThread] 버퍼 매핑 실패";
            gst_sample_unref(sample);
            continue;
        }

        int width = 0, height = 0;
        GstCaps* caps = gst_sample_get_caps(sample);
        GstStructure* s = gst_caps_get_structure(caps, 0);
        gst_structure_get_int(s, "width", &width);
        gst_structure_get_int(s, "height", &height);

        // qDebug() << "[VideoThread] width =" << width << ", height =" << height;

        cv::Mat mat(height, width, CV_8UC3, (char*)map.data);
        
        QVector<int> wdata;
        {
            QMutexLocker locker(&m_coord->mutex);
            wdata = m_coord->width_data;
            // qDebug() << "[VideoThread] 받은 width_data:" << wdata;
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

        cv::Mat undistorted_frames[3];
        for (size_t i = 0; i < 3; ++i) 
        {
            // if (false) {
            if (cv::ocl::haveOpenCL()) {

                
                cv::UMat srcU, dstU;
                mat(cv::Rect(i*cam_W, 0, cam_W, cam_H)).copyTo(srcU);

                // remap 수행 (GPU 가속)
                cv::remap(srcU, dstU, umaps[i].first, umaps[i].second, cv::INTER_LINEAR);

                // 다시 Mat으로 복사
                dstU.copyTo(undistorted_frames[i]);
            }
            else
            {
                cv::remap(mat(cv::Rect(i*cam_W, 0, cam_W, cam_H)), undistorted_frames[i], maps[i].first, maps[i].second, cv::INTER_LINEAR);
                // cv::remap(mat(cv::Rect(i*cam_W, 0, cam_W, cam_H)), undistorted_frames[i], maps[i].first, maps[i].second, cv::INTER_NEAREST);

                // undistorted_frames[i] = mat(cv::Rect(i*cam_W, 0, cam_W, cam_H)).clone();
            }
        }

        cv::Mat copy;
        cv::hconcat(undistorted_frames, 3, copy);

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

        // qDebug() << "[VideoThread] fullPix size:" << fullPix.size() << " isNull:" << fullPix.isNull();
        emit fullFrame(fullPix);
        int fullW=fullPix.width();
        int fullH=fullPix.height();



        // ’wdata’ 크기가 4의 배수인지 확인
        int rectCount = wdata.size() / 4;
        if (rectCount == 0) {
            //  qDebug() << "[VideoThread] 크롭 정보 없음 — 전체 프레임으로 대체";
            // 크롭 정보가 없으면 (0,0)부터 W×H 자르기
            emit cropped(0, fullPix.copy(0, 0, W, H));
            continue;
        }

        QVector<std::pair<int,QPixmap>> crops;
        crops.reserve(rectCount);

        int highlighted_rect_index = -1; // 하이라이팅할 인덱스
        int min_distance = fullW; // 최소 거리 초기화

        for (int i = 0; i < rectCount; ++i) {
            int cx = wdata[4*i + 0];
            int cy = wdata[4*i + 1];
            // 너비·높이 w,h = wdata[4*i+2], wdata[4*i+3] 은 무시하고
            // 항상 W×H 로 자르고 싶음 아래
            int x0 = std::clamp(cx - W/2, 0, fullW - W);
            int y0 = std::clamp(cy - H/2, 0, fullH - H);

            QRect roi(x0, y0, W, H);
            QPixmap pix = fullPix.copy(roi);
            // qDebug() << "[VideoThread] crop" << i << ": roi =" << roi << ", isNull =" << pix.isNull();
//             //하드코딩으로 확인
//             {
//                 QMutexLocker locker(&m_coord->mutex);
//                 m_coord->angle_data = QVector<int>{90};
//             }

//             // 로컬 복사해서 작업
//             QVector<int> angle_data;
//             {
//                 QMutexLocker locker(&m_coord->mutex);
//                 angle_data = m_coord->angle_data;
//             }
// //여기까지
            // 하이라이팅 조건 검사


            int dx = std::abs(cx - angle_px);
            if (dx > fullW/2) dx = fullW - dx;
            if (dx < min_distance) {
                min_distance = dx;
                highlighted_rect_index = i;
            }

            crops.emplace_back(x0, std::move(pix));
        }

        // 하이라이팅된 영역에 테두리 그리기
        if (highlighted_rect_index != -1) {
            // qDebug() << "[VideoThread] 하이라이팅된 영역:" << highlighted_rect_index;
            // QPixmap &pix = crops[highlighted_rect_index].second;
            // QPainter painter(&pix);

            // // 텍스트 속성 설정
            // painter.setPen(QPen(Qt::yellow));           // 노란색 글씨
            // QFont f = painter.font();
            // f.setPointSize(24);                         // 원하는 크기로
            // painter.setFont(f);

            // // 화면 좌측 상단에 인덱스 번호 출력
            // QRect r = pix.rect();
            // painter.drawText(r.adjusted(10, 10, 0, 0),   // 약간의 마진(10,10)
            //                 Qt::AlignLeft | Qt::AlignTop,
            //                 QString::number(highlighted_rect_index));

            // painter.end();

            QPixmap &highlighted_pix = crops[highlighted_rect_index].second;
            QPainter painter(&highlighted_pix);
            painter.setPen(QPen(Qt::yellow, 5));
            painter.setBrush(Qt::NoBrush);
            painter.drawRect(0, 0, W + 10, H + 10);
            painter.end();

        }
        else {
            qDebug() << "[VideoThread] highlighted index가 -1, 하이라이팅 없음";
        }

        // 4) x0 기준 정렬 & emit
        std::sort(crops.begin(), crops.end(),
                  [](auto &a, auto &b){ return a.first < b.first; });
        for (int i = 0; i < crops.size(); ++i) {
            // qDebug() << "[VideoThread] emit cropped" << i;
            emit cropped(i, crops[i].second);
        }

    }

    qDebug() << "[VideoThread] 스레드 종료 요청됨";

    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
}

int getUndistortionMap_hemi(cv::Mat& transform_map_x, cv::Mat& transform_map_y, int cam_W, int cam_H) 
{
    // allocate transfomation maps
    int n_width = cam_W, n_height = cam_H;
    transform_map_x = cv::Mat(n_height, n_width, CV_32FC1);
    transform_map_y = cv::Mat(n_height, n_width, CV_32FC1);

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

int getUndistortionMap_fisheye(cv::Mat& map1, cv::Mat& map2, int cam_W, int cam_H, QString path) 
{
    // 🔹 K, D 불러오기
    cv::FileStorage fs(path.toStdString(), cv::FileStorage::READ);
    if (!fs.isOpened()) 
    {
        std::cerr << "cannot open " << path.toStdString() << std::endl;
        return -1;
    }

    cv::Mat K, D;
    fs["K"] >> K;
    fs["D"] >> D;
    fs.release();

    cv::Size input_size(cam_W, cam_H);

    int W = static_cast<int>(cam_H * M_PI + 0.5);
    cv::Size output_size(W, cam_H);
    double f = W / M_PI;

    cv::Mat R = cv::Mat::eye(3, 3, CV_64F);
    cv::Mat newK;
    // newK = Mat::eye(3, 3, CV_64F);
    // newK.at<double>(0,0) = f;
    // newK.at<double>(1,1) = f;
    // newK.at<double>(0,2) = W * 0.5;  // principal point x-center
    // newK.at<double>(1,2) = cam_H * 0.5;  // principal point y-center
    
    double alpha = 0.0; // 0.0: 원본 이미지 크기 유지, 1.0: 새 이미지 크기에 맞춤
    
    cv::fisheye::estimateNewCameraMatrixForUndistortRectify(K, D, input_size, R, newK, alpha, output_size);
    cv::fisheye::initUndistortRectifyMap(K, D, R, newK, output_size, CV_16SC2, map1, map2);
    // fisheye::initUndistortRectifyMap(K, D, R, newK, input_size, CV_16SC2, map1, map2);

    return 0;
}

// // //복호화 추가해서 수정한 ver.

// #include "VideoThread.h"
// #include <QImage>
// #include <QPixmap>
// #include <QMetaObject>
// #include <QDebug>
// #include <opencv2/opencv.hpp>
// #include <gst/gst.h>
// #include <gst/app/gstappsink.h>
// #include "Coordinate.h"
// #include <QMutexLocker>
// #include <QPainter>
// #include <QStandardPaths>
// #include <QDir>
// #include <QDateTime>
// #include "client_tls.h"



// VideoThread::VideoThread(const QString& url, QLabel* label, Coordinate* coord)
//     : m_url(url), m_label(label), m_coord(coord), m_stop(false) {}

// void VideoThread::stop() {
//     m_stop = true;
// }

// void VideoThread::run() {
//     gst_init(nullptr, nullptr);

//     //Mac에서 가장 좋은 파이프라인
//     QString pipelineStr = QString(
//                               "rtspsrc name = src location=%1 latency=100 !" //인증서 검증 건너뛴다.
//                               "application/x-rtp,media=video,encoding-name=H264 ! "
//                               "rtph264depay ! "
//                               "h264parse ! "
//                               "vtdec ! "             // ← 여기서 하드웨어 디코딩
//                               "videoconvert ! "
//                               "video/x-raw,format=RGB ! "
//                               "appsink name=mysink"  // ← Qt에서 프레임을 가져오기 위한 sink
//                               ).arg(m_url);


//     qDebug() << "[VideoThread] pipeline =" << pipelineStr;

//     GstElement* pipeline = gst_parse_launch(pipelineStr.toStdString().c_str(), nullptr);
//     if (!pipeline)
//     {
//         qDebug() << "[VideoThread] 파이프라인 생성 실패";
//         return;
//     }

//     //복호화과정 (rtspsrc요소 꺼내기)
//     GstElement* src = gst_bin_get_by_name(GST_BIN(pipeline),"src");
//     if(!src){
//         qDebug()<<"[videothread] src가져오기 실패!!!";
//         return;
//     }

//     //인증서 객체를 생성
//     GError *error=nullptr;
//     GTlsCertificate *cert=g_tls_certificate_new_from_files(
//         rtsp_config.rtsp_cert_pem,
//         rtsp_config.rtsp_cert_key,
//         &error);
//     if(!cert)
//     g_object_set (G_OBJECT (src),
//                  "tls-validation-flags", G_TLS_CERTIFICATE_VALIDATE_ALL,
//                  "tls-database",database,
//                  "tls-interaction",interaction,
//                  nullptr);
//     //g_object_set (G_OBJECT (rtspsrc), "tls-validation-flags", G_TLS_CERTIFICATE_INSECURE, NULL);
//     GTlsCertificate *cert;
//     GError *error=NULL;
//     cert = g_tls_certificate_new_from_files(rtsp_config.rtsp_cert_pem,rtsp_config.rtsp_cert_key,&error);
//     //cert = g_tls_certificate_new_from_files("/home/enthusiasticgeek/gstreamer/cert/toyIntermediate.pem","/home/enthusiasticgeek/gstreamer/cert/toyDecryptedIntermediate.key",&error);
//     if (cert == NULL) {
//         g_printerr ("failed to parse PEM: %s\n", error->message);
//         return -1;
//     }
//     GTlsDatabase* database = g_tls_file_database_new (rtsp_config.rtsp_ca_cert, &error);
//     //GTlsDatabase* database = g_tls_file_database_new ("/home/enthusiasticgeek/gstreamer/cert/toyCA.pem", &error);

//     GTlsCertificate *ca_cert;
//     ca_cert = g_tls_certificate_new_from_file(rtsp_config.rtsp_ca_cert,&error);
//     //ca_cert = g_tls_certificate_new_from_file("/home/enthusiasticgeek/gstreamer/cert/toyCA.pem",&error);
//     if (ca_cert == NULL) {
//         g_printerr ("failed to parse CA PEM: %s\n", error->message);
//         return -1;
//     }

//     g_object_set (G_OBJECT (rtspsrc), "tls-database", database, NULL);
//     RtspClientTlsInteraction *interaction =
//         rtsp_client_tls_interaction_new (cert, ca_cert, database);

//     g_object_set (G_OBJECT (rtspsrc), "tls-interaction", interaction, NULL);

//     /* Putting a Message handler */
//     bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
//     gst_bus_add_watch (bus, bus_call, loop);
//     gst_object_unref (bus);

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
// }

