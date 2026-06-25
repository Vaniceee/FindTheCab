#pragma once
#include "raylib.h"

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

class Player {
public:
    Player();
    Player(Vector2 spawnPosition);
    ~Player();
    Vector2 GetPosition() const { return position;  }
    void SetPosition(Vector2 pos) { position = pos; }
    Rectangle GetHitbox() const { return GetFeetHitbox(); }

    float GetStamina() const { return stamina; } // щоб передавати в мейн поточну стаміну

    // Pass window sizes here to calculate screen edge limits dynamically
    void Update(int windowWidth, int windowHeight);
    void Draw();

   // Vector2 GetPosition() const { return position; }
    Rectangle GetFeetHitbox() const; // Bottom footprint collider remains intact

    bool IsSprinting() const { return (stamina > 0.0f && IsKeyDown(KEY_LEFT_SHIFT) && (IsKeyDown(KEY_W) || IsKeyDown(KEY_S) || IsKeyDown(KEY_A) || IsKeyDown(KEY_D) || IsKeyDown(KEY_UP) || IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT))); }

private:
    Vector2 position;
    float baseSpeed;
    float currentSpeed;

    // СТАМІНА ТА ЗАДИШКА
    float stamina;
    bool isExhausted;

    ////////// PLAYER SPRITE CONFIGURATION //////////////

    const float spriteWidth = 32.0f;
    const float spriteHeight = 32.0f;
    const float scale = 1.0f;

    PlayerDirection direction;
    PlayerState state;
    bool flipX;

    int currentFrame;
    float frameTimer;
    float frameDuration;

    Texture2D textureIdle;
    Texture2D textureWalk;
    Texture2D* currentTexture;
};