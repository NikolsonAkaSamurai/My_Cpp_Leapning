#pragma once
#include "Point.h"
#include <cmath>

#define DEG_TO_RAD 0.01745329251994329576923690768489

class GeoCalculator {
private:
    // Опорная точка (единственная связь с реальностью)
    Point refPixel;      // где на картинке (теперь Point!)
    double refLat;       // реальная широта
    double refLon;       // реальная долгота

    // Масштаб
    double scale;        // км/пиксель

    // Флаги готовности
    bool hasReference;
    bool hasScale;

public:
    double getScale() const { return scale; }

    GeoCalculator() : refPixel(0.0, 0.0), refLat(0), refLon(0), scale(1.0),
                      hasReference(false), hasScale(false) {}

    // 1. Устанавливаем опорную точку (позиционную) - ТЕПЕРЬ Point!
    void setReferencePoint(const Point& pixel, double lat, double lon) {
        refPixel = pixel;
        refLat = lat;
        refLon = lon;
        hasReference = true;
    }

    // 2. Устанавливаем масштаб (после калибровки)
    void setScale(double kmPerPixel) {
        scale = kmPerPixel;
        hasScale = true;
    }

    // 3. Устанавливаем масштаб по двум точкам - ТЕПЕРЬ Point!
    void setScaleFromPoints(const Point& pixelA, const Point& pixelB,
                           double latA, double lonA,
                           double latB, double lonB) {
        // Расстояние в пикселях
        double dx = pixelB.getX() - pixelA.getX();
        double dy = pixelB.getY() - pixelA.getY();
        double pixelDist = sqrt(dx*dx + dy*dy);

        // Расстояние в километрах (приближенно)
        double latKm = (latB - latA) * 111.0;
        double lonKm = (lonB - lonA) * 111.0 * cos(refLat * DEG_TO_RAD);
        double geoDist = sqrt(latKm * latKm + lonKm * lonKm);

        scale = geoDist / pixelDist;
        hasScale = true;
    }

    // 4. Пересчет пикселя в реальные координаты - Point на входе!
    Point pixelToGeo(const Point& pixel) const {
        if (!hasReference || !hasScale) {
            return Point(0, 0);
        }

        double dx = pixel.getX() - refPixel.getX();
        double dy = pixel.getY() - refPixel.getY();

        double kmEast = dx * scale;
        double kmNorth = -dy * scale;  // ← МИНУС! Y экрана вниз, география север вверх

        double lonOffset = kmEast / (111.0 * cos(refLat * DEG_TO_RAD));
        double latOffset = kmNorth / 111.0;

        return Point(refLat + latOffset, refLon + lonOffset);
    }

    // 5. Пересчет реальных координат в пиксели - Point на ВЫХОДЕ тоже!
    Point geoToPixel(const Point& geo) const {
        if (!hasReference || !hasScale) {
            return Point(0, 0);
        }

        double latDiff = geo.getX() - refLat;
        double lonDiff = geo.getY() - refLon;

        double kmNorth = latDiff * 111.0;
        double kmEast = lonDiff * 111.0 * cos(refLat * DEG_TO_RAD);

        double px = refPixel.getX() + (kmEast / scale);
        double py = refPixel.getY() - (kmNorth / scale);  // ← МИНУС! обратное преобразование

        return Point(px, py);
    }

    // 6. Проверка готовности
    bool isReady() const { return hasReference && hasScale; }

    // 7. Получить текущие параметры (для отладки)
    void printCalibration() const {
        printf("Ref pixel: (%.6f, %.6f)\n", refPixel.getX(), refPixel.getY());
        printf("Ref geo: (%.6f, %.6f)\n", refLat, refLon);
        printf("Scale: %.6f km/pixel\n", scale);
    }
};