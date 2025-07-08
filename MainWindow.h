#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QVector>
#include <QPixmap>
#include "VideoThread.h"
#include "TcpThread.h"
#include "Coordinate.h"
#include <QHBoxLayout>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QVector<QLabel*> labels;
    QHBoxLayout *layout;
    VideoThread* videoThread;
    TcpThread* tcpThread;
    Coordinate* coord;

private slots:
    void onCropped(int index,const QPixmap &pix);

};

#endif // MAINWINDOW_H
