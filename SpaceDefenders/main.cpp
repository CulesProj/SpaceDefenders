#include <QApplication>
#include "GameWindow.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    GameWindow w;
    w.show();
    return app.exec();
}
