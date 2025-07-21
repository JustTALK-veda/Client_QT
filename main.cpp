#include <QApplication>
#include "MainWindow.h"
#include "lobby.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    Lobby lobby;
    lobby.showFullScreen();
    QObject::connect(&lobby, &Lobby::accepted, [&]() {
        lobby.close();
        MainWindow *mw = new MainWindow;
        mw->showFullScreen();
    });

    return a.exec();
}

