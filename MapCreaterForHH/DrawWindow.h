// DrawWindow.h
#pragma once
#include "raylib.h"
#include "Contour.h"
#include "IOContours.h"
#include "GeoCalculator.h"
#include <iostream>
#include <string>
#include <cmath>
#include <vector>

#define DEG_TO_RAD 0.01745329251994329576923690768489

class DrawWindow {
private:
    const int screenWidth = 1400;
    const int screenHeight = 900;

    Texture2D background;

    // Геокалькулятор для пересчетов
    GeoCalculator geoCalc;

    // Состояния рисования
    enum Stage {
        STAGE_PLACE_REF_POINT,      // 1. ставим опорную точку
        STAGE_ENTER_REF_COORDS,      // 2. вводим её координаты
        STAGE_SCALE_FIRST,           // 3. первая точка масштаба
        STAGE_SCALE_SECOND,          // 4. вторая точка масштаба
        STAGE_ENTER_DISTANCE,        // 5. вводим расстояние
        STAGE_DRAWING,               // 6. рисуем контур
        STAGE_SAVE                   // 7. сохраняем
    };

    Stage currentStage;

    // Для калибровки (ТЕПЕРЬ Point вместо Vector2!)
    Point refPixel;              // опорная точка на картинке
    double refLat, refLon;        // реальные координаты опорной точки
    Point scalePointA;            // первая точка масштаба
    Point scalePointB;            // вторая точка масштаба
    double realDistance;          // реальное расстояние в км
    double scale;                  // км/пиксель

    // Для ввода текста
    char latInput[32];
    char lonInput[32];
    char distanceInput[32];
    char nameInput[32];
    int inputCharCount;
    int activeField;  // 0-широта, 1-долгота, 2-расстояние, 3-имя

    // Временные точки для отображения при рисовании (ТЕПЕРЬ Point!)
    std::vector<Point> displayPoints;

public:
    DrawWindow() {
        InitWindow(screenWidth, screenHeight, "Draw Contour with Calibration");
        SetTargetFPS(60);

        // Загружаем картинку
        Image image = LoadImage("assets/spb.png");
        if (image.data == nullptr) {
            std::cout << "Error: Cannot load assets/spb.png" << std::endl;
            background.id = 0;
        } else {
            background = LoadTextureFromImage(image);
            UnloadImage(image);
        }

        // Инициализация
        currentStage = STAGE_PLACE_REF_POINT;
        refPixel = Point(0, 0);
        refLat = refLon = 0;
        scalePointA = Point(0, 0);
        scalePointB = Point(0, 0);
        realDistance = 0;
        scale = 1.0;

        latInput[0] = lonInput[0] = distanceInput[0] = nameInput[0] = '\0';
        inputCharCount = 0;
        activeField = 0;

        displayPoints.clear();
    }

    ~DrawWindow() {
        if (background.id != 0) {
            UnloadTexture(background);
        }
        CloseWindow();
    }

    void run() {
        while (!WindowShouldClose()) {
            handleInput();
            draw();
        }
    }

private:
    // Обработка ввода для координат (широта/долгота)
    void handleCoordInput() {
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= '0' && key <= '9') || key == '.' || key == '-') {
                char* currentInput = (activeField == 0) ? latInput : lonInput;
                if (inputCharCount < 31) {
                    currentInput[inputCharCount] = (char)key;
                    currentInput[inputCharCount + 1] = '\0';
                    inputCharCount++;
                }
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && inputCharCount > 0) {
            char* currentInput = (activeField == 0) ? latInput : lonInput;
            inputCharCount--;
            currentInput[inputCharCount] = '\0';
        }
    }

    // Обработка ввода для расстояния
    void handleDistanceInput() {
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= '0' && key <= '9') || key == '.') {
                if (inputCharCount < 31) {
                    distanceInput[inputCharCount] = (char)key;
                    distanceInput[inputCharCount + 1] = '\0';
                    inputCharCount++;
                }
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && inputCharCount > 0) {
            inputCharCount--;
            distanceInput[inputCharCount] = '\0';
        }
    }

    // Обработка ввода для имени
    void handleNameInput() {
        int key = GetCharPressed();
        while (key > 0) {
            // Разрешаем буквы, цифры, дефис, подчеркивание
            if ((key >= 'a' && key <= 'z') || (key >= 'A' && key <= 'Z') ||
                (key >= '0' && key <= '9') || key == '_' || key == '-') {
                if (inputCharCount < 31) {
                    nameInput[inputCharCount] = (char)key;
                    nameInput[inputCharCount + 1] = '\0';
                    inputCharCount++;
                }
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && inputCharCount > 0) {
            inputCharCount--;
            nameInput[inputCharCount] = '\0';
        }
    }

    void handleInput() {
        // Получаем позицию мыши как Point
        Vector2 mouseVec = GetMousePosition();
        Point mousePos(mouseVec.x, mouseVec.y);

        switch(currentStage) {
            case STAGE_PLACE_REF_POINT:
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    refPixel = mousePos;
                    currentStage = STAGE_ENTER_REF_COORDS;
                    inputCharCount = 0;
                    activeField = 0;
                    latInput[0] = '\0';
                    lonInput[0] = '\0';
                }
                break;

            case STAGE_ENTER_REF_COORDS:
                handleCoordInput();
                if (IsKeyPressed(KEY_ENTER) && inputCharCount > 0) {
                    if (activeField == 0) {
                        // Закончили ввод широты
                        refLat = atof(latInput);
                        activeField = 1;
                        inputCharCount = 0;
                    } else if (activeField == 1) {
                        // Закончили ввод долготы
                        refLon = atof(lonInput);

                        // Устанавливаем опорную точку в калькулятор
                        geoCalc.setReferencePoint(refPixel, refLat, refLon);

                        currentStage = STAGE_SCALE_FIRST;
                        inputCharCount = 0;
                    }
                }
                break;

            case STAGE_SCALE_FIRST:
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    scalePointA = mousePos;
                    currentStage = STAGE_SCALE_SECOND;
                }
                break;

            case STAGE_SCALE_SECOND:
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    scalePointB = mousePos;
                    currentStage = STAGE_ENTER_DISTANCE;
                    inputCharCount = 0;
                    distanceInput[0] = '\0';
                }
                break;

            case STAGE_ENTER_DISTANCE:
                handleDistanceInput();
                if (IsKeyPressed(KEY_ENTER) && inputCharCount > 0) {
                    realDistance = atof(distanceInput);

                    // Вычисляем масштаб
                    double dx = scalePointB.getX() - scalePointA.getX();
                    double dy = scalePointB.getY() - scalePointA.getY();
                    double pixelDist = sqrt(dx*dx + dy*dy);
                    scale = realDistance / pixelDist;  // км/пиксель

                    // Устанавливаем масштаб в калькулятор
                    geoCalc.setScale(scale);

                    currentStage = STAGE_DRAWING;
                }
                break;

            case STAGE_DRAWING:
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    // Сохраняем пиксель для отображения
                    displayPoints.push_back(mousePos);
                }

                if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && displayPoints.size() > 0) {
                    displayPoints.pop_back();
                }

                if (IsKeyPressed(KEY_S) && displayPoints.size() > 2) {
                    currentStage = STAGE_SAVE;
                    inputCharCount = 0;
                    nameInput[0] = '\0';
                }
                break;

            case STAGE_SAVE:
                handleNameInput();
                if (IsKeyPressed(KEY_ENTER) && inputCharCount > 0) {
                    // Создаем новый контур с реальными координатами
                    Contour contourToSave;
                    contourToSave.setName(nameInput);

                    // Переводим все пиксели в реальные координаты через GeoCalculator
                    for (const auto& pixel : displayPoints) {
                        Point realPoint = geoCalc.pixelToGeo(pixel);
                        contourToSave.addPoint(realPoint);
                    }

                    // Сохраняем
                    IOContours::writeContour(contourToSave, nameInput);

                    currentStage = STAGE_DRAWING;
                }
                if (IsKeyPressed(KEY_ESCAPE)) {
                    currentStage = STAGE_DRAWING;
                }
                break;
        }

        // ESC для выхода из программы
        if (IsKeyPressed(KEY_ESCAPE) &&
            currentStage != STAGE_SAVE &&
            currentStage != STAGE_ENTER_DISTANCE &&
            currentStage != STAGE_ENTER_REF_COORDS) {
            CloseWindow();
        }
    }

    void draw() {
        BeginDrawing();
        ClearBackground(DARKGRAY);

        // Рисуем фон
        if (background.id != 0) {
            DrawTexture(background, 0, 0, WHITE);
        }

        // Рисуем калибровочные точки
        drawCalibrationPoints();

        // Рисуем контур (по displayPoints)
        drawContour();

        // Рисуем интерфейс
        drawUI();

        EndDrawing();
    }

    void drawCalibrationPoints() {
        // Опорная точка
        if (refPixel.getX() != 0 || refPixel.getY() != 0) {
            DrawCircle(refPixel.getX(), refPixel.getY(), 8, BLUE);
            DrawText("REF", refPixel.getX() + 10, refPixel.getY() - 20, 15, BLUE);
        }

        // Точки масштаба
        if (scalePointA.getX() != 0 || scalePointA.getY() != 0) {
            DrawCircle(scalePointA.getX(), scalePointA.getY(), 8, GREEN);
            DrawText("A", scalePointA.getX() + 10, scalePointA.getY() - 20, 15, GREEN);
        }

        if (scalePointB.getX() != 0 || scalePointB.getY() != 0) {
            DrawCircle(scalePointB.getX(), scalePointB.getY(), 8, YELLOW);
            DrawText("B", scalePointB.getX() + 10, scalePointB.getY() - 20, 15, YELLOW);

            // Линия между точками масштаба
            DrawLine(scalePointA.getX(), scalePointA.getY(),
                     scalePointB.getX(), scalePointB.getY(), GREEN);
        }
    }

    void drawContour() {
        if (displayPoints.size() >= 2) {
            for (size_t i = 0; i < displayPoints.size() - 1; i++) {
                DrawLine(displayPoints[i].getX(), displayPoints[i].getY(),
                        displayPoints[i+1].getX(), displayPoints[i+1].getY(), RED);
            }
        }

        for (size_t i = 0; i < displayPoints.size(); i++) {
            DrawCircle(displayPoints[i].getX(), displayPoints[i].getY(), 5, RED);
            DrawText(TextFormat("%zu", i+1),
                    displayPoints[i].getX() + 10, displayPoints[i].getY() - 10, 10, WHITE);
        }
    }

    void drawUI() {
        // Верхняя панель
        DrawRectangle(0, 0, screenWidth, 100, {0, 0, 0, 200});

        switch(currentStage) {
            case STAGE_PLACE_REF_POINT:
                DrawText("STEP 1: Click to place REFERENCE POINT", 20, 20, 20, GREEN);
                DrawText("(this point should have known coordinates)", 20, 50, 15, WHITE);
                break;

            case STAGE_ENTER_REF_COORDS:
                DrawText("STEP 2: Enter reference point coordinates", 20, 20, 20, GREEN);
                DrawText(TextFormat("Latitude: %s", latInput), 20, 50, 18, activeField == 0 ? YELLOW : WHITE);
                DrawText(TextFormat("Longitude: %s", lonInput), 300, 50, 18, activeField == 1 ? YELLOW : WHITE);
                DrawText("Press ENTER after each field", 600, 50, 15, GRAY);
                if (inputCharCount == 0) {
                    DrawText("(type numbers...)", 20, 75, 12, DARKGRAY);
                }
                break;

            case STAGE_SCALE_FIRST:
                DrawText("STEP 3: Click FIRST scale point", 20, 20, 20, GREEN);
                break;

            case STAGE_SCALE_SECOND:
                DrawText("STEP 4: Click SECOND scale point", 20, 20, 20, GREEN);
                break;

            case STAGE_ENTER_DISTANCE:
                DrawText("STEP 5: Enter distance between scale points (km)", 20, 20, 20, GREEN);
                DrawText(TextFormat("Distance: %s", distanceInput), 20, 50, 18, YELLOW);
                DrawText("Press ENTER to confirm | BACKSPACE to delete", 300, 50, 15, GRAY);
                if (inputCharCount == 0) {
                    DrawText("(type numbers...)", 300, 75, 12, DARKGRAY);
                }
                break;

            case STAGE_DRAWING:
                DrawText(TextFormat("DRAWING | Points: %zu", displayPoints.size()),
                        20, 20, 18, GREEN);
                DrawText("LMB - add point | RMB - delete last | S - save", 20, 50, 15, WHITE);
                break;

            case STAGE_SAVE:
                DrawText("SAVE CONTOUR | Enter name:", 20, 20, 20, GREEN);
                DrawText(TextFormat("Name: %s", nameInput), 20, 50, 18, YELLOW);
                DrawText("Press ENTER to save | ESC - cancel", 300, 50, 15, GRAY);
                if (inputCharCount == 0) {
                    DrawText("(enter file name...)", 300, 75, 12, DARKGRAY);
                }
                break;
        }

        // Информация о масштабе (если есть)
        if (geoCalc.isReady()) {
            DrawText(TextFormat("Scale: %.6f km/pixel", geoCalc.getScale()),
                     screenWidth - 250, 20, 15, LIGHTGRAY);
        }
    }
};