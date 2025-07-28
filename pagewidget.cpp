#include "pagewidget.h"
#include <QPainter>

PageWidget::PageWidget(QWidget* parent) : QWidget(parent) {
    layout = new QGridLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);

    for (int i = 0; i < 4; ++i) {
        QLabel* lbl = new QLabel(this);
        lbl->setFixedSize(480, 360);
        lbl->setAlignment(Qt::AlignCenter);
        //lbl->setScaledContents(true);
        labels.push_back(lbl);
        layout->addWidget(lbl, i / 2, i % 2);
    }
}

QLabel* PageWidget::labelAt(int index) {
    if (index >= 0 && index < labels.size()) return labels[index];
    return nullptr;
}

void PageWidget::highlightLabel(int index) {
    const int radius = 6;
    const int margin = 10;

    for (int i = 0; i < labels.size(); ++i) {
        QLabel* lbl = labels[i];
        qDebug() << "label size:" << labels[i]->size();

        if (i == index) {
            lbl->setStyleSheet("border: 3px solid rgb(237, 107, 6);");
        } else {
            lbl->setStyleSheet("border: none;");
        }
    }
}

void PageWidget::clearHighlights() {
    for (int i = 0; i < labels.size(); ++i) {
        labels[i]->setStyleSheet("border: none;");
    }
}
