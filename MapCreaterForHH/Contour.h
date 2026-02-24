// Contour.h
#pragma once
#include <vector>
#include "Point.h"
#include <string>

class Contour {
private:
    std::vector<Point> points;        // точки контура
    std::string name;                    // имя контура

public:
    // Конструкторы
    Contour(){}

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