#include "videothread.h"
#include <QDebug>

VideoThread::VideoThread(const QString &url, VideoWidget *widget)
    : m_url(url), m_widget(widget), m_stop(false) {}

void VideoThread::stop() {
    m_stop = true;
}

void VideoThread::run() {
    qDebug() << "[Thread] 파이프라인 생성 시작";
    GstElement *pipeline = gst_parse_launch(
        //RTP패킷(H.264)->depayload->H.264스트림->파싱->디코딩->영상 출력
        QString("rtspsrc location=%1 latency=100 ! rtph264depay ! h264parse ! d3d11h264dec ! videoconvert ! glimagesink name=mysink")//rtph264depay !
            .arg(m_url).toStdString().c_str(), nullptr);

    if (!pipeline) {
        qDebug() << "[Thread] 파이프라인 생성 실패";
        return;
    }
    qDebug() << "[Thread] 파이프라인 생성 성공";
    ;
    GstElement *sink = gst_bin_get_by_name(GST_BIN(pipeline), "mysink");
    qDebug() << "[Thread] mysink 요소 검색 시도";

    if (sink && GST_IS_VIDEO_OVERLAY(sink)) {
        qDebug() << "[Thread] mysink 찾음!";
        qDebug() << "[Thread] VideoWidget 핸들값:" << m_widget->getVideoWinId();
        gst_video_overlay_set_window_handle(GST_VIDEO_OVERLAY(sink), (guintptr)m_widget->getVideoWinId());
        qDebug() << "[Thread] videooverlay sink에 핸들 연결 시도 중";
    }

    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    qDebug() << "[Thread] 파이프라인 재생 상태로 전환 시도";
    qDebug() << "[Thread] 파이프라인 실행 성공";

    while (!m_stop) {
        QThread::msleep(100);
    }

    qDebug() << "[Thread] 스레드 종료 요청됨, 파이프라인 중지";
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    qDebug() << "[Thread] 파이프라인 해제 완료";
}

