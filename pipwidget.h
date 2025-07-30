#ifndef PIPWIDGET_H
#define PIPWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QPoint>
#include "CameraWidget.h"

class PipWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PipWidget(QWidget *parent = nullptr, CameraWidget* webcamFrame = nullptr);
    CameraWidget* webcam;
signals:

protected:
    void enterEvent(QEnterEvent*) override;
    void leaveEvent(QEvent*) override;
    void resizeEvent(QResizeEvent*) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    enum ResizeDirection {
        None = 0,
        Left = 0x1, Right = 0x2, Top = 0x4, Bottom = 0x8,
        TopLeft = Top|Left, TopRight = Top|Right, BottomLeft = Bottom|Left, BottomRight = Bottom|Right
    };
    ResizeDirection getResizeDirection(const QPoint& pos);
    bool isCorner(const QPoint& pos);
private:
    QVBoxLayout* topLayout;
    QHBoxLayout* btnLayout;
    QPushButton* btnCam;
    QPushButton* btnHide;
    QPoint m_dragPos, m_dragStartPos;
    QRect m_startRect;
    bool m_dragging;
    bool m_resizing;
    int m_resizeMargin;
    ResizeDirection m_resizeDirection;
    double m_aspectRatio;
};

#endif // PIPWIDGET_H
