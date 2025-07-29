#ifndef LOBBY_H
#define LOBBY_H
#include "CameraWidget.h"
#include <QWidget>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class Lobby; }
QT_END_NAMESPACE

class CameraWidget;

class Lobby : public QWidget
{
    Q_OBJECT

public:
    Lobby(QWidget *parent = nullptr);
    ~Lobby();

signals:
    void enterMeetingRequested();
    void goBackRequested();

private slots:
    void setVideoEnabled(bool enabled);
    void setAudioEnabled(bool enabled);
    void updateTime();
    void handleJoinMeeting();
    void toggleMeetingStatus();
    void showSettings();

private:
    void updateVideoPreview();
    void updateMeetingStatus();
    void updateJoinButton();

    Ui::Lobby *ui;
    QTimer *timeTimer;
    CameraWidget *cameraWidget = nullptr;
    
    bool videoEnabled;
    bool audioEnabled;
    bool isConnecting;
    bool meetingInProgress;
};

#endif // LOBBY_H
