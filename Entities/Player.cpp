#include "Player.h"
#include <cmath>

Player::Player(Vector2 spawnPosition) {
    position = spawnPosition;
    baseSpeed = 2.2f;
    currentSpeed = baseSpeed;

    // Налаштовуємо protected змінні, які прийшли від Character
    spriteWidth = 32.0f;
    spriteHeight = 32.0f;
    scale = 1.0f;
    currentFrame = 0;
    frameTimer = 0.0f;
    frameDuration = 0.14f;

    // Ініціалізація стаміни
    stamina = 100.0f;
    isExhausted = false;

    direction = PlayerDirection::DOWN;
    state = PlayerState::IDLE;
    flipX = false;

    currentTexture = &textureIdle;
}

void Player::Load() {
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

bool Player::IsSprinting() const {
    return (stamina > 0.0f && IsKeyDown(KEY_LEFT_SHIFT) &&
        (IsKeyDown(KEY_W) || IsKeyDown(KEY_S) || IsKeyDown(KEY_A) || IsKeyDown(KEY_D) ||
            IsKeyDown(KEY_UP) || IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT)));
}

void Player::Update(int windowWidth, int windowHeight) {
    float moveX = 0.0f;
    float moveY = 0.0f;

    // Опитуємо клавіші руху
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))    moveY -= 1.0f;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))  moveY += 1.0f;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  moveX -= 1.0f;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) moveX += 1.0f;

    bool isMoving = (moveX != 0.0f || moveY != 0.0f);

    // Система задишки та керування швидкістю
    if (stamina <= 0.0f) isExhausted = true;
    if (isExhausted && stamina >= 40.0f) isExhausted = false;

    if (IsKeyDown(KEY_LEFT_SHIFT) && isMoving && !isExhausted) {
        currentSpeed = baseSpeed * 1.6f;
        frameDuration = 0.08f;
        stamina -= 20.0f * GetFrameTime();
    }
    else {
        currentSpeed = baseSpeed;
        frameDuration = 0.14f;

        if (stamina < 100.0f) {
            if (isMoving) stamina += 5.0f * GetFrameTime();
            else          stamina += 15.0f * GetFrameTime();
        }
    }

    if (stamina < 0.0f) stamina = 0.0f;
    if (stamina > 100.0f) stamina = 100.0f;

    // Двигун станів анімації
    if (!isMoving) {
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

        // Нормалізація діагоналей
        float length = std::sqrt((moveX * moveX) + (moveY * moveY));
        moveX /= length;
        moveY /= length;

        // Розрахунок поворотів та фліпу спрайту
        if (moveX == 0.0f && moveY < 0.0f) { direction = PlayerDirection::UP; flipX = false; }
        else if (moveX == 0.0f && moveY > 0.0f) { direction = PlayerDirection::DOWN; flipX = false; }
        else if (moveX < 0.0f && moveY == 0.0f) { direction = PlayerDirection::RIGHT; flipX = true; }
        else if (moveX > 0.0f && moveY == 0.0f) { direction = PlayerDirection::RIGHT; flipX = false; }
        else if (moveX < 0.0f && moveY < 0.0f) { direction = PlayerDirection::UP_RIGHT; flipX = true; }
        else if (moveX > 0.0f && moveY < 0.0f) { direction = PlayerDirection::UP_RIGHT; flipX = false; }
        else if (moveX < 0.0f && moveY > 0.0f) { direction = PlayerDirection::DOWN_RIGHT; flipX = true; }
        else if (moveX > 0.0f && moveY > 0.0f) { direction = PlayerDirection::DOWN_RIGHT; flipX = false; }
    }

    // Застосовуємо рух (Зараз напряму, а згодом сюди підключимо колізії Character::Move)
    position.x += moveX * currentSpeed;
    position.y += moveY * currentSpeed;

    if (position.x < 0.0f) position.x = 0.0f;
    if (position.y < 0.0f) position.y = 0.0f;

    // Таймер кадрів (спільна логіка Character)
    frameTimer += GetFrameTime();
    if (frameTimer >= frameDuration) {
        frameTimer = 0.0f;
        currentFrame++;
        if (currentFrame >= 4) currentFrame = 0;
    }
}

void Player::Draw() {
    float sourceX = currentFrame * spriteWidth;
    float sourceY = (int)direction * spriteHeight;

    Rectangle sourceRec = { sourceX, sourceY, flipX ? -spriteWidth : spriteWidth, spriteHeight };
    Rectangle destRec = { position.x, position.y, spriteWidth * scale, spriteHeight * scale };

    DrawTexturePro(*currentTexture, sourceRec, destRec, { 0.0f, 0.0f }, 0.0f, WHITE);

    // Малюємо червону рамку ніг, яка тепер береться з базового класу Character!
    DrawRectangleLinesEx(Character::GetFeetHitbox(), 1.0f, RED);
}