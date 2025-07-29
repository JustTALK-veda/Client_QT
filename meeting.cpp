#include "meeting.h"
#include "grid.h"
#include "full.h"
#include "ui_meeting.h"
#include <QButtonGroup>
#include <QDialog>

meeting::meeting(QWidget *parent, CameraWidget* webcamFrame)
    : QWidget(parent)
    , cameraWidget(webcamFrame)
    , ui(new Ui::meeting)
{
    ui->setupUi(this);
    grid* gridPage = new grid(this);
    Full* fullPage = new Full(this);
    ui->stackedWidget->addWidget(gridPage);
    ui->stackedWidget->addWidget(fullPage);

    // footbar
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

    // 동적으로 변경 연결 필요
    ui->peopleCountIconLabel->setText("  4");

    // 페이지 전환
    connect(ui->gridButton, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentWidget(gridPage);
        emit gridPageActive();

    });

    connect(ui->fullButton, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentWidget(fullPage);
        emit fullPageActive();
    });

    // 웹캠 다이얼로그 추가 // meeting에서 진행할 예정
    camDialog = new QDialog(this, Qt::Window);
    camDialog->setWindowTitle("나");
    camDialog->resize(240, 180);
    QVBoxLayout* layout = new QVBoxLayout(camDialog);
    layout->setContentsMargins(0, 0, 0, 0);  // (선택) 여백 제거
    layout->addWidget(cameraWidget);        // cameraWidget 추가
    camDialog->show();
    std::thread(start_rtsp_server).detach();

    ui->camButton->setChecked(cameraWidget->isCamEnabled() ? true : false);
    ui->camButton->setIcon(QIcon(cameraWidget->isCamEnabled() ? ":/Image/config/video_on.png" : ":/Image/config/video_off.png"));
    ui->micButton->setChecked(cameraWidget->isMicEnabled() ? true : false);
    ui->micButton->setIcon(QIcon(cameraWidget->isMicEnabled() ? ":/Image/config/mic_on.png" : ":/Image/config/mic_off.png"));

    // 카메라, 마이크 컨트롤
    connect(ui->camButton, &QPushButton::toggled, this, [=](bool checked) {
        ui->camButton->setIcon(QIcon(checked ? ":/Image/config/video_on.png" : ":/Image/config/video_off.png"));
        cameraWidget->setCamEnabled(checked);
    });

    connect(ui->micButton, &QPushButton::toggled, this, [=](bool checked) {
        ui->micButton->setIcon(QIcon(checked ? ":/Image/config/mic_on.png" : ":/Image/config/mic_off.png"));
        cameraWidget->setMicEnabled(checked);
    });

    connect(ui->exitButton, &QPushButton::clicked, this, [=]() {
        emit exitRequested();
    });
}

meeting::~meeting()
{
    delete ui;
}
