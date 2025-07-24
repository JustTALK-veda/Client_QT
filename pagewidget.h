#ifndef PAGEWIDGET_H
#define PAGEWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <vector>

class PageWidget : public QWidget {
    Q_OBJECT

public:
    explicit PageWidget(QWidget* parent = nullptr);

    QLabel* labelAt(int index);  // index 0~3
    int capacity() const { return 4; }

private:
    QGridLayout* layout;
    std::vector<QLabel*> labels;
};

#endif // PAGEWIDGET_H
