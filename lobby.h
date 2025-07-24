#ifndef LOBBY_H
#define LOBBY_H

#include <QWidget>
#include "CameraWidget.h"

namespace Ui {
class Lobby;
}

class Lobby : public QWidget
{
    Q_OBJECT

public:
    explicit Lobby(QWidget *parent = nullptr);
    ~Lobby();

private:
    Ui::Lobby *ui;
};

#endif // LOBBY_H
