// AbstractSystemOfContours.h
#pragma once
#include "Point.h"
#include "Contour.h"
#include <vector>
#include <cmath>

#define DEG_TO_RAD 0.01745329251994329576923690768489

struct AbstractPoint {
    double x, y;  // координаты в метрах от опорной точки
};

class AbstractSystemOfContours {
private:
    // ОПОРНАЯ ТОЧКА СИСТЕМЫ (една для всех!)
    Point geoRef;           // 59.939184, 30.287846

    // Все контуры в единой системе
    std::vector<std::vector<AbstractPoint>> abstractContours;
    std::vector<std::string> contourNames;

public:
    AbstractSystemOfContours() : geoRef(59.939184, 30.287846) {}

    // Добавить контур в систему
    void addContour(const Contour& c, const std::string& name) {
        std::vector<AbstractPoint> points;

        for (size_t i = 0; i < c.getPointCount(); i++) {
            Point geo = c.getPoint(i);

            // География → метры относительно geoRef
            double latDiff = geo.getX() - geoRef.getX();
            double lonDiff = geo.getY() - geoRef.getY();

            double metersNorth = latDiff * 111000.0;
            double metersEast = lonDiff * 111000.0 * cos(geoRef.getX() * DEG_TO_RAD);

            points.push_back({metersEast, metersNorth});
        }

        abstractContours.push_back(points);
        contourNames.push_back(name);
    }

    // Получить все точки в абстрактной системе
    const auto& getAbstractContours() const { return abstractContours; }
    const auto& getNames() const { return contourNames; }
    Point getGeoRef() const { return geoRef; }
};