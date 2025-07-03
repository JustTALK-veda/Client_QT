#include "videowidget.h"

VideoWidget::VideoWidget(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_NativeWindow);
    setAttribute(Qt::WA_DontCreateNativeAncestors);
}

WId VideoWidget::getVideoWinId() const {
    return winId();
}
