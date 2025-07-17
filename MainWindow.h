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
#include <QScrollArea>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QVector<QLabel*> labels;
    QGridLayout *layout;
    VideoThread* videoThread;
    TcpThread* tcpThread;
    Coordinate* coord;
    QScrollArea *scroll;
    QWidget *container;
    QLabel* panoLabel;               //파노라마용 QLabel
    // JSON 설정 읽기 함수
    bool loadConfigFromJson(QString &ip, int &rtspPort, int &tcpPort);

private slots:
    void onCropped(int index,const QPixmap &pix);
    void updatePano(const QPixmap &pix);
};

#endif // MAINWINDOW_H
