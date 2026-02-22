// IOContours.h
#pragma once
#include <string>
#include <fstream>
#include <filesystem>
#include "Contour.h"

class IOContours {
private:
    static const std::string CONTOURS_DIR;

public:
    static void writeContour(const Contour& c, const std::string& name) {
        std::filesystem::create_directories(CONTOURS_DIR);
        std::string fullPath = CONTOURS_DIR + "/" + name + ".cnt";

        std::ofstream file(fullPath, std::ios::binary);
        if (!file) return;

        // Сохраняем опорную точку
        Point pos = c.getPositionPoint();
        double x = pos.getX();
        double y = pos.getY();
        file.write(reinterpret_cast<const char*>(&x), sizeof(x));
        file.write(reinterpret_cast<const char*>(&y), sizeof(y));

        // Сохраняем масштаб
        double scale = c.getScale();
        file.write(reinterpret_cast<const char*>(&scale), sizeof(scale));

        // Сохраняем имя
        size_t nameLen = c.getName().length();
        file.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
        file.write(c.getName().c_str(), nameLen);

        // Сохраняем точки
        size_t pointCount = c.getPointCount();
        file.write(reinterpret_cast<const char*>(&pointCount), sizeof(pointCount));

        for (size_t i = 0; i < pointCount; i++) {
            Point p = c.getPoint(i);
            double px = p.getX();
            double py = p.getY();
            file.write(reinterpret_cast<const char*>(&px), sizeof(px));
            file.write(reinterpret_cast<const char*>(&py), sizeof(py));
        }

        file.close();
    }

    static Contour readContour(const std::string& name) {
        std::string fullPath = CONTOURS_DIR + "/" + name + ".cnt";
        std::ifstream file(fullPath, std::ios::binary);
        Contour result;

        if (!file) return result;

        double x, y, scale;
        file.read(reinterpret_cast<char*>(&x), sizeof(x));
        file.read(reinterpret_cast<char*>(&y), sizeof(y));
        file.read(reinterpret_cast<char*>(&scale), sizeof(scale));

        result.setPositionPoint(x, y);
        result.setScale(scale);

        size_t nameLen;
        file.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        std::string loadedName(nameLen, ' ');
        file.read(&loadedName[0], nameLen);
        result.setName(loadedName);

        size_t pointCount;
        file.read(reinterpret_cast<char*>(&pointCount), sizeof(pointCount));

        for (size_t i = 0; i < pointCount; i++) {
            double px, py;
            file.read(reinterpret_cast<char*>(&px), sizeof(px));
            file.read(reinterpret_cast<char*>(&py), sizeof(py));
            result.addPoint(px, py);
        }

        file.close();
        return result;
    }
};

const std::string IOContours::CONTOURS_DIR = "contours";