#ifndef STACKPAGE_H
#define STACKPAGE_H

#include <QWidget>

namespace Ui {
class Stackpage;
}

class Stackpage : public QWidget
{
    Q_OBJECT

public:
    explicit Stackpage(QWidget *parent = nullptr);
    ~Stackpage();

private:
    Ui::Stackpage *ui;
};

#endif // STACKPAGE_H
