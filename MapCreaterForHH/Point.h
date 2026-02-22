#pragma once

class Point {
private:
    double x, y;

public:
    Point(): x(0), y(0) {};
    Point(double x, double y): x(x), y(y) {};

    void setX(double x) { this->x = x; }
    void setY(double y) { this->y = y; }

    double getX() const { return this->x; }
    double getY() const { return this->y; }


};