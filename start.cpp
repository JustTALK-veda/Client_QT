// #include "start.h"
// #include "ui_start.h"

// Start::Start(QWidget *parent)
//     : QWidget(parent)
//     , ui(new Ui::Start)
// {
//     ui->setupUi(this);
//     connect(ui->remoteUserButton, &QPushButton::clicked, this, &Start::enterRemotePageRequested);
//     connect(ui->localUserButton, &QPushButton::clicked, this, &Start::enterLocalPageRequested);
// }

// Start::~Start()
// {
//     delete ui;
// }

// #include "start.h"
// #include "ui_start.h"
// #include <QToolButton>
// #include <QIcon>

// Start::Start(QWidget *parent)
//     : QWidget(parent)
//     , ui(new Ui::Start)
// {
//     ui->setupUi(this);

//     // 전체 배경 색상 설정
//     this->setStyleSheet("background-color: #101828;");

//     // 좌우 버튼 균등 배치
//     ui->horizontalLayout->setContentsMargins(0, 0, 0, 0);
//     ui->horizontalLayout->setSpacing(20);  // 버튼 사이 여백
//     ui->horizontalLayout->setStretch(0, 1);
//     ui->horizontalLayout->setStretch(1, 1);

//     // ── 회의실 버튼 (왼쪽)
//     ui->localUserButton->setIcon(QIcon(":/Image/config/local_user.png"));
//     ui->localUserButton->setIconSize(QSize(120, 120));
//     ui->localUserButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
//     ui->localUserButton->setText("회의실\n새로운 회의를 시작하세요");
//     ui->localUserButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
//     ui->localUserButton->setStyleSheet(R"(
//         QToolButton {
//             background-color: #101828;
//             color: white;
//             font-size: 18px;
//             font-weight: bold;
//             padding: 20px;
//             border: none;
//         }
//         QToolButton:hover {
//             background-color: #F47920;
//         }
//     )");

//     // ── 원격 참여자 버튼 (오른쪽)
//     ui->remoteUserButton->setIcon(QIcon(":/Image/config/remote_user.png"));
//     ui->remoteUserButton->setIconSize(QSize(120, 120));
//     ui->remoteUserButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
//     ui->remoteUserButton->setText("원격 참여자\n기존 회의에 참가하세요");
//     ui->remoteUserButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
//     ui->remoteUserButton->setStyleSheet(R"(
//         QToolButton {
//             background-color: #101828;
//             color: white;
//             font-size: 18px;
//             font-weight: bold;
//             padding: 20px;
//             border: none;
//         }
//         QToolButton:hover {
//             background-color: #F47920;
//         }
//     )");

//     // ── 버튼 클릭 signal 연결
//     connect(ui->localUserButton, &QToolButton::clicked, this, &Start::enterLocalPageRequested);
//     connect(ui->remoteUserButton, &QToolButton::clicked, this, &Start::enterRemotePageRequested);
// }

// Start::~Start()
// {
//     delete ui;
// }

#include "start.h"
#include "ui_start.h"
#include <QToolButton>
#include <QIcon>
#include <QSizePolicy>

Start::Start(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Start)
{
    ui->setupUi(this);

    // 전체 배경 설정
    this->setStyleSheet("background-color: #101828;");

    // 레이아웃 마진 및 간격 조정
    ui->horizontalLayout->setContentsMargins(0, 0, 0, 0);
    ui->horizontalLayout->setSpacing(0);
    ui->horizontalLayout->setStretch(0, 1);
    ui->horizontalLayout->setStretch(1, 1);

    // ───────── 회의실 버튼 ─────────
    ui->localUserButton->setIcon(QIcon(":/Image/config/local_user.png"));
    ui->localUserButton->setIconSize(QSize(120, 120));
    ui->localUserButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->localUserButton->setText("회의실\n새로운 회의를 시작하세요");
    ui->localUserButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->localUserButton->setStyleSheet(R"(
        QToolButton {
            background-color: #101828;
            color: white;
            font-size: 18px;
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
    ui->remoteUserButton->setIconSize(QSize(120, 120));
    ui->remoteUserButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->remoteUserButton->setText("원격 참여자\n기존 회의에 참가하세요");
    ui->remoteUserButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->remoteUserButton->setStyleSheet(R"(
        QToolButton {
            background-color: #101828;
            color: white;
            font-size: 18px;
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
