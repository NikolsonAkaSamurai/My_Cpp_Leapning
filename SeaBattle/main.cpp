#include <QApplication>
#include <QPushButton>

#include "mwinndow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    mWinndow mwin;
    mwin.show();

    return QApplication::exec();
}
