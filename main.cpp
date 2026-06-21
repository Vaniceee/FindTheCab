#include "raylib.h"
#include "Player.h"
#include "GameMap.h" // Підключаємо нашу мапу
#include "NPC.h"
#include <vector>

int main() {
    // Налаштування вікна з вашого коду
    SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_HIGHDPI);

    const int windowWidth = 800;
    const int windowHeight = 600;
    //const float cameraZoom = 1.5f;

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
    Player player({ 1400.0f, 2000.0f });
    

    // NPCs
    std::vector<NPC> npcs;

    npcs.emplace_back(
        Vector2{ 1320.0f, 1920.0f },
        "The library is upstairs."
    );

    npcs.emplace_back(
        Vector2{ 1714.0f, 1760.0f },
        "The cafeteria is on the left."
    );

    
    Camera2D camera = { 0 };
    camera.target = player.GetPosition();
    camera.offset = Vector2{ windowWidth / 2.0f, windowHeight / 2.0f }; // Центрування на екрані
    camera.rotation = 0.0f;
    camera.zoom = 1.8f;

    std::string currentDialogue = "";
    float dialogueTimer = 0.0f;
    NPC* activeNPC = nullptr;

    while (!WindowShouldClose()) {

        float deltaTime = GetFrameTime();

        if (dialogueTimer > 0.0f)
        {
            dialogueTimer -= deltaTime;
        }

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

        for (auto& npc : npcs)
        {
            npc.Draw();
        }

        if (activeNPC != nullptr)
        {
            for (auto& npc : npcs)
            {
                if (npc.IsPlayerNear(player.GetPosition()))
                {
                    DrawText(
                        "Press E to talk",
                        npc.position.x - 40,
                        npc.position.y - 30,
                        12,
                        YELLOW
                    );

                    if (IsKeyPressed(KEY_E))
                    {
                        currentDialogue = npc.dialogue;
                        dialogueTimer = 2.0f; // show for 2 seconds
                    }
                }
            }
        }

        activeNPC = nullptr;

        for (auto& npc : npcs)
        {
            if (npc.IsPlayerNear(player.GetPosition()))
            {
                activeNPC = &npc;
                break; // IMPORTANT: only one NPC can be active
            }
        }
        
        gameMap.DrawAbovePlayer();

        for (auto& npc : npcs)
        {
            if (dialogueTimer > 0.0f && activeNPC != nullptr)
            {
                DrawText(
                    currentDialogue.c_str(),
                    npc.position.x - 60,
                    npc.position.y - 60,
                    12,
                    WHITE
                );
            }
        }

        DrawText("Use WASD/Arrows to Move", 1200, 2130, 15, WHITE);
        DrawText("Hold SHIFT to run", 1200, 2150, 10, WHITE);

        

        EndMode2D();
        // Тестовий текст поверх екрана
        DrawText(TextFormat("Player Pos: X: %.1f, Y: %.1f", player.GetPosition().x, player.GetPosition().y), 15, 50, 20, RED);
     
        DrawText("Screen bounds clamped safely!", 15, windowHeight - 10, 16, RAYWHITE);
        
        EndDrawing();
    }

    // 3. Вивантажуємо ресурси мапи при закритті
    gameMap.Unload();
    CloseWindow();

    return 0;
}