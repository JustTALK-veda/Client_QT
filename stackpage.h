#ifndef STACKPAGE_H
#define STACKPAGE_H

#include "PageWidget.h"
#include <QStackedWidget>

class Stackpage : public QStackedWidget
{
    Q_OBJECT

public:
    explicit Stackpage(QWidget *parent = nullptr);
    ~Stackpage();
    void goToPreviousPage();
    void goToNextPage();
    void setLabel(int globalIndex, const QPixmap& pix);

private:
    int perPage = 4;
    std::vector<PageWidget*> pages;
};

#endif // STACKPAGE_H
