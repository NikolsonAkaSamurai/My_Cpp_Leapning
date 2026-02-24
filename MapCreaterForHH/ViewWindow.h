// ViewWindow.h
#pragma once
#include "raylib.h"
#include "AbstractSystemOfContours.h"
#include "IOContours.h"
#include <filesystem>
#include <vector>

class ViewWindow {
private:
    const int screenWidth = 1400;
    const int screenHeight = 900;
    const int centerX = screenWidth / 2;
    const int centerY = screenHeight / 2;

    AbstractSystemOfContours system;

    // Навигация (относительно абстрактной системы)
    double viewX, viewY;    // смещение в метрах
    double zoom;            // масштаб: 1 метр = zoom пикселей

    bool isDragging;
    double lastMouseX, lastMouseY;

public:
    ViewWindow() {
        InitWindow(screenWidth, screenHeight, "View Contours");
        SetTargetFPS(60);

        viewX = viewY = 0;
        zoom = 0.1;  // 1 метр = 0.1 пикселя (чтобы влезло)
        isDragging = false;

        // Загружаем все контуры в абстрактную систему
        loadAllContours();
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
        std::filesystem::create_directories("contours");

        for (const auto& entry : std::filesystem::directory_iterator("contours")) {
            if (entry.path().extension() == ".cnt") {
                std::string name = entry.path().stem().string();
                Contour c = IOContours::readContour(name);
                system.addContour(c, name);
            }
        }
    }

    void handleInput() {
        // Зум
        double wheel = GetMouseWheelMove();
        if (wheel != 0) {
            zoom *= (wheel > 0) ? 1.1 : 0.9;
        }

        // Панорамирование
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            isDragging = true;
            Vector2 mouse = GetMousePosition();
            lastMouseX = mouse.x;
            lastMouseY = mouse.y;
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            isDragging = false;
        }

        if (isDragging) {
            Vector2 mouse = GetMousePosition();
            double dx = mouse.x - lastMouseX;
            double dy = mouse.y - lastMouseY;

            // Смещение в метрах (пиксели / zoom)
            viewX += dx / zoom;
            viewY -= dy / zoom;

            lastMouseX = mouse.x;
            lastMouseY = mouse.y;
        }

        // Сброс вида
        if (IsKeyPressed(KEY_R)) {
            viewX = viewY = 0;
            zoom = 0.1;
        }
    }

    void draw() {
        BeginDrawing();
        ClearBackground(BLACK);

        auto& contours = system.getAbstractContours();
        auto& names = system.getNames();

        for (size_t idx = 0; idx < contours.size(); idx++) {
            Color color = getColor(idx);

            // Рисуем линии
            for (size_t i = 0; i < contours[idx].size() - 1; i++) {
                const auto& p1 = contours[idx][i];
                const auto& p2 = contours[idx][i+1];

                float x1 = (p1.x + viewX) * zoom + centerX;
                float y1 = centerY - (p1.y + viewY) * zoom;   // ← МИНУС!
                float x2 = (p2.x + viewX) * zoom + centerX;
                float y2 = centerY - (p2.y + viewY) * zoom;   // ← МИНУс!

                DrawLineEx({x1, y1}, {x2, y2}, 2, color);
            }

            // Замыкаем
            if (contours[idx].size() > 2) {
                const auto& p1 = contours[idx].back();
                const auto& p2 = contours[idx].front();

                float x1 = (p1.x + viewX) * zoom + centerX;
                float y1 = centerY - (p1.y + viewY) * zoom;   // ← МИНУС!
                float x2 = (p2.x + viewX) * zoom + centerX;
                float y2 = centerY - (p2.y + viewY) * zoom;   // ← МИНУС!

                DrawLineEx({x1, y1}, {x2, y2}, 2, color);
            }
        }

        DrawFPS(10, 10);
        EndDrawing();
    }

    Color getColor(int i) {
        Color colors[] = {GREEN, YELLOW, ORANGE, SKYBLUE, PINK, LIME};
        return colors[i % 6];
    }
};