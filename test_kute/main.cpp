#include "mwindow.h"
#include <QApplication>


int main(int argc, char *argv[]) {

    QApplication app(argc, argv);  // ← СОЗДАТЬ Qt приложение!

    mWindow window;
    window.show();

    return app.exec();  // ← ЗАПУСТИТЬ цикл событий Qt!

}