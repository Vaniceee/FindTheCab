/* #include "raylib.h"
#include "Player.h"

int main() {
    // Explicitly configure your window sizes here
    FLAG_WINDOW_UNDECORATED;
    FLAG_WINDOW_HIGHDPI;
    SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_HIGHDPI);

    const int windowWidth = 1280;
    const int windowHeight = 720;

    InitWindow(windowWidth, windowHeight, "Find the Cab!");
    SetWindowPosition(0, 0);
    SetTargetFPS(60);

    // Spawns the player right in the dead center of the screen size bounds
    Player player({ (float)windowWidth / 2.0f, (float)windowHeight / 2.0f });

    while (!WindowShouldClose()) {

        // Pass window constraints straight to the update loop pipeline
        player.Update(windowWidth, windowHeight);

        BeginDrawing();
        ClearBackground(DARKGRAY);

        player.Draw();

        DrawText("Use WASD/Arrows to Move. Hold LEFT SHIFT to Run.", 15, 15, 20, LIGHTGRAY);
        DrawText("Screen bounds clamped safely!", 15, windowHeight - 30, 16, RAYWHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
} */

#include "raylib.h"
#include "Player.h"
#include "GameMap.h" // Підключаємо нашу мапу

int main() {
    // Налаштування вікна з вашого коду
    SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_HIGHDPI);

    const int windowWidth = 1280;
    const int windowHeight = 720;

    InitWindow(windowWidth, windowHeight, "Find the Cab!");
    SetWindowPosition(0, 0);
    SetTargetFPS(60);

    // 1. Створюємо та завантажуємо мапу
    GameMap gameMap;
    // УВАГА: Перевірте назву папки (assets чи resources) та шлях до файлів!
    if (!gameMap.Load("assets/version2.tmj", "assets/version2.png")) {
        CloseWindow();
        return -1;
    }

    // Створення гравця по центру екрана (з вашого коду)
    // Player player({ (float)windowWidth / 2.0f, (float)windowHeight / 2.0f });
    Player player({ 1120.0f, 600.0f });
    Camera2D camera = { 0 };
    camera.target = player.GetPosition();
    camera.offset = Vector2{ windowWidth / 2.0f, windowHeight / 2.0f }; // Центрування на екрані
    camera.rotation = 0.0f;
    camera.zoom = 2.0f;

    while (!WindowShouldClose()) {


        Vector2 startPos = player.GetPosition();

        // Крок 2: Викликаємо твій стандартний Update (гравець змінює і X, і Y)
        player.Update(windowWidth, windowHeight);
        Vector2 desiredPos = player.GetPosition(); // Куди гравець ХОЧЕ наступити

        // Крок 3: Перевіряємо рух СУТО по осі X
        // Тимчасово ставимо гравцю новий X, але повертаємо старий безпечний Y
        player.SetPosition({ desiredPos.x, startPos.y });
        if (gameMap.CheckWallCollision(player.GetHitbox())) {
            // Якщо врізалися боком — скасовуємо рух по X (повертаємо старий X)
            player.SetPosition({ startPos.x, startPos.y });
        }

        // Крок 4: Перевіряємо рух СУТО по осі Y
        // Фіксуємо позицію, яка вийшла після кроку X, і додаємо туди новий бажаний Y
        Vector2 posAfterX = player.GetPosition();
        player.SetPosition({ posAfterX.x, desiredPos.y });
        if (gameMap.CheckWallCollision(player.GetHitbox())) {
            // Якщо врізалися верхом/низом — скасовуємо рух по Y (повертаємо Y, що був до кроку по Y)
            player.SetPosition({ posAfterX.x, startPos.y });
        }

        // -----------------------------------------------------------------------

        camera.target = player.GetPosition();

        // --- МАЛЮВАННЯ (Порядок шарів дуже важливий!) ---
        BeginDrawing();
        ClearBackground(BLACK);

        BeginMode2D(camera);

        gameMap.DrawBelowPlayer();
        player.Draw();
        gameMap.DrawAbovePlayer();

        EndMode2D();
        // Тестовий текст поверх екрана
        DrawText(TextFormat("Player Pos: X: %.1f, Y: %.1f", player.GetPosition().x, player.GetPosition().y), 15, 50, 20, RED);

        // Тексти інтерфейсу (малюються поверх усього)
        DrawText("Use WASD/Arrows to Move. Hold LEFT SHIFT to Run.", 15, 15, 20, LIGHTGRAY);
        DrawText("Screen bounds clamped safely!", 15, windowHeight - 30, 16, RAYWHITE);

        EndDrawing();
    }

    // 3. Вивантажуємо ресурси мапи при закритті
    gameMap.Unload();
    CloseWindow();

    return 0;
}