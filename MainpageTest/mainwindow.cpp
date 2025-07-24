#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QWidget>
#include <QPushButton>
#include <QStackedWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QGridLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    auto *gridLayout = new QGridLayout(ui->page1);
    auto *gridLayout2 = new QGridLayout(ui->page2);    // QGridLayout을 사용하여 레이아웃 설정
    gridLayout->setContentsMargins(0, 0, 0, 0);
    gridLayout->setSpacing(5); // 여기에 label
    gridLayout2->setContentsMargins(0, 0, 0, 0);
    gridLayout2->setSpacing(5);

    ui->stackedWidget->addWidget(ui->page1);  // 첫 번째 페이지
    ui->stackedWidget->addWidget(ui->page2);  // 두 번째 페이지


   ui->stackedWidget->setCurrentIndex(0);  // 첫 페이지로 설정

    // 4) Next 버튼을 눌렀을 때 페이지 전환하도록 연결
    connect(ui->nextBtn, &QPushButton::clicked, this, &MainWindow::createNewPage);
    connect(ui->preBtn, &QPushButton::clicked, this, &MainWindow::goToPreviousPage);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::goToPreviousPage() {
    // 현재 페이지의 인덱스를 가져옴
    int currentIndex = ui->stackedWidget->currentIndex();

    // 이전 페이지로 전환 (첫 번째 페이지보다 작은 인덱스일 때는 넘어가지 않도록 처리)
    if (currentIndex > 0) {
        ui->stackedWidget->setCurrentIndex(currentIndex - 1);  // 이전 페이지로 설정
    }
}


void MainWindow::createNewPage() {
    // 새로운 페이지를 동적으로 생성
    QWidget* newPage = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(newPage);  // 그리드 레이아웃 설정

    int labelCount = 4;  // 4개의 레이블
    int rows = 2;
    int cols = 2;

    // 레이블을 2x2 형태로 동적으로 생성하여 그리드에 추가
    for (int i = 0; i < labelCount; ++i) {
        // 텍스트만 있는 레이블 생성
        QLabel* newLabel = new QLabel(QString("Label %1").arg(i + 1), newPage);

        // 스타일 적용
        newLabel->setStyleSheet("QLabel {"
                                "color: blue;"           // 텍스트 색상
                                "background-color: yellow;" // 배경 색상
                                "font-size: 18px;"       // 글꼴 크기
                                "padding: 5px;"          // 여백
                                "}");

        int row = i / cols;  // 행 계산
        int col = i % cols;  // 열 계산
        gridLayout->addWidget(newLabel, row, col);  // 그리드 레이아웃에 레이블 추가
    }

    // 레이아웃 설정
    newPage->setLayout(gridLayout);

    // 동적으로 생성된 페이지를 스택에 추가
    ui->stackedWidget->addWidget(newPage);  // 새로운 페이지를 스택에 추가

    // 페이지 전환
    int currentIndex = ui->stackedWidget->currentIndex();
    ui->stackedWidget->setCurrentIndex(currentIndex + 1);  // 새로운 페이지로 전환
}
