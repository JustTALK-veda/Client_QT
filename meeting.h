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
    explicit meeting(QWidget *parent = nullptr);
    ~meeting();
    QDialog *camDialog = nullptr;
    CameraWidget *camerawidget = nullptr;

private:
    Ui::meeting *ui;

signals:
    void gridPageActive();
    void fullPageActive();
    void exitRequested();
};

#endif // MEETING_H
