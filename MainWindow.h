#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include "VideoThread.h"
#include "TcpThread.h"
#include "Coordinate.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QLabel* label;
    VideoThread* videoThread;
    TcpThread* tcpThread;
    Coordinate* coord;
};

#endif // MAINWINDOW_H
