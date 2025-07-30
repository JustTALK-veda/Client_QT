#ifndef START_H
#define START_H

#include <QWidget>
#include <QTimer>
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
    void enterLocalPageRequested();
private:
    Ui::Start *ui;
    QTimer *timeTimer;
private slots:
    void updateTime();


};

#endif // START_H
