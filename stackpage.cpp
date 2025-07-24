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

void Stackpage::setLabel(int globalIndex, const QPixmap& pix) {
    int pageIndex = globalIndex / perPage;
    int labelIndex = globalIndex % perPage;

    while (pageIndex >= pages.size()) {
        PageWidget* page = new PageWidget(this);
        this->addWidget(page);
        pages.push_back(page);
    }

    QLabel* lbl = pages[pageIndex]->labelAt(labelIndex);
    if (lbl) {
        lbl->setPixmap(pix);
        lbl->show();
    }

    //setCurrentIndex(pageIndex);  // 해당 페이지로 자동 전환 (선택)
}
