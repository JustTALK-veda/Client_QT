#include "mypage.h"
#include "lobby.h"
#include "MainWindow.h"
#include <QVBoxLayout>
#include <QStackedWidget>

mypage::mypage(QWidget *parent)
    : QWidget(parent)
    , stack(new QStackedWidget(this))
{
    //this->setWindowState(Qt::WindowFullScreen);

    // 전체 레이아웃
    auto *layout = new QVBoxLayout(this);
    layout->addWidget(stack);          // 스택 위젯 추가
    setLayout(layout);

    Lobby *lobbyPage= new Lobby(this);
    MainWindow *mainWindowPage=new MainWindow(this);

    stack->addWidget(lobbyPage);
    stack->addWidget(mainWindowPage);

    connect(lobbyPage, &Lobby::accepted, this, [=]() {
        stack->setCurrentIndex(1);
    });
}
