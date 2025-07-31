#ifndef LOBBY_H
#define LOBBY_H
#include "CameraWidget.h"
#include "VideoThread.h"
#include "Coordinate.h"
#include <QWidget>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class Lobby; }
QT_END_NAMESPACE

class Lobby : public QWidget
{
    Q_OBJECT

public:
    Lobby(QWidget *parent = nullptr, CameraWidget* webcamFrame = nullptr);
    ~Lobby();

signals:
    void enterMeetingRequested();
    void goBackRequested();

private slots:
    void updateTime();
    void handleJoinMeeting();
    void onServerReady(bool success);

private:
    void updateVideoPreview();
    void updateMeetingStatus();
    void updateJoinButton();
    void checkRtspServer();

    Ui::Lobby *ui;
    QTimer *timeTimer;
    QTimer *serverCheckTimer;
    CameraWidget *cameraWidget;
    VideoThread *videoThread;

    bool isCheckingServer = false;
    bool isConnecting;
    bool meetingInProgress;
};

#endif // LOBBY_H
