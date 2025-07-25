#ifndef MEETINGLOCAL_H
#define MEETINGLOCAL_H

#include <QWidget>

namespace Ui {
class meetingLocal;
}

class meetingLocal : public QWidget
{
    Q_OBJECT

public:
    explicit meetingLocal(QWidget *parent = nullptr);
    ~meetingLocal();

private:
    Ui::meetingLocal *ui;
};

#endif // MEETINGLOCAL_H
