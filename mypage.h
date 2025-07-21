#ifndef MYPAGE_H
#define MYPAGE_H
#include <Qwidget>

class mypage : public QWidget
{
public:
    mypage(QWidget *parent = nullptr);


private:
    class QStackedWidget *stack;
};

#endif // MYPAGE_H
