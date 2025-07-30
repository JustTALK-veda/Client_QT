#include "start.h"
#include "ui_start.h"
#include <QToolButton>
#include <QIcon>
#include <QSizePolicy>
#include <QDateTime>

Start::Start(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Start)
{
    ui->setupUi(this);
    setStyleSheet(
        "QLabel { color: white; }"
        "QFrame#headerFrame { "
        "background-color: #1E2939; "
        "border-bottom: 1px solid #304159; "
        "}"
        );

    timeTimer = new QTimer(this);
    connect(timeTimer, &QTimer::timeout, this, &Start::updateTime);
    timeTimer->start(1000);
    updateTime();

    // ───────── 회의실 버튼 ─────────
    ui->localUserButton->setIcon(QIcon(":/Image/config/local_user.png"));
    ui->localUserButton->setIconSize(QSize(350, 350));
    ui->localUserButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);


    ui->localUserButton->setText("회의실\n새로운 회의를 시작하세요");
    ui->localUserButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->localUserButton->setStyleSheet(R"(
        QToolButton {
            background-color: #101828;
            color: white;
            font-size: 20px;
            font-weight: bold;
            padding: 20px;
            border: none;
        }
        QToolButton:hover {
            background-color: #F47920;
        }
    )");

    // ───────── 원격 참여자 버튼 ─────────
    ui->remoteUserButton->setIcon(QIcon(":/Image/config/remote_user.png"));
    ui->remoteUserButton->setIconSize(QSize(350, 350));
    ui->remoteUserButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->remoteUserButton->setText("원격 참여자\n기존 회의에 참가하세요");
    ui->remoteUserButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->remoteUserButton->setStyleSheet(R"(
        QToolButton {
            background-color: #101828;
            color: white;
            font-size: 20px;
            font-weight: bold;
            padding: 20px;
            border: none;
        }
        QToolButton:hover {
            background-color: #F47920;
        }
    )");

    // ───────── 시그널 연결 ─────────
    connect(ui->localUserButton, &QToolButton::clicked, this, &Start::enterLocalPageRequested);
    connect(ui->remoteUserButton, &QToolButton::clicked, this, &Start::enterRemotePageRequested);
}

Start::~Start()
{
    delete ui;
}

void Start::updateTime()
{
    QString currentTime = QDateTime::currentDateTime().toString("  hh:mm  ");
    ui->timeLabel->setText(currentTime);
}
