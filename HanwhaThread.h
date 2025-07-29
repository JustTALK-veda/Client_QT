#ifndef HANWHATHREAD_H
#define HANWHATHREAD_H

#include <QThread>
#include <QLabel>
#include <QPixmap>
#include <gst/gst.h>
#include <gst/app/gstappsink.h>

class HanwhaThread : public QThread
{
    Q_OBJECT
public:
    HanwhaThread(const QString& rtspUrl, QObject *parent = nullptr);
    ~HanwhaThread();
    void run() override;
    void stop();

private:
    QString m_rtspUrl;
    bool m_stop;
    QElapsedTimer fpsTimer;   // FPS 측정용 타이머
    int fpsFrameCount = 0;    // 프레임 카운터
signals:
    void frameReady(const QImage&);
};

#endif // HANWHATHREAD_H
