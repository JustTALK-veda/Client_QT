#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include "CameraWidget.h"
#include "meeting.h"
#include "start.h"
#include "lobby.h"
#include "forlocal.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QStackedWidget *stackedWidget;
    Lobby* lobbyPage = nullptr;
    meeting* meetingPage = nullptr;
    ForLocal* forLocalPage = nullptr;
    CameraWidget* webcamFrame = nullptr;

private slots:


};
#endif // MAINWINDOW_H
