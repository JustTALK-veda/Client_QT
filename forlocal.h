#ifndef FORLOCAL_H
#define FORLOCAL_H

#include <QWidget>
#include <QTimer>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QImage>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "VideoThreadWebcam.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ForLocal; }
QT_END_NAMESPACE

class ForLocal : public QWidget
{
    Q_OBJECT

public:
    ForLocal(QWidget *parent = nullptr, const QString &roomCode = "");
    ~ForLocal();

signals:
    void leaveMeetingRequested();
    void goBackRequested();

private slots:
    // void onEndCallClicked();
    // void onMuteToggleClicked();
    // void onCameraToggleClicked();
    void updateTime();
    // void onConnected();
    // void showParticipantVideo(QPixmap&);

private:
    //void setupConnections();
    //void updateControlButtons();
    void showWaitingState();
    void checkWebcamServer();
    void startWebcamThread();
    void onWebcamReady(bool success);

    Ui::ForLocal *ui;
    QTimer *timeTimer;
    // QTimer *participantTimer;
    QImage waitingImg;
    QTimer* webcamRetryTimer = nullptr;
    bool isCheckingServer = false;
    VideoThreadWebcam* webcamCheckThread = nullptr;
    VideoThreadWebcam* webcamStreamThread = nullptr;

    // State variables
    bool isMuted;
    bool isCameraEnabled;
    bool hasParticipants;
    bool isGridLayout;
    QString roomCode;
    QString hostName;
    int participantCount;
    QString webcamUrl;
};

#endif // FORLOCAL_H
