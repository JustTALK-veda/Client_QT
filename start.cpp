#include "start.h"
#include "ui_start.h"

Start::Start(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Start)
{
    ui->setupUi(this);
    connect(ui->remoteUserButton, &QPushButton::clicked, this, &Start::enterRemotePageRequested);
}

Start::~Start()
{
    delete ui;
}
