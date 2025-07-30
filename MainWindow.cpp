#include "MainWindow.h"
#include <QSizePolicy>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // 중앙 위젯 설정
    QWidget* central = new QWidget(this);
    central->setStyleSheet("background-color: #101828;");
    this->setCentralWidget(central);

    // 전체 레이아웃
    QVBoxLayout* mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // QStackedWidget 생성 및 추가
    stackedWidget = new QStackedWidget(this);
    stackedWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainLayout->addWidget(stackedWidget);
    
    // 페이지 생성
    Start* startPage = new Start(this);

    // 페이지 등록
    stackedWidget->addWidget(startPage);

    // 초기 페이지 설정
    stackedWidget->setCurrentWidget(startPage);

    // 페이지 전환 연결
    connect(startPage, &Start::enterRemotePageRequested, this, [=]() {
        if (!lobbyPage) {
            // 공유 카메라 위젯 생성
            webcamFrame = new CameraWidget(this);
            lobbyPage = new Lobby(this, webcamFrame);
            stackedWidget->addWidget(lobbyPage);

            connect(lobbyPage, &Lobby::enterMeetingRequested, this, [=]() {
                if (!meetingPage) {
                    meetingPage = new meeting(this, webcamFrame);
                    stackedWidget->addWidget(meetingPage);
                    // 통화 종료
                    connect(meetingPage, &meeting::exitRequested, this, &MainWindow::close);
                }
                stackedWidget->setCurrentWidget(meetingPage);
                emit meetingPage->gridPageActive();
            });
        }
        stackedWidget->setCurrentWidget(lobbyPage);
    });

    connect(startPage, &Start::enterLocalPageRequested, this, [=]() {
        if (!lobbyPage) {
            forLocalPage = new ForLocal(this, "Room1");
            stackedWidget->addWidget(forLocalPage);
        }
        stackedWidget->setCurrentWidget(forLocalPage);
    });    
}
MainWindow::~MainWindow(){}


