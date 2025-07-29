#ifndef MEETING_H
#define MEETING_H

#include <QWidget>
#include "CameraWidget.h"

namespace Ui {
class meeting;
}

class meeting : public QWidget
{
    Q_OBJECT

public:
    explicit meeting(QWidget *parent = nullptr, CameraWidget* webcamFrame = nullptr);
    ~meeting();
    QDialog *camDialog = nullptr;

private:
    Ui::meeting *ui;
    CameraWidget *cameraWidget;

signals:
    void gridPageActive();
    void fullPageActive();
    void exitRequested();
};

#endif // MEETING_H
