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
    Player(Vector2 spawnPosition);
    ~Player();

    // Pass window sizes here to calculate screen edge limits dynamically
    void Update(int windowWidth, int windowHeight);
    void Draw();

    Vector2 GetPosition() const { return position; }
    Rectangle GetFeetHitbox() const; // Bottom footprint collider remains intact

private:
    Vector2 position;
    float baseSpeed;
    float currentSpeed;

    ////////// PLAYER SPRITE CONFIGURATION //////////////

    const float spriteWidth = 16.0f;
    const float spriteHeight = 32.0f;
    const float scale = 3.0f;

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