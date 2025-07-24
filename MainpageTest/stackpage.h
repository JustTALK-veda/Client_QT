#ifndef STACKPAGE_H
#define STACKPAGE_H

#include <QStackedWidget>

class Stackpage : public QStackedWidget
{
    Q_OBJECT

public:
    explicit Stackpage(QWidget *parent = nullptr);
    ~Stackpage();
    void goToPreviousPage();
    void goToNextPage();
    void createNewPage();

private:

};

#endif // STACKPAGE_H
