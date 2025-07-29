#ifndef FORLOCAL_H
#define FORLOCAL_H

#include <QWidget>
#include <QTimer>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
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
    void onEndCallClicked();
    void onMuteToggleClicked();
    void onCameraToggleClicked();
    void updateTime();
    // void onConnected();
    // void showParticipantVideo(QPixmap&);

private:
    void setupConnections();
    void updateControlButtons();
    void showWaitingState();

    Ui::ForLocal *ui;
    QTimer *timeTimer;
    // QTimer *participantTimer;
    VideoThreadWebcam *webcamThread;

    // State variables
    bool isMuted;
    bool isCameraEnabled;
    bool hasParticipants;
    bool isGridLayout;
    QString roomCode;
    QString hostName;
    int participantCount;
};

#endif // FORLOCAL_H
