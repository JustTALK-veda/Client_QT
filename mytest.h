#ifndef MYTEST_H
#define MYTEST_H

#include <QMainWindow>
#include <QWidget>  // QLabel 동적 생성을 위해 추가
class VideoThread;  // 전방 선언

class mytest : public QMainWindow {
    Q_OBJECT

public:
    explicit mytest(QWidget *parent = nullptr);
    ~mytest();

private:
    VideoThread *videoThread;  // VideoThread 포인터
};

#endif // MYTEST_H
