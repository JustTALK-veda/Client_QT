#ifndef START_H
#define START_H

#include <QWidget>
#include <QPushButton>

namespace Ui {
class Start;
}

class Start : public QWidget
{
    Q_OBJECT

public:
    explicit Start(QWidget *parent = nullptr);
    ~Start();
signals:
    void enterRemotePageRequested();
private:
    Ui::Start *ui;


};

#endif // START_H
