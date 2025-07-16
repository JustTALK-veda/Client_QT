#ifndef LOBBY_H
#define LOBBY_H

#include <QWidget>
#include <QVector>
#include <QLabel>
#include <QGridLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

class Lobby : public QWidget {
    Q_OBJECT
public:
    explicit Lobby(QWidget *parent = nullptr);

signals:
    void accepted();    // 수락 버튼 눌렀을 때

private:
    QVector<QLabel*> labels;
    QGridLayout *gridLayout;
};

#endif // LOBBY_H
