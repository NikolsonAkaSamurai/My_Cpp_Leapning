// main.cpp
#include <QApplication>
#include <QSslSocket>
#include "MainWindow.h"


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);



    // Настройка SSL для Windows
    qputenv("QT_SSL", "schannel");

    // Создаем и показываем главное окно
    MainWindow w;
    w.show();

    return app.exec();
}