#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>

// --- Структуры данных для игровых объектов ---
struct Bullet { Vector2 pos; Vector2 speed; bool active; bool fromBoss; bool isShadow; };
struct Enemy { Vector2 pos; Vector2 speed; int hp; bool active; bool isFat; };
struct HealthPack { Vector2 pos; bool active; };
struct Star { Vector2 pos; bool active; };
struct Pivo { Vector2 pos; bool active; };

// Босс для уровней 1-3
struct Boss {
    Vector2 pos; float speed; float health; bool active; bool dying;
    float deathTimer; float rotation; float shootTimer;
} qtBoss = { { 400, 150 }, 200.0f, 600.0f, false, false, 0, 0, 0 };

// Мяч-помощник
struct Ball { Vector2 pos; Vector2 speed; bool active; } qtBall = { {400, 450}, {0, 0}, false };

// Босс для уровня 4
struct BossVanya {
    Vector2 pos; float health; bool active; bool dying;
    float deathTimer;
    float spitAnimTimer;
    int spitAnimFrame;
    float speed; // <--- ДОБАВЛЕНА СКОРОСТЬ
} vanyaBoss = { {400, 150}, 1200.0f, false, false, 0, 0, 0, 150.0f }; // <--- ЗАДАНА НАЧАЛЬНАЯ СКОРОСТЬ


// --- Основная функция игры ---
int main() {
    // --- Инициализация ---
    InitWindow(800, 900, "C++ SURVIVOR: THE DEFIANT ONE");
    InitAudioDevice();
    SetTargetFPS(60);

    // --- Загрузка звуков ---
    Sound shootSnd = LoadSound("assets/sounds/pew.wav");
    Sound hitSnd   = LoadSound("assets/sounds/error.wav");
    Sound healSnd  = LoadSound("assets/sounds/ding.wav");
    Sound alarmSnd = LoadSound("assets/sounds/alarm.wav");
    Sound pivoSnd  = LoadSound("assets/sounds/zvuk_piva.wav");
    Sound breakSnd = LoadSound("assets/sounds/pazbiv.wav");
    Sound hesoyamSnd = LoadSound("assets/sounds/hesoyam_sound.wav");
    Sound bossDeadSnd = LoadSound("assets/sounds/dead_butt.wav");
    Sound playerDeadSnd = LoadSound("assets/sounds/dead_player_sound.wav");
    Sound missionWinSnd = LoadSound("assets/sounds/win_mission.wav");

    std::vector<Sound> farts;
    const char* fartFiles[] = { "farts_16.wav", "farts_18.wav", "farts_21.wav", "farts_24.wav", "farts_25.wav", "farts_29.wav", "farts_45.wav", "farts_9.wav" };
    for(int i = 0; i < 8; i++) {
        farts.push_back(LoadSound(TextFormat("assets/sounds/BossSoundShoots/%s", fartFiles[i])));
    }

    // ЗВУКИ ДЛЯ ВАНИ
    Sound vanyaHelloSnd = LoadSound("assets/sounds/VanyaBossSounds/HelloVanya.wav");
    Sound vanyaDeathSnd = LoadSound("assets/sounds/VanyaBossSounds/belching-7.wav");
    std::vector<Sound> vanyaSpitSnds;
    vanyaSpitSnds.push_back(LoadSound("assets/sounds/VanyaBossSounds/spit-clear.wav"));
    vanyaSpitSnds.push_back(LoadSound("assets/sounds/VanyaBossSounds/spit-man-spat.wav"));
    vanyaSpitSnds.push_back(LoadSound("assets/sounds/VanyaBossSounds/spit-short-strong.wav"));
    vanyaSpitSnds.push_back(LoadSound("assets/sounds/VanyaBossSounds/spit-short.wav"));

    // --- Загрузка текстур ---
    Texture2D pivoTex = LoadTexture("assets/images/pivo.png");
    float pivoScale = 45.0f / pivoTex.height;

    // ТЕКСТУРЫ ДЛЯ ВАНИ
    std::vector<Texture2D> vanyaFrames;
    vanyaFrames.push_back(LoadTexture("assets/images/BossVanyaImages/face1.png"));
    vanyaFrames.push_back(LoadTexture("assets/images/BossVanyaImages/face2.png"));
    vanyaFrames.push_back(LoadTexture("assets/images/BossVanyaImages/face3.png"));


    // --- Игровые переменные ---
    Vector2 pos = { 400, 800 };
    int health = 100; int score = 0; int level = 1; int starCount = 1;
    float hitTimer = 0.0f; float pivoTimer = 0.0f; float victoryTimer = 0.0f;
    int pivoStacks = 0; bool isHacking = false; std::string inputBuffer = "";
    bool playerIsDead = false;

    std::string currentTargetCode = "HESOYAM";
    std::vector<std::string> cheatPool = { "HESOYAM", "AEZAKMI", "ANIME" };
    float hackTimer = 10.0f; float hackStartDelay = 0.0f;

    // --- Контейнеры для объектов ---
    std::vector<Bullet> bullets;
    std::vector<Enemy> enemies;
    std::vector<HealthPack> packs;
    std::vector<Star> stars;
    std::vector<Pivo> pivos;

    // --- Функция для спавна врагов ---
    auto SpawnEnemies = [&](int count) {
        enemies.clear();
        for (int i = 0; i < count; i++)
            enemies.push_back({ {(float)GetRandomValue(0, 800), (float)GetRandomValue(-800, 0)}, {0, 200}, 1, true, false });
    };
    SpawnEnemies(12);

    // --- Основной игровой цикл ---
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // --- СОСТОЯНИЕ 1: В БОЮ (health > 0 и не взлом) ---
        if (health > 0 && !isHacking && victoryTimer <= 0) {
            if (hitTimer > 0) hitTimer -= dt;
            if (pivoTimer > 0) { pivoTimer -= dt; if (pivoTimer <= 0) pivoStacks = 0; }

            // Управление
            if (IsKeyDown(KEY_D)) pos.x += 450 * dt; if (IsKeyDown(KEY_A)) pos.x -= 450 * dt;
            if (IsKeyDown(KEY_W)) pos.y -= 450 * dt; if (IsKeyDown(KEY_S)) pos.y += 450 * dt;
            if (IsKeyPressed(KEY_SPACE)) {
                bullets.push_back({{pos.x, pos.y - 70}, {0, -700.0f}, true, false, false});
                if (pivoStacks >= 1) { bullets.push_back({{pos.x, pos.y - 70}, {180, -680}, true, false, false}); bullets.push_back({{pos.x, pos.y - 70}, {-180, -680}, true, false, false}); }
                if (pivoStacks >= 2) { bullets.push_back({{pos.x, pos.y - 70}, {350, -650}, true, false, false}); bullets.push_back({{pos.x, pos.y - 70}, {-350, -650}, true, false, false}); }
                PlaySound(shootSnd);
            }

            // Появление босса
            if (score >= 1000 && !qtBoss.active && !vanyaBoss.active) {
                if (level == 4) {
                    vanyaBoss.active = true;
                    vanyaBoss.health = 1200.0f;
                    PlaySound(vanyaHelloSnd);
                } else if (level < 4) {
                    qtBoss.active = true;
                    qtBoss.health = (level == 3) ? 100.0f : (level == 1 ? 360.0f : 1000.0f);
                    if (level == 1) for(auto& e : enemies) e.active = false;
                }
            }

            // Логика босса QT
            if (qtBoss.active) {
                if (!qtBoss.dying) {
                    qtBoss.pos.x += qtBoss.speed * dt;
                    if (qtBoss.pos.x > 700 || qtBoss.pos.x < 100) qtBoss.speed *= -1;
                    qtBoss.shootTimer += dt;
                    if (level == 3) {
                        float angle = atan2f(pos.y - qtBoss.pos.y, pos.x - qtBoss.pos.x) * RAD2DEG - 90.0f;
                        qtBoss.rotation = Lerp(qtBoss.rotation, Clamp(angle, -45.0f, 45.0f), 2.0f * dt);
                        if (qtBoss.shootTimer >= 0.5f) { bullets.push_back({{qtBoss.pos.x, qtBoss.pos.y + 60}, {0, 600.0f}, true, true, true}); PlaySound(shootSnd); qtBoss.shootTimer = 0; }
                        if (qtBoss.health <= 50.0f) qtBall.active = true;
                    } else {
                        qtBoss.rotation = sinf(GetTime() * 2) * 20.0f;
                        if (qtBoss.shootTimer >= 1.5f) { bullets.push_back({{qtBoss.pos.x, qtBoss.pos.y + 40}, {0, 400.0f}, true, true, false}); PlaySound(farts[GetRandomValue(0, 7)]); qtBoss.shootTimer = 0; }
                    }
                } else {
                    qtBoss.deathTimer -= dt; qtBoss.rotation += 600.0f * dt;
                    if (qtBoss.deathTimer <= 0) { qtBoss.active = false; qtBoss.dying = false; victoryTimer = 15.0f; }
                }
            }

            // Логика Босса Вани
            if (vanyaBoss.active) {
                if (!vanyaBoss.dying) {
                    vanyaBoss.pos.x += vanyaBoss.speed * dt;
                    if (vanyaBoss.pos.x > 700 || vanyaBoss.pos.x < 100) {
                        vanyaBoss.speed *= -1;
                    }

                    vanyaBoss.spitAnimTimer += dt;
                    if (vanyaBoss.spitAnimTimer >= 0.7f) {
                        vanyaBoss.spitAnimTimer = 0;
                        vanyaBoss.spitAnimFrame++;
                        if (vanyaBoss.spitAnimFrame > 2) {
                            vanyaBoss.spitAnimFrame = 0;
                        }
                        if (vanyaBoss.spitAnimFrame == 2) {
                            Vector2 spitDir = Vector2Normalize(Vector2Subtract(pos, vanyaBoss.pos));
                            bullets.push_back({ vanyaBoss.pos, Vector2Scale(spitDir, 500.0f), true, true, false });
                            PlaySound(vanyaSpitSnds[GetRandomValue(0, (int)vanyaSpitSnds.size() - 1)]);
                        }
                    }
                } else {
                    vanyaBoss.deathTimer -= dt;
                    if (vanyaBoss.deathTimer <= 0) {
                        vanyaBoss.active = false;
                        vanyaBoss.dying = false;
                        victoryTimer = 15.0f;
                    }
                }
            }

            // Мяч-задница
            if (qtBall.active) {
                qtBall.pos = Vector2Add(qtBall.pos, Vector2Scale(qtBall.speed, dt));
                if (qtBall.pos.x < 50 || qtBall.pos.x > 750) qtBall.speed.x *= -0.9f;
                if (qtBall.pos.y < 50 || qtBall.pos.y > 850) qtBall.speed.y *= -0.9f;
                if (CheckCollisionCircles(pos, 30, qtBall.pos, 40)) { health -= 10; hitTimer = 0.2f; qtBall.speed = Vector2Scale(Vector2Normalize(Vector2Subtract(qtBall.pos, pos)), 400); }
                if (qtBoss.active && CheckCollisionCircles(qtBoss.pos, 50, qtBall.pos, 40)) { qtBoss.health -= 1; qtBall.speed = Vector2Scale(Vector2Normalize(Vector2Subtract(qtBall.pos, qtBoss.pos)), 400); }
            }

            // Враги (не спавнятся, если активен босс 1-го уровня или Ваня)
            if (!(level == 1 && qtBoss.active) && !vanyaBoss.active) {
                for (auto& e : enemies) {
                    if (!e.active) {
                        bool fat = (GetRandomValue(1, 4) == 1); float ang = (float)GetRandomValue(-30, 30) * DEG2RAD;
                        e.pos = {(float)GetRandomValue(0, 800), (float)GetRandomValue(-500, -50)};
                        e.speed = {sinf(ang) * 150.0f, cosf(ang) * (200.0f + (level * 50))};
                        e.hp = fat ? 3 : 1; e.isFat = fat; e.active = true;
                    }
                    e.pos = Vector2Add(e.pos, Vector2Scale(e.speed, dt));
                    if (e.pos.y > 950) e.active = false;
                    if (e.active && CheckCollisionCircles(pos, 30, e.pos, 25)) { health -= 25; hitTimer = 0.2f; e.active = false; PlaySound(hitSnd); }
                }
            }

            // Пули и коллизии
            for (auto& b : bullets) {
                if (!b.active) continue;
                b.pos = Vector2Add(b.pos, Vector2Scale(b.speed, dt));
                if (b.pos.y < -50 || b.pos.y > 950) b.active = false;
                if (qtBall.active && CheckCollisionCircles(b.pos, 10, qtBall.pos, 40)) { qtBall.speed = Vector2Add(qtBall.speed, Vector2Scale(b.speed, 0.4f)); b.active = false; }

                if (!b.fromBoss && qtBoss.active && !qtBoss.dying && CheckCollisionCircles(b.pos, 10, qtBoss.pos, 60)) {
                    qtBoss.health -= (level == 3 ? 1 : 5); b.active = false;
                    if (qtBoss.health <= 0) { qtBoss.dying = true; qtBoss.deathTimer = 2.0f; PlaySound(bossDeadSnd); PlaySound(missionWinSnd); }
                }
                if (!b.fromBoss && vanyaBoss.active && !vanyaBoss.dying && CheckCollisionCircles(b.pos, 10, vanyaBoss.pos, 100)) {
                    vanyaBoss.health -= 5; b.active = false;
                    if (vanyaBoss.health <= 0) {
                        vanyaBoss.dying = true;
                        vanyaBoss.deathTimer = 2.0f;
                        PlaySound(bossDeadSnd);
                        PlaySound(missionWinSnd);
                        PlaySound(vanyaDeathSnd);
                    }
                }

                if (b.fromBoss && CheckCollisionCircles(b.pos, 10, pos, 30)) { health -= (level == 4 ? 20 : (level == 3 ? 10 : 75)); b.active = false; PlaySound(hitSnd); }
                if (!b.fromBoss) for (auto& e : enemies) if (e.active && CheckCollisionCircles(b.pos, 10, e.pos, 30)) { e.hp--; b.active = false; if (e.hp <= 0) { e.active = false; score += e.isFat ? 30 : 10; } break; }
                if (!b.fromBoss) for (auto& piv : pivos) if (piv.active && CheckCollisionCircles(b.pos, 10, piv.pos, 25)) { piv.active = false; b.active = false; PlaySound(breakSnd); break; }
            }

            // Предметы
            if (GetRandomValue(1, 400) == 1) pivos.push_back({{(float)GetRandomValue(50, 750), -50}, true});
            for (auto& piv : pivos) if (piv.active) { piv.pos.y += 200 * dt; if (CheckCollisionCircles(pos, 30, piv.pos, 25)) { if (pivoStacks < 2) { pivoStacks++; pivoTimer = 20.0f; PlaySound(pivoSnd); } piv.active = false; } }

            if (GetRandomValue(1, 200) == 1) packs.push_back({{(float)GetRandomValue(50, 750), -50}, true});
            for (auto& p : packs) if (p.active) { p.pos.y += 200 * dt; if (CheckCollisionCircles(pos, 30, p.pos, 20)) { health = std::min(100, health+25); p.active = false; PlaySound(healSnd); } }

            if (GetRandomValue(1, 300) == 1) stars.push_back({{(float)GetRandomValue(50, 750), -50}, true});
            for (auto& s : stars) if (s.active) { s.pos.y += 200 * dt; if (CheckCollisionCircles(pos, 30, s.pos, 20)) { if (starCount < 5) { starCount++; PlaySound(healSnd); } s.active = false; } }
        }

        if (health <= 0 && starCount > 0 && !isHacking) {
            isHacking = true; starCount--; health = 0;
            hackTimer = 10.0f; hackStartDelay = 0.5f; inputBuffer = "";
            currentTargetCode = cheatPool[GetRandomValue(0, (int)cheatPool.size() - 1)];
            PlaySound(alarmSnd); PlaySound(hesoyamSnd);
        }

        if (isHacking) {
            if (hackStartDelay > 0) hackStartDelay -= dt;
            else {
                hackTimer -= dt;
                for (int k = 65; k <= 90; k++) if (IsKeyPressed(k)) inputBuffer += (char)k;
                if (IsKeyPressed(KEY_BACKSPACE) && !inputBuffer.empty()) inputBuffer.pop_back();

                if (inputBuffer.find(currentTargetCode) != std::string::npos) {
                    health = 100; isHacking = false;
                    StopSound(alarmSnd); StopSound(hesoyamSnd);
                }
            }

            if (hackTimer <= 0) {
                if (starCount > 0) {
                    starCount--; hackTimer = 10.0f; inputBuffer = "";
                    currentTargetCode = cheatPool[GetRandomValue(0, (int)cheatPool.size() - 1)];
                } else {
                    isHacking = false;
                    StopSound(alarmSnd); StopSound(hesoyamSnd);
                }
            }
        }

        if (victoryTimer > 0) {
            victoryTimer -= dt;
            if (victoryTimer <= 0) {
                level++; score = 0;
                qtBoss.active = false; vanyaBoss.active = false; qtBall.active = false;
                bullets.clear(); enemies.clear();
                SpawnEnemies(12);
            }
        }

        // --- Отрисовка ---
        BeginDrawing();
        ClearBackground({30, 30, 35, 255});

        if (qtBoss.active) {
            if (!(qtBoss.dying && ((int)(GetTime() * 15) % 2 == 0))) {
                rlPushMatrix(); rlTranslatef(qtBoss.pos.x, qtBoss.pos.y, 0); rlRotatef(qtBoss.rotation, 0, 0, 1);
                if (level < 3) { DrawEllipse(-45, 0, 55, 65, PINK); DrawEllipse(45, 0, 55, 65, PINK); DrawText("QT", -35, -20, 50, BLACK); }
                else { Color sCol = {20, 20, 80, 255}; DrawCircle(-20, 10, 20, sCol); DrawCircle(20, 10, 20, sCol); DrawRectangle(-15, -60, 30, 70, sCol); DrawCircle(0, -60, 18, BLACK); }
                rlPopMatrix();
            }
            if (!qtBoss.dying) DrawRectangle(200, 70, (int)(qtBoss.health * (level == 3 ? 4 : 1.11f)), 12, VIOLET);
        }

        if (vanyaBoss.active) {
            if (!(vanyaBoss.dying && ((int)(GetTime() * 15) % 2 == 0))) {
                Texture2D currentFrame = vanyaFrames[vanyaBoss.spitAnimFrame];
                float sourceRecWidth = (vanyaBoss.speed < 0) ? -(float)currentFrame.width : (float)currentFrame.width;
                Rectangle sourceRec = { 0.0f, 0.0f, sourceRecWidth, (float)currentFrame.height };
                float scale = 0.56f;
                Rectangle destRec = { vanyaBoss.pos.x, vanyaBoss.pos.y, currentFrame.width * scale, currentFrame.height * scale };
                DrawTexturePro(currentFrame, sourceRec, destRec, { destRec.width / 2, destRec.height / 2 }, 0, WHITE);
            }
            if (!vanyaBoss.dying) {
                DrawRectangle(200, 70, (int)(vanyaBoss.health / 1200.0f * 400.0f), 12, VIOLET);
            }
        }

        if (qtBall.active) { DrawEllipse(qtBall.pos.x - 35, qtBall.pos.y, 40, 45, GRAY); DrawEllipse(qtBall.pos.x + 35, qtBall.pos.y, 40, 45, GRAY); DrawText("QT", qtBall.pos.x - 20, qtBall.pos.y - 15, 30, BLACK); }

        if (health > 0 || isHacking) {
            Color bodyColor = (hitTimer > 0 && !isHacking) ? RED : (isHacking ? GOLD : (pivoStacks > 0 ? ORANGE : PINK));
            DrawCircle(pos.x-20, pos.y+10, 20, bodyColor); DrawCircle(pos.x+20, pos.y+10, 20, bodyColor);
            DrawRectangle(pos.x-15, pos.y-60, 30, 70, bodyColor); DrawCircle(pos.x, pos.y-60, 18, VIOLET);
        }

        for (auto& b : bullets) {
            if (b.active) {
                Color c = YELLOW;
                if (b.fromBoss) {
                    if (level == 4) c = WHITE;
                    else c = b.isShadow ? SKYBLUE : BROWN;
                } else if (pivoStacks > 0) {
                    c = ORANGE;
                }
                DrawCircle(b.pos.x, b.pos.y, b.fromBoss && level == 4 ? 12 : (b.isShadow ? 6 : (b.fromBoss ? 16 : 6)), c);
            }
        }

        if (health <= 0 && !isHacking) {
            if (!playerIsDead) {
                PlaySound(playerDeadSnd);
                playerIsDead = true;
            }
            DrawText("RUNTIME ERROR", 250, 450, 40, RED);
        }

        for (auto& e : enemies) if (e.active) DrawText(e.isFat ? "C" : "C++", e.pos.x-20, e.pos.y-15, 35, BROWN);
        for (auto& piv : pivos) if (piv.active) DrawTextureEx(pivoTex, {piv.pos.x-20, piv.pos.y}, 0, pivoScale, WHITE);
        for (auto& p : packs) if (p.active) DrawText("+", p.pos.x-10, p.pos.y-15, 40, GREEN);
        for (auto& s : stars) if (s.active) DrawText("*", s.pos.x-10, s.pos.y-15, 50, GOLD);

        // --- Интерфейс ---
        DrawRectangle(10, 10, health*2, 25, GREEN);
        for(int i=0; i<starCount; i++) DrawText("*", 10+(i*35), 40, 60, GOLD);
        if (pivoStacks > 0) DrawText(TextFormat("BEER x%i: %.1fs", pivoStacks, pivoTimer), 10, 100, 25, ORANGE);
        DrawText(TextFormat("LVL: %i SCORE: %i", level, score), 500, 12, 20, WHITE);

        if (victoryTimer > 0) {
            DrawText("SUCCESSFUL REFACTORING!", 120, 400, 40, GREEN);
            DrawText(TextFormat("Next level in: %.1fs", victoryTimer), 250, 450, 30, WHITE);
        }
        if (isHacking) {
            DrawRectangle(0, 0, 800, 900, {0, 0, 0, 180});
            DrawText("!!! VPISHI KOD !!!", 220, 250, 40, GREEN);
            DrawText(TextFormat("%.1fs", hackTimer), 360, 320, 50, RED);
            Vector2 tPos = { 400 - (MeasureTextEx(GetFontDefault(), currentTargetCode.c_str(), 80, 10).x / 2), 380 };
            DrawTextEx(GetFontDefault(), currentTargetCode.c_str(), tPos, 80, 10, {100, 100, 100, 150});
            DrawTextEx(GetFontDefault(), inputBuffer.substr(0, currentTargetCode.length()).c_str(), tPos, 80, 10, WHITE);
        }
        EndDrawing();
    }

    // --- Очистка ресурсов ---
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
