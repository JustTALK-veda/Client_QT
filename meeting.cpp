#include "meeting.h"
#include "grid.h"
#include "full.h"
#include "ui_meeting.h"
#include <QButtonGroup>

meeting::meeting(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::meeting)
{
    ui->setupUi(this);
    grid* gridPage = new grid(this);
    Full* fullPage = new Full(this);
    ui->stackedWidget->addWidget(gridPage);
    ui->stackedWidget->addWidget(fullPage);

    // footbar
    QButtonGroup* viewModeGroup = new QButtonGroup(this);
    viewModeGroup->addButton(ui->gridButton);
    viewModeGroup->addButton(ui->fullButton);
    viewModeGroup->setExclusive(true);

    ui->gridButton->setChecked(true);
    ui->stackedWidget->setCurrentWidget(gridPage);
    ui->gridButton->setIcon(QIcon(":/Image/config/grid_white.png"));
    ui->fullButton->setIcon(QIcon(":/Image/config/full_black.png"));

    connect(ui->gridButton, &QPushButton::toggled, this, [=](bool checked) {
        if (checked)
            ui->gridButton->setIcon(QIcon(":/Image/config/grid_white.png"));
        else
            ui->gridButton->setIcon(QIcon(":/Image/config/grid_black.png"));
    });
    connect(ui->fullButton, &QPushButton::toggled, this, [=](bool checked) {
        if (checked)
            ui->fullButton->setIcon(QIcon(":/Image/config/full_white.png"));
        else
            ui->fullButton->setIcon(QIcon(":/Image/config/full_black.png"));
    });

    connect(this, &meeting::gridPageActive, gridPage, &grid::onGridPageActive);
    connect(this, &meeting::fullPageActive, fullPage, &Full::onFullPageActive);

    // 페이지 전환
    connect(ui->gridButton, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentWidget(gridPage);
        emit gridPageActive();

    });
    connect(ui->fullButton, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentWidget(fullPage);
        emit fullPageActive();
    });
}

meeting::~meeting()
{
    delete ui;
}
