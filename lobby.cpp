#include "lobby.h"
#include "ui_lobby.h"
#include <QSize>

Lobby::Lobby(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Lobby)
{
    ui->setupUi(this);
    connect(ui->enterButton, &QPushButton::clicked, this, &Lobby::enterMeetingRequested);
}

Lobby::~Lobby()
{
    delete ui;
}
