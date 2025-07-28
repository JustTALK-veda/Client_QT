#include "lobby.h"
#include "ui_lobby.h"
#include <QSize>

Lobby::Lobby(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Lobby)
{
    ui->setupUi(this);
    QPixmap vediooffPixmap(":/Image/config/video_off.png");
    QPixmap vedioonPixmap(":/Image/config/video_on.png");
    QPixmap micoffPixmap(":/Image/config/mic_off.png");
    QPixmap miconPixmap(":/Image/config/mic_on.png");
    ui->cam_icon_label->setPixmap(vediooffPixmap.scaled(
        ui->cam_icon_label->size()
        ));
    ui->mic_icon_label->setPixmap(micoffPixmap.scaled(
        ui->mic_icon_label->size(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        ));
    // connect(ui->cam_checkBox, &QCheckBox::toggled, ui->cameraWidget, &CameraWidget::setCamEnabled);
    // connect(ui->cam_checkBox, &QCheckBox::toggled, ui->cameraWidget, &CameraWidget::setMicEnabled);
    connect(ui->enterButton, &QPushButton::clicked, this, &Lobby::enterMeetingRequested);
}

Lobby::~Lobby()
{
    delete ui;
}
