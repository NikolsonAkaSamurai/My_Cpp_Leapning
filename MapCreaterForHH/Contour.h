// Contour.h
#pragma once
#include <vector>
#include "Point.h"
#include <string>

class Contour {
private:
    std::vector<Point> points;        // точки контура
    Point positionPoint;               // опорная точка
    double scaleCoefficient;            // коэффициент масштаба
    std::string name;                    // имя контура

public:
    // Конструкторы
    Contour() : positionPoint(0, 0), scaleCoefficient(1.0) {}
    Contour(double x, double y, double scale)
        : positionPoint(x, y), scaleCoefficient(scale) {}

    // Работа с опорной точкой
    void setPositionPoint(double x, double y) {
        positionPoint.setX(x);
        positionPoint.setY(y);
    }

    Point getPositionPoint() const { return positionPoint; }
    double getXPosition() const { return positionPoint.getX(); }
    double getYPosition() const { return positionPoint.getY(); }

    // Работа с масштабом
    void setScale(double scale) { scaleCoefficient = scale; }
    double getScale() const { return scaleCoefficient; }

    // Работа с точками контура
    void addPoint(double x, double y) {
        points.push_back(Point(x, y));
    }

    void addPoint(const Point& p) {
        points.push_back(p);
    }

    void delLastPoint() {
        if (!points.empty()) {
            points.pop_back();
        }
    }

    size_t getPointCount() const { return points.size(); }
    Point getPoint(size_t index) const { return points[index]; }
    void clear() { points.clear(); }
    bool isEmpty() const { return points.empty(); }
    const std::vector<Point>& getPoints() const { return points; }

    // Работа с именем
    void setName(const std::string& newName) { name = newName; }
    std::string getName() const { return name; }

    // Итераторы для range-based for
    std::vector<Point>::iterator begin() { return points.begin(); }
    std::vector<Point>::iterator end() { return points.end(); }
    std::vector<Point>::const_iterator begin() const { return points.begin(); }
    std::vector<Point>::const_iterator end() const { return points.end(); }
};