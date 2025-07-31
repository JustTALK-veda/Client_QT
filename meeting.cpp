#include "meeting.h"
#include "grid.h"
#include "full.h"
#include "ui_meeting.h"
#include <QButtonGroup>
#include <QDialog>
#include <QDateTime>

meeting::meeting(QWidget *parent, CameraWidget* webcamFrame)
    : QWidget(parent), camerawidget(webcamFrame)
    , ui(new Ui::meeting)
{
    ui->setupUi(this);
    grid* gridPage = new grid(this);
    Full* fullPage = new Full(this);
    ui->stackedWidget->addWidget(gridPage);
    ui->stackedWidget->addWidget(fullPage);

    // footbar
    // Setup timer for clock
    timeTimer = new QTimer(this);
    connect(timeTimer, &QTimer::timeout, this, &meeting::updateTime);
    timeTimer->start(1000);
    updateTime();

    QButtonGroup* viewModeGroup = new QButtonGroup(this);
    viewModeGroup->addButton(ui->gridButton);
    viewModeGroup->addButton(ui->fullButton);
    viewModeGroup->setExclusive(true);

    ui->gridButton->setChecked(true);
    ui->stackedWidget->setCurrentWidget(gridPage);
    ui->gridButton->setIcon(QIcon(":/Image/config/grid_white.png"));
    ui->fullButton->setIcon(QIcon(":/Image/config/full_black.png"));

    connect(ui->gridButton, &QPushButton::toggled, this, [=](bool checked) {
        ui->gridButton->setIcon(QIcon(checked ? ":/Image/config/grid_white.png" : ":/Image/config/grid_black.png"));
    });
    connect(ui->fullButton, &QPushButton::toggled, this, [=](bool checked) {
        ui->fullButton->setIcon(QIcon(checked ? ":/Image/config/full_white.png" : ":/Image/config/full_black.png"));
    });

    connect(this, &meeting::gridPageActive, gridPage, &grid::onGridPageActive);
    connect(this, &meeting::fullPageActive, fullPage, &Full::onFullPageActive);

    // 페이지 전환
    connect(ui->gridButton, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentWidget(gridPage);
        emit gridPageActive();

    });

    connect(ui->fullButton, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentWidget(fullPage);
        emit fullPageActive();
    });

    connect(gridPage->videoThread, &VideoThread::peoplecount, this, &meeting::updatePeopleCount);

    // 웹캠 
    pip = new PipWidget(this, camerawidget);
    pip->show();
    
    std::thread(start_rtsp_server).detach();

    ui->camButton->setChecked(pip->webcam->isCamEnabled() ? true : false);
    ui->camButton->setIcon(QIcon(pip->webcam->isCamEnabled() ? ":/Image/config/video_on.png" : ":/Image/config/video_off.png"));
    ui->micButton->setChecked(pip->webcam->isMicEnabled() ? true : false);
    ui->micButton->setIcon(QIcon(pip->webcam->isMicEnabled() ? ":/Image/config/mic_on.png" : ":/Image/config/mic_off.png"));

    // 카메라, 마이크 컨트롤
    connect(ui->camButton, &QPushButton::toggled, this, [=](bool checked) {
        ui->camButton->setIcon(QIcon(checked ? ":/Image/config/video_on.png" : ":/Image/config/video_off.png"));
        pip->webcam->setCamEnabled(checked);
    });

    connect(ui->micButton, &QPushButton::toggled, this, [=](bool checked) {
        ui->micButton->setIcon(QIcon(checked ? ":/Image/config/mic_on.png" : ":/Image/config/mic_off.png"));
        pip->webcam->setMicEnabled(checked);
    });

    connect(ui->exitButton, &QPushButton::clicked, this, [=]() {
        emit exitRequested();
    });
}

void meeting::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    // 여기서 PipWidget 위치 조정
    QSize parentSize = this->size();
    QSize pipSize = pip->size();

    int x = parentSize.width() - pipSize.width() + 10;
    int y = parentSize.height() - pipSize.height() - 10;
    pip->move(x, y);
}

void meeting::updateTime(){
    QString currentTime = QDateTime::currentDateTime().toString("AP hh:mm");
    ui->currentTimeLabel->setText(currentTime);
}

void meeting::updatePeopleCount(int count)
{
    ui->peopleCountIconLabel->setText(QString("  %1").arg(count));
    qDebug()<<"count update";
}

meeting::~meeting()
{
    delete ui;
}
