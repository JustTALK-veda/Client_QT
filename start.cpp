#include "start.h"
#include "ui_start.h"

Start::Start(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Start)
{
    ui->setupUi(this);
    connect(ui->remoteUserButton, &QPushButton::clicked, this, &Start::enterRemotePageRequested);
    connect(ui->localUserButton, &QPushButton::clicked, this, &Start::enterLocalPageRequested);
}

Start::~Start()
{
    delete ui;
}
