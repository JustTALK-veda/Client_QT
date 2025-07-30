#ifndef MEETING_H
#define MEETING_H

#include <QWidget>
#include <QTimer>
#include "CameraWidget.h"
#include "pipwidget.h"

namespace Ui {
class meeting;
}

class meeting : public QWidget
{
    Q_OBJECT

public:
    explicit meeting(QWidget *parent = nullptr, CameraWidget* webcamFrame = nullptr);
    ~meeting();

private:
    Ui::meeting *ui;
    CameraWidget* camerawidget;
    QTimer *timeTimer;
    PipWidget *pip;

signals:
    void gridPageActive();
    void fullPageActive();
    void exitRequested();
private slots:
    void updateTime();
    void updatePeopleCount(int count);
};

#endif // MEETING_H
