#ifndef VIDEOTHREAD_H
#define VIDEOTHREAD_H

#include <QThread>
#include <QLabel>
#include "Coordinate.h"
#include <gst/gst.h>
#include <gst/app/gstappsink.h>

class VideoThread : public QThread {
    Q_OBJECT
public:
    VideoThread(const QString& url, QLabel* label, Coordinate* coord);
    void run() override;
    void stop();

private:
    QString m_url;
    QLabel* m_label;
    Coordinate* m_coord;
    bool m_stop;
};

#endif // VIDEOTHREAD_H
