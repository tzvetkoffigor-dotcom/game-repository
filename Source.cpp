#define _CRT_SECURE_NO_WARNINGS

#include "raylib.h"
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>

using namespace std;

// Структура для хранения рекордов
struct HighScore {
    string playerName;
    int score;
};

// Глобальные переменные
vector<HighScore> highScores;
string currentPlayer = "Player";
int screenWidth = 400;
int screenHeight = 600;

// Прототипы функций
void SaveHighScores();
void LoadHighScores();
void ShowMainMenu();
void ShowPlayerMenu();
void ShowHighScores();
void PlayGame();
void DrawButton(const char* text, int y, bool hovered);
void AddHighScore(int score);

// Загрузка рекордов при запуске
void LoadHighScores() {
    highScores.clear();

    ifstream file("highscores.txt", ios::binary);
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            if (line.size() >= 3 &&
                (unsigned char)line[0] == 0xEF &&
                (unsigned char)line[1] == 0xBB &&
                (unsigned char)line[2] == 0xBF) {
                line = line.substr(3);
            }

            size_t commaPos = line.find(',');
            if (commaPos != string::npos) {
                HighScore hs;
                hs.playerName = line.substr(0, commaPos);
                try {
                    hs.score = stoi(line.substr(commaPos + 1));
                    highScores.push_back(hs);
                }
                catch (...) {
                    // Игнорируем некорректные строки
                }
            }
        }
        file.close();
    }

    sort(highScores.begin(), highScores.end(),
        [](const HighScore& a, const HighScore& b) {
            return a.score > b.score;
        });
}

// Сохранение рекордов
void SaveHighScores() {
    ofstream file("highscores.txt", ios::binary);
    if (file.is_open()) {
        file << "\xEF\xBB\xBF";

        for (const auto& hs : highScores) {
            file << hs.playerName << "," << hs.score << endl;
        }
        file.close();
    }
}

// Добавление нового рекорда
void AddHighScore(int score) {
    bool found = false;
    for (auto& hs : highScores) {
        if (hs.playerName == currentPlayer) {
            if (score > hs.score) {
                hs.score = score;
            }
            found = true;
            break;
        }
    }

    if (!found) {
        HighScore newScore;
        newScore.playerName = currentPlayer;
        newScore.score = score;
        highScores.push_back(newScore);
    }

    sort(highScores.begin(), highScores.end(),
        [](const HighScore& a, const HighScore& b) {
            return a.score > b.score;
        });

    SaveHighScores();
}

// Отрисовка кнопки
void DrawButton(const char* text, int y, bool hovered) {
    Rectangle button = { screenWidth / 2 - 100, y, 200, 40 };
    Color buttonColor = hovered ? BLUE : DARKBLUE;
    Color textColor = WHITE;

    DrawRectangleRec(button, buttonColor);
    DrawRectangleLinesEx(button, 2, BLACK);

    int textWidth = MeasureText(text, 20);
    DrawText(text, screenWidth / 2 - textWidth / 2, y + 10, 20, textColor);
}

// Главное меню
void ShowMainMenu() {
    int selectedButton = 0;
    const int buttonCount = 4;
    const char* buttonTexts[] = { "START GAME", "CHANGE PLAYER", "HIGH SCORES", "EXIT" };

    while (!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition();

        selectedButton = -1;
        for (int i = 0; i < buttonCount; i++) {
            Rectangle button = { screenWidth / 2 - 100, 200 + i * 60, 200, 40 };
            if (CheckCollisionPointRec(mousePos, button)) {
                selectedButton = i;
                break;
            }
        }

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            if (selectedButton == 0) {
                PlayGame();
            }
            else if (selectedButton == 1) {
                ShowPlayerMenu();
            }
            else if (selectedButton == 2) {
                ShowHighScores();
            }
            else if (selectedButton == 3) {
                break;
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("DOODLE JUMP", screenWidth / 2 - MeasureText("DOODLE JUMP", 40) / 2, 100, 40, DARKBLUE);
        DrawText(TextFormat("Player: %s", currentPlayer.c_str()), 10, 10, 20, DARKGRAY);

        for (int i = 0; i < buttonCount; i++) {
            DrawButton(buttonTexts[i], 200 + i * 60, selectedButton == i);
        }

        EndDrawing();
    }
}

// Меню смены игрока
void ShowPlayerMenu() {
    string playerName = currentPlayer;
    bool textEditMode = true;

    while (!WindowShouldClose()) {
        if (textEditMode) {
            int key = GetCharPressed();
            while (key > 0) {
                if ((key >= 32 && key <= 126) || (key >= 1040 && key <= 1103) || key == 1025 || key == 1105) {
                    if (playerName.length() < 50) {
                        playerName += (char)key;
                    }
                }
                key = GetCharPressed();
            }

            if (IsKeyPressed(KEY_BACKSPACE) && !playerName.empty()) {
                playerName.pop_back();
            }
        }

        if (IsKeyPressed(KEY_ENTER)) {
            if (!playerName.empty()) {
                currentPlayer = playerName;
                SaveHighScores();
            }
            break;
        }

        if (IsKeyPressed(KEY_ESCAPE)) {
            break;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("ENTER PLAYER NAME", screenWidth / 2 - MeasureText("ENTER PLAYER NAME", 30) / 2, 150, 30, DARKBLUE);

        Rectangle textBox = { screenWidth / 2 - 150, 200, 300, 40 };
        DrawRectangleRec(textBox, LIGHTGRAY);
        DrawRectangleLinesEx(textBox, 2, textEditMode ? BLUE : DARKGRAY);
        DrawText(playerName.c_str(), textBox.x + 5, textBox.y + 10, 20, MAROON);

        DrawText("Press ENTER to save", screenWidth / 2 - MeasureText("Press ENTER to save", 20) / 2, 260, 20, DARKGRAY);
        DrawText("ESC to cancel", screenWidth / 2 - MeasureText("ESC to cancel", 20) / 2, 290, 20, DARKGRAY);

        EndDrawing();
    }
}

// Таблица рекордов
void ShowHighScores() {
    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_ESCAPE) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            break;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("HIGH SCORES", screenWidth / 2 - MeasureText("HIGH SCORES", 30) / 2, 50, 30, DARKBLUE);

        DrawText("RANK", 50, 120, 20, DARKBLUE);
        DrawText("PLAYER", 150, 120, 20, DARKBLUE);
        DrawText("SCORE", 300, 120, 20, DARKBLUE);

        for (int i = 0; i < min(10, (int)highScores.size()); i++) {
            Color rowColor = (highScores[i].playerName == currentPlayer) ? BLUE : BLACK;

            DrawText(TextFormat("%d", i + 1), 50, 150 + i * 30, 20, rowColor);
            DrawText(highScores[i].playerName.c_str(), 150, 150 + i * 30, 20, rowColor);
            DrawText(TextFormat("%d", highScores[i].score), 300, 150 + i * 30, 20, rowColor);
        }

        if (highScores.empty()) {
            DrawText("No high scores yet!", screenWidth / 2 - MeasureText("No high scores yet!", 20) / 2, 200, 20, GRAY);
        }

        DrawText("Press ESC to return", screenWidth / 2 - MeasureText("Press ESC to return", 20) / 2, 500, 20, DARKGRAY);

        EndDrawing();
    }
}

// Игровая логика - узкие платформы и большие расстояния
void PlayGame() {
    struct Platform {
        float x, y;
        float width = 45;  // ?? УЗКИЕ ПЛАТФОРМЫ (было 70)
        float height = 12; // ?? ТОНЬШЕ (было 15)
        Color color = GREEN;
    };

    struct Player {
        float x = 185, y = 285;
        float width = 30, height = 30;
        float velocity = 0;
        Color color = RED;
        bool onGround = false;
    };

    Player player;
    vector<Platform> platforms;
    srand(time(0));

    Platform startPlatform;
    startPlatform.x = 0;
    startPlatform.y = 550;
    startPlatform.width = screenWidth;
    startPlatform.height = 20;
    startPlatform.color = DARKGREEN;
    platforms.push_back(startPlatform);

    player.x = screenWidth / 2 - player.width / 2;
    player.y = startPlatform.y - player.height;

    // ?? УВЕЛИЧЕННЫЕ РАССТОЯНИЯ МЕЖДУ ПЛАТФОРМАМИ
    const float MAX_JUMP_HEIGHT = 150.0f;   // Увеличил высоту прыжка
    const float PLATFORM_SPACING = 110.0f;  // ?? БОЛЬШЕ РАССТОЯНИЕ (было 90)
    const int VISIBLE_PLATFORMS = 8;

    float currentY = 450;

    for (int i = 0; i < VISIBLE_PLATFORMS; i++) {
        Platform platform;

        // ?? АДАПТИРОВАННЫЕ ПОЗИЦИИ ДЛЯ УЗКИХ ПЛАТФОРМ
        if (i % 3 == 0) {
            platform.x = 50; // Левая сторона
        }
        else if (i % 3 == 1) {
            platform.x = screenWidth / 2 - platform.width / 2; // Центр
        }
        else {
            platform.x = screenWidth - 50 - platform.width; // Правая сторона
        }

        platform.y = currentY;
        currentY -= PLATFORM_SPACING;
        platform.color = GREEN;

        platforms.push_back(platform);
    }

    int score = 0;
    bool gameOver = false;
    float cameraOffset = 0;

    // ?? СБАЛАНСИРОВАННАЯ ФИЗИКА ДЛЯ БОЛЬШИХ РАССТОЯНИЙ
    const float MAX_JUMP_FORCE = -14.0f;    // Увеличил силу прыжка
    const float MIN_JUMP_FORCE = -7.0f;
    const float GRAVITY = 0.5f;
    const float MAX_FALL_SPEED = 18.0f;     // Увеличил скорость падения
    const float MOVE_SPEED = 6.0f;          // Увеличил скорость движения

    // Переменные для прыжка
    float jumpPressDuration = 0.0f;
    const float MAX_PRESS_DURATION = 0.2f;
    bool isPressingJump = false;
    bool autoJumped = false;

    while (!WindowShouldClose() && !gameOver) {
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
            player.x -= MOVE_SPEED;
        }
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
            player.x += MOVE_SPEED;
        }

        // СИСТЕМА ПРЫЖКА
        if (player.onGround) {
            if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
                isPressingJump = true;
                jumpPressDuration = 0.0f;
                autoJumped = false;
            }

            if (IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
                if (isPressingJump && !autoJumped) {
                    jumpPressDuration += GetFrameTime() * 3.0f;

                    if (jumpPressDuration >= MAX_PRESS_DURATION) {
                        jumpPressDuration = MAX_PRESS_DURATION;

                        float jumpForce = MIN_JUMP_FORCE + (MAX_JUMP_FORCE - MIN_JUMP_FORCE) * 1.0f;
                        player.velocity = jumpForce;
                        player.onGround = false;
                        isPressingJump = false;
                        autoJumped = true;
                    }
                }
            }

            if (IsKeyReleased(KEY_SPACE) || IsKeyReleased(KEY_UP) || IsKeyReleased(KEY_W)) {
                if (isPressingJump && !autoJumped) {
                    float powerRatio = jumpPressDuration / MAX_PRESS_DURATION;

                    if (powerRatio < 0.1f) {
                        powerRatio = 0.1f;
                    }
                    else if (powerRatio > 0.9f) {
                        powerRatio = 1.0f;
                    }

                    float jumpForce = MIN_JUMP_FORCE + (MAX_JUMP_FORCE - MIN_JUMP_FORCE) * powerRatio;
                    player.velocity = jumpForce;
                    player.onGround = false;

                    isPressingJump = false;
                    jumpPressDuration = 0.0f;
                }
            }
        }
        else {
            isPressingJump = false;
            jumpPressDuration = 0.0f;
            autoJumped = false;
        }

        if (player.x < -player.width) player.x = screenWidth;
        if (player.x > screenWidth) player.x = -player.width;

        player.velocity += GRAVITY;

        if (player.velocity > MAX_FALL_SPEED) {
            player.velocity = MAX_FALL_SPEED;
        }

        player.y += player.velocity;

        player.onGround = false;

        for (auto& platform : platforms) {
            bool horizontallyOnPlatform = (player.x + player.width > platform.x) &&
                (player.x < platform.x + platform.width);

            bool verticallyOnPlatform = (player.y + player.height >= platform.y) &&
                (player.y + player.height <= platform.y + platform.height) &&
                (player.velocity >= 0);

            if (horizontallyOnPlatform && verticallyOnPlatform) {
                player.y = platform.y - player.height;
                player.velocity = 0;
                player.onGround = true;
                break;
            }
        }

        if (player.y < 200) {
            float delta = 200 - player.y;
            cameraOffset += delta;

            for (auto& platform : platforms) {
                platform.y += delta;
            }

            player.y += delta;
            score += (int)delta;
        }

        // Генерация новых платформ с узкими настройками
        if (platforms.back().y > -100) {
            Platform newPlatform;

            float lastPlatformY = platforms.back().y;

            static int patternIndex = 0;
            patternIndex = (patternIndex + 1) % 6;

            // ?? АДАПТИРОВАННЫЕ ПОЗИЦИИ ДЛЯ УЗКИХ ПЛАТФОРМ
            switch (patternIndex) {
            case 0: newPlatform.x = 40; break;                       // Лево
            case 1: newPlatform.x = screenWidth / 2 - 22; break;      // Центр
            case 2: newPlatform.x = screenWidth - 40 - newPlatform.width; break; // Право
            case 3: newPlatform.x = 80; break;                       // Лево-центр
            case 4: newPlatform.x = screenWidth - 120 - newPlatform.width; break; // Право-центр
            case 5: newPlatform.x = screenWidth / 2 - 22; break;      // Центр
            }

            newPlatform.y = lastPlatformY - PLATFORM_SPACING;
            newPlatform.color = GREEN;

            platforms.push_back(newPlatform);
        }

        if (!platforms.empty() && platforms[0].y > 800) {
            platforms.erase(platforms.begin());
        }

        // Проверка смерти
        if (player.y > 600) {
            if (player.velocity > 0) {
                gameOver = true;
                AddHighScore(score);
            }
        }

        if (player.y > 650) {
            gameOver = true;
            AddHighScore(score);
        }

        BeginDrawing();
        ClearBackground(BLUE);

        // ?? ОТРИСОВКА УЗКИХ ПЛАТФОРМ С ЯРКИМИ КОНТУРАМИ
        for (auto& platform : platforms) {
            DrawRectangle(platform.x, platform.y, platform.width, platform.height, platform.color);
            // ЯРКИЕ КОНТУРЫ ДЛЯ ЛУЧШЕЙ ВИДИМОСТИ УЗКИХ ПЛАТФОРМ
            DrawRectangleLines(platform.x, platform.y, platform.width, platform.height, BLACK);
            DrawRectangleLines(platform.x + 1, platform.y + 1, platform.width - 2, platform.height - 2, WHITE);
            DrawRectangleLines(platform.x + 2, platform.y + 2, platform.width - 4, platform.height - 4, platform.color);
        }

        DrawRectangle(player.x, player.y, player.width, player.height, player.color);
        DrawRectangleLines(player.x, player.y, player.width, player.height, BLACK);

        DrawText(TextFormat("Score: %d", score), 10, 10, 20, WHITE);
        DrawText(TextFormat("Player: %s", currentPlayer.c_str()), 10, 40, 20, WHITE);

        // ?? ОТЛАДОЧНАЯ ИНФОРМАЦИЯ О РАЗМЕРАХ
        DrawText(TextFormat("Platforms: %d", platforms.size()), 10, 70, 15, YELLOW);
        DrawText(TextFormat("Platform W: %.0f", platforms[1].width), 10, 90, 15, YELLOW);
        DrawText(TextFormat("Spacing: %.0f", PLATFORM_SPACING), 10, 110, 15, YELLOW);

        if (player.onGround && player.y + player.height == startPlatform.y) {
            DrawText("A/D - Move", screenWidth / 2 - MeasureText("A/D - Move", 18) / 2, 520, 18, WHITE);
            DrawText("SPACE - Precision jumps!", screenWidth / 2 - MeasureText("SPACE - Precision jumps!", 18) / 2, 545, 18, WHITE);
        }

        EndDrawing();
    }

    if (gameOver) {
        while (!WindowShouldClose()) {
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                break;
            }

            BeginDrawing();
            ClearBackground(BLACK);

            DrawText("GAME OVER", screenWidth / 2 - MeasureText("GAME OVER", 40) / 2, 200, 40, RED);
            DrawText(TextFormat("Final Score: %d", score), screenWidth / 2 - MeasureText(TextFormat("Final Score: %d", score), 30) / 2, 260, 30, WHITE);
            DrawText("Press SPACE to continue", screenWidth / 2 - MeasureText("Press SPACE to continue", 20) / 2, 320, 20, WHITE);

            EndDrawing();
        }
    }
}

// Главная функция
int main() {
    InitWindow(screenWidth, screenHeight, "Doodle Jump - Ultimate Edition");
    SetTargetFPS(60);

    LoadHighScores();
    ShowMainMenu();

    CloseWindow();
    return 0;
}