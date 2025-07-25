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


bool Stackpage::isFirstPage(){
    if (this->currentIndex() > 0)
        return false;
    else
        return true;
}

bool Stackpage::isLastPage(){
    if (this->currentIndex() < this->count() - 1)
        return false;
    else
        return true;
}

void Stackpage::goToPreviousPage() {
    // 이전 페이지로 전환 (첫 번째 페이지보다 작은 인덱스일 때는 넘어가지 않도록 처리)
    if (!isFirstPage()) {
        this->setCurrentIndex(this->currentIndex() - 1);  // 이전 페이지로 설정
    }
}

void Stackpage::goToNextPage() {
    // 다음 페이지가 존재할 경우에만 이동
    if (!isLastPage()) {
        this->setCurrentIndex(this->currentIndex() + 1);  // 다음 페이지로 이동
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
