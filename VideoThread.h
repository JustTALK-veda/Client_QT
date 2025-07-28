#ifndef VIDEOTHREAD_H
#define VIDEOTHREAD_H

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

class VideoThread : public QThread {
    Q_OBJECT

signals:
    void cropped(int index,const QPixmap& pixmap); //Index 번째 크롭된 이미지를 전달
    void fullFrame(const QPixmap &pix);//전체 프레임용 시그널
    void highlightIndexChanged(const int index);



public:
    VideoThread(const QString& url, QLabel* label, Coordinate* coord);
    void run() override;
    void stop();
    //void startRecording



private:
    QString m_url;
    QLabel* m_label;
    Coordinate* m_coord;
    bool m_stop;
    QElapsedTimer fpsTimer;   // FPS 측정용 타이머
    int fpsFrameCount = 0;    // 프레임 카운터
    QVector<int>prevX;
    //RtspConfig rtsp_config;
};

#endif // VIDEOTHREAD_H
