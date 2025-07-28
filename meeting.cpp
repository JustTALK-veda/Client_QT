#include "meeting.h"
#include "grid.h"
#include "ui_meeting.h"

meeting::meeting(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::meeting)
{
    ui->setupUi(this);
    grid* gridPage = new grid(this);

    // 페이지 등록
    ui->stackedWidget->addWidget(gridPage);
}

meeting::~meeting()
{
    delete ui;
}
