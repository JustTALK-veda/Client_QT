#ifndef MEETING_H
#define MEETING_H

#include <QWidget>
#include "ui_Meeting.h"
#include "VideoThread.h"
#include "Coordinate.h"
#include "TcpThread.h"
#include "CameraWidget.h"
#include "RtspServer.h"
#include "audio_control.h"

class meeting : public QWidget
{
    Q_OBJECT
public:
    explicit meeting(QWidget *parent = nullptr);
    ~meeting();

private:
    Ui::Form *ui;
    QDialog *camDialog = nullptr;
    CameraWidget *camerawidget = nullptr;

    //QLabel* pano = nullptr;
    QVector<QLabel*> labels;
    const int perPage=4;

    VideoThread *videoThread;
    TcpThread   *tcpThread;
    Coordinate  *coord;
    // QTimer      *webcamTimer;
    // bool         audioEnabled;
    // QWidget *container;

    bool micEnabled = false;
    bool camEnabled = false;

    void setupPages();
    bool loadConfigFromJson(QString &ip,int &rtspPort,int &tcpPort);

signals:

private slots:
    void onCropped(int index, const QPixmap &pix);
    void updatePano(const QPixmap &pix);
    void onMicButtonClicked();
    void onCamButtonClicked();
};

#endif // MEETING_H
