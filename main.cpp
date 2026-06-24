#include "raylib.h"
#include "Player.h"
#include "GameMap.h" 
#include "NPC.h"
#include "UI.h"
#include <vector>
#include <string>

enum class GameState {
    MAIN_MENU,
    GAMEPLAY,
    PAUSE,
    SETTINGS
};

// Структура для опцій екрану
struct ResolutionOption {
    int width;
    int height;
    const char* label;
};

int main() {
    //  SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_HIGHDPI);

    const int windowWidth = 1920;
    const int windowHeight = 1080;

    InitWindow(windowWidth, windowHeight, "Find the Cab!");
    SetWindowPosition(0, 0);
    SetTargetFPS(60);

    ToggleFullscreen();
    SetExitKey(KEY_NULL);

    InitAudioDevice();

    const int virtualWidth = 1920;
    const int virtualHeight = 1080;
    RenderTexture2D target = LoadRenderTexture(virtualWidth, virtualHeight);

    Rectangle canvasSource = { 0.0f, 0.0f, (float)virtualWidth, -(float)virtualHeight };
    Rectangle canvasDest = { 0.0f, 0.0f, (float)windowWidth, (float)windowHeight };
    Vector2 canvasOrigin = { 0.0f, 0.0f };

    GameState currentState = GameState::MAIN_MENU;
    bool exitGame = false;

    // ------------------- ЗАВАНТАЖЕННЯ АСЕТІВ МЕНЮ -------------------
    Texture2D menuBg = LoadTexture("assets/MainMenu.png");
    Texture2D btnTemplate = LoadTexture("assets/TemplateButton.png");

    SetTextureFilter(menuBg, TEXTURE_FILTER_POINT);
    SetTextureFilter(btnTemplate, TEXTURE_FILTER_POINT);

    Music menuMusic = LoadMusicStream("assets/MainMenuTheme.wav");
    SetMusicVolume(menuMusic, 0.15f);
    PlayMusicStream(menuMusic);

    // ------------------- ІГРОВІ АСЕТИ -------------------
    std::vector<Sound> Steps;
    Steps.push_back(LoadSound("assets/WalkSound1.wav"));
    Steps.push_back(LoadSound("assets/WalkSound2.wav"));
    Steps.push_back(LoadSound("assets/WalkSound3.wav"));

    Sound interactSound = LoadSound("assets/InteractSound.wav");
    for (size_t i = 0; i < Steps.size(); i++) {
        SetSoundVolume(Steps[i], 0.65f);
    }
    SetSoundVolume(interactSound, 0.6f);

    Music backgroundMusic = LoadMusicStream("assets/ThemeMusic.wav");
    backgroundMusic.looping = false;
    SetMusicVolume(backgroundMusic, 0.08f);

    bool musicWaiting = false;
    float musicPauseTimer = 0.0f;
    const float MUSIC_PAUSE_DURATION = 5.0f;
    float stepTimer = 0.0f;

    GameMap gameMap;
    if (!gameMap.Load("assets/floor1_vr3.tmj", "assets/version2.png")) {
        CloseWindow();
        return -1;
    }

    Texture2D npcTexture = LoadTexture("assets/16x16 Idle.png");
    SetTextureFilter(npcTexture, TEXTURE_FILTER_POINT);

    Player player({ 1500.0f, 3500.0f });

    // ------------------- ШКАЛА СТАМІНИ (UI) -------------------
    StaminaBar staminaBar;  // <--- 2. СТВОРЮЄМО ОБ'ЄКТ UI
    staminaBar.Load();      // <--- 3. ЗАВАНТАЖУЄМО КАРТИНКИ СТАМІНИ В ПАМ'ЯТЬ

    std::vector<NPC> npcs;
    npcs.emplace_back(Vector2{ 1270.0f, 3320.0f }, "The library is upstairs.", &npcTexture);
    npcs.emplace_back(Vector2{ 2140.0f, 2930.0f }, "The cafeteria is on the left.", &npcTexture);

    Camera2D camera = { 0 };
    camera.target = player.GetPosition();
    camera.offset = Vector2{ virtualWidth / 2.25f, virtualHeight / 2.5f };
    camera.rotation = 0.0f;
    camera.zoom = 5.0f;

    std::string currentDialogue = "";
    float dialogueTimer = 0.0f;
    NPC* activeNPC = nullptr;
    NPC* speakingNPC = nullptr;

    // Координати кнопок головного меню
    float menuStartX = 150.0f;
    float playY = 424.0f;
    float settingsY = 508.0f;
    float quitY = 592.0f;

    Rectangle playRect = { menuStartX, playY, 256.0f, 64.0f };
    Rectangle settingsRect = { menuStartX, settingsY, 256.0f, 64.0f };
    Rectangle quitRect = { menuStartX, quitY, 256.0f, 64.0f };

    // Кнопки паузи та налаштувань
    Rectangle resumePauseRect = { (float)virtualWidth / 2 - 128, 450, 256, 64 };
    Rectangle menuPauseRect = { (float)virtualWidth / 2 - 128, 540, 256, 64 };
    Rectangle backSettingsRect = { 150.0f, 900.0f, 256.0f, 64.0f };

    // ------------------- ЗМІННІ ДЛЯ НАЛАШТУВАНЬ -------------------
    bool musicEnabled = true;

    ResolutionOption resOptions[3] = {
        { 1920, 1080, "1920 x 1080" },
        { 1600, 900,  "1600 x 900"  },
        { 1280, 720,  "1280 x 720"  }
    };
    int currentResIndex = 0; // За замовчуванням 1920х1080 активний

    // Геометрія елементів інтерфейсу налаштувань (інтерактивні квадрати)
    Rectangle musicToggleRect = { 150.0f, 350.0f, 30.0f, 30.0f };
    Rectangle resRects[3] = {
        { 150.0f, 500.0f, 30.0f, 30.0f },
        { 150.0f, 560.0f, 30.0f, 30.0f },
        { 150.0f, 620.0f, 30.0f, 30.0f }
    };

    while (!WindowShouldClose() && !exitGame) {
        float deltaTime = GetFrameTime();

        // Розрахунок координат віртуальної мишки
        Vector2 realMousePos = GetMousePosition();
        Vector2 virtualMouse = {
            (realMousePos.x / (float)GetScreenWidth()) * (float)virtualWidth,
            (realMousePos.y / (float)GetScreenHeight()) * (float)virtualHeight
        };

        // ===================================================================
        // 1. UPDATE LOGIC
        // ===================================================================
        switch (currentState) {
        case GameState::MAIN_MENU: {
            UpdateMusicStream(menuMusic);

            if (CheckCollisionPointRec(virtualMouse, playRect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                StopMusicStream(menuMusic);
                if (musicEnabled) PlayMusicStream(backgroundMusic);
                currentState = GameState::GAMEPLAY;
            }
            else if (CheckCollisionPointRec(virtualMouse, settingsRect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                currentState = GameState::SETTINGS;
            }
            else if (CheckCollisionPointRec(virtualMouse, quitRect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                exitGame = true;
            }
            break;
        }

        case GameState::SETTINGS: {
            UpdateMusicStream(menuMusic);

            // Клік по квадрату музики (вкл/викл)
            if (CheckCollisionPointRec(virtualMouse, musicToggleRect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                musicEnabled = !musicEnabled;
                if (!musicEnabled) {
                    SetMusicVolume(menuMusic, 0.0f);
                    SetMusicVolume(backgroundMusic, 0.0f);
                }
                else {
                    SetMusicVolume(menuMusic, 0.15f);
                    SetMusicVolume(backgroundMusic, 0.08f);
                }
            }

            // Клік по квадратах роздільної здатності
            for (int i = 0; i < 3; i++) {
                if (CheckCollisionPointRec(virtualMouse, resRects[i]) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    currentResIndex = i;
                    int newWidth = resOptions[currentResIndex].width;
                    int newHeight = resOptions[currentResIndex].height;

                    SetWindowSize(newWidth, newHeight);
                    // Оновлюємо canvasDest, щоб RenderTexture ідеально масштабувався під нове вікно
                    canvasDest = { 0.0f, 0.0f, (float)newWidth, (float)newHeight };
                }
            }

            // Кнопка BACK
            if (CheckCollisionPointRec(virtualMouse, backSettingsRect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                currentState = GameState::MAIN_MENU;
            }
            break;
        }

        case GameState::PAUSE: {
            if (CheckCollisionPointRec(virtualMouse, resumePauseRect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                currentState = GameState::GAMEPLAY;
            }
            else if (CheckCollisionPointRec(virtualMouse, menuPauseRect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                StopMusicStream(backgroundMusic);
                if (musicEnabled) PlayMusicStream(menuMusic);
                currentState = GameState::MAIN_MENU;
            }
            if (IsKeyPressed(KEY_ESCAPE)) {
                currentState = GameState::GAMEPLAY;
            }
            break;
        }

        case GameState::GAMEPLAY: {
            if (IsKeyPressed(KEY_ESCAPE)) {
                currentState = GameState::PAUSE;
                break;
            }

            if (!musicWaiting) {
                if (musicEnabled) UpdateMusicStream(backgroundMusic);
                if (GetMusicTimePlayed(backgroundMusic) >= GetMusicTimeLength(backgroundMusic) - 0.1f) {
                    StopMusicStream(backgroundMusic);
                    musicWaiting = true;
                    musicPauseTimer = MUSIC_PAUSE_DURATION;
                }
            }
            else {
                musicPauseTimer -= deltaTime;
                if (musicPauseTimer <= 0.0f) {
                    if (musicEnabled) PlayMusicStream(backgroundMusic);
                    musicWaiting = false;
                }
            }

            for (auto& npc : npcs) { npc.Update(); }

            if (dialogueTimer > 0.0f) {
                dialogueTimer -= deltaTime;
                if (dialogueTimer <= 0.0f) { speakingNPC = nullptr; }
            }

            Vector2 startPos = player.GetPosition();
            player.Update(virtualWidth, virtualHeight);
            Vector2 desiredPos = player.GetPosition();

            player.SetPosition({ desiredPos.x, startPos.y });
            if (gameMap.CheckWallCollision(player.GetHitbox())) { player.SetPosition({ startPos.x, startPos.y }); }

            Vector2 posAfterX = player.GetPosition();
            player.SetPosition({ posAfterX.x, desiredPos.y });
            if (gameMap.CheckWallCollision(player.GetHitbox())) { player.SetPosition({ posAfterX.x, startPos.y }); }

            gameMap.UpdateDoorTriggers(player.GetHitbox());

            Vector2 finalPos = player.GetPosition();
            float deltaX = finalPos.x - startPos.x;
            float deltaY = finalPos.y - startPos.y;
            bool isMoving = (long double)(deltaX * deltaX + deltaY * deltaY) > 0.0025f;

            if (isMoving) {
                // Звук тепер чітко знає, чи біжить гравець насправді (з урахуванням задишки)
                float stepCooldown = player.IsSprinting() ? 0.26f : 0.44f;

                stepTimer += deltaTime;
                if (stepTimer >= stepCooldown) {
                    int randomIndex = GetRandomValue(0, Steps.size() - 1);
                    PlaySound(Steps[randomIndex]);
                    stepTimer = 0.0f;
                }
            }
            else {
                stepTimer = 0.35f;
                for (size_t i = 0; i < Steps.size(); i++) { StopSound(Steps[i]); }
            }

            camera.target = player.GetPosition();

            activeNPC = nullptr;
            for (auto& npc : npcs) {
                if (npc.IsPlayerNear(player.GetPosition())) {
                    activeNPC = &npc;
                    break;
                }
            }
            break;
        }
        }

        // ===================================================================
        // 2. DRAW LOGIC
        // ===================================================================
        BeginTextureMode(target);
        ClearBackground(BLACK);

        switch (currentState) {
        case GameState::MAIN_MENU: {
            DrawTexture(menuBg, 0, 0, WHITE);

            /// GAME TITLE IN MAIN MENU
            const char* gameTitle = "Find the Cab!!";
            int titleFontSize = 80;
            int titleX = menuStartX + (256 - MeasureText(gameTitle, titleFontSize)) / 3;
            int titleY = 180;
            DrawText(gameTitle, titleX, titleY, titleFontSize, GOLD);

            // MENU BUTTONS
            DrawTexture(btnTemplate, playRect.x, playRect.y, WHITE);
            DrawText("PLAY", playRect.x + 102, playRect.y + 20, 20,
                CheckCollisionPointRec(virtualMouse, playRect) ? YELLOW : WHITE);

            DrawTexture(btnTemplate, settingsRect.x, settingsRect.y, WHITE);
            DrawText("SETTINGS", settingsRect.x + 72, settingsRect.y + 20, 20,
                CheckCollisionPointRec(virtualMouse, settingsRect) ? YELLOW : WHITE);

            DrawTexture(btnTemplate, quitRect.x, quitRect.y, WHITE);
            DrawText("QUIT", quitRect.x + 102, quitRect.y + 20, 20,
                CheckCollisionPointRec(virtualMouse, quitRect) ? YELLOW : WHITE);
            break;
        }

        case GameState::SETTINGS: {
            DrawTexture(menuBg, 0, 0, WHITE);
            DrawText("SETTINGS MENU", 150, 200, 40, WHITE);

            // --- МАЛЮЄМО НАЛАШТУВАННЯ МУЗИКИ ---
            DrawText("Music Audio:", 150, 310, 24, LIGHTGRAY);
            DrawRectangleRec(musicToggleRect, musicEnabled ? GREEN : RED);
            DrawRectangleLinesEx(musicToggleRect, 2, WHITE);
            DrawText(musicEnabled ? "ENABLED" : "DISABLED", musicToggleRect.x + 50, musicToggleRect.y + 3, 20, musicEnabled ? GREEN : RED);

            // --- МАЛЮЄМО ВИБІР РОЗДІЛЬНОЇ ЗДАТНОСТІ ---
            DrawText("Screen Resolution (16:9):", 150, 450, 24, LIGHTGRAY);
            for (int i = 0; i < 3; i++) {
                // Якщо індекс збігається з активним — квадрат зелений, інші — сірі
                DrawRectangleRec(resRects[i], (currentResIndex == i) ? GREEN : GRAY);
                DrawRectangleLinesEx(resRects[i], 2, WHITE);

                DrawText(resOptions[i].label, resRects[i].x + 50, resRects[i].y + 3, 20,
                    (currentResIndex == i) ? GREEN : WHITE);
            }

            // Кнопка НАЗАД (Тут залишив темплейт для стилістики)
            DrawTexture(btnTemplate, backSettingsRect.x, backSettingsRect.y, WHITE);
            DrawText("BACK", backSettingsRect.x + 100, backSettingsRect.y + 20, 20,
                CheckCollisionPointRec(virtualMouse, backSettingsRect) ? YELLOW : WHITE);
            break;
        }

        case GameState::PAUSE:
        case GameState::GAMEPLAY: {
            BeginMode2D(camera);
            gameMap.DrawBelowPlayer();
            for (auto& npc : npcs) { npc.Draw(); }
            player.Draw();
            gameMap.DrawAbovePlayer();

            if (currentState == GameState::GAMEPLAY && activeNPC != nullptr) {
                DrawText("Press E to talk", activeNPC->position.x - 25, activeNPC->position.y - 20, 9, YELLOW);
                if (IsKeyPressed(KEY_E)) {
                    currentDialogue = activeNPC->dialogue;
                    dialogueTimer = 2.0f;
                    speakingNPC = activeNPC;
                    PlaySound(interactSound);
                }
            }

            if (dialogueTimer > 0.0f && speakingNPC != nullptr) {
                DrawText(currentDialogue.c_str(), speakingNPC->position.x - 40, speakingNPC->position.y - 40, 9, WHITE);
            }

            DrawText("Use WASD/Arrows to Moveeee", 1260, 3770, 15, WHITE);
            DrawText("Hold SHIFT to run", 1260, 3800, 10, WHITE);
            DrawText("Use 'E' to interact", 1550, 3770, 15, WHITE);
            EndMode2D();

            // 4. ВІДОБРАЖЕННЯ ШКАЛИ СТАМІНИ (наприклад, у верхньому лівому кутку)
            Vector2 staminaBarPos = { 40.0f, 40.0f };
            staminaBar.Draw(player.GetStamina(), staminaBarPos);

            if (!gameMap.currentDoorMessage.empty()) {
                DrawRectangle(20, virtualHeight - 110, virtualWidth - 40, 60, Fade(BLACK, 0.7f));
                DrawText(gameMap.currentDoorMessage.c_str(), 40, virtualHeight - 92, 24, RAYWHITE);
            }

            DrawText(TextFormat("Player Pos: X: %.1f, Y: %.1f", player.GetPosition().x, player.GetPosition().y), 15, 50, 20, RED);
            DrawText("Screen bounds clamped safely!", 15, virtualHeight - 30, 16, RAYWHITE);

            int centerX = virtualWidth / 2;
            int centerY = virtualHeight / 2;
            float crossLength = 25.0f;
            float thickness = 2.0f;

            DrawLineEx(Vector2{ (float)centerX - crossLength, (float)centerY }, Vector2{ (float)centerX + crossLength, (float)centerY }, thickness, RED);
            DrawLineEx(Vector2{ (float)centerX , (float)centerY - crossLength }, Vector2{ (float)centerX, (float)centerY + crossLength }, thickness, RED);

            if (currentState == GameState::PAUSE) {
                DrawRectangle(0, 0, virtualWidth, virtualHeight, Fade(BLACK, 0.6f));
                DrawText("PAUSE", virtualWidth / 2 - 60, 300, 40, WHITE);

                DrawTexture(btnTemplate, resumePauseRect.x, resumePauseRect.y, WHITE);
                DrawText("RESUME", resumePauseRect.x + 88, resumePauseRect.y + 20, 20,
                    CheckCollisionPointRec(virtualMouse, resumePauseRect) ? YELLOW : WHITE);

                DrawTexture(btnTemplate, menuPauseRect.x, menuPauseRect.y, WHITE);
                DrawText("MAIN MENU", menuPauseRect.x + 72, menuPauseRect.y + 20, 20,
                    CheckCollisionPointRec(virtualMouse, menuPauseRect) ? YELLOW : WHITE);
            }
            break;
        }
        }
        EndTextureMode();

        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexturePro(target.texture, canvasSource, canvasDest, canvasOrigin, 0.0f, WHITE);
        EndDrawing();
    }

    // ------------------- ОЧИЩЕННЯ ПАМ'ЯТІ -------------------
    staminaBar.Unload(); // <--- ОЧИЩАЄМО ТЕКСТУРИ ШКАЛИ СТАМІНИ
    UnloadRenderTexture(target);
    UnloadTexture(menuBg);
    UnloadTexture(btnTemplate);
    UnloadMusicStream(menuMusic);

    UnloadMusicStream(backgroundMusic);
    for (size_t i = 0; i < Steps.size(); i++) UnloadSound(Steps[i]);
    Steps.clear();
    UnloadSound(interactSound);
    CloseAudioDevice();
    UnloadTexture(npcTexture);
    gameMap.Unload();
    CloseWindow();

    return 0;
}