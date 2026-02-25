// MapManager.cpp
#include "MapManager.h"
#include "MapWidget.h"
#include <QMessageBox>

MapManager::MapManager(Controller* controller, QObject* parent)
    : QObject(parent)
    , m_controller(controller)
    , m_mapWidget(nullptr)  // инициализируем nullptr
{
    // Настройка GeoCalculator с параметрами для Питера
    m_calc.setReferencePoint({700, 450}, 59.95, 30.31);
    m_calc.setScale(0.001);
}

QPushButton* MapManager::createMapButton(QWidget* parent) {
    QPushButton* btn = new QPushButton("🗺 КАРТА", parent);
    btn->setStyleSheet(R"(
        QPushButton {
            background-color: #27ae60;
            color: white;
            font-weight: bold;
            padding: 8px 15px;
            border: none;
            border-radius: 4px;
        }
        QPushButton:hover {
            background-color: #2ecc71;
        }
    )");

    connect(btn, &QPushButton::clicked, this, [this, parent]() {
        showMap(parent);
    });

    return btn;
}

void MapManager::showMap(QWidget* parent) {
    // Проверка: если окно уже открыто
    if (m_mapWidget) {
        m_mapWidget->raise();          // поднять на передний план
        m_mapWidget->activateWindow(); // активировать
        return;
    }

    auto contours = IOContours::getAvailableContours();
    if (contours.empty()) {
        QMessageBox::warning(parent, "Ошибка",
            "Нет контуров в папке data/contours/\n"
            "Сначала нарисуйте контуры в редакторе");
        return;
    }

    // Создаем виджет карты как отдельное окно
    m_mapWidget = new MapWidget();
    m_mapWidget->setAttribute(Qt::WA_DeleteOnClose);
    m_mapWidget->setWindowTitle("Карта контуров");
    m_mapWidget->resize(800, 800);

    // При закрытии окна обнуляем указатель
    connect(m_mapWidget, &QObject::destroyed, [this]() {
        m_mapWidget = nullptr;
    });

    auto* profile = m_controller->getCurrentProfile();
    if (profile) {
        m_mapWidget->updateVacancies(profile->vacancies);
    }

    m_mapWidget->show();
}