#ifndef VIDEOTHREAD_H
#define VIDEOTHREAD_H

#include <QThread>
#include <QLabel>
#include <QPixmap>
#include "Coordinate.h"
#include <gst/gst.h>
#include <gst/app/gstappsink.h>

class VideoThread : public QThread {
    Q_OBJECT

signals:
    void cropped(int index,const QPixmap& pixmap); //Index 번째 크롭된 이미지를 전달

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
