#include "PageWidget.h"

PageWidget::PageWidget(QWidget* parent) : QWidget(parent) {
    layout = new QGridLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);

    for (int i = 0; i < 4; ++i) {
        QLabel* lbl = new QLabel(this);
        lbl->setMinimumSize(100, 100);
        lbl->setAlignment(Qt::AlignCenter);
        lbl->setStyleSheet("background-color: lightgray;");
        labels.push_back(lbl);
        layout->addWidget(lbl, i / 2, i % 2);
    }
}

QLabel* PageWidget::labelAt(int index) {
    if (index >= 0 && index < labels.size()) return labels[index];
    return nullptr;
}
