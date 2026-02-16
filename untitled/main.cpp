// main.cpp
#include <QApplication>
#include <QSslSocket>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    qputenv("QT_SSL", "schannel");

    MainWindow w;
    w.show();

    return app.exec();
}