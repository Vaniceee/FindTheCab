#include "Player.h"
#include <cmath>

Player::Player(Vector2 spawnPosition) {
    position = spawnPosition;
    baseSpeed = 4.0f;
    currentSpeed = baseSpeed;

    direction = PlayerDirection::DOWN;
    state = PlayerState::IDLE;
    flipX = false;

    currentFrame = 0;
    frameTimer = 0.0f;
    frameDuration = 0.14f;

    textureIdle = LoadTexture("assets/16x32 Idle.png");
    textureWalk = LoadTexture("assets/16x32 Walk.png");
    SetTextureFilter(textureIdle, TEXTURE_FILTER_POINT);
    SetTextureFilter(textureWalk, TEXTURE_FILTER_POINT);

    currentTexture = &textureIdle;
}

Player::~Player() {
    UnloadTexture(textureIdle);
    UnloadTexture(textureWalk);
}

void Player::Update(int windowWidth, int windowHeight) {
    float moveX = 0.0f;
    float moveY = 0.0f;

    // 1. Sprint Modifier Configuration
    if (IsKeyDown(KEY_LEFT_SHIFT)) {
        currentSpeed = baseSpeed * 1.6f;
        frameDuration = 0.08f;
    }
    else {
        currentSpeed = baseSpeed;
        frameDuration = 0.14f;
    }

    // 2. Poll Directions
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))    moveY -= 1.0f;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))  moveY += 1.0f;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  moveX -= 1.0f;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) moveX += 1.0f;

    // 3. State Engine & Calibrated Direction Matrix
    if (moveX == 0.0f && moveY == 0.0f) {
        if (state != PlayerState::IDLE) {
            state = PlayerState::IDLE;
            currentTexture = &textureIdle;
            currentFrame = 0;
        }
    }
    else {
        if (state != PlayerState::WALKING) {
            state = PlayerState::WALKING;
            currentTexture = &textureWalk;
        }

        float length = std::sqrt((moveX * moveX) + (moveY * moveY));
        moveX /= length;
        moveY /= length;

        if (moveX == 0.0f && moveY < 0.0f) {
            direction = PlayerDirection::UP;
            flipX = false;
        }
        else if (moveX == 0.0f && moveY > 0.0f) {
            direction = PlayerDirection::DOWN;
            flipX = false;
        }
        else if (moveX < 0.0f && moveY == 0.0f) {
            direction = PlayerDirection::RIGHT;
            flipX = true;
        }
        else if (moveX > 0.0f && moveY == 0.0f) {
            direction = PlayerDirection::RIGHT;
            flipX = false;
        }
        else if (moveX < 0.0f && moveY < 0.0f) {
            direction = PlayerDirection::UP_RIGHT;
            flipX = true;
        }
        else if (moveX > 0.0f && moveY < 0.0f) {
            direction = PlayerDirection::UP_RIGHT;
            flipX = false;
        }
        else if (moveX < 0.0f && moveY > 0.0f) {
            direction = PlayerDirection::DOWN_RIGHT;
            flipX = true;
        }
        else if (moveX > 0.0f && moveY > 0.0f) {
            direction = PlayerDirection::DOWN_RIGHT;
            flipX = false;
        }
    }

    // 4. Apply Dynamic Velocity Movement
    position.x += moveX * currentSpeed;
    position.y += moveY * currentSpeed;

    // 5. Edge Screen Boundaries Clamping (Prevents leaving window)
    if (position.x < 0.0f) {
        position.x = 0.0f;
    }
    if (position.y < 0.0f) {
        position.y = 0.0f;
    }
    if (position.x > (float)windowWidth - (spriteWidth * scale)) {
        position.x = (float)windowWidth - (spriteHeight * scale);
    }
    if (position.y > (float)windowHeight - (spriteHeight * scale)) {
        position.y = (float)windowHeight - (spriteHeight * scale);
    }

    // 6. Run Animation Timer
    frameTimer += GetFrameTime();
    if (frameTimer >= frameDuration) {
        frameTimer = 0.0f;
        currentFrame++;
        if (currentFrame >= 4) {
            currentFrame = 0;
        }
    }
}

Rectangle Player::GetFeetHitbox() const {
    // Keeps a footprint box calculated right at the base coordinates of the 24x24 sprite frame
    return Rectangle{ position.x + (8.0f * scale), position.y + (24.0f * scale), 16.0f * scale, 8.0f * scale };
}

void Player::Draw() {
    float sourceX = currentFrame * spriteWidth;
    float sourceY = (int)direction * spriteHeight;

    Rectangle sourceRec = {
        sourceX,
        sourceY,
        flipX ? -spriteWidth : spriteWidth,
        spriteHeight
    };
    Rectangle destRec = {
        position.x,
        position.y,
        spriteWidth * scale,
        spriteHeight * scale
    };

    Vector2 origin = { 0.0f, 0.0f };
    DrawTexturePro(*currentTexture, sourceRec, destRec, origin, 0.0f, WHITE);

    // Debug line to visually trace your footprint hitbox location:(Commet row bellow to disable hitbox)
     DrawRectangleLinesEx(GetFeetHitbox(), 1.0f, RED);
}