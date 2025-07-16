#include "lobby.h"

Lobby::Lobby(QWidget *parent)
    : QWidget(parent)
{
    // 1) 최상위 레이아웃
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(5);

    // 2) 버튼 바 (오른쪽에 “수락하기”)
    auto *btnLayout = new QHBoxLayout;
    btnLayout->addStretch();
    QPushButton *acceptBtn = new QPushButton("수락하기", this);
    connect(acceptBtn, &QPushButton::clicked, this, &Lobby::accepted);
    btnLayout->addWidget(acceptBtn);
    mainLayout->addLayout(btnLayout);

    // 3) 스크롤 + 컨테이너
    auto *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    QWidget *container = new QWidget;
    scroll->setWidget(container);
    mainLayout->addWidget(scroll);

    // 4) 그리드 레이아웃
    gridLayout = new QGridLayout(container);
    gridLayout->setContentsMargins(5,5,5,5);
    gridLayout->setHorizontalSpacing(10);
    gridLayout->setVerticalSpacing(10);

    // 2×2 placeholder 레이블
    const int rows = 2, cols = 2;
    for(int i = 0; i < rows; ++i) {
        for(int j = 0; j < cols; ++j) {
            QLabel *lbl = new QLabel(container);
            lbl->setFixedSize(480, 360);
            lbl->setAlignment(Qt::AlignCenter);
            lbl->setStyleSheet("background-color: rgba(169,169,169,0.5);");
            gridLayout->addWidget(lbl, i, j);
            labels.append(lbl);
        }
    }

    // 5) 윈도우 크기 조절
    int w = cols * 480 + 40;
    int h = acceptBtn->sizeHint().height() + rows * 360 + 80;
    resize(w, h);
}
