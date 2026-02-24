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

        // Читаем имя
        size_t nameLen;
        file.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        std::string loadedName(nameLen, ' ');
        file.read(&loadedName[0], nameLen);
        result.setName(loadedName);

        // Читаем точки
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

    // Вспомогательный метод для получения списка контуров
    static std::vector<std::string> getAvailableContours() {
        std::vector<std::string> contours;
        std::filesystem::create_directories(CONTOURS_DIR);

        for (const auto& entry : std::filesystem::directory_iterator(CONTOURS_DIR)) {
            if (entry.path().extension() == ".cnt") {
                contours.push_back(entry.path().stem().string());
            }
        }
        return contours;
    }
};

const std::string IOContours::CONTOURS_DIR = "contours";