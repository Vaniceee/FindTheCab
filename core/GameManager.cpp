#include "GameManager.h"

// Конструктор ініціалізує гравця та базові параметри
GameManager::GameManager() : player({ 1500.0f, 3500.0f }), camera{ 0 } {
    // Налаштування кнопок
    playRect = { menuStartX, 424.0f, 256.0f, 64.0f };
    settingsRect = { menuStartX, 508.0f, 256.0f, 64.0f };
    quitRect = { menuStartX, 592.0f, 256.0f, 64.0f };

    resumePauseRect = { (float)virtualWidth / 2 - 128, 450, 256, 64 };
    menuPauseRect = { (float)virtualWidth / 2 - 128, 540, 256, 64 };
    backSettingsRect = { 150.0f, 900.0f, 256.0f, 64.0f };

    // Опції роздільної здатності
    resOptions[0] = { 1920, 1080, "1920 x 1080" };
    resOptions[1] = { 1600, 900,  "1600 x 900" };
    resOptions[2] = { 1280, 720,  "1280 x 720" };

    musicToggleRect = { 150.0f, 350.0f, 30.0f, 30.0f };
    resRects[0] = { 150.0f, 500.0f, 30.0f, 30.0f };
    resRects[1] = { 150.0f, 560.0f, 30.0f, 30.0f };
    resRects[2] = { 150.0f, 620.0f, 30.0f, 30.0f };
}

GameManager::~GameManager() {}

// 1. ІНІЦІАЛІЗАЦІЯ ВІКНА ТА ЗАВАНТАЖЕННЯ РЕСУРСІВ
bool GameManager::Initialize() {
    InitWindow(windowWidth, windowHeight, "Find the Cab!");
    SetWindowPosition(0, 0);
    SetTargetFPS(60);
    ToggleFullscreen();
    SetExitKey(KEY_NULL);
    InitAudioDevice();

    target = LoadRenderTexture(virtualWidth, virtualHeight);
    canvasSource = { 0.0f, 0.0f, (float)virtualWidth, -(float)virtualHeight };
    canvasDest = { 0.0f, 0.0f, (float)windowWidth, (float)windowHeight };
    canvasOrigin = { 0.0f, 0.0f };

    // !!! ОДРАЗУ ТУТ ЗАВАНТАЖУЄМО ТЕКСТУРИ ГРАВЦЯ, БО ВІКНО ВЖЕ СТВОРЕНО !!!
    player.Load();

    // Завантаження меню
    menuBg = LoadTexture("assets/MainMenu.png");
    btnTemplate = LoadTexture("assets/TemplateButton.png");
    SetTextureFilter(menuBg, TEXTURE_FILTER_POINT);
    SetTextureFilter(btnTemplate, TEXTURE_FILTER_POINT);

    menuMusic = LoadMusicStream("assets/MainMenuTheme.wav");
    SetMusicVolume(menuMusic, 0.15f);
    PlayMusicStream(menuMusic);

    // Ігрові звуки
    Steps.push_back(LoadSound("assets/WalkSound1.wav"));
    Steps.push_back(LoadSound("assets/WalkSound2.wav"));
    Steps.push_back(LoadSound("assets/WalkSound3.wav"));
    interactSound = LoadSound("assets/InteractSound.wav");

    for (size_t i = 0; i < Steps.size(); i++) SetSoundVolume(Steps[i], 0.65f);
    SetSoundVolume(interactSound, 0.6f);

    backgroundMusic = LoadMusicStream("assets/ThemeMusic.wav");
    backgroundMusic.looping = false;
    SetMusicVolume(backgroundMusic, 0.08f);

    // Мапа
    if (!gameMap.Load("assets/floor1_vr3.tmj", "assets/version2.png")) {
        return false;
    }

    // NPC та Гравець
    npcTexture = LoadTexture("assets/16x16 Idle.png");
    SetTextureFilter(npcTexture, TEXTURE_FILTER_POINT);

    staminaBar.Load();

    npcs.emplace_back(Vector2{ 1270.0f, 3320.0f }, "The library is upstairs.", &npcTexture);
    npcs.emplace_back(Vector2{ 2140.0f, 2930.0f }, "The cafeteria is on the left.", &npcTexture);

    // Камера
    camera.target = player.GetPosition();
    camera.offset = Vector2{ virtualWidth / 2.25f, virtualHeight / 2.5f };
    camera.rotation = 0.0f;
    camera.zoom = 5.0f;

    return true;
}

// 2. ГОЛОВНИЙ ІГРОВИЙ ЦИКЛ (Став дуже простим та охайним)
void GameManager::Run() {
    if (!Initialize()) {
        CloseWindow();
        return;
    }

    while (!WindowShouldClose() && !exitGame) {
        float deltaTime = GetFrameTime();
        Vector2 realMousePos = GetMousePosition();
        Vector2 virtualMouse = {
            (realMousePos.x / (float)GetScreenWidth()) * (float)virtualWidth,
            (realMousePos.y / (float)GetScreenHeight()) * (float)virtualHeight
        };

        UpdateLogic(deltaTime, virtualMouse);
        RenderGame(virtualMouse);
    }

    Shutdown();
}

// 3. ОБРОБКА ВСІЄЇ ЛОГІКИ ГРИ (Оновлення координат, натискання кнопок)
void GameManager::UpdateLogic(float deltaTime, Vector2 virtualMouse) {
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
        for (int i = 0; i < 3; i++) {
            if (CheckCollisionPointRec(virtualMouse, resRects[i]) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                currentResIndex = i;
                int newWidth = resOptions[currentResIndex].width;
                int newHeight = resOptions[currentResIndex].height;
                SetWindowSize(newWidth, newHeight);
                canvasDest = { 0.0f, 0.0f, (float)newWidth, (float)newHeight };
            }
        }
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
        bool isMoving = (deltaX * deltaX + deltaY * deltaY) > 0.0025f;

        if (isMoving) {
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
}

// 4. ВІДМАЛЮВАННЯ ВСЬОГО ІНТЕРФЕЙСУ ТА СВІТУ (Рендеринг)
void GameManager::RenderGame(Vector2 virtualMouse) {
    BeginTextureMode(target);
    ClearBackground(BLACK);

    switch (currentState) {
    case GameState::MAIN_MENU: {
        DrawTexture(menuBg, 0, 0, WHITE);
        const char* gameTitle = "Find the Cab!!";
        int titleFontSize = 80;
        int titleX = menuStartX + (256 - MeasureText(gameTitle, titleFontSize)) / 3;
        DrawText(gameTitle, titleX, 180, titleFontSize, GOLD);

        DrawTexture(btnTemplate, playRect.x, playRect.y, WHITE);
        DrawText("PLAY", playRect.x + 102, playRect.y + 20, 20, CheckCollisionPointRec(virtualMouse, playRect) ? YELLOW : WHITE);

        DrawTexture(btnTemplate, settingsRect.x, settingsRect.y, WHITE);
        DrawText("SETTINGS", settingsRect.x + 72, settingsRect.y + 20, 20, CheckCollisionPointRec(virtualMouse, settingsRect) ? YELLOW : WHITE);

        DrawTexture(btnTemplate, quitRect.x, quitRect.y, WHITE);
        DrawText("QUIT", quitRect.x + 102, quitRect.y + 20, 20, CheckCollisionPointRec(virtualMouse, quitRect) ? YELLOW : WHITE);
        break;
    }
    case GameState::SETTINGS: {
        DrawTexture(menuBg, 0, 0, WHITE);
        DrawText("SETTINGS MENU", 150, 200, 40, WHITE);
        DrawText("Music Audio:", 150, 310, 24, LIGHTGRAY);
        DrawRectangleRec(musicToggleRect, musicEnabled ? GREEN : RED);
        DrawRectangleLinesEx(musicToggleRect, 2, WHITE);
        DrawText(musicEnabled ? "ENABLED" : "DISABLED", musicToggleRect.x + 50, musicToggleRect.y + 3, 20, musicEnabled ? GREEN : RED);

        DrawText("Screen Resolution (16:9):", 150, 450, 24, LIGHTGRAY);
        for (int i = 0; i < 3; i++) {
            DrawRectangleRec(resRects[i], (currentResIndex == i) ? GREEN : GRAY);
            DrawRectangleLinesEx(resRects[i], 2, WHITE);
            DrawText(resOptions[i].label, resRects[i].x + 50, resRects[i].y + 3, 20, (currentResIndex == i) ? GREEN : WHITE);
        }

        DrawTexture(btnTemplate, backSettingsRect.x, backSettingsRect.y, WHITE);
        DrawText("BACK", backSettingsRect.x + 100, backSettingsRect.y + 20, 20, CheckCollisionPointRec(virtualMouse, backSettingsRect) ? YELLOW : WHITE);
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
        DrawLineEx(Vector2{ (float)centerX - 25.0f, (float)centerY }, Vector2{ (float)centerX + 25.0f, (float)centerY }, 2.0f, RED);
        DrawLineEx(Vector2{ (float)centerX , (float)centerY - 25.0f }, Vector2{ (float)centerX, (float)centerY + 25.0f }, 2.0f, RED);

        if (currentState == GameState::PAUSE) {
            DrawRectangle(0, 0, virtualWidth, virtualHeight, Fade(BLACK, 0.6f));
            DrawText("PAUSE", virtualWidth / 2 - 60, 300, 40, WHITE);

            DrawTexture(btnTemplate, resumePauseRect.x, resumePauseRect.y, WHITE);
            DrawText("RESUME", resumePauseRect.x + 88, resumePauseRect.y + 20, 20, CheckCollisionPointRec(virtualMouse, resumePauseRect) ? YELLOW : WHITE);

            DrawTexture(btnTemplate, menuPauseRect.x, menuPauseRect.y, WHITE);
            DrawText("MAIN MENU", menuPauseRect.x + 72, menuPauseRect.y + 20, 20, CheckCollisionPointRec(virtualMouse, menuPauseRect) ? YELLOW : WHITE);
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

// 5. ОЧИЩЕННЯ ПАМ'ЯТІ ПРИ ЗАКРИТТІ ГРИ
void GameManager::Shutdown() {
    staminaBar.Unload();
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
}