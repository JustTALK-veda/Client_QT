#ifndef GRID_H
#define GRID_H

#include <QWidget>
#include "ui_Grid.h"
#include "stackpage.h"
#include "VideoThread.h"
#include "Coordinate.h"
#include "TcpThread.h"
#include "RtspServer.h"
#include "audio_control.h"
#include "CameraWidget.h"

class grid : public QWidget
{
    Q_OBJECT
public:
    explicit grid(QWidget *parent = nullptr);
    ~grid();
protected:
    void resizeEvent(QResizeEvent* event) override;
private:
    Ui::gridForm *ui;
    QLabel* panoLabel = nullptr;
    const int panoMarginTop = 100;
    QDialog *camDialog = nullptr;
    CameraWidget *camerawidget = nullptr;

    //QLabel* pano = nullptr;
    QVector<QLabel*> labels;
    const int perPage=4;

    VideoThread *videoThread;
    TcpThread   *tcpThread;
    Coordinate  *coord;
    // QTimer      *webcamTimer;
    // bool         audioEnabled;
    // QWidget *container;

    void setupPages();
    bool loadConfigFromJson(QString &ip,int &rtspPort,int &tcpPort);

signals:

private slots:
    //void onCropped(int index, const QPixmap &pix);
    void updatePano(const QPixmap &pix);
};

#endif // GRID_H
