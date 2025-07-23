#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "VideoThread.h"
#include "ui_MainWindow.h"
#include "Coordinate.h"
#include "TcpThread.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow ui;
    VideoThread *videoThread;
    TcpThread   *tcpThread;
    Coordinate  *coord;
    QTimer      *webcamTimer;
    bool         audioEnabled;
    QVector<QLabel*> labels;
    QWidget *container;
    QLabel* pano;
    const int perPage=4;


    void setupPages();
    void updateNavButtons(int idx);
    bool loadConfigFromJson(QString &ip,int &rtspPort,int &tcpPort);

private slots:
    void showPrevPage();
    void showNextPage();
    void onCropped(int index, const QPixmap &pix);
    void updatePano(const QPixmap &pix);


};
#endif // MAINWINDOW_H
