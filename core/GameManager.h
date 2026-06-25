#pragma once
#include "raylib.h"
#include "GameMap.h" 
#include "../Entities/Player.h"
#include "../Entities/NPC.h"
#include "../Entities/Entities.h"
#include "../UI/UI.h"
#include <vector>
#include <string>

class GameManager {
public:
    // Енуми та структури тепер належать класу
    enum class GameState {
        MAIN_MENU,
        GAMEPLAY,
        PAUSE,
        SETTINGS
    };

    struct ResolutionOption {
        int width;
        int height;
        const char* label;
    };

    GameManager();
    ~GameManager();

    bool Initialize();  // Завантаження вікна та асетів
    void Run();         // Головний цикл
    void Shutdown();    // Вивантаження ресурсів

private:
    // Допоміжні методи для логіки та рендерингу
    void UpdateLogic(float deltaTime, Vector2 virtualMouse);
    void RenderGame(Vector2 virtualMouse);

    // --- Системні змінні ---
    const int windowWidth = 1920;
    const int windowHeight = 1080;
    const int virtualWidth = 1920;
    const int virtualHeight = 1080;

    RenderTexture2D target;
    Rectangle canvasSource;
    Rectangle canvasDest;
    Vector2 canvasOrigin;

    GameState currentState = GameState::MAIN_MENU;
    bool exitGame = false;

    // --- Графіка меню ---
    Texture2D menuBg;
    Texture2D btnTemplate;

    // --- Звуки та Музика ---
    Music menuMusic;
    Music backgroundMusic;
    std::vector<Sound> Steps;
    Sound interactSound;

    // --- Таймери та прапорці логіки ---
    bool musicWaiting = false;
    float musicPauseTimer = 0.0f;
    const float MUSIC_PAUSE_DURATION = 5.0f;
    float stepTimer = 0.0f;
    float dialogueTimer = 0.0f;

    // --- Ігрові об'єкти ---
    GameMap gameMap;
    Texture2D npcTexture;
    Player player; // Ініціалізуємо в .cpp через список ініціалізації
    StaminaBar staminaBar;
    std::vector<NPC> npcs;
    Camera2D camera;

    // --- Діалоги ---
    std::string currentDialogue = "";
    NPC* activeNPC = nullptr;
    NPC* speakingNPC = nullptr;

    // --- Геометрія інтерфейсу (Кнопки) ---
    float menuStartX = 150.0f;
    Rectangle playRect;
    Rectangle settingsRect;
    Rectangle quitRect;
    Rectangle resumePauseRect;
    Rectangle menuPauseRect;
    Rectangle backSettingsRect;

    // --- Налаштування ---
    bool musicEnabled = true;
    ResolutionOption resOptions[3];
    int currentResIndex = 0;
    Rectangle musicToggleRect;
    Rectangle resRects[3];
};