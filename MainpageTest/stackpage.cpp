#include "stackpage.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QGridLayout>

Stackpage::Stackpage(QWidget *parent)
    : QStackedWidget(parent)
{
}

Stackpage::~Stackpage()
{
}

void Stackpage::goToPreviousPage() {
    // 현재 페이지의 인덱스를 가져옴
    int currentIndex = this->currentIndex();

    // 이전 페이지로 전환 (첫 번째 페이지보다 작은 인덱스일 때는 넘어가지 않도록 처리)
    if (currentIndex > 0) {
        this->setCurrentIndex(currentIndex - 1);  // 이전 페이지로 설정
    }
}

void Stackpage::goToNextPage() {
    // 현재 페이지 인덱스
    int currentIndex = this->currentIndex();

    // 전체 페이지 수
    int totalPages = this->count();

    // 다음 페이지가 존재할 경우에만 이동
    if (currentIndex < totalPages - 1) {
        this->setCurrentIndex(currentIndex + 1);  // 다음 페이지로 이동
    }
}

void Stackpage::createNewPage() {
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
    this->addWidget(newPage);  // 새로운 페이지를 스택에 추가

    // 페이지 전환
    int currentIndex = this->currentIndex();
    this->setCurrentIndex(currentIndex + 1);  // 새로운 페이지로 전환
}
