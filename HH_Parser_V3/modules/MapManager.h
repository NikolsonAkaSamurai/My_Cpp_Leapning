// MapManager.h
#ifndef MAPMANAGER_H
#define MAPMANAGER_H

#include <QObject>
#include <QPushButton>
#include <QWidget>
#include "../core/Controller.h"
#include "../core/GeoCalculator.h"
#include "../core/IOContours.h"
#include "MapWidget.h"

class MapManager : public QObject {
    Q_OBJECT

public:
    explicit MapManager(Controller* controller, QObject* parent = nullptr);

    // Объявление метода для создания кнопки
    QPushButton* createMapButton(QWidget* parent);

    void showMap(QWidget* parent);

    // Новый метод для получения указателя на виджет карты
    MapWidget* getMapWidget() const { return m_mapWidget; }

private:
    Controller* m_controller;
    GeoCalculator m_calc;
    MapWidget* m_mapWidget;  // указатель на виджет карты
};

#endif // MAPMANAGER_H