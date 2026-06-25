#pragma once
#include "Entities.h" // Підключаємо нашого базового "батька"

enum class PlayerDirection {
    DOWN = 0,        // Row 0
    DOWN_RIGHT = 1,  // Row 1
    RIGHT = 2,       // Row 2
    UP_RIGHT = 3,    // Row 3
    UP = 4           // Row 4
};

enum class PlayerState {
    IDLE,
    WALKING
};

// Застосовуємо ООП успадкування: Player є типом Character
class Player : public Character {
public:
    Player();
    Player(Vector2 spawnPosition);
    ~Player();

    void Load();
    void Update(int windowWidth, int windowHeight);
    void Draw();

    float GetStamina() const { return stamina; } // Передача стаміни в GameManager/UI
    bool IsSprinting() const;

private:
    float baseSpeed;

    // СТАМІНА ТА ЗАДИШКА (Унікальні фішки гравця)
    float stamina;
    bool isExhausted;

    PlayerDirection direction;
    PlayerState state;
    bool flipX;

    // Текстури гравця
    Texture2D textureIdle;
    Texture2D textureWalk;
    Texture2D* currentTexture;
};