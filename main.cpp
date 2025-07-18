#include <QApplication>
#include "MainWindow.h"
#include "lobby.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    Lobby lobby;

    lobby.show();
    QObject::connect(&lobby, &Lobby::accepted, [&](){
        lobby.close();
        MainWindow *mw = new MainWindow;
        mw->show();
    });


    return a.exec();
}
// #include <QApplication>
// #include "MainWindow.h"


// int main(int argc, char *argv[]) {
//     QApplication a(argc, argv);

//     MainWindow w;
//     w.show();

//     return a.exec();
// }

