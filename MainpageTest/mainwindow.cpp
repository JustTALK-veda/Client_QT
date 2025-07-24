#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QWidget>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

   //  auto *gridLayout = new QGridLayout(ui->page1);
   //  auto *gridLayout2 = new QGridLayout(ui->page2);    // QGridLayout을 사용하여 레이아웃 설정
   //  gridLayout->setContentsMargins(0, 0, 0, 0);
   //  gridLayout->setSpacing(5); // 여기에 label
   //  gridLayout2->setContentsMargins(0, 0, 0, 0);
   //  gridLayout2->setSpacing(5);

   //  ui->stackedWidget->addWidget(ui->page1);  // 첫 번째 페이지
   //  ui->stackedWidget->addWidget(ui->page2);  // 두 번째 페이지


   // ui->stackedWidget->setCurrentIndex(0);  // 첫 페이지로 설정

    // 4) Next 버튼을 눌렀을 때 페이지 전환하도록 연결
    connect(ui->nextBtn, &QPushButton::clicked, ui->stackedWidget, &Stackpage::createNewPage);
    connect(ui->preBtn, &QPushButton::clicked, ui->stackedWidget, &Stackpage::goToPreviousPage);
}

MainWindow::~MainWindow()
{
    delete ui;
}




