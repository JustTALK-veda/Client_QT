#ifndef STACKPAGE_H
#define STACKPAGE_H

#include "pagewidget.h"
#include <QStackedWidget>

class Stackpage : public QStackedWidget
{
    Q_OBJECT

public:
    explicit Stackpage(QWidget *parent = nullptr);
    ~Stackpage();
    bool isFirstPage();
    bool isLastPage();
    void goToPreviousPage();
    void goToNextPage();
    void setLabel(int globalIndex, const QPixmap& pix);
public slots:
    void highlightLabel(int globalIndex);

private:
    int perPage = 4;
    std::vector<PageWidget*> pages;
};

#endif // STACKPAGE_H
