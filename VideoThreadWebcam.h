#ifndef VIDEOTHREADWEBCAM_H
#define VIDEOTHREADWEBCAM_H

#include <QThread>
#include <QLabel>
#include <QPixmap>
#include "Coordinate.h"
#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <glib.h>
#include <QElapsedTimer>
#include <opencv2/opencv.hpp>
//#include "client_tls.h"

class VideoThreadWebcam : public QThread {
    Q_OBJECT

signals:
    void connected();
    void disconnected();

public:
    VideoThreadWebcam(const QString& url, QLabel* label);
    void run() override;
    void stop();

private:
    QString m_url;
    bool m_stop;
    QLabel* m_label;
    QElapsedTimer fpsTimer;   // FPS 측정용 타이머
    int fpsFrameCount = 0;    // 프레임 카운터
    int consecutiveFailures = 0;
};

#endif // VIDEOTHREADWEBCAM_H
