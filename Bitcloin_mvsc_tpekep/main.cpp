#include <QApplication>
#include "pricetrackerwindow.h"

// Убираем этот макрос отовсюду, кроме pricetrackerwindow.h
// QT_CHARTS_USE_NAMESPACE

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    PriceTrackerWindow window; // Создаем наш главный виджет
    window.show();             // Показываем его

    return app.exec();
}
