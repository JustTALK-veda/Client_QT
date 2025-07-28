#ifndef MEETING_H
#define MEETING_H

#include <QWidget>

namespace Ui {
class meeting;
}

class meeting : public QWidget
{
    Q_OBJECT

public:
    explicit meeting(QWidget *parent = nullptr);
    ~meeting();

private:
    Ui::meeting *ui;
};

#endif // MEETING_H
