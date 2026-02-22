// ViewWindow.h - исправленная версия
#pragma once
#include "raylib.h"
#include "Contour.h"
#include "IOContours.h"
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <cmath>
#include <algorithm>

class ViewWindow {
private:
    const int screenWidth = 1400;
    const int screenHeight = 900;

    std::vector<Contour> contours;
    std::vector<std::string> contourNames;

    // Для навигации
    Vector2 viewOffset;
    float zoom;
    bool isDragging;
    Vector2 lastMousePos;

    // Общая базовая точка для всех контуров (средняя по всем positionPoint)
    Point globalRefPoint;
    bool hasGlobalRef;

public:
    ViewWindow() {
        InitWindow(screenWidth, screenHeight, "View All Contours");
        SetTargetFPS(60);

        viewOffset = {0, 0};
        zoom = 1.0f;
        isDragging = false;
        hasGlobalRef = false;
        globalRefPoint = Point(0,0);

        loadAllContours();
        calculateGlobalReference();

        std::cout << "Loaded " << contours.size() << " contours" << std::endl;
    }

    ~ViewWindow() {
        CloseWindow();
    }

    void run() {
        while (!WindowShouldClose()) {
            handleInput();
            draw();
        }
    }

private:
    void loadAllContours() {
        contours.clear();
        contourNames.clear();

        std::filesystem::create_directories("contours");

        for (const auto& entry : std::filesystem::directory_iterator("contours")) {
            if (entry.path().extension() == ".cnt") {
                std::string name = entry.path().stem().string();
                Contour c = IOContours::readContour(name);
                contours.push_back(c);
                contourNames.push_back(name);
            }
        }
    }

    void calculateGlobalReference() {
        if (contours.empty()) return;

        double sumX = 0, sumY = 0;
        int count = 0;

        for (const auto& c : contours) {
            Point ref = c.getPositionPoint();
            sumX += ref.getX();
            sumY += ref.getY();
            count++;
        }

        if (count > 0) {
            globalRefPoint = Point(sumX / count, sumY / count);
            hasGlobalRef = true;
        }
    }

    void handleInput() {
        float wheel = GetMouseWheelMove();
        if (wheel != 0) {
            zoom *= (wheel > 0) ? 1.1f : 0.9f;
            if (zoom < 0.1f) zoom = 0.1f;
            if (zoom > 5.0f) zoom = 5.0f;
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            isDragging = true;
            lastMousePos = GetMousePosition();
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
            isDragging = false;
        }

        if (isDragging) {
            Vector2 mousePos = GetMousePosition();
            Vector2 delta = {
                (mousePos.x - lastMousePos.x) / zoom,
                (mousePos.y - lastMousePos.y) / zoom
            };
            viewOffset.x += delta.x;
            viewOffset.y += delta.y;
            lastMousePos = mousePos;
        }

        if (IsKeyPressed(KEY_R)) {
            viewOffset = {0, 0};
            zoom = 1.0f;
        }

        if (IsKeyPressed(KEY_ESCAPE)) {
            CloseWindow();
        }
    }

    void draw() {
        BeginDrawing();
        ClearBackground(BLACK);

        if (contours.empty()) {
            DrawText("No contours found in 'contours' folder",
                     screenWidth/2 - 200, screenHeight/2, 20, RED);
            DrawText("Draw and save a contour first",
                     screenWidth/2 - 150, screenHeight/2 + 40, 18, ORANGE);
        } else {
            // Рисуем все контуры
            for (size_t idx = 0; idx < contours.size(); idx++) {
                drawContour(contours[idx], contourNames[idx], idx);
            }
        }

        drawUI();
        EndDrawing();
    }

    void drawContour(const Contour& c, const std::string& name, int index) {
        if (c.getPointCount() < 2) return;

        // Получаем опорную точку контура
        Point refPoint = c.getPositionPoint();

        // Смещение относительно глобальной опорной точки
        double refOffsetX = (refPoint.getX() - globalRefPoint.getX()) * 100; // масштабируем для видимости
        double refOffsetY = (refPoint.getY() - globalRefPoint.getY()) * 100;

        Color contourColor = getContourColor(index);

        // Рисуем линии контура
        for (size_t i = 0; i < c.getPointCount() - 1; i++) {
            Point p1 = c.getPoint(i);
            Point p2 = c.getPoint(i+1);

            // Каждая точка смещается относительно опорной точки своего контура
            float x1 = (p1.getX() + refOffsetX + viewOffset.x) * zoom + screenWidth/2;
            float y1 = (p1.getY() + refOffsetY + viewOffset.y) * zoom + screenHeight/2;
            float x2 = (p2.getX() + refOffsetX + viewOffset.x) * zoom + screenWidth/2;
            float y2 = (p2.getY() + refOffsetY + viewOffset.y) * zoom + screenHeight/2;

            DrawLineEx({x1, y1}, {x2, y2}, 2.0f, contourColor);
        }

        // Замыкаем контур
        if (c.getPointCount() > 2) {
            Point p1 = c.getPoint(c.getPointCount() - 1);
            Point p2 = c.getPoint(0);

            float x1 = (p1.getX() + refOffsetX + viewOffset.x) * zoom + screenWidth/2;
            float y1 = (p1.getY() + refOffsetY + viewOffset.y) * zoom + screenHeight/2;
            float x2 = (p2.getX() + refOffsetX + viewOffset.x) * zoom + screenWidth/2;
            float y2 = (p2.getY() + refOffsetY + viewOffset.y) * zoom + screenHeight/2;

            DrawLineEx({x1, y1}, {x2, y2}, 2.0f, contourColor);
        }

        // Рисуем название
        float labelX = (refOffsetX + viewOffset.x) * zoom + screenWidth/2;
        float labelY = (refOffsetY + viewOffset.y) * zoom + screenHeight/2;
        DrawText(name.c_str(), labelX, labelY, 15 * zoom, contourColor);
    }

    Color getContourColor(int index) {
        Color colors[] = {
            GREEN, YELLOW, ORANGE, SKYBLUE, PINK, LIME, GOLD, VIOLET
        };
        return colors[index % 8];
    }

    void drawUI() {
        // Верхняя панель
        DrawRectangle(0, 0, screenWidth, 80, {0, 0, 0, 200});

        DrawText(TextFormat("Total contours: %zu", contours.size()),
                 20, 20, 18, WHITE);
        DrawText(TextFormat("Zoom: %.2fx", zoom),
                 20, 45, 15, LIGHTGRAY);

        // Легенда управления
        DrawText("RMB - drag | Mouse wheel - zoom | R - reset | ESC - exit",
                 screenWidth - 500, screenHeight - 30, 15, GRAY);

        // Легенда цветов
        if (contours.size() <= 8) {
            int legendY = screenHeight - 150;
            DrawRectangle(screenWidth - 250, legendY - 10, 240, 30 + contours.size() * 20,
                         {0, 0, 0, 200});

            for (size_t i = 0; i < contours.size(); i++) {
                DrawRectangle(screenWidth - 240, legendY + i*20, 15, 15, getContourColor(i));
                DrawText(contourNames[i].c_str(), screenWidth - 220, legendY + i*20, 12, WHITE);
            }
        }
    }
};