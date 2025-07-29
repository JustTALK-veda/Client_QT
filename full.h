#ifndef FULL_H
#define FULL_H

#include <QWidget>
#include <QVBoxLayout>
#include "HanwhaThread.h"

namespace Ui {
class Full;
}

class Full : public QWidget
{
    Q_OBJECT

public:
    explicit Full(QWidget *parent = nullptr);
    ~Full();

private:
    QLabel* m_label;
    HanwhaThread* m_thread;
    QVBoxLayout* m_layout;
    bool isFullServerReady = false;
private slots:
    void onFrameReady(const QImage&);
};

#endif // FULL_H
