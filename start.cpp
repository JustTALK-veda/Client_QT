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

// #include "start.h"
// #include <QLabel>
// #include <QVBoxLayout>
// #include <QHBoxLayout>
// #include <QMouseEvent>
// #include <QCursor>

// // CustomButtonWidget 정의
// class CustomButtonWidget : public QWidget {
//     Q_OBJECT
// public:
//     CustomButtonWidget(const QString& iconPath,
//                        const QString& titleText,
//                        const QString& subtitleText,
//                        QWidget* parent = nullptr)
//         : QWidget(parent)
//     {
//         QVBoxLayout* layout = new QVBoxLayout(this);
//         layout->setContentsMargins(0, 0, 0, 0);
//         layout->setSpacing(10);
//         setCursor(Qt::PointingHandCursor);

//         QLabel* iconLabel = new QLabel;
//         iconLabel->setPixmap(QPixmap(iconPath).scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation));
//         iconLabel->setAlignment(Qt::AlignCenter);

//         QLabel* textLabel = new QLabel;
//         textLabel->setText(QString(R"(
//             <div align='center'>
//                 <span style='font-size:20pt; font-weight:bold; color:white;'>%1</span><br>
//                 <span style='font-size:11pt; color:white;'>%2</span>
//             </div>
//         )").arg(titleText, subtitleText));
//         textLabel->setAlignment(Qt::AlignCenter);
//         textLabel->setStyleSheet("background-color: transparent;");

//         layout->addWidget(iconLabel);
//         layout->addWidget(textLabel);
//         layout->setAlignment(Qt::AlignCenter);

//         setStyleSheet("background-color: #101828;");
//     }

// signals:
//     void clicked();

// protected:
//     void mousePressEvent(QMouseEvent* event) override {
//         emit clicked();
//         QWidget::mousePressEvent(event);
//     }
// };

// // Start 클래스 생성자 구현
// Start::Start(QWidget *parent)
//     : QWidget(parent)
// {
//     QHBoxLayout* layout = new QHBoxLayout(this);
//     layout->setContentsMargins(0, 0, 0, 0);
//     layout->setSpacing(20);
//     layout->setStretch(0, 1);
//     layout->setStretch(1, 1);

//     // 전체 배경
//     this->setStyleSheet("background-color: #101828;");

//     // 회의실 버튼
//     auto* localButton = new CustomButtonWidget(
//         ":/Image/config/local_user.png",
//         "회의실",
//         "새로운 회의를 시작하세요",
//         this);
//     connect(localButton, &CustomButtonWidget::clicked, this, &Start::enterLocalPageRequested);

//     // 원격 참여자 버튼
//     auto* remoteButton = new CustomButtonWidget(
//         ":/Image/config/remote_user.png",
//         "원격 참여자",
//         "기존 회의에 참가하세요",
//         this);
//     connect(remoteButton, &CustomButtonWidget::clicked, this, &Start::enterRemotePageRequested);

//     // 레이아웃에 추가
//     layout->addWidget(localButton);
//     layout->addWidget(remoteButton);
// }

// Start::~Start()
// {
//     // nothing to delete
// }
