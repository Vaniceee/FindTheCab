#include "raylib.h"
#include "Player.h"
#include "GameMap.h" // Підключаємо нашу мапу
#include "NPC.h"
#include <vector>

int main() {
    // Налаштування вікна з вашого коду
    SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_HIGHDPI);

    const int windowWidth = 1920;
    const int windowHeight = 1080;

    InitWindow(windowWidth, windowHeight, "Find the Cab!");
    SetWindowPosition(0, 0);
    SetTargetFPS(60);

    // 1. Створюємо та завантажуємо мапу
    GameMap gameMap;
    if (!gameMap.Load("assets/version2.tmj", "assets/version2.png")) {
        CloseWindow();
        return -1;
    }

    // Завантажуємо текстуру для всіх NPC один раз
    Texture2D npcTexture = LoadTexture("assets/16x16 Idle.png");
    SetTextureFilter(npcTexture, TEXTURE_FILTER_POINT);

    // Створення гравця
    Player player({ 1400.0f, 2000.0f });

    // Масив наших NPC
    std::vector<NPC> npcs;

    npcs.emplace_back(
        Vector2{ 1270.0f, 1920.0f },
        "The library is upstairs.",
        &npcTexture
    );

    npcs.emplace_back(
        Vector2{ 1724.0f, 1730.0f },
        "The cafeteria is on the left.",
        &npcTexture
    );

    Camera2D camera = { 0 };
    camera.target = player.GetPosition();
    camera.offset = Vector2{ windowWidth / 2.25f, windowHeight / 2.5f }; // Центрування на екрані
    camera.rotation = 0.0f;
    camera.zoom = 5.0f;

    std::string currentDialogue = "";
    float dialogueTimer = 0.0f;

    NPC* activeNPC = nullptr;      
    NPC* speakingNPC = nullptr;    

    while (!WindowShouldClose()) {

        float deltaTime = GetFrameTime();

        // Оновлюємо внутрішні таймери кадрів анімації для кожного NPC
        for (auto& npc : npcs)
        {
            npc.Update();
        }

        // Логіка таймера діалогу
        if (dialogueTimer > 0.0f)
        {
            dialogueTimer -= deltaTime;
            if (dialogueTimer <= 0.0f)
            {
                speakingNPC = nullptr; 
            }
        }

        Vector2 startPos = player.GetPosition();

        // Крок 2: Рух гравця
        player.Update(windowWidth, windowHeight);
        Vector2 desiredPos = player.GetPosition();

        // Крок 3: Перевіряємо рух по осі X
        player.SetPosition({ desiredPos.x, startPos.y });
        if (gameMap.CheckWallCollision(player.GetHitbox())) {
            player.SetPosition({ startPos.x, startPos.y });
        }

        // Крок 4: Перевіряємо рух по осі Y
        Vector2 posAfterX = player.GetPosition();
        player.SetPosition({ posAfterX.x, desiredPos.y });
        if (gameMap.CheckWallCollision(player.GetHitbox())) {
            player.SetPosition({ posAfterX.x, startPos.y });
        }

        // -----------------------------------------------------------------------

        camera.target = player.GetPosition();

        
        activeNPC = nullptr;
        for (auto& npc : npcs)
        {
            if (npc.IsPlayerNear(player.GetPosition()))
            {
                activeNPC = &npc;
                break; 
            }
        }

        // --- МАЛЮВАННЯ ---
        BeginDrawing();
        ClearBackground(BLACK);

        BeginMode2D(camera);

        gameMap.DrawBelowPlayer();
      

        // Малюємо самих NPC на карті
        for (auto& npc : npcs)
        {
            npc.Draw();
        }

         player.Draw(); 
        

        gameMap.DrawAbovePlayer();

       if (activeNPC != nullptr)
        {
            DrawText(
                "Press E to talk",
                activeNPC->position.x - 25,
                activeNPC->position.y - 20,
                9,
                YELLOW
            );

            // Реєструємо натискання розмови
            if (IsKeyPressed(KEY_E))
            {
                currentDialogue = activeNPC->dialogue;
                dialogueTimer = 2.0f;
                speakingNPC = activeNPC; 
            }
        } 
        if (dialogueTimer > 0.0f && speakingNPC != nullptr)
        {
            DrawText(
                currentDialogue.c_str(),
                speakingNPC->position.x - 40,
                speakingNPC->position.y - 40,
                9,
                WHITE
            );
        }

        DrawText("Use WASD/Arrows to Move", 1200, 2130, 15, WHITE);
        DrawText("Hold SHIFT to run", 1200, 2150, 10, WHITE);

        EndMode2D();

        // UI елементи поверх камери
        int centerX = windowWidth / 2;
        int centerY = windowHeight / 2;
        float crossLength = 25.0f;
        float thickness = 2.0f;
        // Перехрестя по центру вікна для дебагу
      //  DrawLineEx(Vector2{ (float)centerX - crossLength, (float)centerY },
          //  Vector2{ (float)centerX + crossLength, (float)centerY },
          //  thickness, RED);
      //  DrawLineEx(Vector2{ (float)centerX , (float)centerY - crossLength},
           // Vector2{ (float)centerX, (float)centerY + crossLength },
          //  thickness, RED);
        DrawText(TextFormat("Player Pos: X: %.1f, Y: %.1f", player.GetPosition().x, player.GetPosition().y), 15, 50, 20, RED);
        DrawText("Screen bounds clamped safely!", 15, windowHeight - 30, 16, RAYWHITE);

        EndDrawing();
    }

   
    UnloadTexture(npcTexture);
    gameMap.Unload();
    CloseWindow();

    return 0;
}